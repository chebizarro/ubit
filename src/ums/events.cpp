/************************************************************************
*
*  events.cpp - UMS Server
*  Part of the Ubit GUI Toolkit
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

//EX: include <ubit/ubit_config.h>
#include "umserver.hpp"
#include <cstdio>
#include "flow.hpp"
#include "events.hpp"
#ifdef MACOSX
#  include "macevents.hpp"
#endif
using namespace std;
 
bool Events::has_xtest = true;

/* ==================================================== [(c)Elc] ======= */
   
Events::Events(UMServer* _ums) : ums(*_ums) {
#ifdef MACOSX
  macev = new MacEvents(_ums);
#else
  macev = null;
#endif
}

Events::~Events() {
#ifdef MACOSX
  delete macev;
#endif
}

/* ==================================================== ======== ======= */

void Events::leaveLocalDisplay() {
#ifdef MACOSX
  macev->leaveLocalDisplay();
#else
#endif  
}

void Events::enterLocalDisplay() {
#ifdef MACOSX
  macev->enterLocalDisplay();
#else
#endif
}

/* ==================================================== [(c)Elc] ======= */

void Events::getNativePointer(long& x, long& y) {
#ifdef MACOSX
  macev->getPointer(x, y);
#else
  getXPointer(x, y);
#endif
}

void Events::getPointer(const MouseFlow& flow, long& x, long& y) {
  if (flow.id == 0) 
    getNativePointer(x, y);
  else {
    x = flow.getX();
    y = flow.getY();
  }
}

bool Events::getPointer(const MouseFlow& flow, Pos& pos, bool check_props) {
  if (flow.id == 0)
    return getXPointer(pos); //att: initialise PAS in_ubitwin!
  else
    return ums.getXWin(flow.getX(), flow.getY(), pos, check_props);
}

/* ==================================================== ======== ======= */

void Events::warpNativePointer(long x, long y) {
#ifdef MACOSX
  macev->warpPointer(x, y);
#else
  warpXPointer(x, y);
#endif
}

// devrait etre dans events.cpp
void Events::warpPointer(MouseFlow& flow, long x, long y) {
  if (flow.id == 0)
    warpNativePointer(x, y);
  else 
    flow.movePointer(x, y);
}

/* ==================================================== [(c)Elc] ======= */

void Events::sendMessage(USocket* sock, const char* message) {
  if (!message) return;
  int ll = strlen(message) + 1;
  sock->sendBlock(message, ll);
}

/* ==================================================== ======== ======= */

void Events::sendMotion(MouseFlow& flow, long x, long y, bool move_ptr) {
#ifndef MACOSX
  sendXMotion(flow, x, y, move_ptr);
#else
  if (flow.id == 0) {
    macev->sendMotion(flow, x, y, move_ptr);
  }
  else {
    Pos pos;
    if (getPointer(flow, pos, true) && pos.in_ubitwin)
      sendXMotion(flow, x, y, move_ptr);
    else
      macev->sendMotion(flow, x, y, move_ptr);
  }
#endif
}

/* ==================================================== ======== ======= */

void Events::sendButton(MouseFlow& flow, int btn_id, bool press, Pos* use_pos) {

#ifndef MACOSX
  sendXButton(flow, btn_id, press, use_pos);
#else
  if (flow.id == 0) {
    if (btn_id & (WheelUpID | WheelDownID))
      sendXButton(flow, btn_id, press, use_pos);  // HACK pour wheel !!!
    else {
      long x, y;
      macev->getPointer(x, y);
      macev->sendButton(flow, x, y, btn_id, press, true/*move_ptr*/);
    }
  }
  else if (use_pos) {
    if (use_pos->in_ubitwin)
      sendXButton(flow, btn_id, press, use_pos);
    else 
      macev->sendButton(flow, use_pos->rx, use_pos->ry, btn_id, press, false);
  }
  else {  // flow_id != 0
    Pos pos;
    if (getPointer(flow, pos, true) && pos.in_ubitwin)
      sendXButton(flow, btn_id, press, &pos);
    else
      macev->sendButton(flow, pos.rx, pos.ry, btn_id, press, false);
  }
#endif
}

/* ==================================================== ======== ======= */

void Events::sendKey(MouseFlow& flow, unsigned long keysym,
                     bool press, Pos* use_pos) {
#ifdef MACOSX
  sendXKey(flow, keysym, press, use_pos);   // A FAIRE!!!
#else
  sendXKey(flow, keysym, press, use_pos);
#endif
}

/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */


