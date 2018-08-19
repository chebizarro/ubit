/*
 *  number.cpp: ACtive Numbers (can have callbacks)
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
#include <cstdlib>
#include <cstdio>
#include <ubit/core/call.h>
#include <ubit/core/on.h>
#include <ubit/core/string.h>
#include <ubit/core/event.h>
#include <ubit/core/number.h>
#include <ubit/core/application.h>
using namespace std;

namespace ubit {


  
Number& Number::onChange(UCall &c) {   // @@@ devrait heriter de UObject !!!
  addChangeCall(c);
  return *this;
}

void Number::changed(bool update_now) {
  /// if (update_now && (bmodes & UMode::NO_AUTO_UPDATE) == 0) update();

  if (!omodes.IGNORE_CHANGE_CALLBACKS) {
    if (_abegin() != _aend()) {
      Event e(UOn::change, this);  //UNodeEvent
      fire(e);
    }
    // ensuite on lance les callbacks des parents
    fireParents(UOn::propChange, this);
  }
}


Int::Int(const String& s) {
  setImpl(s.c_str(), false);  // dont call callbacks
}

Int::Int(const string& s) {
  setImpl(s.c_str(), false);  // dont call callbacks
}


Int& Int::operator=(const String& s) {
  setImpl(s.c_str(), true);  // call callbacks
  return *this;
}

Int& Int::operator=(const std::string& s) {
  setImpl(s.c_str(), true);  // call callbacks
  return *this;
}

istream& operator>>(istream& s, Int& v) {
  int n;
  s >> n;
  v.setImpl(n, true);  // call callbacks
  return s;
}

ostream& operator<<(ostream& s, const Int& v) {
  return (s << v.value);
}


Int Int::operator++(int) {
  Int clone = *this;
  setImpl(value+1);
  return clone;
}

Int  Int::operator--(int) {
  Int clone = *this;
  setImpl(value+1);
  return clone;
}


Int& Int::setImpl(int v, bool call_cb) {
  if (checkConst()) return *this;
  if (value == v) return *this;
  value = v;
  if (call_cb) changed(true);
  return *this;
}

Int& Int::setImpl(const char* s, bool call_cb) {
  if (checkConst()) return *this;
  if (!s) {
    value = 0;
    Application::warning("Int::set","null char* argument");
  }
  else return setImpl(strtol(s, null, 0), call_cb);
  return *this;
}

String Int::toString() const {
  char _s[50] = "";
  sprintf(_s, "%d", value);
  return _s;
}


Float::Float(const String& s) {
  setImpl(s.c_str(), false);  // dont call callbacks
}

Float::Float(const string& s) {
  setImpl(s.c_str(), false);  // dont call callbacks
}


Float& Float::operator=(const String& s) {
  setImpl(s.c_str(), true);  // call callbacks
  return *this;
}

Float& Float::operator=(const std::string& s) {
  setImpl(s.c_str(), true);  // call callbacks
  return *this;
}

istream& operator>>(istream& s, Float& v) {
  float n;
  s >> n;
  v.setImpl(n, true);  // call callbacks
  return s;
}


ostream& operator<<(ostream& s, const Float& v) {
  return (s << v.value);
}

Float Float::operator++(int) {
  Float clone = *this;
  (*this)++;
  return clone;
}

Float Float::operator--(int) {
  Float clone = *this;
  (*this)++;
  return clone;
}


Float& Float::setImpl(float v, bool call_cb) {
  if (checkConst()) return *this;
  if (value == v) return *this;
  value = v;
  if (call_cb) changed(true);
  return *this;
}

Float& Float::setImpl(const char* s, bool call_cb) {
  if (checkConst()) return *this;
  if (!s) {
    value = 0;
    Application::warning("Double::set","null char* argument");
  }
  else return setImpl(strtod(s, null), call_cb);
  return *this;
}

String Float::toString() const {
  char _s[50] = "";
  sprintf(_s, "%f", value);
  return _s;
}


//Double::Double(const char* s) {
//  setImpl(s, false);  // dont call callbacks
//}

Double::Double(const String& s) {
  setImpl(s.c_str(), false);  // dont call callbacks
}

Double::Double(const string& s) {
  setImpl(s.c_str(), false);  // dont call callbacks
}


Double& Double::operator=(const String& s) {
  setImpl(s.c_str(), true);  // call callbacks
  return *this;
}

Double& Double::operator=(const std::string& s) {
  setImpl(s.c_str(), true);  // call callbacks
  return *this;
}

istream& operator>>(istream& s, Double& v) {
  double n;
  s >> n;
  v.setImpl(n, true);  // call callbacks
  return s;
}


ostream& operator<<(ostream& s, const Double& v) {
  return (s << v.value);
}

Double Double::operator++(int) {
  Double clone = *this;
  (*this)++;
  return clone;
}

Double Double::operator--(int) {
  Double clone = *this;
  (*this)++;
  return clone;
}


Double& Double::setImpl(double v, bool call_cb) {
  if (checkConst()) return *this;
  if (value == v) return *this;
  value = v;
  if (call_cb) changed(true);
  return *this;
}

Double& Double::setImpl(const char* s, bool call_cb) {
  if (checkConst()) return *this;
  if (!s) {
    value = 0;
    Application::warning("Double::set","Null char* argument");
  }
  else return setImpl(strtod(s, null), call_cb);
  return *this;
}

String Double::toString() const {
  char _s[50] = "";
  sprintf(_s, "%f", value);
  return _s;
}

}
