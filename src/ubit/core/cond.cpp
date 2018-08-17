/*
 *  cond.cpp: Callback Conditions (see also class UOn)
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
#include <ubit/ucall.hpp>
#include <ubit/ucond.hpp>
#include <ubit/uelem.hpp>
#include <ubit/uupdatecontext.hpp>
#include <ubit/core/event.h>
#include <ubit/ustr.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

/* TEST
 template <class A, class R> 
 Truc_F1<A,R> operator/(R(*f)(A)) {return Truc_F1<A,R>(*this,f);}
 
 template <class A, class R, class E> 
 Truc_F1E<A,R,E> operator/(R(*f)(E&,A)) {return Truc_F1E<A,R,E>(*this,f);}
 
 template <class A1, class A2, class R> 
 Truc_F2<A1,A2,R> operator/(R(*f)(A1,A2)) {return Truc_F2<A1,A2,R>(*this,f);}
 
 template <class A1, class A2, class R, class E> 
 Truc_F2E<A1,A2,R,E> operator/(R(*f)(E&,A1,A2)) {return Truc_F2E<A1,A2,R,E>(*this,f);}
 */

MultiCondition& MultiCondition::add(const Condition& c) {
  condlist.push_back(&c);
  return *this;
}

void MultiCondition::remove(const Condition& c) {
  for (CondList::iterator p = condlist.begin(); p != condlist.end(); ++p) {
    if (*p == &c) {
      condlist.erase(p);
      return;
    }
  }
}

// ATT: renvoie *p et non c !
const Condition* MultiCondition::matches(const Condition& c) const {
  for (CondList::const_iterator p = condlist.begin(); p != condlist.end(); ++p) {
    if ((*p)->matches(c)) return *p;
  }
  return null;
}

bool MultiCondition::verifies(const UpdateContext& ctx, const Element& par) const {
  for (CondList::const_iterator p = condlist.begin(); p != condlist.end(); ++p) {
    if ((*p)->verifies(ctx, par)) return true;
  }
  return false;
}

void MultiCondition::setParentModes(Element& parent) const {
  for (CondList::const_iterator p = condlist.begin(); p != condlist.end(); ++p) {
    (*p)->setParentModes(parent);
  }
}

}
