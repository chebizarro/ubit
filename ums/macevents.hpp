/*************************************************************************
 *
 *  macevents.hpp - UMS Server
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

#ifndef _macevents_hpp_
#define	_macevents_hpp_
#include "events.hpp"
#define __CARBONSOUND__ 1  // hack pour eviter un bug de mise a jour de MacOSX
#include <Carbon/Carbon.h>

class MacEvents {
public:
  MacEvents(UMServer*);
  ~MacEvents();
  
  void leaveLocalDisplay();
  void enterLocalDisplay();
  
  void getPointer(long& x, long& y);
  void warpPointer(long x, long y);
  void warpPointerToCenter();
  
  void sendButton(MouseFlow&, long x, long y, int btn_id, bool press, bool move_ptr);
  void sendMotion(MouseFlow&, long x, long y, bool move_ptr);
  //void sendKey(MouseFlow&, unsigned long keysym, bool press, Pos* use_pos);
  
  static OSStatus processEventCB(EventHandlerCallRef, EventRef, void* macev);
  void processMouseEvents(EventRef);
  void processKeyEvents(EventRef);
  
  UMServer& ums;
  WindowRef grabwin;
  int scr_xcenter, scr_ycenter;
  bool left_btn_pressed, right_btn_pressed;
  EventHandlerUPP event_handler;
};

#endif
