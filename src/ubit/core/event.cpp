/*
 *  event.cpp: Ubit Event
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2009 | Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
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

#include <ubit/ubit_features.h>
#include <iostream>
#include <ubit/ugl.hpp>
#include <ubit/ucall.hpp>
#include <ubit/uon.hpp>
#include <ubit/ubox.hpp>
#include <ubit/ustr.hpp>
#include <ubit/ukey.hpp>
#include <ubit/uupdatecontext.hpp>
#include <ubit/ui/uviewImpl.hpp>
#include <ubit/ui/uwinImpl.hpp>
#include <ubit/ucolor.hpp>
#include <ubit/ugraph.hpp>
#include <ubit/core/uappliImpl.hpp>
#include <ubit/ueventflow.hpp>
#include <ubit/umsproto.hpp>
#include <ubit/umessage.hpp>
#include <ubit/uflag.hpp>
using namespace std;
namespace ubit {


static const int _BUTTONS = 
  Modifier::LeftButton | Modifier::MidButton | Modifier::RightButton;

/*
class UNodeEvent : public UAnyEvent {
public:
  UNodeEvent(const Condition&, Node* source, Node* target = null);  
  
  virtual UNodeEvent* toNodeEvent() {return this;}  
  
  virtual Node* getSource() const {return source;}  
  Node* getTarget() const {return target;}
  
protected:
  Node* source;      // the object that produced this event
  Node* target;
};

class UElemEvent : public UAnyEvent {
public:
  UElemEvent(const Condition&, Element* source, Node* target = null);  
  
  virtual UElemEvent* toElemEvent() {return this;}
  
  virtual Element* getSource() const {return source;}  
  
  Node* getTarget() const {return target;}
  **< returns an object that is related to this event (or null).
   * the returned object depends on the event condition (@see getCond()).
   * Examples: for UOn::change/add/remove returns the object that was 
   * modified, selected, added, removed...
   *

protected:
  Element* source;
  Node* target;
};

 class Event : public UAnyEvent, public Modifier {
protected:
  Event(const Condition& c) : UAnyEvent(c) {} 
  
public:
  virtual Element* getSource() const = 0;
  ///< returns the object that received this event.  
  
 //void Event::goThrough(Box* _obj, bool _go_through_children) {
 //  aux = _obj;
 //  if (_go_through_children) sp.modes |=  UEventProps::GO_THROUGH_CHILDREN;
 //  else sp.modes |=  UEventProps::GO_THROUGH;
 //}
  
  // - - - Impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
  //protected:  
  void addFlagdef(const UFlagdef*);
  ///< [advanced] adds a Flag to the Event (for transparent tools, etc); see: UFlag.
  
  const UFlagdef* getFlagdef(const UFlag&) const;
  ///< [advanced] returns the flagdef associated to this flag (if any); see: UFlag.
  
  const UPropdef* getPropdef(const UFlag&) const;
  ///< [advanced] returns the flagprop associated to this flag (if any); see: UFlag.
  
  // void goThrough(Box*, bool go_through_children = true);
  // events will go through this object.
  
  std::vector<const UFlagdef*>flagdefs;   // event flags
#endif
};

 void Event::addFlagdef(const UFlagdef* f) {
 flagdefs.push_back(f);
 }
 
 const class UFlagdef* Event::getFlagdef(const UFlag &f) const {
 for (unsigned int k = 0; k < flagdefs.size(); ++k) {
 if ((flagdefs)[k]->getFlag() == &f) return flagdefs[k];
 }
 return null;  // not found
 }
 
 const UPropdef* Event::getPropdef(const UFlag& f) const {
 const UPropdef* last_pdef = null;
 // att: comme il peut y avoir plusieurs propdef empilees il faut
 // toujours prendre la derniere
 
 for (unsigned int k = 0; k < flagdefs.size(); ++k) {
 const UPropdef* pdef = null;
 if ((flagdefs)[k]->getFlag() == &f
 && (pdef = dynamic_cast<const UPropdef*>((flagdefs)[k]))
 ) {
 last_pdef = pdef;
 }
 }
 
 return last_pdef;
 }
 */
// ==================================================== ======== ===============

Event::Event(const Condition& c, UObject* s, UObject* a)
: cond(&c), source(s), aux(a) {} 

Event::~Event() {}
 
void Event::setCond(const Condition& c) {cond = &c;}

//UNodeEvent::UNodeEvent(const Condition& c, Node* s, Node* t) 
//: UAnyEvent(c), source(s), target(t) {}

// ==================================================== ======== ===============
/*
UElemEvent::UElemEvent(const Condition& _c, Element* _src, Node* _target)
: UAnyEvent(_c), source(_src), target(_target) {}
  */

UWinEvent::UWinEvent(const Condition& c, View* v, int t, int s) : 
Event(c, (v? v->getBox(): null)), 
source_view(v), 
type(t), 
state(s) {
}

//Box* UWinEvent::getSource() const {
//  return source_view ? source_view->getBox() : null;
//}

USysWMEvent::USysWMEvent(const Condition& c, View* v, void* sev) : 
Event(c, (v? v->getBox(): null)), 
source_view(v), 
sys_event(sev) {
}

//Box* USysWMEvent::getSource() const {
//  return source_view ? source_view->getBox() : null;
//}
  
// ==================================================== ======== ===============

UInputEvent::UInputEvent(const Condition& c, View* v, EventFlow* f, 
                         unsigned long time, int st) : 
Event(c, (v? v->getBox(): null)), 
state(st), 
when(time),
source_view(v),
eflow(f),
event_observer(null) {
  modes.PROPAGATE = false;
  modes.IS_MDRAG = modes.IS_FIRST_MDRAG = modes.IS_BROWSING 
  = modes.DONT_CLOSE_MENU = modes.SOURCE_IN_MENU = false;
} 

Element* UInputEvent::getSource() const {
  return source_view ? source_view->getBox() : null;
}

View* UInputEvent::getViewOf(const Box& box) const {
  return box.getView(*this);
}

View* UInputEvent::getWinView() const {
  return source_view ? source_view->getWinView() : null;
}
  
Window* UInputEvent::getWin() const {
  return (source_view && source_view->hardwin) ? source_view->hardwin->win : null;
}

Display* UInputEvent::getDisp() const {
  return (source_view && source_view->hardwin) ? source_view->hardwin->disp : null;
}
  
//#if UBIT_WITH_GLUT
//int UInputEvent::getModifiers() const {return glutGetModifiers();}
//#endif

int UInputEvent::getModifiers() const {return state;}

bool UInputEvent::isShiftDown() const 
{return (getModifiers() & Modifier::ShiftDown) != 0;}

bool UInputEvent::isControlDown() const 
{return (getModifiers() & Modifier::ControlDown) != 0;}

bool UInputEvent::isAltDown() const 
{return (getModifiers() & Modifier::AltDown) != 0;}

bool UInputEvent::isMetaDown() const
{return (getModifiers() & Modifier::MetaDown) != 0;}

bool UInputEvent::isAltGraphDown() const 
{return (getModifiers() & Modifier::AltGraphDown) != 0;}

// ==================================================== ======== ===============
  
UMouseEvent::UMouseEvent(const Condition& c, View* source, EventFlow* f, 
                         unsigned long time, int state,
                         const Point& pos, const Point& abs_pos, int btn) :
UInputEvent(c, source, f, time, state), 
button(btn), click_count(0),
pos(pos), abs_pos(abs_pos) {
}
  
int UMouseEvent::getButtons() const {
  return state & _BUTTONS;
}

void UMouseEvent::translatePos(float x, float y) {  // NB: change pas source !
  pos.x += x; pos.y += y;
  abs_pos.x += x; abs_pos.y += y;
}

Point UMouseEvent::getPosIn(const View& view) const {
  if (!source_view) return Point(0,0);
  else return View::convertPosTo(view, *source_view, pos);
}

Point UMouseEvent::getPosIn(const Box& parent) const {
  if (!source_view) return Point(0,0);
  View* v = parent.getViewContaining(*source_view);
  if (v) return View::convertPosTo(*v, *source_view, pos);
  else return Point(0,0);
}

Point UMouseEvent::getPosAndViewIn(const Box& parent, View*& parent_view) const {
  if (!source_view) return Point(0,0);
  parent_view = parent.getViewContaining(*source_view);
  if (parent_view) return View::convertPosTo(*parent_view, *source_view, pos);
  else return Point(0,0);
}

// ==================================================== ======== ===============

Data* UMouseEvent::getData() {
  if (!source_view) return null;
  UDataContext dc;
  return source_view->findData(dc, pos, null, 0, -1);
}

Data* UMouseEvent::getData(UDataContext& dc) {
  if (!source_view) return null;
  return source_view->findData(dc, pos, null, 0, -1);
}

// same as findData() but discards UDatas that are not Strings
String* UMouseEvent::getStr() {
  if (!source_view) return null;
  UDataContext dc;
  Data* data = getData(dc);
  return (data ? data->toStr() : null);
}

String* UMouseEvent::getStr(UDataContext& dc) {
  if (!source_view) return null;
  Data* data = getData(dc);
  return (data ? data->toStr() : null);
}

// ==================================================== ======== ===============

UWheelEvent::UWheelEvent(const Condition& c, View* source, EventFlow* f, 
                         unsigned long time, int state,
                         const Point& pos, const Point& abs_pos, 
                         int wheel_btn, int wheel_delta) :
UMouseEvent(c, source, f, time, state, pos, abs_pos, wheel_btn), 
delta(wheel_delta) {
} 
    
// ==================================================== ======== ===============

UKeyEvent::UKeyEvent(const Condition& c, View* source, EventFlow* f, unsigned long when, 
                     int mods, int kcode, short kchar)
: UInputEvent(c, source, f, when, mods), keycode(kcode), keychar(kchar) {}

void UKeyEvent::setKeyChar(short ch) {keychar = ch;}
void UKeyEvent::setKeyCode(int ks) {keycode = ks;}

// ==================================================== ======== ===============

UViewEvent::UViewEvent(const Condition& c, View* v) : 
Event(c, (v? v->getBox(): null)), 
source_view(v) {
}
  
Box* UViewEvent::getSource() const {
  return source_view ? source_view->getBox() : null;
}

Display* UViewEvent::getDisp() const {
  UHardwinImpl* hw = null;
  if (source_view && (hw = source_view->getHardwin())) return hw->getDisp();
  else return null;
}

// ==================================================== ======== ===============

TimerEvent::TimerEvent(const Condition& c, Timer* t, unsigned long w) 
: Event(c, t), /*timer(t),*/ when(w) {}

UserEvent::UserEvent(const Condition& c, UObject* s, int t, void* d) 
: Event(c, s), /*source(s),*/ type(t), data(d) {}

MessageEvent::MessageEvent(const Condition& c, MessagePort* p) 
: Event(c, p) /*,port(p)*/ {}

const String* MessageEvent::getMessage() const {
  //if (port) return &port->getValue(); else return null;
  if (source) return &((MessagePort*)source)->getValue(); 
  else return null;
}

// ==================================================== ======== ===============

UResizeEvent::UResizeEvent(const Condition& c, View* v) : UViewEvent(c, v) {} 

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

PaintEvent::PaintEvent(const Condition& c, View* v, const Rectangle* winclip) : 
UViewEvent(c, v),
current_context(null),
layout_view(null)
{
  if (winclip) {
    is_clip_set = true;
    redraw_clip = *winclip;
  }
  else {
    is_clip_set = false;
    redraw_clip.setRect(0,0,0,0);
  }
} 
 
bool PaintEvent::setSourceAndProps(View* v) {
  UViewContext vc;
  bool stat = v->findContext(vc, View::FIND_CLIP);
  source_view = v;
  setProps(vc);
  return stat;
}

void PaintEvent::setProps(const UViewContext& vc) {
  is_clip_set = vc.is_clip_set;
  redraw_clip = vc.clip;
  layout_view = vc.layout_view;
}
  
void PaintEvent::getClip(Rectangle& r) const {
  if (source_view && is_clip_set) {
    r.x = redraw_clip.x - source_view->x;
    r.y = redraw_clip.x - source_view->y;
    r.width  = redraw_clip.width;
    r.height = redraw_clip.height;
  }
  else if (source_view) {
    r.x = r.y = 0;
    r.width  = source_view->width;
    r.height = source_view->height;
  }
  else {
    r.x = r.y = 0;
    r.width = r.height = 0;
  }  
}
    
/*
Rectangle PaintEvent::getGLClip() const {
  Rectangle r;
  getHardwinClip(r);       // pas tres clair: on veut le clip, ou la vue entiere
  Dimension s; hardwin->getSize(s);   // pour faire glviewport ?
  r.y = s.height - r.height - r.y;
  return r;
}
*/

Rectangle UDataContext::getBounds() const {
  if (!source_view) return Rectangle();
  else return Rectangle(win_clip.x - source_view->x,  //- dataContext->getView()->x, 
                    win_clip.y - source_view->y,  //- dataContext->getView()->y, 
                    win_clip.width, win_clip.height);
}

UDataContext::UDataContext() {
  data = null;
  it = it2 = ChildIter();
  strpos = -1;
  strpos2 = -1;       // !must be se to -1 if unused
  exact_match = false;
  //winclip.setRect(0,0,0,0); // !important pour set/union
  //dataContext = null;  // !important pour ~UDataContext
}

UDataContext::~UDataContext() {
  //delete winContext;
  //delete dataContext;
}

// -- exact_match   = true  : Data is exactly located under the Mouse
//                    false : Data is the last Data before the Mouse position
// -- merge_regions = true  : region 'r' is merged with the previous 'region'
//                    false : region 'r' replaces the previous value of 'region'
//
void UDataContext::set(UpdateContext& ctx, Data* d, ChildIter _it, ChildIter end,
                       const Rectangle& r, int _strpos, bool _exact_match) {
  data = d;
  it = _it;
  it2 = end;
  strpos = _strpos;   //don't change strpos2!
  //if (!dataContext) dataContext = new UpdateContext(ctx);
  exact_match = _exact_match;
  win_clip = r;
}

void UDataContext::merge(UpdateContext& ctx, Data* d, ChildIter _it, ChildIter end,
                       const Rectangle& r, bool _exact_match) {
  data = d;
  it = _it;
  it2 = end;  //don't change strpos and strpos2!
  //if (!dataContext) dataContext = new UpdateContext(ctx);
  exact_match = _exact_match;
  win_clip.doUnion(r);
}

}