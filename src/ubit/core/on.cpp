/*
 *  on.cpp: UOn Callback Conditions
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
#include <iostream>
#include <ubit/udefs.hpp>
#include <ubit/core/call.h>
#include <ubit/core/on.h>
#include <ubit/core/element.h>
#include <ubit/ui/updatecontext.h>
#include <ubit/core/event.h>
#include <ubit/core/string.h>
using namespace std;

namespace ubit {


typedef  UOn UMode;
  
UOn UOn::mpress(1,  UMode::MOUSE_CB);
UOn UOn::mrelease(2,UMode::MOUSE_CB);
UOn UOn::click(3,   UMode::MOUSE_CB);
UOn UOn::doubleClick(4, UMode::MOUSE_CB);
UOn UOn::enter(5,   UMode::MOUSE_CROSS_CB, UMode::ENTERED);
UOn UOn::leave(6,   UMode::MOUSE_CROSS_CB);  
//UOn UOn::mrelax(7,  UMode::MOUSE_CB);
UOn UOn::mmove(8,   UMode::MOUSE_MOVE_CB);
UOn UOn::mdrag(9,   UMode::MOUSE_DRAG_CB);
UOn UOn::wheel(10,  UMode::MOUSE_CB);

UOn UOn::kpress(11,  UMode::KEY_CB);
UOn UOn::krelease(12,UMode::KEY_CB);
UOn UOn::ktype(13,   UMode::KEY_CB);

UOn UOn::idle(20,   UMode::MOUSE_CB, UMode::IDLE);  // il FAUT un callmask non nul

UOn& UOn::arm =
  *new UOn2(21, UMode::MOUSE_CB, UMode::ARMED, &Element::setArmable);
UOn& UOn::disarm = 
  *new UOn2(22, UMode::MOUSE_CB, UMode::ANY, &Element::setArmable);

// NB: action n'entraine pas necessairement CAN_ARM :
// ce n'est vrai que si on n'est pas en CAN_EDIT_TEXT
UOn UOn::action(23,  UMode::MOUSE_CB, UMode::IDLE); // il FAUT un callmask non nul

/* ATT: le mode SELECTED pose probleme car ce n'est pas un mode qu'il faut affecter
* au parent mais juste un referent pour le UOn (actuellement c'est regle en ne rajoutant
*  pas ces modes et en testant directement l'egalite des UOn dans fire() et match()
*/
// ?? setArmableAndSelectable doit il exister et etre different de setSelectable ??
UOn& UOn::select = 
  *new UOn2(25, UMode::MOUSE_CB, UMode::ANY, &Element::_setArmableAndSelectable); 
UOn& UOn::deselect = 
  *new UOn2(26, UMode::MOUSE_CB, UMode::ANY, &Element::_setArmableAndSelectable); 

UOn UOn::change(31,    UMode::CHANGE_CB);
UOn UOn::propChange(32,UMode::CHILD_CHANGE_CB);
UOn UOn::dataChange(33,UMode::CHILD_CHANGE_CB);
UOn UOn::strChange(34, UMode::CHILD_CHANGE_CB);
//UOn UOn::caretChange(35, UMode::CHILD_CHANGE_CB);

UOn& UOn::dragStart = 
  *new UOn2(41, UMode::DND_CB, UMode::ANY, &Element::setDraggable);
UOn& UOn::dragDone = 
  *new UOn2(42,  UMode::DND_CB, UMode::ANY, &Element::setDraggable);
UOn& UOn::dropEnter = 
  *new UOn2(43, UMode::DND_CB, UMode::ANY, &Element::setDroppable);
UOn& UOn::dropLeave = 
  *new UOn2(44, UMode::DND_CB, UMode::ANY, &Element::setDroppable);
UOn& UOn::dropDone = 
  *new UOn2(45,  UMode::DND_CB, UMode::ANY, &Element::setDroppable);

//UOn UOn::message(47, UMode::MESSAGE_CB);
  
UOn UOn::destruct(50,   UMode::DESTRUCT_CB);
UOn UOn::addChild(51,   UMode::CONTAINER_CB);
UOn UOn::removeChild(52,UMode::CONTAINER_CB);
UOn UOn::close(53, UMode::SHOW_HIDE_CB);

UOn UOn::show(57,  UMode::SHOW_HIDE_CB);
UOn UOn::hide(58,  UMode::SHOW_HIDE_CB);
UOn UOn::paint(60,  UMode::VIEW_PAINT_CB);
UOn UOn::motion(61, UMode::VIEW_CHANGE_CB);
UOn UOn::resize(62, UMode::VIEW_CHANGE_CB);

UOn UOn::winState(70,UMode::WIN_STATE_CB);
UOn UOn::sysWM(72, UMode::SYSWM_CB);
//UOn UOn::filterEvent(80, UMode::FILTER_EVENT_CB);
UOn UOn::userEvent(100,  UMode::USER_EVENT_CB);

/*
UOn::UOn(short _id) 
  : ID(_id), callback_mask(UMode::USER_EVENT_CB), istate(UMode::ANY) {}

UOn::UOn(short _id, long _callback_mask) 
  : ID(_id), callback_mask(_callback_mask), istate(UMode::ANY) {}
*/
  
UOn::UOn(int _id, long _callback_mask, int _state) 
  : ID(_id), callback_mask(_callback_mask), istate(_state) {}

UOn2::UOn2(int _id, long _callback_mask, int _state, SetModeFunc f) 
  : UOn(_id, _callback_mask, _state), set_mode_func(f) {}
  

bool UOn::operator==(const Condition& c) const {
  const UOn* on = c.toOn();
  return on ? (ID == on->ID) : false;
}

bool UOn::operator!=(const Condition& c) const  {
  const UOn* on = c.toOn();
  return on ? (ID != on->ID) : false;
}

const Condition* UOn::matches(const Condition& c) const {
  const UOn* on = c.toOn();
  if (on && ID == on->ID) return this;
  else return null;
}

// NB: programme' pour faire un OU entre modes et action
bool UOn::verifies(const UpdateContext&, const Element& par) const {
  if (*this == UOn::select) {
    return par.isSelected();
  }
  else  if (*this == UOn::deselect) {
    return !par.isSelected();
  }

  //!!!PBM: pas le meme test que pour les callbacks!!
  if (istate != UMode::ANY) return (par.ostate == istate);
  else return false;
}

/* ATTENTION: PROBLEME
* setModes() pose probleme avec select/unselect car ce n'est pas un mode
* qu'il faut propager au parent mais juste un referent pour le UOn
* (pour l'instant c'est regle en ne rajoutant pas ces modes et
* en testant directement l'egalite des UOn dans fire() et match()
*/

void UOn::setParentModes(Element& parent) const {
  parent.callback_mask |= this->callback_mask;
}

void UOn2::setParentModes(Element& parent) const {
  UOn::setParentModes(parent);
  (parent.*set_mode_func)(true);
}

} 
