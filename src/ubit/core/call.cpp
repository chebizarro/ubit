/*
 *  call.cpp: Callback Object
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
#include <ubit/ucall.hpp>
#include <ubit/uon.hpp>
#include <ubit/ui/box.h>
#include <ubit/core/event.h>
#include <ubit/ui/window.h>
#include <ubit/ui/view.h>
#include <ubit/ui/menu.h>
#include <ubit/ui/timer.h>
#include <iostream>
using namespace std; 

namespace ubit {


void UCall::wrongEventType(const char* evname, const char* callname) {
  error("operator",
         " - this callback requires an event of type: %s \n"
         " - but the type of the actual event is: %s \n"
         "To solve this problem, put a breakpoint in ubit::UCall::wrongEventType()",         evname, callname);
}

void UCall::addingTo(Child& child, Element& parent) {
  // sert a specifier la condition par defaut
  if (!child.getCond()) child.setCond(UOn::action);
  Node::addingTo(child, parent);
}

/*
 void UCall::removingFrom(Child *selflink, Element *parent) {
 // ne PAS faire " par->setCmodes(on, false) " car il peut y avoir
 // plusieurs callbacks avec la meme condition
 // (et de toute facon ce n'est pas une erreur, juste un peu plus long)
 Node::removingFrom(selflink, parent);
 }
 */

UCall& ucloseWin(int stat) {return ucall(stat, &Element::closeWin);}

/* TEST
 template <class A, class R> 
 class Truc_F1 {
 const Condition& cond;
 R (*fun)(A);
 public:
 Truc_F1(const Condition& c, R(*f)(A)) : cond(c), fun(f) {}
 Child operator()(A a) {return Child(new UCall_F1<A,R>(fun,a), cond);}
 };
 
 template <class A, class R, class E> 
 class Truc_F1E {
 const Condition& cond;
 R (*fun)(E&,A);
 public:
 Truc_F1E(const Condition& c, R(*f)(E&,A)) : cond(c), fun(f) {}
 Child operator()(A a) {return Child(new UCall_F1E<A,R,E>(fun,a), cond);}
 };
 
 - - - - - - 
 
 template <class A1, class A2, class R> 
 class Truc_F2 {
 const Condition& cond;
 R (*fun)(A1,A2);
 public:
 Truc_F2(const Condition& c, R(*f)(A1,A2)) : cond(c), fun(f) {}
 Child operator()(A1 a1, A2 a2) 
 {return Child(new UCall_F2<A1,A2,R>(fun,a1,a2), cond);}
 };
 
 template <class A1, class A2, class R, class E> 
 class Truc_F2E {
 const Condition& cond;
 R (*fun)(E&,A1,A2);
 public:
 Truc_F2E(const Condition& c, R(*f)(E&,A1,A2)) : cond(c), fun(f) {}
 Child operator()(A1 a1, A2 a2) 
 {return Child(new UCall_F2E<A1,A2,R,E>(fun,a1,a2), cond);}
 };
 */

/*
 //UCall& ucompactEvents(UCall& c) {return c;}
 
 UCompactEvents::UCompactEvents(UCall& c) :
 postponed_event(*new Event(Event::timer, null, null, null)),
 timer(*new Timer()),
 pcall(c),
 delay(0) {
 timer.onAction(ucall(this, &UCompactEvents::timeout));
 }
 
 UCompactEvents::~UCompactEvents() {
 delete &timer;
 delete &postponed_event;
 }
 
 void UCompactEvents::operator()(Event& e) {
 //postponed_event.copy(e);
 postponed_event = e;
 timer.start(delay,1,false);
 }
 
 void UCompactEvents::timeout() {
 //postponed_event.setTime(e.getTime());
 (*pcall)(postponed_event);
 }
 */

}

