/*************************************************************************
 *
 *  umsflow.cpp : UMS event flows.
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2003-2008 Eric Lecolinet / ENST Paris / www.enst.fr/~elc/ubit
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

#include <ubit/umservice.hpp>
#include <X11/Xlib.h>
#include "umserver.hpp"
#include "flow.hpp"
#include "source.hpp"
#include "events.hpp"
#include "remoteserver.hpp"
using namespace std;

/* ==================================================== ======== ======= */

EventFlow::EventFlow(UMServer* _ums, int _id) :
ums(*_ums),
events(_ums->events),
id(_id) {
}

EventFlow::~EventFlow() {
}

/* ==================================================== ======== ======= */

MouseFlow::MouseFlow(UMServer* _ums, int _id, bool create_pointer)
: EventFlow(_ums, _id) {
  mx = 0, my = 0;
  btn_mask = 0;
  mod_mask = 0;
  last_entered_win = None;
  last_entered_in_ubitwin = false;
  last_entered_winsock = -1;
  if (id == 0) pointer_win = None; else createPointer();
  h_out = v_out = false;
}

MouseFlow::~MouseFlow() {}

/* ==================================================== ===== ======= */
/* ==================================================== [Elc] ======= */

void MouseFlow::createPointer() {
  // create cursor window
  XSetWindowAttributes wattr;
  unsigned long wattr_mask = 0;
  
  // window pas geree par le WM (no borders...)
  wattr.override_redirect = True;
  wattr_mask |= CWOverrideRedirect;
  
  //wattr.background_pixmap = pix;
  //wattr_mask |= CWBackPixmap;
  
  //wattr.background_pixel = WhitePixelOfScreen(xscreen);
  //wattr_mask |= CWBackPixel;
  
  //wattr.border_pixel = bgcolor;
  //wattr_mask |= CWBorderPixel;
  
  pointer_win = XCreateWindow(ums.xdisplay, ums.xrootwin,
                    0,0,      // x,y position
                    8, 8,     // width , height (0,0 crashes!)
                    0,	// border_width
                    ums.getScreenDepth(),
                    InputOutput, CopyFromParent,
                    wattr_mask, &wattr);
  XMapRaised(ums.xdisplay, pointer_win);
  XFlush(ums.xdisplay);
}

/* ==================================================== ===== ======= */

void MouseFlow::movePointer(long x, long y) {
  if (pointer_win != None) {
    XMoveWindow(ums.xdisplay, pointer_win, x, y);
    XRaiseWindow(ums.xdisplay, pointer_win);     // always on top !
  }
  XFlush(ums.xdisplay);
}

/* ==================================================== ===== ======= */

void MouseFlow::showPointer(bool state) {
  if (state) {
    if (pointer_win != None) XMapRaised(ums.xdisplay, pointer_win);
  }
  else {
    if (pointer_win != None) XUnmapWindow(ums.xdisplay, pointer_win);
  }
  // NB: toujours faire XFlush car d'autre fct supposent que raisePointer
  // le fait inconditionnellement
  XFlush(ums.xdisplay);
}

/* ==================================================== ======== ======= */

void MouseFlow::changePointer(const char* fgcolor_name, const char* bgcolor_name) {
  if (!pointer_win) return;
  
  Screen* scr = (Screen*)ums.xscreen;
  Display* disp = ums.xdisplay;
  long bgcolor = WhitePixelOfScreen(scr);
  long fgcolor = BlackPixelOfScreen(scr);
  
  XColor color, exact_color;
  if (fgcolor_name && XAllocNamedColor(disp, DefaultColormapOfScreen(scr),
                                       fgcolor_name, &color, &exact_color)) {
    fgcolor = color.pixel;
  }
  if (bgcolor_name && XAllocNamedColor(disp, DefaultColormapOfScreen(scr),
                                       bgcolor_name, &color, &exact_color)) {
    bgcolor = color.pixel;
  }
  
  static char mouse_bits[] = {
    0x00, 0xfe, 0x7e, 0x3e, 0x3e, 0x7e, 0xe6, 0xc2   // 8x8
  };
  
  Pixmap pix = XCreatePixmapFromBitmapData(disp,
                                           RootWindowOfScreen(scr),
                                           mouse_bits, 8, 8,
                                           fgcolor,
                                           bgcolor,
                                           ums.getScreenDepth());
  XUnmapWindow(disp,  pointer_win); // necessaire sur le Mac!
  XSetWindowBackgroundPixmap(disp, pointer_win, pix);
  XMapRaised(disp, pointer_win);
  XFlush(disp);
}

 /* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */

void MouseFlow::pressMouse(int btn_id) {
  long n_x, n_y;
  UMService* n_client = null;
  
  if (isOutside(mx, my, n_x, n_y, n_client)) {  // outside local screen
    if (n_client) {
      n_client->moveMouse(id, n_x, n_y, true);
      n_client->pressMouse(id, btn_id); 
    }
    else keepMouseInsideScreen(mx, my);
  }
  else {  // inside local screen
    events.sendButton(*this, btn_id, true);
  } 

  // NB: ne sert que pour les pseudo-pointers
  // X oblige de rajouter le btn_mask APRES le mousePress
  btn_mask = btn_mask | btn_id;
}

/* ==================================================== ======== ======= */

void MouseFlow::releaseMouse(int btn_id)  {
  long n_x, n_y;
  UMService* n_client = null;

  if (isOutside(mx, my, n_x, n_y, n_client)) {  // outside local screen
    if (n_client) {
      n_client->moveMouse(id, n_x, n_y, true);
      n_client->releaseMouse(id, btn_id);
    }
    else keepMouseInsideScreen(mx, my);
  }
  else  {        // inside local screen
    events.sendButton(*this, btn_id, false);
  }

  btn_mask = btn_mask & ~btn_id;
}

/* ==================================================== ======== ======= */

void MouseFlow::pressMouse(const UMSbutton* b) {
  if (!b) {
    cerr << "MouseFlow::pressMouse: null button mapping" << endl;
    return;
  }
  
  if (b->out_mod_mask != 0) {
    for (unsigned int k = 0; k < Events::Mmask_Count; k++) {
      if (b->out_mod_mask & Events::Mmasks[k])
        pressKey(events.modMaskToKeySym(Events::Mmasks[k]));
    }
  }
  pressMouse(b->out_btn_mask);
}

/* ==================================================== ======== ======= */

