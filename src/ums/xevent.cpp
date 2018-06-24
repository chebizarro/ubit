/*************************************************************************
 *
 *  xevent.cpp - UMS Server
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

//include <ubit/config.h>  // for HAVE_STDINT_H
//ifdef HAVE_STDINT_H
//  include <stdint.h>     // uint16_t etc.
//endif
#include "umserver.hpp"
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <ubit/umsproto.hpp>
#include "flow.hpp"
#include "events.hpp"
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
using namespace std;

/* ==================================================== [(c)Elc] ======= */

static inline void FIX_TIME(timeval& time) {
  while (time.tv_usec >= 1000000) {
    time.tv_usec -= 1000000;                  
    time.tv_sec++;                            
  }
  while (time.tv_usec < 0) {
    if (time.tv_sec > 0) {
      time.tv_usec += 1000000;
      time.tv_sec--;
    }
    else {         
      time.tv_usec = 0;
      break;
    }      
  }
}

TimeID UMServer::getTime() {
  timeval time;
  gettimeofday(&time, 0);
  FIX_TIME(time);
  return time.tv_sec * 1000 + time.tv_usec % 1000;
}

/* ==================================================== ======== ======= */

// devrait etre dans events.cpp
 bool Events::init() {
  // tester si XTEST est effectivement installe sur le serveur!
  int event_base, error_base;
  int major_version, minor_version;
  has_xtest = XTestQueryExtension(ums.xdisplay, &event_base, &error_base,
                                  &major_version, &minor_version);
  if (has_xtest)
    cout << " - XTest "<< major_version <<"."<< minor_version <<endl;
  
  //Cause the executing client to become impervious to server grabs.
  //That is, it can continue to execute requests even if another client grabs
  //the server.
  XTestGrabControl(ums.xdisplay, True);
  return has_xtest;
}

/* ==================================================== ======== ======= */

void Events::warpXPointer(long x, long y) {
  XWarpPointer(ums.xdisplay, ums.xrootwin, ums.xrootwin, 0, 0, 0, 0, x, y);
  XFlush(ums.xdisplay);
}

void Events::getXPointer(long& x, long& y) {
  Window xroot = None, xchild = None;
  int root_x = 0, root_y = 0, win_x, win_y;
  unsigned int mask;
  XQueryPointer(ums.xdisplay, ums.xrootwin, &xroot, &xchild,
                &root_x, &root_y, &win_x, &win_y, &mask);
  x = root_x;
  y = root_y;
}

bool Events::getXPointer(Pos& pos) {
  int root_x = 0, root_y = 0;
  int x_in_win, y_in_win;
  unsigned int mask;
  Window root_win = None;
  Window win = ums.xrootwin;
  Window win_child = None;

  pos.win = None;
  pos.win_sock = -1;
  pos.wx = pos.wy = 0;
  pos.rx = pos.ry = 0;

  while (XQueryPointer(ums.xdisplay, win, &root_win, &win_child,
                       &root_x, &root_y, &x_in_win, &y_in_win, &mask)) {
    pos.win = win;
    pos.wx = x_in_win;
    pos.wy = y_in_win;
    pos.rx = root_x;
    pos.ry = root_y;

    if (win_child == None || win_child == win) break;
    else win = win_child;
  }

  return (pos.win != None);
}

/* ==================================================== [(c)Elc] ======= */

void Events::sendXKey(MouseFlow& flow, unsigned long keysym,
                      bool press, Pos* pseudo_pos) {
  // cas pointeur natif X
  if (!pseudo_pos && flow.id == 0 && has_xtest) {
    // event envoye a la dernier pos du pointer natif
    KeyCode c = XKeysymToKeycode(ums.xdisplay, keysym);
    // ne pas envoyer un keycode null
    if (c != 0) XTestFakeKeyEvent(ums.xdisplay, c, press, 0);
    XFlush(ums.xdisplay); // utile?
    return;
  }

  // sinon il faut localiser le ptr avant d'envoyer l'event
  Pos pos;

  // si pseudo, la pos est donnee en arg
  if (pseudo_pos) pos = *pseudo_pos;

  // sinon chercher la pos (et la la win a laquelle on envoie l'event)
  if (!getPointer(flow, pos)) return;

  XEvent e;
  e.xkey.type = (press ? KeyPress : KeyRelease);
  e.xkey.serial = 0;
  e.xkey.send_event = false;    // true if this came from a SendEvent request

  // ici il y un pbm: ca doit etre le display de l'appli pas celui du mouse
  // driver. c'est OK pour XSendEvent (qui corrige) mais pas pour write direct
  // (il faut alors corriger a la reception)
  e.xkey.display   = ums.xdisplay;    // Display the event was read from
  e.xkey.window    = pos.win;     // event window it is reported relative to
  e.xkey.root      = ums.xrootwin;    // root window that the event occurred on
  e.xkey.subwindow = None;        // child window: utile??
  e.xkey.time      = ums.getTime();   // milliseconds
  e.xkey.x         = pos.wx;      // pointer x, y coordinates in event window
  e.xkey.y         = pos.wy;
  e.xkey.x_root    = pos.rx;      // coordinates relative to root
  e.xkey.y_root    = pos.ry;
  e.xkey.same_screen = true;

  // The state member is set to indicate the logical state of the pointer
  // buttons and modifier keys JUST PRIOR to the event
  e.xkey.state = flow.mod_mask | flow.btn_mask;
  
  e.xkey.keycode = XKeysymToKeycode(ums.xdisplay, keysym);
  if (e.xkey.keycode == 0) return;   // ne pas envoyer un keycode null

  KeySym ks = XKeycodeToKeysym(ums.xdisplay, e.xkey.keycode, 0);

  /*                                 // A REVOIR
    if (mods & ControlMask) {
      e.xkey.state = ControlMask;    // on ne regarde pas les autres indexes ?
      // et faire de meme avec Meta et Alt
    }
  else */
  if (ks != keysym) {
    if ((ks = XKeycodeToKeysym(ums.xdisplay, e.xkey.keycode, 1)) == keysym) {
      e.xkey.state = ShiftMask;  // conforme a la norme
    }
    else if ((ks = XKeycodeToKeysym(ums.xdisplay, e.xkey.keycode, 2)) == keysym) {
      e.xkey.state = ALT_GRAPH_MASK;  // 0x2000
    }
    else if ((ks = XKeycodeToKeysym(ums.xdisplay, e.xkey.keycode, 3)) == keysym) {
      e.xkey.state = ALT_GRAPH_MASK | 0x1;  // 0x2001
    }
    //else cerr << "not found "<<endl;
  }

  if (pos.in_ubitwin && flow.id != 0) {
    e.xkey.state = e.xkey.state | UMS_EVENT_MASK;
    e.xkey.subwindow = flow.id;  // conventional !!
  }
  else e.xbutton.subwindow = pos.subwin;
  
#ifdef USE_SOCKET
  // cas (UMS flow  ET  Ubit win) => envoyer event sur canal specifique
  if (flow.getID() != 0 && pos.winsock > -1) {
    write(pos.winsock, &e, sizeof(XEvent));
  }
  else
#endif
  {
    long ev_mask = (press ? KeyPressMask : KeyReleaseMask);
    XSendEvent(ums.xdisplay, pos.win, False, ev_mask, &e);
  }

  flow.showPointer(true);  // remettre le ptr au 1er plan
  XFlush(ums.xdisplay); // (deja fait)
}

/* ==================================================== [(c)Elc] ======= */
// pseudo_pos : envoyer event a cette position directement sans gerer le
// MouseFlow ni afficher le ptr (utilise par sendMousePress(), etc)

void Events::sendXButton(MouseFlow& flow, int btn_id,
                         bool press, Pos* pseudo_pos) {

  // ATTENTION: le mask doit indiquer UN SEUL button !!!
  unsigned int btn = maskToXButton(btn_id);

  // cas pointeur natif X (et pas pseudo event)
  if (!pseudo_pos && flow.id == 0 && has_xtest) {
    // event envoye a la dernier pos du pointer natif
    XTestFakeButtonEvent(ums.xdisplay, btn, press, CurrentTime);
    // ca n'a pas de sens de reafficher le ptr dans ce cas
    return;
  }
  
  // sinon il faut localiser le ptr avant d'envoyer l'event
  Pos pos;

  // si pseudo, la pos est donnee en arg
  if (pseudo_pos) pos = *pseudo_pos;

  // sinon chercher la pos (et la la win a laquelle on envoie l'event)
  // true final => check ubit_props
  else if (!getPointer(flow, pos, true)) return;
       
  XEvent e;
  e.xbutton.type = (press ? ButtonPress : ButtonRelease);
  e.xbutton.serial = 0;
  e.xbutton.send_event = false;  // true if this came from a SendEvent request

  // NB: ici on met le display de l'appli pas celui du mouse driver.
  // c'est auto corrige par XSendEvent mais pas pour write direct
  // (il faut alors corriger a la reception) 
  e.xbutton.display   = ums.xdisplay;    // Display the event was read from 
  e.xbutton.window    = pos.win;     // event window it is reported relative to
  e.xbutton.root      = ums.xrootwin;    // root window that the event occurred on
  e.xbutton.subwindow = None;        // child window: 
  e.xbutton.time      = ums.getTime();   // milliseconds
  e.xbutton.x         = pos.wx;      // pointer x, y coordinates in event window
  e.xbutton.y         = pos.wy;
  e.xbutton.x_root    = pos.rx;      // coordinates relative to root
  e.xbutton.y_root    = pos.ry;

  // The state member is set to indicate the logical state of the pointer 
  // buttons and modifier keys JUST PRIOR to the event
  //e.xbutton.state = flow.btn_mask /* | mods */;
  e.xbutton.state = flow.mod_mask | flow.btn_mask;
  e.xbutton.button = btn;
  e.xbutton.same_screen = true;
  
  if (pos.in_ubitwin && flow.id != 0) {
    e.xbutton.state = e.xbutton.state | UMS_EVENT_MASK;
    e.xbutton.subwindow = flow.id;  // conventional !!
  }
  else e.xbutton.subwindow = pos.subwin;

#ifdef USE_SOCKET
  if (flow.id != 0 && pos.winsock > -1) {
    write(pos.winsock, &e, sizeof(XEvent));
  }
  else 
#endif
  {
    long ev_mask = (press ? ButtonPressMask : ButtonReleaseMask);
    XSendEvent(ums.xdisplay, pos.win, False, ev_mask, &e);
  }

  // si pas de pseudo_pos, remettre le ptr au 1er plan
  if (pseudo_pos) XFlush(ums.xdisplay);
  else flow.showPointer(true);
  // XFlush(xdisplay); deja fait
}

/* ==================================================== ======== ======= */
// NB: il ne faut appeler cette fct si xtest est utilise (pas de sens)

static void sendXCrossingEvent(UMServer& ums, MouseFlow& flow,
                              Pos& pos, bool enter) {
  XEvent e;
  e.xcrossing.type       = (enter ? EnterNotify : LeaveNotify);
  e.xcrossing.serial     = 0;
  e.xcrossing.send_event = false;
  e.xcrossing.display    = ums.xdisplay;
  e.xcrossing.window     = pos.win;
  e.xcrossing.root       = ums.xrootwin;
  e.xcrossing.subwindow  = None;
  e.xcrossing.time       = ums.getTime();
  e.xcrossing.x          = pos.wx;
  e.xcrossing.y          = pos.wy;
  e.xcrossing.x_root     = pos.rx;
  e.xcrossing.y_root     = pos.ry;
  e.xcrossing.mode       = NotifyNormal; // ?? NotifyNormal, NotifyGrab, NotifyUngrab
  // ?? NotifyAncestor, NotifyVirtual, NotifyInferior,
  // ?? NotifyNonlinear,NotifyNonlinearVirtual
  e.xcrossing.detail = NotifyAncestor;
  e.xcrossing.same_screen = true;
  e.xcrossing.focus = True;  // boolean focus (?)
  //e.xcrossing.state = flow.btn_mask;
  e.xcrossing.state = flow.mod_mask | flow.btn_mask;

  if (pos.in_ubitwin && flow.id != 0) {
    e.xcrossing.state = e.xcrossing.state | UMS_EVENT_MASK;
    e.xcrossing.subwindow = flow.id;  // conventional !!
  }
  else e.xbutton.subwindow = pos.subwin;

#ifdef USE_SOCKET
  // cas (UMS flow  ET  Ubit win) => envoyer event sur canal specifique
  if (flow.getID() != 0 && pos.winsock > -1) {
    write(pos.winsock, &e, sizeof(XEvent));
  }
  else
#endif
  {
    long ev_mask = enter ? EnterWindowMask : LeaveWindowMask;
    XSendEvent(ums.xdisplay, pos.win, False, ev_mask, &e);
    XFlush(ums.xdisplay);
  }
}

/* ==================================================== ======== ======= */

void Events::sendXMotion(MouseFlow& flow, long x, long y,bool move_ptr) {
  if (move_ptr) {
    if (flow.id == 0)
      XWarpPointer(ums.xdisplay, ums.xrootwin, ums.xrootwin, 0, 0, 0, 0, x, y);
    else if (flow.getPointer() != None) {
      XMoveWindow(ums.xdisplay, flow.getPointer(), x, y);
      XRaiseWindow(ums.xdisplay, flow.getPointer());     // always on top !
    }
    XFlush(ums.xdisplay);
  }
  
  // cas pointeur natif X 
  if (flow.id == 0 && has_xtest) {
    // event envoye a la dernier pos du pointer natif
    // if screen_nbr is -1, the current screen (that the pointer is on) is used
    XTestFakeMotionEvent(ums.xdisplay, /*screen*/-1, x, y, CurrentTime);
    // ca n'a pas de sens de reafficher le ptr dans ce cas
    return;
  }

  Pos pos;
  // chercher la pos (et la la win a laquelle on envoie l'event)
  if (!getPointer(flow, pos)) return;
  
  // il faut d'abord gerer les crossing events s'il y a lieu
  if (pos.win != flow.last_entered_win) {
    if (flow.last_entered_win != None) {      // wx, wy corrects ?
      Pos leave_pos(pos.rx, pos.ry, 0, 0,
                       flow.last_entered_win,
                       flow.last_entered_in_ubitwin,
                       flow.last_entered_winsock);

      // pas de sens si xtest est utilise
      sendXCrossingEvent(ums, flow, leave_pos, false);
    }

    sendXCrossingEvent(ums, flow, pos, true);
    flow.last_entered_win = pos.win;
    flow.last_entered_in_ubitwin = pos.in_ubitwin;
    flow.last_entered_winsock = pos.win_sock;
  }
  
  XEvent e;
  e.xmotion.type = MotionNotify;
  e.xmotion.serial = 0;
  e.xmotion.send_event = false;
  e.xmotion.display    = ums.xdisplay;
  e.xmotion.window     = pos.win;
  e.xmotion.root       = ums.xrootwin;
  e.xmotion.subwindow  = None;
  e.xmotion.time       = ums.getTime();
  e.xmotion.x          = pos.wx;
  e.xmotion.y          = pos.wy;
  e.xmotion.x_root     = pos.rx;
  e.xmotion.y_root     = pos.ry;

  // The state member is set to indicate the logical state of the pointer 
  // buttons and modifier keys JUST PRIOR to the event
  // e.xmotion.state = flow.btn_mask;   // il manque les Ctrl, etc.
  e.xmotion.state = flow.mod_mask | flow.btn_mask;
  e.xmotion.is_hint = NotifyNormal; // ????
  e.xmotion.same_screen = true;	    // same screen flag:  A REVOIR!!!

  if (pos.in_ubitwin && flow.id != 0) {
    e.xmotion.state = e.xmotion.state | UMS_EVENT_MASK;
    e.xmotion.subwindow = flow.id;  // conventional !!
  }
  else e.xmotion.subwindow = pos.subwin;

#ifdef USE_SOCKET
  // cas (UMS flow  ET  Ubit win) => envoyer event sur canal specifique
  if (flow.id != 0 && pos.winsock > -1) {
    write(pos.winsock, &e, sizeof(XEvent));
  }
  else
#endif
  {
    long ev_mask = PointerMotionMask;
    if (e.xmotion.state & Button1Mask)
      ev_mask = ev_mask | (Button1MotionMask | ButtonMotionMask);
    if (e.xmotion.state & Button2Mask)
      ev_mask = ev_mask | (Button2MotionMask | ButtonMotionMask);
    if (e.xmotion.state & Button3Mask)
      ev_mask = ev_mask | (Button3MotionMask | ButtonMotionMask);
    if (e.xmotion.state & Button4Mask)
      ev_mask = ev_mask | (Button4MotionMask | ButtonMotionMask);
    if (e.xmotion.state & Button5Mask)
      ev_mask = ev_mask | (Button5MotionMask | ButtonMotionMask);

    // fprintf(stderr, "send: id: %d / win %x / state: %d\n",
    //         flow.id, pos.win, e.xmotion.state);
    XSendEvent(ums.xdisplay, pos.win, False, ev_mask, &e);
  }

  flow.showPointer(true);  // remettre le ptr au 1er plan
  // XFlush(xdisplay); deja fait
}

/* ==================================================== [(c)Elc] ======= */

static void sendShortXMessage(UMServer& ums, Window win, const char *message) {
  XEvent e;
  e.xclient.type = ClientMessage;
  e.xclient.serial = 0;
  e.xclient.send_event = false;	//true if this came from a SendEvent request

  // ici il y un pbm: ca doit etre le display de l'appli pas celui du mouse
  // driver. c'est OK pour XSendEvent (qui corrige) mais pas pour write
  // direct (il faut alors corriger a la reception) 
  e.xclient.display = ums.xdisplay; // Display the event was read from
  e.xclient.window = win;     //``event'' window it is reported relative to
  e.xclient.message_type = ums._UMS_MESSAGE;
  e.xclient.format = 8;       // table of chars

  // buggy!
  //memcpy(e.xclient.data.b, message, sizeof(e.xclient.data.b)-1);
  //e.xclient.data.b[sizeof(e.xclient.data.b)-1] = 0;
  //new:
  strcpy(e.xclient.data.b, message);

#ifdef USE_SOCKET
  if (socket) write(socket, &e, sizeof(XEvent));
  else
#endif
  {
    long ev_mask = 0;
    //fprintf(stderr, "send %x %s \n" , win , message);
    XSendEvent(ums.xdisplay, win, False, ev_mask, &e);
    XFlush(ums.xdisplay);
  }
}

/* ==================================================== ======== ======= */

static void sendLongXMessage(UMServer& ums, Window win,
                             const char* message, int message_len) {
  // ATT: If there is insufficient space, a BadAlloc error results.

  XChangeProperty(ums.xdisplay, win,
                  ums._UMS_MESSAGE/*property*/,
                  XA_STRING/*type*/, 8/*format*/,
                  // PropModeAppend: ajoute a la fin (NB: PropModeReplace remplacerait 
                  // la valeur courante ce qui aurait pour effet de perdre des donnees 
                  // si le callback du client est appele apres un 2e appel de XChangeProperty)
                  PropModeAppend, 
                  (unsigned char*)message, message_len);

  XFlush(ums.xdisplay);  // necessaire sinon notification differee

  // a revoir: le cas ou les XChangeProperty sont trop proches et s'accumulent
  
/*
  message = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa aaaaaaaaaaaaaaaaaaaaaaaaaaaa aaaaaaaaaaaaaaaaaaaaaaaaaa aaaaaaaaaaaaaa";
  message_len = strlen(message);
  XChangeProperty(serv->xdisplay, win,
                  serv->_UMS_MESSAGE, //property
                  XA_STRING, 8, PropModeAppend, 
                  (unsigned char*)message, message_len);
  message = "bbbbbbbbbbbbbbbbbbb bbbbbbbbbbbbbbbbbbbbbbbbbbbbbb bbbbbbbbbbbbbbbbbbbbbbbbbb bbbbbbbbbbbbbbbbbbbbbbb zzzzzzzzzzz";
  message_len = strlen(message);
  XChangeProperty(serv->xdisplay, win,
                  serv->_UMS_MESSAGE, //property
                  XA_STRING, 8, PropModeAppend,
                  (unsigned char*)message, message_len);
  XFlush(serv->xdisplay);  // necessaire sinon decalage
*/
}

/* ==================================================== ======== ======= */

void Events::sendXMessage(Window win, const char* message) {
  if (!message) return;
  unsigned int len = strlen(message);
  XEvent e;
  if (len < sizeof(e.xclient.data.b))
    sendShortXMessage(ums, win, message);
  else sendLongXMessage(ums, win, message, len);
}

/* ==================================================== [(c)Elc] ======= */
// on ne sait comment deviner cette valeur ni la correspondance avec les indexes des keycodes
// ALT_GRAPH_MASK = 0x2000

int Events::Bmasks[] = {
  MouseLeftID, MouseMiddleID, MouseRightID, WheelUpID, WheelDownID
};
unsigned int Events::Bmask_Count = sizeof(Events::Bmasks)/sizeof(Events::Bmasks[0]);

int Events::Mmasks[] = {
  ShiftMask, ControlMask, LockMask, Mod1Mask, Mod2Mask, Mod3Mask, Mod4Mask, Mod5Mask
};
unsigned int Events::Mmask_Count = sizeof(Events::Mmasks) / sizeof(Events::Mmasks[0]);


unsigned int Events::mbuttonToMask(int mbtn) {
  switch (mbtn) {
    case 1:
      return MouseLeftID;
    case 2:
      return MouseRightID;  // 2 et 3 inverses par rapport a X!
    case 3:
      return MouseMiddleID;
    default:
      return 0;
  }
}

unsigned int Events::xbuttonToMask(int xbtn) {
  switch (xbtn) {
    case Button1:
      return MouseLeftID;
    case Button2:
      return MouseMiddleID;
    case Button3:
      return MouseRightID;
    case Button4:
      return WheelUpID;
    case Button5:
      return WheelDownID;
    default:
      return 0;
  }
}

unsigned int Events::maskToXButton(unsigned int mask) {
  switch (mask) {
    case MouseLeftID:
      return Button1;
    case MouseMiddleID:
      return Button2;
    case MouseRightID:
      return Button3;
    case WheelUpID:
      return Button4;
    case WheelDownID:
      return Button5;
    default:
      return 0;
  }
}

unsigned int Events::keySymToModMask(unsigned long keysym) {
  switch (keysym) {
    case XK_Shift_L:
    case XK_Shift_R:
      return ShiftMask;
    case XK_Control_L:
    case XK_Control_R:
      return ControlMask;
    case XK_Caps_Lock:
    case XK_Shift_Lock:
      return LockMask;
    case XK_Meta_L:
    case XK_Meta_R:
      return Mod1Mask;  // A REVOIR
    case XK_Alt_L:
    case XK_Alt_R:
      return Mod2Mask;  // A REVOIR
    case XK_Super_L:
    case XK_Super_R:
      return Mod3Mask;  // A REVOIR
    case XK_Hyper_L:
    case XK_Hyper_R:
      return Mod4Mask;  // A REVOIR
    default:
      return 0;
  }
}

unsigned long Events::modMaskToKeySym(unsigned int mod) {
  switch (mod) {
    case ShiftMask :
      return XK_Shift_L;
    case ControlMask :
      return XK_Control_L;
    case LockMask :
      return XK_Caps_Lock;
    case Mod1Mask:
      return XK_Meta_L ;  // A REVOIR
    case Mod2Mask:
      return XK_Alt_L;  // A REVOIR
    case Mod3Mask:
      return XK_Super_L;  // A REVOIR
    case Mod4Mask:
      return XK_Hyper_L;  // A REVOIR
    default:
      return 0;
  }
}

/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */
