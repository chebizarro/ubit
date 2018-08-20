/*
 *  class.cpp: Ubit MetaClasses
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
#include <ubit/core/string.h>
#include <ubit/core/class.h>
#include <ubit/core/uclassImpl.hpp>
#include <ubit/core/application.h>
#include <ubit/core/appimpl.h>
using namespace std;
namespace ubit {



unsigned int Class::count = 0;
  
Class::Class(const char* n) : 
no(++count), name(new String(n)), style(null), attributes(null) {
}

Class::Class(const String& n) : 
no(++count), name(new String(n)), style(null), attributes(null) {
}

Class::~Class() {
  delete name;
  delete style;
}

/*
//#define UBIT_CLASS(CC) \
//static  const Class& Class() {static Class& c = *new USubclass<CC>(#CC); return c;} \
//virtual const Class& getClass() const {return Class();}    

template <class CC>
struct USubclass : public Class {
  USubclass(const char* name): Class(name) {}
  virtual bool isInstance(Object& obj) const {return dynamic_cast<CC*>(&obj);}
  virtual Style* newStyle() const {return CC::createStyle();}
};

template <class CC>
inline const Class& MetaClass(const char*cc_name) {static Class& c = *new USubclass<CC>(cc_name); return c;}
*/

StyleSheet::~StyleSheet() {
  for (Map::iterator k = map.begin(); k != map.end(); k++) {
    delete k->second; // deletes the nodes
  }
}


ElementClassMap::~ElementClassMap() {}

const Class* ElementClassMap::findClass(const String& classname) const {
  Map::const_iterator k = map.find(&classname);
  if (k == map.end()) return null;
  else return k->second;
}

// add or replace
void ElementClassMap::addClass(const Class& c) {
  Map::iterator k = map.find(&c.getName());
  if (k != map.end()) map.erase(k);
  map[&c.getName()] = &c;
}

const Class* ElementClassMap::obtainClass(const String& classname) {
  const Class* c = findClass(classname);
  if (c) return c;
  else {
    c = new UDefaultInlineElement::MetaClass(classname);   // !!! A REVOIR !!!
    map[&c->getName()] = c;
    return c;
  }
}


AttributeClassMap::~AttributeClassMap() {
  //for (Map::iterator k = map.begin(); k != map.end(); k++) {
  //  delete k->second; // deletes the Class
  //}
}

const Class* AttributeClassMap::findClass(const String& classname) const {
  Map::const_iterator k = map.find(&classname);
  if (k == map.end()) return null;
  else return k->second;
}

// add or replace
void AttributeClassMap::addClass(const Class& c) {
  Map::iterator k = map.find(&c.getName());
  if (k != map.end()) map.erase(k);
  map[&c.getName()] = &c;
}

const Class* AttributeClassMap::obtainClass(const String& classname) {
  const Class* c = findClass(classname);
  if (c) return c;
  else {
    c = new UDefaultAttribute::MetaClass(classname);   // !!! A REVOIR !!!
    map[&c->getName()] = c;
    return c;
  }
}

}
