/*************************************************************************
*
*  request.cpp : UMS requests from a UMS client
*  Part of the Ubit GUI Toolkit
*  (C) 2003-2008 Eric Lecolinet / ENST Paris
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

#include <string.h>
#include <cstdio>
#include <cstdlib>       // getenv, atexit
#include <unistd.h>      // darwin
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ubit/umsproto.hpp>
#include "umserver.hpp"
#include "flow.hpp"
#include "events.hpp"
#include "remoteserver.hpp"
#include "calib.hpp"
#include <X11/Xlib.h>
using namespace std;

#define ANY_BUTTON (MouseLeftID|MouseMiddleID|MouseRightID|WheelUpID|WheelDownID)
#define ANY_MOD (ShiftMask|LockMask|ControlMask|Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask)

// protocol: see UMSrequest in ubit/src/umsproto.hpp

static bool processKeyMouseRequest(UMServer* ums, UInbuf&);
static bool processEventRequest(UMServer* ums, UInbuf&);
static bool processMessageRequest(UMServer* ums, UInbuf&);
static bool receiveFileRequest(UMServer* ums, USocket* sock, UInbuf&);

/* ==================================================== ======== ======= */
// client REQUESTs to server
// returns UMSrequest status
// !ATT: CloseCnx ferme la connection!

