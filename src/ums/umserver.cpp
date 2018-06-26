/************************************************************************
 *
 *  umserver.cpp: UMS Server: Ubit Multi-Mouse Message Server
 *  provides multiple event flows, pseudo pointers, two-handed interaction,
 *  RendezVous identification, remote control facilities, etc.
 *
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2008 Eric Lecolinet / ENST Paris / www.enst.fr/~elc/ubit
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

#include <algorithm>
#include <string.h>
#include <cstdlib>       // getenv, atexit
#include <cstdio>
#include <unistd.h>       // darwin
#include <ctype.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <netinet/in.h>
#include <sys/socket.h>
//include <ubit/ubit_features.h>
//EX: include <ubit/ubit_config.h>
#include <ubit/umsproto.hpp>
#include <ubit/umservice.hpp>
#include "umserver.hpp"
#include "source.hpp"
#include "flow.hpp"
#include "events.hpp"
#include "zeroconf.hpp"
#include "calib.hpp"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
using namespace std;

// default serial ports
static const char* SUNOS_SERIAL_PORT     = "/dev/ttya";
static const char* LINUX_SERIAL_PORT     = "/dev/ttyS0";   // (= COM1)
static const char* LINUX_USB2SERIAL_PORT = "/dev/ttyUSB0";
static const char* KEYSPAN_SERIAL_PORT   = "/dev/tty.USA*";

const Cursor ROOT_CURSOR_SHAPE = XC_left_ptr;
const char*  WHERE_WIN_COLOR = "orange";
 
// retrieves pointer if one clicks MULTI_CLICK_ESCAPE times on a where_win
const int MULTI_CLICK_ESCAPE = 6;
const unsigned long MULTI_CLICK_DELAY = 300;  //millisec

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */

string UMServer::resolveDeviceName(const char* name) {
  char cas = (name && *name) ? *name : 'd';  // 'd' for default

  if (cas == 'd') {
    if (system == "SunOS") cas = 's';
    else if (system == "Linux") cas = 'l';
    else if (system == "Cygwin") cas = 'l';
    else if (system == "Darwin") cas = 'k';
    else cerr << "!!! No default input device for system " << system << endl;
  }

  switch (cas) {
    case 's': return SUNOS_SERIAL_PORT;
    case 'l': return LINUX_SERIAL_PORT;
    case 'u': 
    case 'k': 
      if (system == "Linux") return LINUX_USB2SERIAL_PORT;
      else {
        char com[100] = "";
        sprintf(com, "ls %s", KEYSPAN_SERIAL_PORT);
        
        FILE* p = popen(com, "r");
        char dev[250] = "";
        int n = 0;
        if (p) {
          n = fscanf(p, "%s", dev);
          pclose(p);
        }
        if (n <= 0 || dev[0]==0) {
          cerr << "!!! Can't find device " << KEYSPAN_SERIAL_PORT << endl;
          dev[0] = 0;
        }
        return dev;
      }
        break;
      
  default: return name;  // unchanged
  }
}

/* ==================================================== ======== ======= */

