/*
 *  object.cpp: Base Class for all Ubit Objects
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
#include <typeinfo>
#include <cstdarg>
#include <ubit/core/node.h>
#include <ubit/core/string.h>
#include <ubit/core/application.h>
#include <ubit/core/appimpl.h>
using namespace std;
namespace ubit {


Error::Error(int _errnum, const Object* obj, const char* funcname) :
errnum(_errnum),
object(obj),
function_name(funcname) {
  message[0] = 0;
}

Error::~Error() throw () {
  //cerr << "Error destroyed " << (function_name? function_name:"")
  //<< " / " <<  (message?*message:"") << endl << endl;
}

const char* Error::what() const throw() {
  return message;
}


Object::UConst Object::UCONST;

const char* Object::getVersion() {
  return UBIT_VERSION;
}

const String& Object::getClassName() const {
  return getClass().getName();
}

void Object::error(const char* funcname, const char* format, ...) const {
  va_list ap;
  va_start(ap, format);
  Application::raiseError(Error::ERROR, this, funcname, format, ap);
  va_end(ap);
}

void Object::warning(const char* funcname, const char* format, ...) const {
  va_list ap;
  va_start(ap, format);
  Application::raiseError(Error::WARNING, this, funcname, format, ap);
  va_end(ap);
}

void uerror(const char* funcname, const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  Application::raiseError(Error::ERROR, null/*object*/, funcname, format, ap);
  va_end(ap);
}

void uwarning(const char* funcname, const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  Application::raiseError(Error::WARNING, null/*object*/, funcname, format, ap);
  va_end(ap);
}

Object& Object::setAutoUpdate(bool state) {
  omodes.DONT_AUTO_UPDATE = !state;
  return *this;
}

Object& Object::ignoreChangeCallbacks(bool state) {
  omodes.IGNORE_CHANGE_CALLBACKS = state;
  return *this;
}


void* Object::operator new(size_t sz) {
  Object* obj = (Object*) ::operator new(sz);
  // trick to detect whether this object is created by 'new' (see constructor)
  obj->ptr_count = (PtrCount)(long((obj)));
  return obj;
}

void Object::operator delete(void* p) {
  if (!p) return;
  Object* obj = static_cast<Object*>(p);
  
  if (!obj->omodes.IS_DYNAMIC) {
    Application::error("delete Object",
                  "%p: attempt to delete an object that was not created by new", p);
    return;
  }
  
  if (Application::impl.isTerminated()) {
    ::operator delete(p);
    return;
  }
  
  // NB: ptr_count<0 means that the object has already been destructed
  if (obj->ptr_count > 0) {
    Application::error("delete Object",
                  "%p: attempt to delete an object that is pointed by a 'std::unique_ptr' (Ubit smart pointer)", p);
    // peut etre rellement detruit plus tard
    obj->omodes.IS_DESTRUCTING = false;
    obj->omodes.IS_DESTRUCTED = false;
    return;
  }
  
  // this object will be deleted when this is safe to do so
  Application::impl.addDeleteRequest(obj);
}


Object::Object() {
  memset(&omodes, 0, sizeof(omodes));
  // object created by 'new' that can be destroyed by 'delete'
  if (ptr_count == (PtrCount)(long((this)))) omodes.IS_DYNAMIC = true; 
  ptr_count = 0;
  ostate = 0;
}

Object::Object(const Object& obj) {
  memset(&omodes, 0, sizeof(omodes));
  // object created by 'new' that can be destroyed by 'delete'
  if (ptr_count == (PtrCount)(long((this)))) omodes.IS_DYNAMIC = true; 
  ptr_count = 0;
}

Object& Object::operator=(const Object& obj) {
  // ptr_count et omodes ne doivent pas changer!
  return *this;
} 

Object::~Object() {
  ptr_count = -1;  // ptr_count<0 means that this object has been destructed
} 


Object& Object::setConst() {
  omodes.IS_CONST = true;
  return *this;
}

bool Object::checkConst() const {
  if (omodes.IS_CONST) {
    error("Node::checkConst","attempt to change the value of this constant object");
    return true;
  }
  else return false;
}

// PBM: addPtr() faux dans le cas des constantes types Color::white, etc.
// car l'ordre d'init des vars globales est aleatoire. ceci dit ca n'a aucune 
// importance car ces vars ne sont jamais detruitre en cours d'exec (le ptr_count 
// sera faux mais on n'en a cure)

void Object::addPtr() const {
  ++ptr_count;                           // @@@ !!!! MUST CHECK LIMIT !!!
} 

void Object::removePtr() const {
  --ptr_count;
  
  // ptr_count < 0 means that the object has already been destructed
  // CAN_DELETE means that teh object has been created by 'new'
  
  if (ptr_count == 0 && omodes.IS_DYNAMIC && !hasSceneGraphParent())
    delete this;     // detacher du graphe, detruire l'objet et ses enfants
}

void UPtr::deferenceError() const {
  Application::fatalError("std::unique_ptr::operator * or ->",
                     "can't derefence a std::unique_ptr that points to null; std::unique_ptr address= %p",
                     this);
}


}