void UMServer::processRequest(USocket* sock, UInbuf& req) {
  unsigned char reqtype;
  req.readChar(reqtype);
  
  switch (reqtype) {
    case UMSrequest::KEY_MOUSE_CTRL :
      processKeyMouseRequest(this, req);
      break;
      
    case UMSrequest::SEND_EVENT :
      processEventRequest(this, req);
      break;
      
    case UMSrequest::SEND_MESSAGE :
      processMessageRequest(this, req);
      break;
      
   case UMSrequest::SET_NEIGHBOR : {
     char* com = req.data() + req.consumed();
     char* p = strchr(com, ' ');
     //if address==empty la la topologie est annulee
     if (p) {*p = 0; setNeighborUMS(com[0], p+1);}
     else setNeighborUMS(com[0], "");
   } break;

    case UMSrequest::CALIBRATE : {
      Calibration* calib = getCalibration();
      if (calib) calib->startCalibration("Calibration");
    } break;

    case UMSrequest::MOVE_BACK_POINTER :
      moveNativePointerBackHome(true);
    break;
      
    case UMSrequest::GET_SCREEN_SIZE : {  // !!multi-screen pas gere
      char msg[100];
      sprintf(msg, "_umsSize 0 %ld %ld", getScreenWidth(), getScreenWidth());
      events.sendMessage(sock, msg);
    } break;
      
    case UMSrequest::BROWSE_SERVERS : {
      Cnx* c = findCnx(sock);
      if (!c) return;
      c->browse_servers = true;
      sendRemoteUMSList(c);
    } break;

   case UMSrequest::BROWSE_NEIGHBORS : {
      Cnx* c = findCnx(sock);
      if (!c) return;
      c->browse_servers = true; // c->is_browsing_neighbors ?
      sendNeighborList(c);
    } break;

   case UMSrequest::BROWSE_WINDOWS : {
      Cnx* c = findCnx(sock);
      if (!c) return;
      c->browse_windows = true;
      sendWindowList(c);
    } break;
   
    case UMSrequest::RESOLVE_SERVER : {
      char* name = req.data() + req.consumed();
      char msg[300] = "_umsResolve";  // renvoie un message vide si erreur

      if (name && name) {
        for (RemoteUMSList::iterator k = remotes.begin(); k != remotes.end(); k++) {
          if ((*k)->name == name) {
            sprintf(msg, "_umsResolve %s\t%s\t%d",   // !TABs entre les arguments!
                    name, (*k)->address.c_str(), (*k)->port);
            break;
          }
        }
      }

      //pas de sens: serait deja dans la liste si avait un UMS
      //umdns->resolveAndAddRemoteUMS(name, UMS_ZEROCONF, "");
      events.sendMessage(sock, msg);
    }break;

    case UMSrequest::RESOLVE_WINDOW : {
      char* name = req.data() + req.consumed();
      char msg[300] = "_Win";  // renvoie un message vide si erreur

      if (name && name) {
        const Win* w = findWin(name);
        if (w) {
          sprintf(msg, "_Win %ld", w->client_win);
          break;
        }
      }
      //cerr << "RESOLVE_WIN:'" << (name?name:"NUll") <<"' "<< msg << endl;
      events.sendMessage(sock, msg);
    }break;

    case UMSrequest::FILE_TRANSFERT:
      receiveFileRequest(this, sock, req);
      break;
  }
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */

static Win* findTargetWin(UMServer* ums, UStr& target, Pos& winpos) {
  if (target.empty()) return null;
  Win* win = null;

  if (isdigit(target[0]))
    win = ums->findWin(strtol(target.c_str(), null, 0));
  else if (target[0] != '"' && target[0] != '\'') 
    win = ums->findWin(target.c_str());
  else {
    char sep = target[0];
    target.remove(0,1);
    int ix = target.find(sep);
    if (ix >= 0) target.remove(ix, 1);
    win = ums->findWin(target.c_str());
  }

  if (!win) return null;
  else if (win->client_win && ums->getXWinPos(win, winpos)) {
    winpos.win = win->client_win;  // c'est client_win qu'on veut
    return win;
  }
  else return null;
}

/* ==================================================== ======== ======= */

static bool processMessageRequest(UMServer* ums, UInbuf& req) {
  UStr target, msg;
  req.readString(target);
  req.readString(msg);
  if (msg.empty()) return false;

  Pos winpos;
  Win* w = findTargetWin(ums, target, winpos);
  if (!w) return false;

  ums->events.sendXMessage(winpos.win, msg.c_str());
  return true;
}

/* ==================================================== ======== ======= */

static bool processEventRequest(UMServer* ums, UInbuf& req) {
  u_char event_type;
  u_char event_flow;
  long x, y;
  unsigned long detail;

  req.readEvent(event_type, event_flow, x, y, detail);

  UStr target;
  req.readString(target);

  Pos winpos;
  Win* w = findTargetWin(ums, target, winpos);
  if (!w) return false;

  if (w->client_win == None) return false;
  else w->win = w->client_win;
  Pos pos;
  if (!ums->getXSubwin(winpos, x, y, pos)) return false;

  // pos.win_sock = action.pos.win_sock;

  int btnmask = detail & ANY_BUTTON;
  int mods = detail & ANY_MOD;

  // marche pas sur NativeMouseFlow a cause du XTestFakeButtonEvent 
  // qui prend en compte les modifieurs appuyes => pas le bon event!
  // => prendre flow 1
  MouseFlow* mf = ums->getMouseFlow(1);
  if (!mf) return false;

  // last arg =  pseudo_pos : envoyer event a cette position directement
  // sans gerer le MouseFlow ni afficher le ptr

  // fprintf(stderr, "%s / %x %x %x / %d %d / %d %d  \n",
  //  target.chars(), w->win, winpos.win, pos.win, x, y, pos.rx, pos.ry);
  
  switch (event_type) {
    case ButtonPress:
      if (mods != 0) {
        for (unsigned int k = 0; k < Events::Mmask_Count; k++) {
          if (mods & Events::Mmasks[k])
            ums->events.sendKey(*mf,
                                Events::modMaskToKeySym(Events::Mmasks[k]),
                                true, &pos);
        }
      }
      ums->events.sendButton(*mf, btnmask, true, &pos);
      return true;

    case ButtonRelease:
      ums->events.sendButton(*mf, btnmask, false, &pos);

      if (mods != 0) {
        for (unsigned int k = 0; k < Events::Mmask_Count; k++) {
          if (mods & Events::Mmasks[k])
            ums->events.sendKey(*mf,
                                Events::modMaskToKeySym(Events::Mmasks[k]),
                                false, &pos);
        }
      }
      return true;

    default:
      return false;
  }
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */

static bool processKeyMouseRequest(UMServer* ums, UInbuf& req) {
  u_char event_type;
  u_char event_flow;
  long x,y;
  unsigned long detail;

  req.readEvent(event_type, event_flow, x, y, detail);

  EventFlow* ef = ums->getEventFlow(event_flow);
  MouseFlow* mf = (ef ? dynamic_cast<MouseFlow*>(ef) : null);
  if (!mf) return false;

  // NEW: traitement reseau des valeurs negatives (vers la gauche te le haut)
  if (x < 0) x += ums->getScreenWidth();
  if (y < 0) y += ums->getScreenHeight();
  
  switch (event_type) {
  case MotionNotify:
    mf->moveMouse(x, y, detail);    // detail = abs_coord
    return true; 

  case ButtonPress: {
    int btnmask = detail & ANY_BUTTON;  // detail = BtnMask
    mf->pressMouse(btnmask);
    return true;
  }

  case ButtonRelease: {
    int btnmask = detail & ANY_BUTTON;   // detail = BtnMask
    mf->releaseMouse(btnmask);
    return true;
  }

  case KeyPress: 
    mf->pressKey(detail);  // detail = keysym
    return true;
    
  case KeyRelease:
    mf->releaseKey(detail);  // detail = keysym
    return true;      
      
  default:
    return false;
  }
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */

static bool notifyWin(UMServer* ums, const char* name, const char* msg) {
  if (!name || !name[0]) return false;
  const Win* w = ums->findWin(name);

  if (!w) return false;
  else if (w->client_win == None) return false;
  else ums->events.sendXMessage(w->client_win, msg);
  return true;
}

static bool receiveFileRequest(UMServer* ums, USocket* sock, UInbuf& req) {
  static int fd = -1;                              // un seul file !!!
  static UStr path;
    
  UStr proto, type;
  req.readString(proto);
  req.readString(type);

  // signifie que cette action est interdite
  if (ums->getPublicDir().empty()) goto ERROR;

  if (proto != "UFP1") {
    //cerr << "UMSD:receiveFileRequest: wrong proto" << endl;
    goto ERROR;
  }
  
  if (type == "RFB") {
    UStr header;
    req.readString(header);
    path = req.data() + req.consumed();  // pourquoi PBM des blancs ????

    // securite: on vire les / et les ..
    path = ums->getPublicDir() & path.basename();

    if (path.empty()) goto ERROR;

    struct stat filestat;
    if (stat(path.c_str(), &filestat) == 0) {  // file exists
      int pos = path.rfind('.');
      if (pos > 0) path.insert(pos, '1');
      else path.append('1');
      // a completer !!!!
    }

    // O_EXCL pour interdire l'ecrasement
    // verifier pas de .. et imposerebut un d
    fd = open(path.c_str(), O_CREAT|O_WRONLY, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if (fd < 0) goto ERROR;
  } 
  
  else if (type == "RFC") {
    char* data = req.data() + req.consumed();
    unsigned int len  = req.size() - req.consumed();

    if (fd < 0 || write(fd, data, len) < 0) {
      //cerr << "UMSD:receiveFileRequest: write error " << endl;
      goto ERROR;
    }
  }
  
  else if (type == "RFE") {
    if (fd < 0 || close(fd) < 0) {
      //cerr << "UMSD:receiveFileRequest: close error " << endl;
      goto ERROR;
    }    
    fd = -1;
  
    //UStr msg = "put http://localhost/" & UStr(ums->getPublicDir()).basename() & "Public/" & path.basename();
    //notifyWin(ums, "vreng", msg.c_str());

    // A REVOIR !!!
    string msg = "open " + ums->getPublicDir() + path.basename().c_str();
    notifyWin(ums, "browser", msg.c_str());
    notifyWin(ums, "campus", msg.c_str());
  }
  
  else goto ERROR;
  return true;
  
ERROR:
  sock->close();
  if (fd >= 0) {
    close(fd);
    //unlink(fd);
  }
  fd = -1;
  return false;
}

/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */

