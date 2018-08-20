/*
 *  attr.cpp: Attributes of the Scene Graph
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

#include <iostream>
#include <memory>

#include <ubit/ubit_features.h>
#include <ubit/udefs.hpp>
#include <ubit/core/attribute.h>
#include <ubit/ui/window.h>
#include <ubit/core/on.h>
#include <ubit/core/call.h>
#include <ubit/ui/updatecontext.h>
#include <ubit/core/application.h>

using namespace std;

namespace ubit {


//bool Attribute::isSpecified() const {return getClass().isSpecified();}

// defini dans impl sinon pbms compil (a cause du std::unique_ptr<>) avce gcc2
Attribute::Attribute() {}
Attribute::Attribute(UConst) {omodes.IS_CONST = true;}

// defini dans impl sinon pbms compil
Attribute::~Attribute() {destructs();}
  
String Attribute::getNodeValue() const {
  String val; getValue(val); return val;
}

bool Attribute::getValue(String& val) const {
  val.clear();
  return false;
}

Attribute& Attribute::onChange(UCall& c) {
  addChangeCall(c);
  return *this;
}

void Attribute::changed(bool _update) {
  if (_update && !omodes.DONT_AUTO_UPDATE) update();
  
  if (!omodes.IGNORE_CHANGE_CALLBACKS) {
    if (_abegin() != _aend()) {
      Event e(UOn::change, this);  //UNodeEvent
      fire(e);
    }
    // ensuite on lance les callbacks des parents
    fireParents(UOn::propChange, this);
  }
}


struct UNamedValue: public Attribute {
  UNamedValue(const String& _name, const String& _value) 
  : pname(new String(_name)), pvalue(new String(_value)) {}
    
  virtual const String& getName() const {return *pname;}   //attention herite!!!
  virtual bool getValue(String& val) const {val = *pvalue; return true;}  //attention herite!!!  
  
  std::unique_ptr<String> pname, pvalue;
};


void AttributeList::addAttr(Attribute& c) {
  _addAttr(c);
}

void AttributeList::addAttr(const String& name, const String& value) {
  if (name.empty()) return;
  AttributeList::addAttr(*new UNamedValue(name, value));
}

void AttributeList::update() {
  updateAutoParents(Update::LAYOUT_PAINT);   // !! simplification  qui fait eventuellement des choses en trop...
}

String* AttributeList::getAttr(const String& name, bool ignore_case) {
  if (_abegin() == _aend()) return null;
 
  String* res = null;         // renvoie le dernier trouve!
  UNamedValue* p;
   for (ChildIter a = _abegin(); a != _aend(); ++a) {
     if ((p = dynamic_cast<UNamedValue*>(*a)) && p->getName().equals(name,ignore_case)) {
       res = p->pvalue;
    }
  }
  return res;
}

void AttributeList::putProp(UpdateContext *props, Element& par) {
  if (_abegin() == _aend()) return;
   Attribute* p;
   for (ChildIter a = _abegin(); a != _aend(); ++a) {
    if ((p = dynamic_cast<Attribute*>(*a))) p->putProp(props, par);
  }
}


Tip::Tip(const char* label) {set(label);}
Tip::Tip(const Args& a) {set(a);}

Tip::~Tip() {destructs();}  // removingFrom() impose un destructeur

Tip& Tip::set(const char* label) {
  content = label ? new String(label) : null;
  return *this;
}

Tip& Tip::set(const Args& a) {
  content = new Element(a);
  return *this;
}

void Tip::addingTo(Child& c, Element& parent) {
  Attribute::addingTo(c, parent);
  //if (parent.emodes.HAS_TIP) {
  //  warning("Tip::addingTo","This Tip has a parent (%s %p) that contains another Tip object", this, &parent.getClassName(), &parent);
  //}
  // rendre parent sensitif aux events ad hoc
  parent.emodes.HAS_TIP = true;
}

void Tip::removingFrom(Child& c, Element& parent) {
  parent.emodes.HAS_TIP = false;
  Attribute::removingFrom(c, parent);
}

// ==================================================== Ubit Toolkit =========

Title::Title(const char* _s) {
  pstring = new String(_s);     // EX: pvalue!!!
  pstring->onChange(ucall(this, true, &Attribute::changed));
}

Title::Title(const String& _s) {
  pstring = new String(_s);     // EX: pvalue!!!
  pstring->onChange(ucall(this, true, &Attribute::changed));
}

void Title::update() {
  for (ParentIter p = pbegin(); p != pend(); ++p) {
    Element* grp = *p;
    Window* win = grp ? grp->toWin() : null;
    if (win) win->setTitle(*pstring);     // EX: pvalue!!!
  } 
}

/*
Comment::Comment(const char* _s) : pvalue(new String(_s)) {}
Comment::Comment(const String& _s) : pvalue(new String(_s)) {}
*/

}
