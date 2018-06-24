/************************************************************************
*
*  macevents.cpp
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

#include "umserver.hpp"
#ifdef MACOSX

#include <cstdlib>       // getenv, atexit
#include <cstdio>
#include <string>
#include <unistd.h>       // darwin
#include "source.hpp"
#include "flow.hpp"
#include "macevents.hpp"
using namespace std;

/* ==================================================== [(c)Elc] ======= */

void MacEvents::getPointer(long& x, long& y) {
  Point p;
  GetGlobalMouse(&p);
  x = p.h;
  y = p.v;
}

void MacEvents::warpPointer(long x, long y) {
  CGPoint pos;
  pos.x = x;
  pos.y = y;
  CGWarpMouseCursorPosition(pos);
}

void MacEvents::warpPointerToCenter() {
  warpPointer(ums.getScreenWidth()/2, ums.getScreenHeight()/2);
}

/* ==================================================== ======== ======= */
// use_pos : envoyer event a cette position directement sans gerer le
// MouseFlow ni afficher le ptr (utilise par sendMousePress(), etc)

void MacEvents::sendButton(MouseFlow& flow, long x, long y, 
                           int btn_id, bool press, bool move_ptr) {  
  // att: b2 et b3 inverses sur Mac par rapport a X
  // recuper l'etat des boutons (enfonces auparavant)
  boolean_t b1 = (flow.btn_mask & MouseLeftID) != 0;
  boolean_t b3 = (flow.btn_mask & MouseMiddleID) != 0;
  boolean_t b2 = (flow.btn_mask & MouseRightID) != 0;

  if (press) {
    // rajouter le nouveau bouton (un seul a la fois!)
    if (btn_id & MouseLeftID) b1 = true;
    if (btn_id & MouseMiddleID) b3 = true;
    if (btn_id & MouseRightID) b2 = true; 
  }
  else {
    // enlever ce bouton (car mis a true car encore dans btn_mask)
    if (btn_id & MouseLeftID) b1 = false;
    if (btn_id & MouseMiddleID) b3 = false;
    if (btn_id & MouseRightID) b2 = false;
  }
  
  CGPoint point;
  point.x = x;
  point.y = y;
  /*
  // si use_pos, la pos est donnee en arg
  if (use_pos) {
    pos.x = use_pos->rx;
    pos.y = use_pos->ry;
  }
  // sinon il faut localiser le ptr avant d'envoyer l'event
  else {
    long x, y;
    getPointer(flow, x, y);
    pos.x = x;
    pos.y = y;
  }
   */
  
  // on ne met a jour le ptr (updateMouseCursorPosition = true)
  // que pour ptr natif Mac et si pas pseudo
  //if (!use_pos && flow.id == 0) {
  if (move_ptr) {
    CGPostMouseEvent(point, true, //updateMouseCursorPosition
                     3/*buttonCount*/, b1, b2, b3);
  }
  else {
    // sinon c'est le pointeur qui recupere l'event!
    flow.showPointer(false);
    CGPostMouseEvent(point, false, //updateMouseCursorPosition
                     3/*buttonCount*/, b1, b2, b3);

    // afficher le ptr et le remettre le ptr au 1er plan
    flow.showPointer(true);
  }
}

/* ==================================================== ======== ======= */

void MacEvents::sendMotion(MouseFlow& flow, long x, long y, bool move_ptr) {
  // recuperer l'etat des boutons
  boolean_t b1 = (flow.btn_mask & MouseLeftID) != 0;
  boolean_t b3 = (flow.btn_mask & MouseMiddleID) != 0;
  boolean_t b2 = (flow.btn_mask & MouseRightID) != 0;
  
  CGPoint pos;
  pos.x = x;
  pos.y = y;

  //static int no;
  //cerr << "AppleMove " << ++no << " pos " << pos.x <<" " << pos.y
  //  << " btn " << b1 <<":"<< b2 <<":"<< b3 << endl;

  // on ne met a jour le ptr (updateMouseCursorPosition = true)
  // que pour ptr natif Mac
  if (flow.id == 0) {
    CGPostMouseEvent(pos, move_ptr, //updateMouseCursorPosition
                     3/*buttonCount*/, b1, b2, b3);
  }
  else {
    CGPostMouseEvent(pos, false, //updateMouseCursorPosition
                     3/*buttonCount*/, b1, b2, b3);
    // remettre le ptr au 1er plan, mais APRES l'event sinon c'est le ptr
    // qui recupere l'event
    //if (move_ptr) warpPointer(flow, x, y);
    if (move_ptr) flow.movePointer(x, y);
  }
}

/* ==================================================== ======== ======= */
/*tests
WindowRef tstwin;
DragTrackingHandlerUPP tracking_handler; 
DragReceiveHandlerUPP receive_handler; 

static OSErr trackingCB(DragTrackingMessage message, WindowRef theWindow, 
                 void* handlerRefCon, DragRef theDrag) {  
  cerr << "trackingCB "<< endl;
}

static OSErr receiveCB(WindowRef theWindow, void* handlerRefCon, DragRef theDrag) {
  cerr << "receiveCB "<< endl;
}
*/

MacEvents::MacEvents(UMServer* _ums) : 
ums(*_ums), 
grabwin(0), 
scr_xcenter(0), scr_ycenter(0),
left_btn_pressed(false), right_btn_pressed(false)
{
  OSStatus err; 

  // create grab window 
  // this win catches events on the local screen. these events are then sent 
  // to the remote screen by processEventCB()
  Rect rect = {0, 0, 400, 400};
  CreateNewWindow(kUtilityWindowClass,
                  kWindowNoShadowAttribute | kWindowOpaqueForEventsAttribute,
                  &rect, &grabwin);
  // NB: sans kWindowOpaqueForEventsAttribute une win totalement transparente
  // ne recoit plus aucun event!
  SetWindowAlpha(grabwin, 0);  // transparent

  static EventTypeSpec event_spec[] = {
  {kEventClassMouse, kEventMouseDown},
  {kEventClassMouse, kEventMouseUp},
  {kEventClassMouse, kEventMouseDragged},
  {kEventClassMouse, kEventMouseMoved},
  {kEventClassMouse, kEventMouseWheelMoved},
  {kEventClassKeyboard, kEventRawKeyUp},
  {kEventClassKeyboard, kEventRawKeyDown},
  {kEventClassKeyboard, kEventRawKeyRepeat},
  {kEventClassKeyboard, kEventRawKeyModifiersChanged}
  };
  event_handler = NewEventHandlerUPP(processEventCB);
  err = InstallWindowEventHandler(grabwin, event_handler, 9, event_spec,
                                  this, null); // UserData, EventHandlerRef
  if (err!=noErr) cerr<<"MacEvents: InstallWindowEventHandler failed"<< endl;
     
  /* (test) Drag and drop
  Rect rect2 = {0, 0, 400, 700};
  CreateNewWindow(kUtilityWindowClass,
                  //kWindowIgnoreClicksAttribute, 
                  kWindowNoActivatesAttribute,
                  &rect2, &tstwin);
  SetWindowAlpha(tstwin, 0.5);
  ShowWindow(tstwin);

  tracking_handler = NewDragTrackingHandlerUPP(trackingCB);
  err = InstallTrackingHandler(tracking_handler, tstwin, null);
  if (err!=noErr) cerr<<"MacEvents: InstallTrackingHandler failed"<< endl;
  
  receive_handler  = NewDragReceiveHandlerUPP(receiveCB);
  err = InstallReceiveHandler(receive_handler, tstwin, null);
  if (err!=noErr) cerr<<"MacEvents: InstallReceiveHandler failed"<< endl;
  */
}

MacEvents::~MacEvents() {
  DisposeEventHandlerUPP(event_handler);
  ReleaseWindow(grabwin);
  /*
  RemoveTrackingHandler(tracking_handler, tstwin);
  RemoveReceiveHandler(receive_handler, tstwin);
  ReleaseWindow(tstwin);
  */
}

/* ==================================================== ======== ======= */
 
void MacEvents::leaveLocalDisplay() {
  scr_xcenter = ums.getScreenWidth()/2;
  scr_ycenter = ums.getScreenHeight()/2;
  warpPointer(scr_xcenter, scr_ycenter);

  MouseFlow& f = ums.getNativeMouseFlow();
  if (ums.edges->x_crossings > 0) 
    f.syncMouse(ums.getScreenWidth(), f.getY());
  else if (ums.edges->x_crossings < 0) 
    f.syncMouse(-1, f.getY());
  else if (ums.edges->y_crossings > 0) 
    f.syncMouse(f.getX(), ums.getScreenHeight());
  else if (ums.edges->y_crossings < 0) 
    f.syncMouse(f.getX(), -1);

  SetMouseCoalescingEnabled(false, null);
  CGSetLocalEventsSuppressionInterval(0.0);
  ShowWindow(grabwin);
  SetUserFocusWindow(grabwin);
  RepositionWindow(grabwin, grabwin, kWindowCenterOnMainScreen);
}

void MacEvents::enterLocalDisplay() {
  SetMouseCoalescingEnabled(true, null);
  CGSetLocalEventsSuppressionInterval(HUGE_VAL);
  HideWindow(grabwin);
}

/* ==================================================== ======== ======= */

OSStatus MacEvents::processEventCB(EventHandlerCallRef, EventRef ev, void* data) {
  if (!ev) return noErr;
  MacEvents* me = (MacEvents*)data;
  UInt32 evclass = GetEventClass(ev);
  
  switch (evclass) {
    case kEventClassMouse: 
      me->processMouseEvents(ev);
      break;
      
    case kEventClassKeyboard: 
      me->processKeyEvents(ev);
      break;
  }
  
  return noErr;
}

/* ==================================================== ======== ======= */

void MacEvents::processMouseEvents(EventRef ev) {
  MouseFlow& flow = ums.getNativeMouseFlow();

  switch (GetEventKind(ev)) {
    case kEventMouseDragged:
    case kEventMouseMoved: {
      HIPoint point;
      GetEventParameter(ev, kEventParamMouseLocation, typeHIPoint, 
                        null, sizeof(point), null, &point);
      warpPointer(scr_xcenter, scr_ycenter);

      int mx = flow.getX() + int(point.x) - scr_xcenter;
      int my = flow.getY() + int(point.y) - scr_ycenter;
    
      //cerr << "mouse " << mx <<" " << my
      //  << " / delta " << (point.x - scr_xcenter) << " " << (point.y - scr_ycenter)
      //  << " / point " << point.x << " " << point.y<< endl;

      bool ok = flow.moveMouseOutsideDisplay(mx, my);
      if (!ok) { // mouse inside local scr or connection is broken
        ums.moveNativePointerBackHome(false);
      }
    } break;
      
    case kEventMouseDown: {
      UInt16 btn;
      GetEventParameter(ev, kEventParamMouseButton, typeMouseButton, 
                        null, sizeof(btn), null, &btn);
      // att: 2et3 sont inverses sous Mac
      if (btn == 1) left_btn_pressed = true;
      else if (btn == 2) right_btn_pressed = true;
      
      // MOVE_BACK si on clique A LA FOIS sur left et right buttons
      if (left_btn_pressed && right_btn_pressed) {
        left_btn_pressed = right_btn_pressed = false;
        ums.moveNativePointerBackHome(true);
      }
      else flow.pressMouse(Events::mbuttonToMask(btn));
    } break;

    case kEventMouseUp: {
      UInt16 btn;
      GetEventParameter(ev, kEventParamMouseButton, typeMouseButton, 
                        null, sizeof(btn), null, &btn);
      // att: 2et3 sont inverses sous Mac
      if (btn == 1) left_btn_pressed = false;
      else if (btn == 2) right_btn_pressed = false;
      
      flow.releaseMouse(Events::mbuttonToMask(btn));
    } break;
           
    case kEventMouseWheelMoved: {
      EventMouseWheelAxis wheelax;
      SInt32 delta;
      GetEventParameter(ev, kEventParamMouseWheelAxis, typeMouseWheelAxis,
                        null, sizeof(wheelax), null, &wheelax);
      
      if (wheelax == kEventMouseWheelAxisY) {
        GetEventParameter(ev, kEventParamMouseWheelDelta, typeLongInteger,
                          null, sizeof(delta), null, &delta);
        if (delta > 0)
          ums.getMouseFlow(0)->pressMouse(WheelUpID);
        else
          ums.getMouseFlow(0)->pressMouse(WheelDownID);
      }
    } break;
  }
}

/* ==================================================== ======== ======= */

void MacEvents::processKeyEvents(EventRef ev) {
  UInt32 evkind = GetEventKind(ev);
  switch (evkind) {
    case kEventRawKeyUp:
    case kEventRawKeyDown:
    case kEventRawKeyRepeat:
    case kEventRawKeyModifiersChanged: {
      UInt32 virtualKey;
      GetEventParameter(ev, kEventParamKeyCode, typeUInt32,
                        null, sizeof(virtualKey), null, &virtualKey);
      //cerr << "keyevent " << evkind << " " << virtualKey << endl;
    } break;
  }
}

#endif
/* ==================================================== =TheEnd= ======= */
/* ==================================================== [(c)Elc] ======= */