void UMServer::setPublicDir(const char* s) {
  if (!s || !*s) {
    public_dir = ""; // interdire utilisation
  }
  else {
    if (s[0] != '~') public_dir = s;
    else {
      static char *home = getenv("HOME");
      if (home) {public_dir = home; public_dir += (s+1);}
      else public_dir = s;
    }
    if (public_dir[public_dir.length()-1] != '/')
      public_dir.append("/");
  }
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */
// POS

Pos::Pos() {
  rx = ry = 0;
  wx = wy = 0;
  win = None;
  subwin = None;
  in_ubitwin = false;
  win_sock = -1;
}

Pos::Pos(int _rx, int _ry, int _wx, int _wy, Window _win,
               bool _in_ubitwin, int _win_sock) {
  rx = _rx, ry = _ry;
  wx = _wx, wy = _wy;
  win = _win;
  subwin = None;
  in_ubitwin = _in_ubitwin;
  win_sock = _win_sock;
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */
// CNX

Cnx::Cnx(USocket* s) {
  sock = s; 
  //app_name = _app_name;
  browse_servers = browse_neigbors = browse_windows = false;
}

Cnx::~Cnx() {
  delete sock;
  // if (sock > 0) {
  //  ::shutdown(sock, 2);
  //  ::close(sock);
  //}
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */

UMServer::~UMServer() {
  delete mdns;
  delete &events;
  for (unsigned int k = 0; k < sources.size(); k++) delete sources[k];
  for (unsigned int k = 0; k < eflows.size(); k++) delete eflows[k];
  for (CnxList::iterator c = cnxs.begin(); c != cnxs.end(); c++) delete *c;
}

/* ==================================================== ======== ======= */

UMServer::UMServer(const char* display_name, int port, bool reuse_address,
                   int cursor_mode, int neighbor_mode) :
is_init(false),
events(*new Events(this)),
edges(null),
calib(null),
mdns(null),
natsource(*new EventSource()) {

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // options

  struct utsname unm;
  if (uname(&unm) < 0) {
    cerr << "UMServer::UMServer: uname failed: " << endl;
    hostname = "localhost";
    system = ""; 
  }
  else {
    char* p = strchr(unm.nodename, '.'); if (p) *p = 0;
    hostname = unm.nodename;
    system = unm.sysname;
  }
  //cout << " - System: [" << system <<"]" << endl;
  
  if (strncmp(unm.machine, "arm", 3) == 0 && cursor_mode == -1)
    cursor_mode = 1;
#ifdef MACOSX
  if (neighbor_mode == -1) 
    neighbor_mode = 1;
#endif

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  // UMS server initialization

  if (port == 0) port = UMS_PORT_NUMBER;
  serv_sock = new UServerSocket();

  // bind est lance en mode reuse_address et on verifie a posteriori que
  // le port est celui qu'on souhaite. ca permet, de reutilser 9666 dans
  // le cas ou il n'y a plus de serveur mais toujours des applis connectees
  if (!serv_sock->bind(port, 0, true)) {
    delete serv_sock;
    serv_sock = null;
  }
  
  if (serv_sock->getLocalPort() != port) {
    if (reuse_address)
      cerr << " ! UMServer: port " << port << " is busy; using a different port: "
        << serv_sock->getLocalPort()  << endl;
    else  {
      delete serv_sock;
      serv_sock = null;
    }    
  }

  if (!serv_sock) {
    cerr
      << "!!! Can't open socket on port "<< UMS_PORT_NUMBER
      << ": another UMS server is probably already running"  << endl;
    cerr
      << "    umsd -l   lists all running servers" << endl
      << "    umsd -k   kills them" << endl
      << "    umsd -f   kills running servers and starts a new one" << endl
      << "    umsd -r   starts a new server on another (random) port" << endl
      << "    umsd -h   shows all options" << endl;
    return;
  }
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // create the native mouse flow (doit etre avant initX car cette fct envoie
  // des messages aux windows)

  // eflows[0] corresponds to the standard X server (its id must be 0)
  eflows.push_back(new MouseFlow(this, /*flow_id*/0, /*no pointer*/false));
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  // open and init the X server

  const char* darg, *dname;
  if (display_name) {
    darg = "-display ";
    dname = display_name;
  }
  else {
    darg = "$DISPLAY= ";
    dname = ::getenv("DISPLAY");
    if (!dname) dname = "UNDEFINED";
  }

  cout << " - Display: " << darg << dname << endl;
  cout << " - UMS port: " << serv_sock->getLocalPort() << endl;

  if (!initX(display_name, cursor_mode, neighbor_mode)) {
    cerr << "UMServer: can't open X server on display: "<< dname << endl;
    return;
  }
  
  l_neighbor = r_neighbor = t_neighbor = b_neighbor = null;
  events.init();
  initMainLoop();
  is_init = true;
}

/* ==================================================== ======== ======= */

void UMServer::start() {
  if (!is_init) {
    cerr << "UMServer::start: UMS not initialized; can't start" << endl;
    return;
  }

  //NB: doit etre apres initMainLoop
  mdns = new Mdns(this, hostname.c_str());
  runMainLoop();
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */

static int xerror;

static int quietXErrorHandler(Display*, XErrorEvent *xer) {
  xerror = xer->error_code;
  return 0;
}

long UMServer::getScreenWidth() const {
  return WidthOfScreen((Screen*)xscreen);
}

long UMServer::getScreenHeight() const {
  return HeightOfScreen((Screen*)xscreen);
}

int UMServer::getScreenDepth() const {
  return DefaultDepthOfScreen((Screen*)xscreen);
}

bool UMServer::initX(const char* display_name,
                     int cursor_mode, int neighbor_mode) {
  xdisplay = XOpenDisplay(display_name);
  if (!xdisplay) return false;

  Screen* scr = DefaultScreenOfDisplay(xdisplay);
  xconnection   = XConnectionNumber(xdisplay);
  xscreen       = (XScreen*)scr;
  xrootwin      = RootWindowOfScreen(scr);
  root_cursor   = None;
  _WM_DELETE_WINDOW = XInternAtom(xdisplay, "WM_DELETE_WINDOW", False);
  _UMS_WINDOW   = XInternAtom(xdisplay, UMS_WINDOW_PROPERTY, False);
  _UMS_MESSAGE  = XInternAtom(xdisplay, UMS_MESSAGE_PROPERTY, False);
  grab_cursor   = None;
  
  // we don't want stupid X errors to crash the UMS server
  XSetErrorHandler(quietXErrorHandler);

  // imposer root_cursor pour chaque win si cursor_shape >0 
  // (c'est necessaire pour l'IPAQ qui n'affiche pas de cursor)
  if (cursor_mode > 0) {
    cerr << " - UMS Cursor created" << endl;
    root_cursor = XCreateFontCursor(xdisplay, ROOT_CURSOR_SHAPE);
    if (root_cursor != None) XDefineCursor(xdisplay, xrootwin, root_cursor);
  }
  
  // this will make it possible to detect window creations & deletions,
  // and window (un)mapping, moving and stacking operations
  XSelectInput(xdisplay, xrootwin, SubstructureNotifyMask | ResizeRedirectMask);
  
  // creates the edges of the screen for detecting when the pointer reach the borders
  edges = new Edges(this, neighbor_mode);

  // create the calibration window
  // calib = new UMScalibration(this); // cree a la demande

  // retrieves the windows that have been created before the UMS is launched
  retrieveXWindows();
  return true;
}

void UMServer::moveNativePointerBackHome(bool center) {
  if (edges) edges->moveNativePointerBackHome(center);
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */

ActionKey::ActionKey(unsigned int _keycode, int _modifier,
                           void (*_press_action)(class UMServer&, XEvent&),
                           void (*_release_action)(class UMServer&, XEvent&)) {
  keycode  = _keycode;
  modifier = _modifier;
  press_action = _press_action;
  release_action = _release_action;
}

void UMServer::setActionKey(unsigned int keycode, int modifier,
                            void (*press_action)(class UMServer&, XEvent&),
                            void (*release_action)(class UMServer&, XEvent&)) {
  if (keycode != 0) {
    ActionKey akey(keycode, modifier, press_action, release_action);

    action_keys.push_back(akey);

    XGrabKey(xdisplay, keycode, modifier, xrootwin, True,
             GrabModeAsync, GrabModeAsync);
  }
}

void UMServer::setNativeButton(int btn_number, unsigned int mod_mask,
			       unsigned int to_btn_mask, unsigned int to_mod_mask) {

  natsource.setButton(btn_number, mod_mask, to_btn_mask, to_mod_mask);
  XGrabButton(xdisplay, btn_number, mod_mask,
              xrootwin, True,  // owner_events
              ButtonPressMask | ButtonReleaseMask | ButtonMotionMask,
              GrabModeAsync, GrabModeAsync, None, /*Cursor*/None);
}


/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */
// EVENT FLOWS

bool UMServer::addEventFlow(EventFlow* fl) {
  if (fl->getID() == 0) {
    cerr << "UMServer::addEventFlow: invalid ID (ID = 0 is reserved for the native X event flow)" << endl;
    return false;
  }

  if (getEventFlow(fl->getID())) {
    cerr << "UMServer::addEventFlow: invalid ID (an event flow with the same ID is already registered)" << endl;
    return false;
  }

  eflows.push_back(fl);
  return true;
}

/* ==================================================== ======== ======= */

bool UMServer::removeEventFlow(EventFlow* fl) {
  if (fl->getID() == 0) {
    cerr << "UMServer::removeEventFlow: invalid ID (ID = 0 is reserved for the native X event flow and can't be removed)" << endl;
    return false;
  }

  unsigned int sz = eflows.size();
  eflows.erase( remove(eflows.begin(), eflows.end(), fl), eflows.end());

  if (sz == eflows.size()) {
    cerr << "UMServer::removeEventFlow: flow not found" << endl;
    return false;
  }
  else return true;
}

/* ==================================================== ======== ======= */

EventFlow* UMServer::getEventFlow(int id) const {
  if (id == 0) return eflows[0];

  for (unsigned int k = 0; k < eflows.size(); k++) {
    if (eflows[k]->getID() == id) return eflows[k];
  }
  return null;  // not found
}

MouseFlow& UMServer::getNativeMouseFlow() const {
  return *(MouseFlow*)eflows[0];
}

MouseFlow* UMServer::getMouseFlow(int id) const {
  if (id == 0) return (MouseFlow*)eflows[0];
  
  EventFlow* f = getEventFlow(id);
  return f ? dynamic_cast<MouseFlow*>(f) : null;
}

MouseFlow* UMServer::getOrCreateMouseFlow(int id) {
  EventFlow* eflow = getEventFlow(id);
  if (eflow) {
    return dynamic_cast<MouseFlow*>(eflow);
  }
  else {
    MouseFlow* mflow = new MouseFlow(this, id, true);
    if (addEventFlow(mflow)) return mflow;
    else {
      delete mflow;
      return null;
    }
  }
}

/* ==================================================== ======== ======= */

Cnx* UMServer::addCnx(USocket* s) {
  CnxList::iterator k = findCnxIt(s);
  if (k != cnxs.end()) return *k;
  // not in the list
  Cnx* c = new Cnx(s);
  addSourceToMainLoop(s);
  cnxs.push_back(c);
  return c;
}

void UMServer::removeCnx(USocket* s) {
  CnxList::iterator k = findCnxIt(s);
  if (k != cnxs.end()) {       // in the list
    delete *k;
    cnxs.erase(k);
  }
}

CnxList::iterator UMServer::findCnxIt(USocket* s) {
  for (CnxList::iterator k = cnxs.begin(); k != cnxs.end(); k++) {
    if ((*k)->sock == s) return k;
  }
  return cnxs.end();
}

Cnx* UMServer::findCnx(USocket* s) {
  CnxList::iterator k = findCnxIt(s);
  return (k == cnxs.end()) ? null : *k;
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */
// EDGES

#define ScreenP(ums) ((Screen*)(ums.xscreen))
          
Edges::Edges(UMServer* _ums, int neighbor_mode) :
ums(*_ums), is_outside(false),
x_crossings(0), y_crossings(0), x_delta(0), y_delta(0)
{
  Display* disp    = ums.getDisplay();
  Window root      = ums.getRoot();
  long root_width  = ums.getScreenWidth();
  long root_height = ums.getScreenHeight();
  XColor color, exact_color;

  { // Cursor that is shown when the pointer is inside a neighbor
    if (!XAllocNamedColor(disp, DefaultColormapOfScreen(ScreenP(ums)),
                          WHERE_WIN_COLOR, &color, &exact_color))
      color.pixel = BlackPixelOfScreen(ScreenP(ums));

    // on cree un mini pointer (on pourrait aussi creer un pointer completement
    // invisible mais ca peut servir pour le debug)
    char pix_bits[] = {0x03, 0x03, 0x03};
    //{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,0xff, 0xff, 0xff,
    //0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    Pixmap pix = XCreateBitmapFromData(disp, root, pix_bits, 2, 2);
    ums.grab_cursor = XCreatePixmapCursor(disp, pix, pix, &color, &color, 0, 0);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  XSetWindowAttributes attr;
  unsigned long valuemask;

  attr.event_mask = EnterWindowMask | LeaveWindowMask | VisibilityChangeMask;
  attr.override_redirect = True;
  valuemask = CWEventMask | CWOverrideRedirect;

  left_edge = XCreateWindow(disp, root,
                            0, EDGE_THICKNESS,
                            EDGE_THICKNESS, root_height - 2*EDGE_THICKNESS,
                            0, // no border
                            CopyFromParent,  // depth
                            InputOnly, // !! InputOnly class !!
                            CopyFromParent,
                            valuemask, &attr);

  top_edge = XCreateWindow(disp, root,
                           EDGE_THICKNESS, 0,
                           root_width - 2*EDGE_THICKNESS, EDGE_THICKNESS,
                           0, CopyFromParent, InputOnly, CopyFromParent,
                           valuemask, &attr);

  right_edge = XCreateWindow(disp, root,
                             root_width - EDGE_THICKNESS, EDGE_THICKNESS,
                             EDGE_THICKNESS, root_height - 2*EDGE_THICKNESS,
                             0, CopyFromParent, InputOnly, CopyFromParent,
                             valuemask, &attr);

  bottom_edge = XCreateWindow(disp, root,
                              EDGE_THICKNESS, root_height - EDGE_THICKNESS,
                              root_width - 2*EDGE_THICKNESS, EDGE_THICKNESS,
                              0, CopyFromParent, InputOnly, CopyFromParent,
                              valuemask, &attr);

  XMapRaised(disp, left_edge);
  XMapRaised(disp, top_edge);
  XMapRaised(disp, right_edge);
  XMapRaised(disp, bottom_edge);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  attr.override_redirect = True;
  attr.background_pixel = BlackPixelOfScreen(ScreenP(ums));
  if (XAllocNamedColor(disp, DefaultColormapOfScreen(ScreenP(ums)),
                       WHERE_WIN_COLOR, &color, &exact_color)) {
    attr.background_pixel = color.pixel;
  }
  valuemask = CWBackPixel | CWOverrideRedirect;
  
  h_where_win = XCreateWindow(disp, root, 0, 0, 50, 50, 0, // no border
                              CopyFromParent, InputOutput, CopyFromParent,
                              valuemask, &attr);

  v_where_win = XCreateWindow(disp, root, 0, 0, 50, 50, 0, // no border
                              CopyFromParent, InputOutput, CopyFromParent,
                              valuemask, &attr);
}

/* ==================================================== ======== ======= */

bool UMServer::isPointer(WindowID win) {
  for (unsigned int f = 0; f < eflows.size(); f++) {
    if (eflows[f]->getPointer() == win) return true;
  }
  return false;
}

void Edges::raiseEdges() {
  Display* disp = ums.getDisplay();
  XRaiseWindow(disp, left_edge);
  XRaiseWindow(disp, top_edge);
  XRaiseWindow(disp, right_edge);
  XRaiseWindow(disp, bottom_edge);
  XFlush(disp);
}

bool Edges::isEdge(Window w) {
  return (w == left_edge || w == top_edge || w == right_edge || w == bottom_edge);
}

void Edges::moveNativePointerBackHome(bool center) {
  Display* disp = ums.getDisplay();
  ums.getMouseFlow(0)->releaseAll();

  //XMoveResizeWindow(disp, v_where_win, 50, 50, 200, 200);
  XUnmapWindow(disp, h_where_win);
  XUnmapWindow(disp, v_where_win);
  XFlush(disp);
  
  XUngrabPointer(disp, CurrentTime);
  XUngrabKeyboard(disp, CurrentTime);
  is_outside = false;
  x_crossings = 0;
  y_crossings = 0;
  x_delta = 0;
  y_delta = 0;

  ums.events.enterLocalDisplay();

  if (center) ums.events.warpPointer(ums.getNativeMouseFlow(), 
                                     ums.getScreenWidth()/2,
                                     ums.getScreenHeight()/2);
}

/* ==================================================== ======== ======= */
// NOTE: le truc c'est que lorsque l'on arrive a un bord on fait un WarpPointer
// sur l'autre bord +/- EDGE_THICKNESS. Ce decalage fait qu'on saute l'edge
// de l'autre bord et donc qu'on ne genere pas un EnterNotify qui ne manquerait
// pas de poser probleme. De plus, si on revient en arriere on est oblige de
// traveser cet edge et donc de generer un EnterNotify. De de fait ca marche
// a tous les coups et on n'a pas besoin de considerer les LeaveNotify

void Edges::enterEdge(Window win, int ev_x, int ev_y) {
#ifdef MACOSX
  if (is_outside) return;  // gere dans la grabwin de MacEvents
#endif

  int x_warp = ev_x, y_warp = ev_y;

  Display* disp = ums.getDisplay();
  int x_cross = 0, y_cross = 0;
    
  if (win == right_edge) {
    x_cross = x_crossings +1;
    x_warp = EDGE_THICKNESS;  // att: 1 eviter de tomber sur left_edget!
  }
  else if (win == left_edge) {
    x_cross = x_crossings -1;
    x_warp = ums.getScreenWidth() - EDGE_THICKNESS-1;
  }
  else if (win == top_edge) {
    y_cross = y_crossings -1;
    y_warp = ums.getScreenHeight() - EDGE_THICKNESS-1;
  }
  else if (win == bottom_edge) {
    y_cross = y_crossings +1;
    y_warp = EDGE_THICKNESS;
  }
  else {  // sur les corners
    return;
  }

  long x = x_cross * ums.getScreenWidth();
  long y = y_cross * ums.getScreenHeight();
  long n_x = 0, n_y = 0;
  UMService* n_client = null;

  ums.getMouseFlow(0)->isOutside(x, y, n_x, n_y, n_client);

  // back to home screen
  if (x_cross == 0 && y_cross == 0) {
    // mettre le pointeur sur le bord oppose
    if (is_outside) XWarpPointer(disp, ums.getRoot(), ums.getRoot(),
                                 0, 0, 0, 0, x_warp, y_warp);
    moveNativePointerBackHome(false);
    return;
  }

  // pas de client a cet endroit: ne pas entrer dans cette zone
  if (!n_client) return;

  // tout est OK: sauver les coords
  x_crossings = x_cross;
  y_crossings = y_cross;
  x_delta = x;
  y_delta = y;
  
#ifdef MACOSX
  if (!is_outside) {
    ums.getNativeMouseFlow().syncMouse(ev_x, ev_y);
    ums.events.leaveLocalDisplay();
    is_outside = true;    
  }
#else
  if (!is_outside) {
    // empeche les applis de recevoir les events
    int stat = 0;
    stat = XGrabPointer(disp, ums.getRoot(), True,   // owner_events=True
                        PointerMotionMask| ButtonPressMask | ButtonReleaseMask
                        | EnterWindowMask | LeaveWindowMask | VisibilityChangeMask,
                        GrabModeAsync, GrabModeAsync, None/*confineto*/,
                        ums.grab_cursor, CurrentTime);
    if (stat != GrabSuccess) {
      cerr << "UMServer: Can't grab the mouse" << endl;
      x_crossings = y_crossings = 0;
      x_delta = y_delta = 0;
      return;  // ne pas faire la suite
    }

    stat = XGrabKeyboard(disp, ums.getRoot(), True,   // owner_events=True
                         GrabModeAsync, GrabModeAsync, CurrentTime);
    if (stat != GrabSuccess) {
      cerr << "UMServer: Can't grab the keyboard" << endl;
    }
    
    //if (ums.grabwin != None) {
    // XMapRaised(disp, ums.grabwin);
    //ums.raiseXWindows();
    //}
    ums.events.leaveLocalDisplay();
    //necessaire car leaveLocalDisplay peut avoir ouvert une grabwin
    //raiseEdges();
    is_outside = true;
    
    // mettre le pointeur sur le bord oppose
    XWarpPointer(disp, ums.getRoot(), ums.getRoot(), 0,0,0,0, x_warp, y_warp);
  }
#endif

  { // placer les where_win
    int xx, yy;
    unsigned int ww, hh;
  
    if (x_crossings < 0) {
      ww = 35; hh = 300;
      xx = 0;
      yy = (ums.getScreenHeight()-hh) / 2;
      XMoveResizeWindow(disp, h_where_win, xx, yy, ww, hh);
      XMapRaised(disp, h_where_win);
    }
    else if (x_crossings > 0) {
      ww = 35; hh = 300;
      xx = ums.getScreenWidth()-ww;
      yy = (ums.getScreenHeight()-hh) / 2;
      XMoveResizeWindow(disp, h_where_win, xx, yy, ww, hh);
      XMapRaised(disp, h_where_win);
    }
    else {
      XUnmapWindow(disp, h_where_win);
    }
    
    if (y_crossings < 0) {
      ww = 400; hh = 35;
      xx = (ums.getScreenWidth()-ww) / 2;
      yy = 0;
      XMoveResizeWindow(disp, v_where_win, xx, yy, ww, hh);
      XMapRaised(disp, v_where_win);
    }
    else if (y_crossings > 0) {
      ww = 400; hh = 35;
      xx = (ums.getScreenWidth()-ww) / 2;
      yy = ums.getScreenHeight()-hh;
      XMoveResizeWindow(disp, v_where_win, xx, yy, ww, hh);
      XMapRaised(disp, v_where_win);
    }
    else {
      XUnmapWindow(disp, v_where_win);
    }
  }
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */

void UMServer::getAndProcessXEvents() {
  XEvent e;

  while (XPending(xdisplay)) {
    XNextEvent(xdisplay, &e);

  if (calib && calib->is_calibrating) {
    switch (e.type) {
    case ConfigureNotify:
      // if the calwin is moved or resize
      if (e.xconfigure.window == calib->calwin) {
        calib->configureCalibrationWin(e.xconfigure.x, e.xconfigure.y,
                                       e.xconfigure.width, e.xconfigure.height);
        return;
      }
     break;

    case Expose:
      // refreshes the calibration window (use for absolute positioning
      // devices such as the MIMIO)
      if (e.xexpose.window == calib->calwin) {
        calib->drawCalibrationPoint();
        return;
      }
     break;

    case ButtonPress:
      if (e.xbutton.window == calib->calwin) {
	if (e.xbutton.button == 1)
	  calib->setCalibrationPoint(e.xbutton.x, e.xbutton.y);
	else
	  calib->unsetLastCalibrationPoint();
        return;
      }
     break;

    case MotionNotify:
      if (e.xbutton.window == calib->calwin) {
	// on prend en compte des motion events (bien qu'on ne fasse rien)
	// pour eviter cas ou l'affichage se bloque
	return;
      }
     break;
    }
  }  //endif (calibrating)

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  switch (e.type) {
    
  case EnterNotify:
    if (edges->isEdge(e.xcrossing.window)) {
      edges->enterEdge(e.xcrossing.window, e.xcrossing.x_root, e.xcrossing.y_root);
    }
    break;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // changes the events generated by the native mouses
    // (typically to emulation of mouse button 2 and 3 events for machines
    //  that do nothave 3 buttons such as the IPAQ):
    // Notes:
    // - concerns ButtonPress, ButtonRelease and MotionNotify

  case MotionNotify:
#ifndef MACOSX
    if (edges->is_outside) {   // remote control
      getMouseFlow(0)->moveMouseOutsideDisplay(e.xmotion.x_root + edges->x_delta,
                                               e.xmotion.y_root + edges->y_delta);
    }
    else
#endif
    { // button emulation
      // NB: comme c'est lie a un grabButton, ne peut intervenir
      // que si pseudo_button 2 ou 3_modifier pressed
      if (e.xmotion.window == xrootwin)
        getMouseFlow(0)->syncMouse(e.xmotion.x, e.xmotion.y);
    }
    return;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
  case ButtonPress:
#ifndef MACOSX
    if (edges->is_outside) {    // remote control
    
      // MOVE_BACK si on clique A LA FOIS sur Button1 et Button3
      if ((e.xbutton.button == 1 && (e.xbutton.state & Button3Mask))
          || (e.xbutton.button == 3 && (e.xbutton.state & Button1Mask)))
        moveNativePointerBackHome(true);
      
      else {
        // MOVE_BACK if one clicks MULTI_CLICK_ESCAPE times on a where_win
        // subwindow pas window, a cause du X grab
        if (e.xbutton.subwindow == edges->h_where_win
            || e.xbutton.subwindow == edges->v_where_win) {
          static unsigned long last_click_time = 0;
          static int click_count = 0;
          
          if (click_count == 0 || e.xbutton.time-last_click_time < MULTI_CLICK_DELAY)
            click_count++;
          else click_count = 0;
          
          if (click_count >= MULTI_CLICK_ESCAPE) moveNativePointerBackHome(true);
          last_click_time = e.xbutton.time;
        }

        getMouseFlow(0)->pressMouse(Events::xbuttonToMask(e.xbutton.button));
      }
    }  // end remote control
    else 
#endif
    {     // button emulation
      if (e.xbutton.window == xrootwin) {
        UMSbutton* b = natsource.getButton(e.xbutton.button, e.xbutton.state);
        
        // marche pas sur NativeMouseFlow a cause du XTestFakeButtonEvent 
        // qui prend en compte les modifieurs appuyes => pas le bon event!
        // => prendre flow 1
        if (b) {
          MouseFlow* f = getMouseFlow(1);
          Pos pos;
          if (f && events.getXPointer(pos)) 
            events.sendButton(*f, b->out_btn_mask, true, &pos);
        }
      }
    }
    return;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
  case ButtonRelease:
#ifndef MACOSX
    if (edges->is_outside) {   // remote control
      getMouseFlow(0)->releaseMouse(Events::xbuttonToMask(e.xbutton.button));
    }
    else
#endif
    {    // button emulation
      if (e.xbutton.window == xrootwin) {
        UMSbutton* b = natsource.getButton(e.xbutton.button, e.xbutton.state);
        if (b) {
          // voir remarque plus haut sur flow 1
          MouseFlow* f = getMouseFlow(1);
          Pos pos;
          if (f && events.getXPointer(pos)) 
            events.sendButton(*f, b->out_btn_mask, false, &pos);
        }
      }
    }
    return;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // pre-defined keys that perform some action action (such as opening
    // menus or whatever)

  case KeyPress:
#ifndef MACOSX
    if (edges->is_outside) {
      KeySym keysym = 0;
      char buf[1] = "";
      XLookupString(&(e.xkey), buf, sizeof(buf), &keysym, NULL); 
      if (keysym != 0) getMouseFlow(0)->pressKey(keysym /*, e.xkey.state*/);
    }
    else
#endif
    {  // key emulation
      for (unsigned int k = 0; k < action_keys.size(); k++) {
        if (action_keys[k].keycode == e.xkey.keycode
            && (action_keys[k].modifier == 0 || action_keys[k].modifier & e.xkey.state)
            && action_keys[k].press_action)
          (action_keys[k].press_action)(*this, e);
      }
    }
    return;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  case KeyRelease:
#ifndef MACOSX
    if (edges->is_outside) {
      KeySym keysym = 0;
      char buf[1] = "";
      XLookupString(&(e.xkey), buf, sizeof(buf), &keysym, NULL); 
      if (keysym != 0) getMouseFlow(0)->releaseKey(keysym);
    }
    else 
#endif
    { // key emulation
      for (unsigned int k = 0; k < action_keys.size(); k++) {
        if (action_keys[k].keycode == e.xkey.keycode
            && (action_keys[k].modifier == 0 || action_keys[k].modifier & e.xkey.state)
            && action_keys[k].release_action)
          (action_keys[k].release_action)(*this, e);
      }
    }
    return;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // window management:
    // - ConfigureNotify : stacking order
    // - MapNotify/UnmapNotify : updates the list of visible windows

  case ConfigureNotify: {  // (among other things) stacking order is changed
    //cerr <<"ConfigureNotify " << e.xconfigure.window << endl;

    // chercher si window et above sont dans la liste
    WinList::iterator window = wins.end();
    WinList::iterator above = wins.end();

    for (WinList::iterator k = wins.begin(); k != wins.end(); k++) {
      if ((*k)->win == e.xconfigure.window) window = k;
      if ((*k)->win == e.xconfigure.above)  above = k;
    }
    if (window == wins.end()) return;  // not found

    if (e.xconfigure.above == None) {
      // window est au debut de rien => deplacer en debut de liste
      reorderWin(window, wins.begin());
    }
    else {
      if (above == wins.end()) return;  // not found
      // window est au dessus de above => deplacer apres above
      reorderWin(window, ++above);   // deplacer *apres* above
    }

    // remettre les edges au premier plan
    if (edges && !edges->isEdge(e.xconfigure.window)) edges->raiseEdges();
  } return;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
  case MapNotify:       // a window is raised
    // mettre en fin de liste : par dessus les autres windows
    // att: ne pas mettre les pointer wins dans la liste !
    
    if (!isPointer(e.xmap.window)) {
      //cerr <<"MapNotify " << e.xmap.window << endl;
      Win* w = findWin(e.xmap.window);
      if (!w) w = addWin(e.xmap.window, true);
      if (w) {  // att: addWin peut retourner null!
        w->is_shown = true;  // move ? fait par configure ?
        sendWindowState(w, Win::SHOW);
      }
    }
    return;
    
  case UnmapNotify: {    // a window is iconified
    //cerr <<"UnmapNotify " << e.xunmap.window << endl;
    Win* w = findWin(e.xunmap.window);
    if (w) {
      w->is_shown = false;
      sendWindowState(w, Win::HIDE);
    }
  } return;
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
  case CreateNotify:     // a window is created
    //cerr <<"Create " << e.xcreatewindow.window << endl;
    //TST: 30sept05: le MapNotify ajoute la fenetre !!!!!!
    // addWin(e.xcreatewindow.window, true/*notify*/);
    return;
    
  case DestroyNotify:     // a window is destroyed
    removeWin(e.xdestroywindow.window, true/*notify*/);
    return;

  //case FocusIn:
    //cerr << "FocusIn "<< e.xconfigure.window << " " << xrootwin << endl;
    //break;

  //case FocusOut:
    //cerr << "FocusIn "<< e.xconfigure.window << " " << xrootwin << endl;
    //break;
  }
}
}

/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */

