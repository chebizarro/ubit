/*
 *  eventflow.cpp: Event Flow
 *  Ubit GUI Toolkit - Version 8
 *  (C) 2018 Chris Daley
 *  (C) 2009 | Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */

#include <ubit/ubit_features.h>
#include <algorithm>
#include <ubit/uboxgeom.hpp>
#include <ubit/ui/window.h>
#include <ubit/ui/menu.h>
#include <ubit/ui/umenuImpl.hpp>
#include <ubit/ui/uwinImpl.hpp>
#include <ubit/core/application.h>
#include <ubit/core/appimpl.h>
#include <ubit/core/event.h>
#include <ubit/ui/eventflow.h>
#include <ubit/Selection.hpp>
#include <ubit/upix.hpp>
#include <ubit/uon.hpp>
#include <ubit/ui/timer.h>
#include <ubit/draw/color.h>
#include <ubit/ui/background.h>
#include <ubit/ui/updatecontext.h>
#include <ubit/ui/uviewImpl.hpp>
#include <ubit/ui/3d.h>
using namespace std;
namespace ubit {


int EventFlow::TIP_X_SHIFT = 20;

void EventFlow::LastPressed::reset() {
  view = null;
  box = null;
  canvas_view = null;
  refpos_in_canvas = null;
  behavior = UBehavior::MOUSE;
}

void EventFlow::LastPressed::set(View* source_view, 
                                  const Point& win_pos, const Point& screen_pos, 
                                  const ViewFind& vf) {
  view = source_view;
  box = view ? view->getBox() : null;
  // pos of the window in the screen (for drag and release events)
  win_in_screen.x = screen_pos.x - win_pos.x;
  win_in_screen.y = screen_pos.y - win_pos.y;
  canvas_view = vf.canvas_view;           // != null if the source is in a 3Dwidget
  refpos_in_canvas = vf.refpos_in_canvas; // 3Dpos of 3Dwidget in canvas_view (if it is != null)
  behavior = vf.bp;
}
  
EventFlow::EventFlow(Display& _d, int _channel) :
id(-1),     // not init!
channel(_channel),
disp(_d),
lastEntered(null),
currentFocus(null),
beingClicked(null),
auto_repeat_timer(new Timer(Application::conf.auto_repeat_delay, -1, false)),
tip_timer(new Timer(false)),
tip_win(*new Menu()),
menu_man(*new MenuManager(this)),
selection(*new Selection(disp.getConfig().selection_color,
                          disp.getConfig().selection_bgcolor,
                          disp.getConfig().selection_font)),
user_data(null) {
  lastArmed     = null;
  dragSource    = dropTarget = null;
  lastCursor    = null;
  click_time    = 0;
  click_count   = 0;
  click_pos.x = click_pos.y = -5;
  auto_repeat_timer->onAction(ucall(this, &EventFlow::autoRepeatCB));
  tip_timer->onAction(ucall(this, &EventFlow::openTipCB));
  tip_win.addAttr(Background::wheat);
  tip_win.addAttr(ualpha(0.8));
  tip_win.addAttr(upadding(2,2));
  disp.add(tip_win);
  id = Application::impl.flowlist.size();
  Application::impl.flowlist.push_back(this);
}

EventFlow::~EventFlow() {
   // ici il faudrait notifier le Display !!!
  delete auto_repeat_timer;
  delete tip_timer;
  delete &tip_win;
  delete &menu_man;
  delete &selection;
  for (int k=0; k < (int)tele_pointers.size(); k++) {
    if (tele_pointers[k]) delete tele_pointers[k];
  }
}


void EventFlow::deleteNotify(View* del_view) {
  //cerr << "DEL_VIEW " << del_view << endl;
  if (del_view == lastEntered) lastEntered = null;
  if (del_view == lastPressed.view) lastPressed.reset();
  if (del_view == currentFocus) currentFocus = null;
}

void EventFlow::deleteNotify(Element* del_box) {
  //cerr << "DEL_BOX " << del_box << endl;
  if (del_box == lastArmed)    lastArmed  = null;
  if (del_box == beingClicked) beingClicked  = null;
  if (del_box == dragSource)   dragSource = null;
  if (del_box == dropTarget)   dropTarget = null;
  if (del_box == selection.getObj()) selection.setObj(null);
}

void EventFlow::closeAllMenus() {
  getMenuManager().closeAllMenus(true);
}  
  

void EventFlow::setFocus(View* v) {
  currentFocus = v;
}

void EventFlow::setCursor(Event& e, const Cursor* c) {
  if (channel != 0) return;            // Cursor pas gere pour flowID > 0
  
  if (c != lastCursor) {
    lastCursor = c;
    UHardwinImpl* win = null;
    View* v = null;
    InputEvent* ie = e.toInputEvent();

    if (ie && (v = ie->getView()) && (win = v->getHardwin())) win->setCursor(c);
    // hum, un peu problematique, le curseur devrait etre un atribut du display
    // et now des window comme c'est le cas avec x11
  }
}
    
void EventFlow::startAutoRepeat(Event& e) {
  // !!! ici il faudrait faire en sorte que e.source_view soit correct !!!
  auto_repeat_timer->start();
}
  
void EventFlow::stopAutoRepeat(Event& e) {
  auto_repeat_timer->stop();
}
  
void EventFlow::autoRepeatCB(TimerEvent& e) {
  // !!! ici il faudrait faire en sorte que e.source_view soit correct !!!
  if (lastArmed) {
    e.setCond(UOn::arm);
    lastArmed->fire(e);
  }
  else auto_repeat_timer->stop(); // securite
}

// ==================================================== ===== =======
  
bool EventFlow::mustCloseMenus(View* source_view) {
  // NB: bug avec scrollpanes car les scrollbars sont dans border ce qui fait
  // echouer isChildOf => isMenuClosingDisabled() compense ca
  return 
  (menu_man.top_menu != null
   && (source_view == null        // mouse outside GUI
       || (!source_view->getBox()->isMenuClosingDisabled()
           && !source_view->getBox()->isChildOf(*menu_man.top_menu)) // mouse outside menu
       ));
}
  
void EventFlow::redirectMousePress(MouseEvent& e, View* winview) {
  //e.setBrowsingGroup(getBrowsingGroup());
  //cerr << "redirect " << winview << " " << winview->getBox()->getClassName() << endl;
  mousePress(winview,  // should be the view of the menu
             e.when, e.state, 
             Point(e.getScreenPos().x - winview->getScreenPos().x,
                    e.getScreenPos().y - winview->getScreenPos().y),
             e.abs_pos, e.button);
}
  
// ==================================================== ===== =======
  
void EventFlow::mousePress(View* winview, unsigned long time, int state, 
                            const Point& win_pos, const Point& screen_pos, int btn) 
{
  Window* modalwin = static_cast<Window*>(Application::impl.modalwins->getChild(0)); //@@@??? why 0 ??
  if (modalwin && modalwin != winview->getBox()) {  // box outside modalwin
    modalwin->highlight(true);
    return;
  }
  Point source_pos;
  ViewFind vf(winview, win_pos, UBehavior::MOUSE, Element::Modes::CATCH_MOUSE_MASK);
  View* source_view = winview->findSource(vf, source_pos);
    
  if (mustCloseMenus(source_view)) {
    menu_man.closeAllMenus(true);
    return;
  }
  
  if (!source_view) return;   // must be AFTER test with closeAllMenus() !
  
  MouseEvent e(UOn::mpress, source_view, this, time, state, source_pos, screen_pos, btn);  
  e.event_observer = vf.bp.event_observer;
  e.modes.DONT_CLOSE_MENU = vf.bp.DONT_CLOSE_MENU;
  e.modes.SOURCE_IN_MENU = vf.bp.SOURCE_IN_MENU;
  
  if (Application::hasTelePointers()) showTelePointers(e, +1);
  
AGAIN:
  // security if the crossing was not detected by boxMotion()
  //boxCross(source_view, time, state, vf.bp);
  boxCross(source_view, time, state, vf.bp.cursor, false);
  
  Box* box = source_view->getBox();
  if (box->emodes.IS_TEXT_SELECTABLE 
      || (box->callback_mask & UOn::KEY_CB)
      || (box->emodes.CATCH_EVENTS & Element::Modes::CATCH_KEY_MASK)) {
    currentFocus = source_view;   // FOCUS!
    //cerr << "Focus " << box << " " << box->getClassName() << endl;
  }
  
  // ne pas changer lastPressed si on presse sur un 2eme bouton de 
  // la souris sans avoir relache le premier
  if (e.getButton() == e.getButtons()) {
    lastPressed.set(source_view, win_pos, screen_pos, vf);
  }
  
  // UOn::press AVANT UOn::armBehavior comme Java etc. 
  // fire() sur 'box' et non lastPressed car le press peut se faire sur une autre
  // source que celle de depart si on presse ensuite un autre bouton de la souris
  box->fire(e); 
  
  if (vf.catched && e.modes.PROPAGATE) {
    source_view = source_view->findSourceNext(vf, source_pos);
    if (!source_view) { // cas anormal!
      Application::internalError("EventFlow::mousePress","Null source view");
      return;
    }
    
    e.source_view = source_view;
    e.pos = source_pos;
    e.event_observer = vf.bp.event_observer;
    e.modes.DONT_CLOSE_MENU = vf.bp.DONT_CLOSE_MENU;
    e.modes.SOURCE_IN_MENU = vf.bp.SOURCE_IN_MENU;
    e.modes.PROPAGATE = false;
    goto AGAIN;
    // sinon cas standard : on execute la suite
  }
  
  //box->armBehavior(e, vf.bp);  // MOUSE, not browsing
  box->armBehavior(e, false);
  
  if (box->emodes.IS_TEXT_SELECTABLE && e.getButton() == Application::conf.mouse_select_button)
    selection.start(e);
}

// ==================================================== ===== =======

void EventFlow::mouseRelease(View* winview, unsigned long time, int state, 
                              const Point& win_pos, const Point& screen_pos, int btn) 
{  
  //if (Application::hasTelePointers()) showTelePointers(e, -1);
  // security if the crossing was not detected by boxMotion()
  //boxCross(e.source_view, e.when, e.state, Element::NORMAL);
  
  if (lastPressed.view) {
    // if lastPressed is set there is no reason to check the modal dialog condition

    // win_pos cannot be used because it is relative to the window that the mouse
    // is over when using X11: this won't be the lastPressed win if another window
    // was opened on the top of it in the meantime (typically a popup menu)
    Point where(screen_pos.x - lastPressed.win_in_screen.x,
                 screen_pos.y - lastPressed.win_in_screen.y);
    
    // if lastPressed was in a 3D widget, 'where' must be relative to this widget
    if (lastPressed.canvas_view) {
      lastPressed.canvas_view->unproject(lastPressed.refpos_in_canvas, win_pos, where);
    }
    
    // pos of source = pos in ref (win or 3Dwidget) - pos of source in ref
    where.x -= lastPressed.view->x;
    where.y -= lastPressed.view->y;

    /*
    cerr << endl <<"* mouseRelease: srcview: "<< lastPressed.view << " " 
    << (lastPressed.view ? lastPressed.view->getBox()->getClassName() : "")
    << " / pos-in-screen " << screen_pos.x << " " << screen_pos.y
    << " / pos-in-source " << where.x << " " << where.y
    << " / source-in-win " << lastPressed.view->x << " " << lastPressed.view->y
    << endl << endl;
    */
    
    MouseEvent e(UOn::mrelease, lastPressed.view, this, time, state, where, 
                  screen_pos, btn);
    e.event_observer = lastPressed.behavior.event_observer;
    e.modes.DONT_CLOSE_MENU = lastPressed.behavior.DONT_CLOSE_MENU;
    e.modes.SOURCE_IN_MENU = lastPressed.behavior.SOURCE_IN_MENU;
    lastPressed.box->fire(e);
      
    if (selection.beingSelected()) {
      if (selection.complete(e)) {
        // ! Alternative: Ctrl-C Pomme-C au lieu d'etre fait implicitement !!@@@
        disp.copySelection(e, selection);
      }
    }
  
    if (lastPressed.view) {  // verify that lastPressed has not been destroyed by fire()
      //lastPressed.box->disarmBehavior(e, lastPressed.behavior);
      lastPressed.box->disarmBehavior(e, false);
    }
    
    if (e.getButtons() == 0)  // plus aucun button pressed
      lastPressed.reset();
  }
}

// ==================================================== ===== =======

void EventFlow::mouseMotion(View* winview, unsigned long time, int state,
                             const Point& win_pos, const Point& screen_pos) {
  // MOUSE_DRAG CASE
  if (lastPressed.view) {
    // NB: if lastPressed is set there is no reason to check the modal dialog condition
    
    // win_pos cannot be used because it is relative to the window that the mouse
    // is over when using X11: this won't be the lastPressed win if another window
    // was opened on the top of it in the meantime (typically a popup menu)
    Point where(screen_pos.x - lastPressed.win_in_screen.x,
                 screen_pos.y - lastPressed.win_in_screen.y);
    
    // if lastPressed was in a 3D widget, 'where' must be relative to this widget
    if (lastPressed.canvas_view) {
      lastPressed.canvas_view->unproject(lastPressed.refpos_in_canvas, win_pos, where);
    }
    
    // pos of source = pos in ref (win or 3Dwidget) - pos of source in ref
    where.x -= lastPressed.view->x;
    where.y -= lastPressed.view->y;
    //cerr << where  << " / win " << win_pos << " / scr " << screen_pos << endl;

    // the button number is 0 for drag and movve events    
    MouseEvent e(UOn::mdrag, lastPressed.view, this, time, state, where, screen_pos, 0);
    e.event_observer = lastPressed.behavior.event_observer;
    e.modes.DONT_CLOSE_MENU = lastPressed.behavior.DONT_CLOSE_MENU;
    e.modes.SOURCE_IN_MENU = lastPressed.behavior.SOURCE_IN_MENU;
    e.modes.IS_MDRAG = true;
 
    if (selection.beingSelected() /* && box->emodes.IS_TEXT_SELECTABLE*/)
      selection.extend(e);
    
    if (Application::hasTelePointers()) showTelePointers(e);

    // Drag callbacks called if MOUSE_DRAG_CB is ON if we are in the
    if (lastPressed.box->hasCallback(UOn::MOUSE_DRAG_CB) || e.event_observer)
      lastPressed.box->fire(e);
  }
  
  // MOUSE_MOVE CASE
  else {
    ViewFind vf(winview, win_pos, UBehavior::MOUSE, Element::Modes::CATCH_MOUSE_MOVE_MASK);
    Point source_pos;
    View* source_view = winview->findSource(vf, source_pos);
    if (!source_view) return;
  
    MouseEvent e(UOn::mmove, source_view, this, time, state, source_pos, screen_pos, 0);
    e.event_observer = vf.bp.event_observer;
    e.modes.DONT_CLOSE_MENU = vf.bp.DONT_CLOSE_MENU;
    e.modes.SOURCE_IN_MENU = vf.bp.SOURCE_IN_MENU;

    // bool mbtn_pressed = (e.getButtons() != 0);
    //vf.bp.intype = mbtn_pressed? UBehavior::BROWSE: UBehavior::MOUSE;

    if (Application::hasTelePointers()) showTelePointers(e);

AGAIN:
    //boxCross(source_view, time, state, vf.bp);
    boxCross(source_view, time, state, vf.bp.cursor, vf.bp.intype == UBehavior::BROWSE);

    Box* box = source_view->getBox();
    if (box && (box->hasCallback(UOn::MOUSE_MOVE_CB) || e.event_observer))
      box->fire(e);
      
    if (vf.catched && e.modes.PROPAGATE) {
      source_view = source_view->findSourceNext(vf, source_pos);
      if (!source_view) { // cas anormal!
        Application::internalError("EventFlow::mouseMotion","Null source view");
        return;
      }
      e.source_view = source_view;
      e.pos = source_pos;
      e.event_observer = vf.bp.event_observer;
      e.modes.DONT_CLOSE_MENU = vf.bp.DONT_CLOSE_MENU;
      e.modes.SOURCE_IN_MENU = vf.bp.SOURCE_IN_MENU;
      e.modes.PROPAGATE = false;
      goto AGAIN;
    }
  }
}

// ==================================================== ===== =======

void EventFlow::wheelMotion(View* winview, unsigned long time, int state, 
                             const Point& win_pos, const Point& screen_pos,
                             int type, int delta) {
  Window* modalwin = static_cast<Window*>(Application::impl.modalwins->getChild(0));
  if (modalwin && modalwin != winview->getBox()) {  // box outside modalwin
    modalwin->highlight(true);
    return;
  }
  ViewFind vf(winview, win_pos, UBehavior::MOUSE/* !!! */, Element::Modes::CATCH_WHEEL_MASK);
  Point source_pos;
  View* source_view = winview->findSource(vf, source_pos);
  if (!source_view) return;
  
  if (mustCloseMenus(source_view)) {
    //cerr <<"wheelMotion mustCloseMenus()"<<endl;
    return;
  }
  
  UWheelEvent e(UOn::wheel, source_view, this, time, state, source_pos, 
                screen_pos, type, delta);
  e.event_observer = vf.bp.event_observer;
  e.modes.DONT_CLOSE_MENU = vf.bp.DONT_CLOSE_MENU;
  e.modes.SOURCE_IN_MENU = vf.bp.SOURCE_IN_MENU;

AGAIN:
  source_view->getBox()->fire(e);
  
  if (vf.catched && e.modes.PROPAGATE) {
    source_view = source_view->findSourceNext(vf, source_pos);
    if (!source_view) { // cas anormal!
      Application::internalError("EventFlow::wheelMotion","Null source view");
      return;
    }
    e.source_view = source_view;
    e.pos = source_pos;
    e.event_observer = vf.bp.event_observer;
    e.modes.DONT_CLOSE_MENU = vf.bp.DONT_CLOSE_MENU;
    e.modes.SOURCE_IN_MENU = vf.bp.SOURCE_IN_MENU;
    e.modes.PROPAGATE = false;
    goto AGAIN;
  }
}

// ==================================================== ===== =======

void EventFlow::keyPress(View* winview, unsigned long time, int state,
                          int keycode, short keychar) {
  if (currentFocus == null) {
    //cerr <<  "winKeyPress: NO CURRENT FOCUS"<<endl;
    return;
  }
  
  Window* modalwin = static_cast<Window*>(Application::impl.modalwins->getChild(0));
  if (modalwin && modalwin != currentFocus->getWin()) {  // focus outside modalwin
    // !!! ICI on pourrait changer le focus !!!
    modalwin->highlight(true);
    return;
  }

  //if (mustCloseMenus(source_view)) return;
  
  UKeyEvent e(UOn::kpress, currentFocus, this, time, state, keycode, keychar);
  Box* box = currentFocus->getBox();  
  //box->keyPressBehavior(e, UBehavior::KEY);
  box->keyPressBehavior(e);
  
  // efface ou detruit la selection suivant entered key
  if (box->emodes.IS_TEXT_SELECTABLE) selection.keyPress(e);
}  
  
// ==================================================== ===== =======

void EventFlow::keyRelease(View* winview, unsigned long time, int state,
                            int keycode, short keychar) {
  if (currentFocus == null) {
    //cerr <<  "winKeyRelease: NO CURRENT FOCUS"<<endl;
    return;
  } 
  
  Window* modalwin = static_cast<Window*>(Application::impl.modalwins->getChild(0));
  if (modalwin && modalwin != currentFocus->getWin()) {  // focus outside modalwin
    modalwin->highlight(true);
    return;
  }

  //if (mustCloseMenus(source_view)) return;

  UKeyEvent e(UOn::krelease, currentFocus, this, time, state, keycode, keychar);
  Box* box = currentFocus->getBox();
  //box->keyReleaseBehavior(e, UBehavior::KEY);
  box->keyReleaseBehavior(e);
}
  
// ==================================================== ===== =======
/*
 void EventFlow::winDestroy(Window *win, Event& e) {
   win->emodes.IS_SHOWABLE = false;
   if (menu_man.contains(win->toMenu()))  menu_man.closeAllMenus(true);
   delete win;
 }
*/
  
void EventFlow::winEnter(View* winview, unsigned long time) {                         
                            // !!! A COMPLETER....
  //UBehavior bp(UBehavior::MOUSE);
  //boxCross(winview, time, 0, bp); 
  //!!! should be the entered box PAS WINVIEW !!!@@@@
  boxCross(winview, time, 0, null, false); 
}
  
void EventFlow::winLeave(View* winview, unsigned long time) {
  // ATTENTION: le XGrab genere des LeaveWindow qd on ouvre le menu associe a un 
  // bouton et qd on bouge la souris sur ce bouton une fois que le menu est ouvert.
  // Ne pas en tenir compte sinon le bouton ouvrant oscille entre Enter et Leave 
  // qd on deplace la souris sur le menubar
  
  //cerr << "winview  "<<winview << endl;
  //cerr << "--  "<<winview->getBox()<< endl;
  //cerr << "--  "<<winview->getBox()->getClassName() << endl;
  
  if (menu_man.contains(winview->getBox()->toMenu())) return;
  
  //UBehavior bp(UBehavior::MOUSE);
  //boxCross(null, time, 0, bp);
  boxCross(winview, time, 0, null, false); 
}
  
void EventFlow::boxCross(View* new_view, unsigned long time, int state, 
                           const Cursor* cursor, bool is_browing) {   
  if (lastEntered == new_view) return;

  if (lastEntered) {
    InputEvent e(UOn::leave, lastEntered, this, time, state);
    lastEntered->getBox()->leaveBehavior(e, is_browing);
    lastEntered = null;
  }
    
  lastEntered = new_view;

  if (new_view) {
    // ATT: e.source_view==null si on est hors de la window (auquel cas
    // il ne faut PAS faire boxEnter()
    InputEvent e(UOn::enter, lastEntered, this, time, state);
    setCursor(e, cursor);
    lastEntered->getBox()->enterBehavior(e, is_browing);
  }
}

// ==================================================== ===== =======
 
void EventFlow::openTipRequest(Event& e) {
#ifndef UBIT_WITH_GLUT
                                               // !!! bug avec GLUT a corriger !!!
  if (lastEntered) {
    tip_timer->start(Application::conf.open_tip_delay,1);
  }
#endif
}

void EventFlow::closeTipRequest(Event& e) {
#ifndef UBIT_WITH_GLUT
                                                 // !!! bug avec GLUT a corriger !!!
  tip_timer->stop();
  // ne pas ouvrir/fermer tip_win comme un menu mais comme une window normale
  tip_win.Window::show(false);
  tip_win.removeAll();
#endif
}

void EventFlow::openTipCB(TimerEvent& e) {
  if (lastEntered) {
    Box* box = lastEntered->getBox();
    UTip* tip = null;
    box->attributes().findClass(tip);
    if (!tip) box->children().findClass(tip);
    
    if (tip && tip->content) {
      tip_win.add(*tip->content);
      tip_win.setPos(*lastEntered, Point(TIP_X_SHIFT, lastEntered->getHeight()));
      // ne pas ouvrir/fermer tip_win comme un menu mais comme une window normale
      tip_win.Window::show(true, lastEntered->getDisp());
      tip_win.update(Update::ADJUST_WIN_SIZE);
    }
  }
}


static const char* telepointer_xpm[] = {
  " 8 8 2 1",
  ". c white",
  "# c None s None",
  "........",
  ".#######",
  ".######.",
  ".#####..",
  ".#####..",
  ".######.",
  ".##..##.",
  ".#.....#",
};
static UPix telepointer_pix(telepointer_xpm, UPix::UCONST);


Window* EventFlow::retrieveTelePointer(Display* rem_disp) {
  unsigned int disp_id = rem_disp->getID();
    
  if (disp_id < tele_pointers.size() && tele_pointers[disp_id]) {
    return tele_pointers[disp_id];
  }
  else {
    for (unsigned int k = tele_pointers.size(); k <= disp_id; k++) 
      tele_pointers.push_back(null);
    
    Window* ptr = new Menu(Border::none
                          + Color::red
                          + Background::red
                          + UOn::select/Background::green
                          + telepointer_pix
                          );
    ptr->ignoreEvents();
    tele_pointers[disp_id] = ptr;
    ptr->setSoftwin(false);
    rem_disp->add(*ptr);
    ptr->show();
    return ptr;
  }
}
  
// il y a un pbm du au fait que les vues des objects conditionnels
  // sont toujours generees. non seulement c'est inutilement couteux
  // (cf le zoom semantique etc) mais en plus ca fait deconner les
  // telepointeurs car ces vues conditionnelles n'ont pas de sens
  // (et ne sont d'ailleurs pas affichees)
  
void EventFlow::showTelePointers(MouseEvent& e, int mode)  {
  Box* source = e.getSource() ? e.getSource()->toBox() : null;
  if (!source) return;
  
  for (View* v = source->getView(0); v != null; v = v->getNext()) {
    if (v->getDisp() != e.getDisp()) {
      Window* remptr = retrieveTelePointer(v->getDisp());
      if (remptr){
        if (mode) {
          if(mode> 0) remptr->setSelected(true);
          else if(mode< 0) remptr->setSelected(false);
          remptr->update();
        }
        remptr->toFront();
        Point pos = v->getScreenPos(); 
        remptr->setScreenPos(Point(pos.x + e.getX(), pos.y + e.getY()));
      }
    }
  }
}


}