void MouseFlow::releaseMouse(const UMSbutton* b) {
  if (!b) {
    cerr << "MouseFlow::releaseMouse: null button mapping" << endl;
    return;
  }
  
  releaseMouse(b->out_btn_mask);
  
  if (b->out_mod_mask != 0) {
    for (unsigned int k = 0; k < Events::Mmask_Count; k++) {
      if (b->out_mod_mask & Events::Mmasks[k])
        releaseKey(events.modMaskToKeySym(Events::Mmasks[k]));
    }
  }
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */

void MouseFlow::pressKey(unsigned long keysym) {
  long n_x, n_y;
  UMService* n_client = null;

  if (isOutside(mx, my, n_x, n_y, n_client)) {  // inside local screen
    if (n_client) {
      n_client->moveMouse(id, n_x, n_y, true);      
      n_client->pressKey(id, keysym);
    }
    else keepMouseInsideScreen(mx, my);
  }
  else {        // outside local screen
    events.sendKey(*this, keysym, true);
  }

  // NB: ne sert que pour les pseudo-pointers
  // X oblige de rajouter le mod_mask APRES le keyPress
  mod_mask = mod_mask | events.keySymToModMask(keysym);
}

/* ==================================================== ======== ======= */

void MouseFlow::releaseKey(unsigned long keysym)  {
  long n_x, n_y;
  UMService* n_client = null;

  if (isOutside(mx, my, n_x, n_y, n_client)) {  // outside local screen
    if (n_client) {
      n_client->moveMouse(id, n_x, n_y, true);
      n_client->releaseKey(id, keysym);
    }
    else keepMouseInsideScreen(mx, my);
  }

  else  {         // inside local screen
    events.sendKey(*this, keysym, false);
  }

  mod_mask = mod_mask & ~events.keySymToModMask(keysym);
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */
// releases all mouses and keys (when leaving a remote display)

void MouseFlow::releaseAll() {
  //cerr << endl << "*** RELEASE ALL "<< endl << endl;
  long n_x, n_y;
  UMService* n_client = null;

  if (isOutside(mx, my, n_x, n_y, n_client)) {  // outside screen

    if (n_client) {
      //cerr << " -- sending RELEASE ALL " << endl;
      for (unsigned int k = 0; k < Events::Bmask_Count; k++) {
        if (btn_mask & Events::Bmasks[k]) {
          n_client->releaseMouse(id, Events::Bmasks[k]);
          btn_mask = btn_mask & ~Events::Bmasks[k];
        }
      }
      for (unsigned int k = 0; k < Events::Mmask_Count; k++) {
        if (mod_mask & Events::Mmasks[k]) {
          n_client->releaseKey(id, events.modMaskToKeySym(Events::Mmasks[k]));
          mod_mask = mod_mask & ~Events::Mmasks[k];
        }
      }
    }
  }
  
  else {
    //cerr << " -- local RELEASE ALL " << endl;
    for (unsigned int k = 0; k < Events::Bmask_Count; k++) {
      if (btn_mask & Events::Bmasks[k]) {
        events.sendButton(*this, Events::Bmasks[k], false);
        btn_mask = btn_mask & ~Events::Bmasks[k];
      }
    }

    for (unsigned int k = 0; k < Events::Mmask_Count; k++) {
      if (mod_mask & Events::Mmasks[k]) {
        events.sendKey(*this, events.modMaskToKeySym(Events::Mmasks[k]), false);
        mod_mask = mod_mask & ~Events::Mmasks[k];
      }
    }
  }
  
  btn_mask = 0;
  mod_mask = 0;
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */

void MouseFlow::moveMouse(long x, long y, bool abs_coords) {
  long n_x, n_y;
  UMService* n_client = null;

  if (id != 0) {       // id != 0 : alternate mouse flow
    if (abs_coords) {mx = x; my = y;}
    else {mx += x; my += y;}
  }
  
  else {              // native X mouse 
    if (abs_coords) {mx = x; my = y;}
    else {           // relative coords
      // mx += x; my += y;
  
      // cette sequence permet de synchroniser mflow[0] avec la position
      // reelle du pointer natif. c'est utile en cas de fusion, quand le ptr
      // est controle par plusieurs sources (p.ex. une "vraie" souris X
      // et une source de l'UMS)
      events.getNativePointer(mx, my);
      mx += x; my += y;
    }
  }

  if (isOutside(mx, my, n_x, n_y, n_client)) {
    // ICI une question: quel pointeur veut-on bouger ?
    // le pointeur natif 0 ou un autre pointeur ?
    
    // ATT: si on met autre chose que id (=0) mettre a jour press/release
    if (n_client) n_client->moveMouse(id, n_x, n_y, true);
    else keepMouseInsideScreen(mx, my);
  }
  else {             // inside local screen
    // generer les evenements et bouger le pointer (last arg = true)
    events.sendMotion(*this, mx, my, true);
  }
}

bool MouseFlow::moveMouseOutsideDisplay(long x, long y) {
  long n_x, n_y;
  UMService* n_client = null;
  mx = x;
  my = y;
  
  if (isOutside(mx, my, n_x, n_y, n_client)) {
    // ICI une question: quel pointeur veut-on bouger ?
    // le pointeur natif 0 ou un autre pointeur ?
    
    // ATT: si on met autre chose que id (=0) mettre a jour press/release
    if (n_client) {
      n_client->moveMouse(id, n_x, n_y, true);
      return true;
    }
  }
  
  // if inside or connection is broken
  keepMouseInsideScreen(mx, my); 
  return false;
}

// synchronizes this flow with the actual pos of the mouse (and send events).
void MouseFlow::syncMouse(long x, long y) {
  mx = x;
  my = y;
  // generer les evenements sans bouger le pointer (last arg = false)
  // car c'est deja fait
  events.sendMotion(*this, x, y, false);
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */
// returns true if mouse is outside current screen

bool MouseFlow::isOutside(long mx, long my, long& n_x, long& n_y,
                          UMService*& n_client) {
  int pos = 0;

  if (!v_out) {
    if (mx < 0)
      pos = RemoteUMS::LEFT, h_out = true;
    else if (mx >= ums.getScreenWidth())
      pos = RemoteUMS::RIGHT, h_out = true;
  }

  if (!h_out) {
    if (my < 0)
      pos = RemoteUMS::TOP, v_out = true;
    else if (my >= ums.getScreenHeight())
      pos = RemoteUMS::BOTTOM, v_out = true;
  }

  if (pos == 0) {
    h_out = v_out = false;
    return false;
  }
  
  n_x = 0; n_y = 0;
  RemoteUMS* n = ums.getNeighborUMS(pos);
  if (n) {
    if (n->client && n->client->isConnected())
      n_client = n->client;

    else if (!n->address.empty()
	     && UMServer::getTime()-n->cnx_time > UMServer::NEIGHBOR_RETRY_DELAY) {
      delete n->client;
      n->client = new UMService(n->address, n->port, "*UMSD*");

      if (n->client->isConnected()) n_client = n->client;
      else {
        delete n->client;
        n->client = null;
        //cerr << "MouseFlow: can't connect to: " << n->host << endl;
      }
      n->cnx_time = UMServer::getTime();
    }
  }

  if (n_client) {
    // cas LEFT traite dans processKeyMouseRequest
    if (pos == RemoteUMS::RIGHT) n_x = mx - ums.getScreenWidth();
    else n_x = mx;

    // cas TOP traite dans processKeyMouseRequest
    if (pos == RemoteUMS::BOTTOM) n_y = my - ums.getScreenHeight();
    else n_y = my;
  }

  return true;   // IS_INSIDE
}

/* ==================================================== ======== ======= */

void MouseFlow::keepMouseInsideScreen(long& mx, long& my) {
  if (mx < 0) mx = 0;
  else if (mx >= ums.getScreenWidth()) mx = ums.getScreenWidth() -1;

  if (my < 0) my = 0;
  else if (my >= ums.getScreenHeight()) my = ums.getScreenHeight() -1;
}

/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */
