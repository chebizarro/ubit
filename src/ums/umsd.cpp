/*************************************************************************
 *
 *  umsd.cpp : Ubit Multiple-Mouse/Message Server
 *  provides multiple event flows, pseudo pointers, two-handed interaction,
 *  RendezVous identification, remote control facilities, etc.
 * 
 *  NOTE: -lsocket -lnsl linking directives required on Solaris
 *
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2003-2008 Eric Lecolinet / ENST Paris / http://www.enst.fr/~elc/ubit
 *
 * ***********************************************************************
 * COPYRIGHT NOTICE : 
 * THIS PROGRAM IS DISTRIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE 
 * IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. 
 * YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER THE TERMS OF THE GNU 
 * GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION; 
 * EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 * SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
 * ***********************************************************************/

#include <string>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <signal.h>
#include <sys/utsname.h>
#include <ubit/umsproto.hpp>
#include "umserver.hpp"
#include "flow.hpp"
#include "drivers/serial_mouse.hpp"
#include "drivers/mimio.hpp"
#include <X11/Xlib.h>
using namespace std;

/* ==================================================== ======== ======= */

class UMSD {
public:
  UMSD();
  ~UMSD();

  void usage();
  void parseOptions(int argc, char *argv[]);
  void start();
  static void findProcesses(bool kill);

private:
  struct Pointer {
    int ptr_no;
    const char *fg, *bg;
    Pointer() {ptr_no = 1; fg = null; bg = null;}
    Pointer(int _no, const char* _fg, const char* _bg) {
      ptr_no = _no; fg = _fg; bg = _bg;}
  };

  struct Device {
    enum Type {MOUSE, MIMIO, TACTOS} type; 
    const char* dev_alias;
    string dev_name;
    int ptr_no;
    Device(Type t) {type = t; dev_alias = null; ptr_no = 1;}
  };

  struct Neighbor {
    const char* pos;
    const char* host;
    Neighbor() {pos = null; host = null;}
  };

  // these fct can possibly do something useful (see: setActionKey())
  void key_pressed(UMServer&, XEvent& e) {
    cerr << "UMSD: action key " << e.xkey.keycode << " was pressed" << endl;
  }
  void key_released(UMServer&, XEvent& e) {
    cerr << "UMSD: action key " << e.xkey.keycode << " was released" << endl;
  }

  const char* testOption(int& k, const char* name, const char* ending = "");
  const char* getStr(int& k);
  int getUInt(int& k);

  int argc; char** argv;
  const char *display_name, *public_dir;
  int cursor_mode, neighbor_mode;
  bool test_ums, kill_ums, force_ums, emulate_buttons, reuse_address;
  int umsport;
  UMServer* ums;
  vector <Device>  idevices;
  vector <Device>  odevices;
  vector <Pointer> pointers;
  vector <Neighbor> neighbors;
};

/* ==================================================== ======== ======= */

// destructor is always called at exit (closes sockets, devices and mDNS)
// because umsd is static
static UMSD umsd;

int main(int argc, char *argv[]) {
  umsd.parseOptions(argc, argv);
  umsd.start();
  exit(0);
}

/* ==================================================== ======== ======= */

static void restart(int s) {
  cerr << "SIGHUP was received; continuing..." << endl;
  signal(s, restart);
}

static void ignore(int s) {
  signal(s, ignore);
}

static void end(int s) {
  cerr << "signal " << s << "; exiting" << endl;
  exit(s);
}

UMSD::UMSD() :
  argc(0), 
  argv(null),
  display_name(null),
  public_dir(null),
  cursor_mode(-1),
  neighbor_mode(-1),
  test_ums(false),
  kill_ums(false),
  force_ums(false),
  emulate_buttons(false),
  reuse_address(false),
  umsport(0),    // will use UMS_PORT_NUMBER
  ums(null)
{    
  // SIGHUP (kill -1) peut permettre de debloquer le serveur (si ca arrive...)
  // en sortant du select() qunad on est bloque dedans
  // NB: un timeout ne serait pas inutile

  signal(SIGHUP, restart);

  // comme umsd est une var static (et c'est pour cela que c'en est une)
  // normalement son destructeur est appele par exit() (c'est necessaire
  // pour liberer les sockets). on s'assure donc que exit() soit bien
  // appele quand il y a une erreur (autrement abort() serait appele)
  for (int s = SIGHUP+1; s < SIGTERM; s++) signal(s, end);

  // il ne faut rien faire en cas de SIGPIPE (= deconnexion abusive), 
  // c'est prevue (et surtout ne pas planter le serveur dans ce cas!)
  signal(SIGPIPE, ignore);

  //atexit(cleanup); inutile
}

UMSD::~UMSD() {
  if (ums) {
    cout << "*** UMS cleanup *** " << endl << endl;
    delete ums;    // closes devices, sockets and mDNS
  }
  ums = null;
}

/* ==================================================== ======== ======= */

void UMSD::start() {
  if (test_ums) {
    findProcesses(false);
    return;
  }
  else if (kill_ums) {
    findProcesses(true);
    return;
  }
  else if (force_ums) {
    findProcesses(true);
  }

  cout 
    << "*** UMS: Ubit Mouse/Message Server (version "
    << UNode::getVersion() << ") ***" 
    << endl;

  ums = new UMServer(display_name, umsport, reuse_address,
                     cursor_mode, neighbor_mode);
  if (!ums->isInit()) exit(1);

  // pas de public dir par defaut ce qui interdit la sauvegarde de
  // remote files sauf si init explicite au lancement de umsd
  if (public_dir && *public_dir) {
    ums->setPublicDir(public_dir);
    cout << " - Public Dir: " << ums->getPublicDir() << endl;
  }

  // =====================================================================
  // create the event flows 
  //  - several event flows can be created
  //  - their IDs MUST be != 0 (and have different values)
  //  - the native event flow (that corresponds to the standard X mouse)
  //    is created implicitely by the UMServer. Its ID is always 0.
  //  - NOTE that X clients can send events to any valid event flow
  //    (including the native X event flow)

  for (unsigned int p = 0; p < pointers.size(); p++) {
    MouseFlow* mflow = ums->getOrCreateMouseFlow(pointers[p].ptr_no);
    if (!mflow)
      cerr << "invalid pointer number: " << pointers[p].ptr_no << endl;
    else if (pointers[p].ptr_no > 0) {
      mflow->changePointer(pointers[p].fg, pointers[p].bg);
      cout << " - Pointer #" << pointers[p].ptr_no << " created" << endl;
    }
  }

  // =====================================================================
  // open the sources and connect them to the event flows
  // NB: several sources can be connected to the same event flow 
    
  for (unsigned int d = 0; d < idevices.size(); d++) {
    MouseFlow* mflow = ums->getOrCreateMouseFlow(idevices[d].ptr_no);
    if (!mflow)  {
      cerr << "invalid pointer number: " << idevices[d].ptr_no << endl;
      continue;
    }

    EventSource* so = null;

    if (idevices[d].type == Device::MIMIO) {
      idevices[d].dev_name = ums->resolveDeviceName(idevices[d].dev_alias);
      so = new MimioSource(mflow, idevices[d].dev_name.c_str());
      if (so) {
	// default mappings that can be changed here
	/*
         // attention: il n'y a que 3 Buttons avec X ; 4 & 5 servent au scroll
         so->setButton(1, MouseLeftID, 0);	        // black pen
         so->setButton(2, MouseLeftID, ShiftMask);	// blue pen
         so->setButton(3, MouseRightID, 0); 	        // green pen
         so->setButton(4, MouseLeftID, ShiftMask|ControlMask); // red pen
         so->setButton(9, MouseMiddleID, ControlMask);	// big eraser
         so->setButton(10,MouseMiddleID, 0);		// small eraser

         // boutons de controle du MIMIO (values: 10 + (1, 2, 4, 8, 16))
         so->setButton(11, MouseRightID, 0);		  // b1 (top)
         so->setButton(12, MouseRightID, ControlMask);  // b2
         so->setButton(14, MouseRightID, ShiftMask);    // b3
         so->setButton(18, MouseRightID, Mod1Mask);	  // b4
         so->setButton(26, MouseRightID, Mod2Mask);	  // b5 (bottom)
	*/
      } 
    }

    else {  //serial mouse
      idevices[d].dev_name = ums->resolveDeviceName(idevices[d].dev_alias);
      so = new MouseSource(mflow, idevices[d].dev_name.c_str());
      if (so) {
        // default mappings that can be changed here
        /*
         so->setButton(1, MouseLeftID, 0);
         so->setButton(2, MouseMiddleID, 0);
         so->setButton(3, MouseRightID, 0);
         so->setButton(4, WheelUpID, 0);
         so->setButton(5, WheelDownID, 0);
        */
      }
    }

    if (so->is_open()) {
      cout << " - Input device opened on: "<< idevices[d].dev_name << endl;
      ums->addEventSource(so);
    }
    else  {
      cerr << "!!! Can't open input device on: " << idevices[d].dev_name
      << " (alias: " << idevices[d].dev_alias << ")"<< endl;
      delete so;
      // exit(2); continuer quand meme
    }
  }

  // =====================================================================
  // emulation of mouse button 2 and 3 events (for machines that do not
  // have 3 buttons such as the IPAQ):

  if (emulate_buttons) {
    cout << " - Mouse button emulation:" << endl;

    cout << "   + Ctrl-Btn1 emulates Btn3" << endl;
    ums->setNativeButton(Button1, ControlMask, MouseRightID, 0);
    
    cout << "   + Mod1-Btn1 emulates Btn2" << endl;
    ums->setNativeButton(Button1, Mod1Mask, MouseMiddleID, 0);
 
    //cout << "   + Ctrl-Btn2 emulates ScrollUp" << endl;
    //ums->setNatButton(Button2, ControlMask, WheelUpID, 0);

    //cout << "   + Alt-Btn2 emulates ScrollDown" << endl;
    //ums->setNatButton(Button2, Mod4Mask, WheelDownID, 0);
    //ums->setNatButton(Button2, Mod1Mask, WheelDownID, 0);
  }
  
  // =====================================================================
  // pre-defined keys that perform some action (such as opening
  // menus or whatever)
  // ums->setActionKey(110, 0, key_pressed, key_released);

  // =====================================================================
  // defines the topology of UMS servers

  for (unsigned int n = 0; n < neighbors.size(); n++) {
    if (ums->setNeighborUMS(neighbors[n].pos ? *neighbors[n].pos : 0,
                            neighbors[n].host))
      
      cout << " - UMS neighbor: " 
           << neighbors[n].pos << " " << neighbors[n].host << endl;
    else
      cerr << "! invalid UMS neighbor: " 
           << neighbors[n].pos << " " << neighbors[n].host << endl;
  }

  // =====================================================================
  // start reading sources, UMS server requests and X server

  // TEST
  // UMScalibration* calib =  ums->getCalibration();
  // calib->startCalibration("MIMIO Calibration");

  ums->start();
}

/* ==================================================== ======== ======= */

void UMSD::findProcesses(bool kill) {
  if (kill) cout << "Killing UMSD processes:"<< endl;
  
  FILE* p = popen("ps axwww", "r");  // NB: pas correct sur SV
  if (!p) return;
  char line[1000];
  int myprocess = getpid();

  while (!feof(p)) {
    char* s = 0;
    if (fgets(line, sizeof(line), p) 
	&& (s = strstr(line, "umsd"))
	// pour eviter que 'umsd' soit une souspartie d'uen string plus longue
	&& (s[4]==0 || s[4]==' ' || s[4]=='\n' || s[4]=='\t' || s[4]=='\r')
	) {
      int process = 0;
      if (sscanf(line, "%d", &process) > 0
          && process > 1 && process != myprocess) {
        if (!kill)
          cout << "- process " << process << " : " << line;
        else {
          char com[100];
          sprintf(com, "kill %d", process);
          cout << " - killing process " << process << " : " << line;
          system(com);
        }
      }
    }
  }
  pclose(p);
}

/* ==================================================== [(c)Elc] ======= */

void UMSD::usage() {
  cerr
  << "Ubit Mouse/Message Server (version "
  << UNode::getVersion() << ")" << endl 
  << "Usage: umsd [options...]" << endl << endl
  << "General options: "<< endl
  << "   -h[elp]                    : prints this page" << endl
  << "   -l[ist]                    : list running umsd processes" << endl
  << "   -k[ill]                    : kill running umsd processes" << endl
  << "   -f[orce]                   : kill running processes then start a new process" << endl
  << endl
  
  << "Devices and pointers:" << endl
  << "   -mi[mio] dev ptr           : add MIMIO input on device (default ptr = 0)" << endl
  << "   -mo[use] dev ptr           : add serial mouse on device (default ptr = 1)" << endl
    //<< "   -tact dev                  : add tactile output on device" << endl
  << "   -p[ointer] ptr fgcol bgcol : set pointer colors" << endl
  << "   with: "<< endl
  << "         dev = /dev/tty... or d/s/l/k (for default/sun/linux/keyspan)" <<endl
  << "         ptr = 0 (native X pointer) or 1,2,3... (pseudo-pointers)" << endl
  << endl

  << "Configuration:" << endl
  << "   -c[ursor] y|n              : the X cursor is always shown (default on Ipaq)" << endl
  << "   -e[mulate] y|n             : enable/disable mouse button emulation" << endl
  << "   -n[neighbors] pos1 host1...: set neighbor servers (see also uremote)" << endl  
  << "                                (pos = left, right, top, down)"<< endl
  << "   -pu[blic] dir_name         : enable file dropping in this directory"<<endl
  << "                                (~/Public is used if dir_name is omitted)" << endl
  << endl

  << "Display and UMSD port:" << endl
  << "   -d[isplay] host[:screen]   : use this display" << endl
  << "   -r[euse]                   : can reuse address (using a different port)" << endl
  << "   -port port_number          : use this port instead of "<<UMS_PORT_NUMBER<< endl
  << endl

  << "See also:" <<endl
  << "   uremote                    : a tool that configures umsd interactively"<<endl
  << "   kill -1 umsd_pid           : unblocks umsd (if this happens...)" <<endl
  <<endl;
}

/* ==================================================== [(c)Elc] ======= */

void UMSD::parseOptions(int _argc, char *_argv[]) {
  argc = _argc;
  argv = _argv;

  // creates 3 pointers by default
  Pointer p1(1, "red", null);
  pointers.push_back(p1);

  Pointer p2(2, "green", null);
  pointers.push_back(p2);

  Pointer p3(3, "blue", null);
  pointers.push_back(p3);
  
  int last_default_ptr = 3;

  int k = 1;
  while (k < argc) { 

    if (testOption(k,"-p","tr") || testOption(k,"-p","ointer")) {
        Pointer p;
        p.ptr_no = getUInt(k);
        if (p.ptr_no <= 0) p.ptr_no = 1;  //defaut
        p.fg = getStr(k);
        p.bg = getStr(k);
        if (p.ptr_no <= last_default_ptr) {
          cerr << "pointer " << p.ptr_no << " exists already; No changed to "
          << p.ptr_no + last_default_ptr << endl;
          p.ptr_no += last_default_ptr;
        }
        pointers.push_back(p);
    }

    else if (testOption(k,"-mo","use")) {
      Device d(Device::MOUSE);
      d.dev_alias = "";
      d.ptr_no = 1;  // default is 1
      if (k < argc) {
        d.dev_alias = getStr(k);
  	d.ptr_no = getUInt(k); if (d.ptr_no < 0) d.ptr_no = 1;
      }
      idevices.push_back(d);
    }

    else if (testOption(k,"-mi","mio")) {
      Device d(Device::MIMIO);
      d.dev_alias = "";
      d.ptr_no = 0;  // default is 0
      if (k < argc) {
        d.dev_alias = getStr(k);
	d.ptr_no = getUInt(k); if (d.ptr_no < 0) d.ptr_no = 0;
      }
      idevices.push_back(d);
    }

    else if (testOption(k,"-tact","")) {
      if (k < argc) {
        Device d(Device::TACTOS);
        d.dev_alias = getStr(k);
        odevices.push_back(d);
      }
      else {cerr << "usage: -tact device" << endl; exit(1);}
    }

    else if (testOption(k,"-n","eighbors") || testOption(k,"-topo","logy")) {
      if (k < argc) {
        while(true) {
          Neighbor n;
	  n.pos  = getStr(k);
	  n.host = getStr(k);
          if (n.pos) neighbors.push_back(n);
          else break;
        } 
      }
      else {cerr << "usage: -neighbors pos1 host1 [pos2 host2] ..." << endl; exit(1);}
    }

    else if (testOption(k,"-d","isplay")) {
      if (k < argc) display_name = getStr(k);
      else {cerr << "usage: -display host[:screen]" << endl; exit(1);}
    }

    else if (testOption(k,"-port","")) {
      if (k < argc) umsport = getUInt(k);
      else {cerr << "usage : -port postnumber" << endl; exit(1);}
    }

    else if (testOption(k,"-r","euse")) {
      reuse_address = true;
    }
    
    else if (testOption(k,"-k","ill")) {
      kill_ums = true;
    }

    else if (testOption(k,"-f","orce")) {
      force_ums = true;
    }

    else if (testOption(k,"-l","ist")) {
      test_ums = true;
    }

    else if (testOption(k,"-pu","blic")) {
      if (k < argc) public_dir = getStr(k);
      else public_dir = "~/Public";  // default public dir
      //else {cerr << "usage: -public dirname" << endl; exit(1);}
    }
    
    else if (testOption(k,"-e","mulate")) {
      emulate_buttons = true;
      if (k < argc) {
	const char* s = getStr(k);
	if (!s || *s==0 || *s=='y'|| *s=='Y' || *s=='1')
	  emulate_buttons = true; else emulate_buttons = false;
      }
    }

    else if (testOption(k,"-c","ursor")) {
      cursor_mode = 1;
      if (k < argc) {
	const char* s = getStr(k);
	if (!s || *s==0 || *s=='y'|| *s=='Y' || *s=='1')
	  cursor_mode = true; else cursor_mode = false;
      }
    }

    //else if (testOption(k,"-grab","win")) {
    //  if (k < argc) neighbor_mode = getUInt(k);
    //  else {cerr << "usage: -grabwin 1|0" << endl; exit(1);}
    //}

    else if (testOption(k,"-h","elp") || testOption(k,"--h","elp")) {
      usage();
      exit(0);
    }

    else {
      cerr << "!!! unrecognized option (umsd -list to see valid options)" << endl;
      //usage();
      exit(1);
    }
  }
}

/* ==================================================== ======== ======= */

const char* UMSD::testOption(int& k, const char* name, const char* ending) {
  const char* pa = argv[k];
  const char* pn;

  for (pn = name; *pn != 0; pn++) {
    if (*pa == *pn) pa++;
    else return null;
  }
  for (pn = ending; *pn != 0; pn++) {
    if (*pa == *pn) pa++;
    else if (*pa == 0) {k++; return pa;}
    else return null;
  }

  k++; return pa;
}

int UMSD::getUInt(int& k) {
  if (k < argc && argv[k][0] >= '0' && argv[k][0] <= '9') {
    return atoi(argv[k++]);
  }
  return -1;   // invalid
}

const char* UMSD::getStr(int& k) {
  if (k < argc && argv[k][0] != '-') {
    return argv[k++];
  }
  return null;  // default
}

/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */
