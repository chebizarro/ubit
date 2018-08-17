/*
 *  args.cpp : class for creating argument lists ("additive notation").
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
#include <ubit/udefs.hpp>
#include <ubit/core/node.h>
#include <ubit/ucall.hpp>
#include <ubit/uargs.hpp>
#include <ubit/ustr.hpp>
#include <ubit/uappli.hpp>
using namespace std;
namespace ubit {


const Args Args::none;

/* test for counting creation/deletion balance
int Ncreate, Ndelete;
UArgsChildren::UArgsChildren() {Ncreate++;}
UArgsChildren::~UArgsChildren() {Ndelete--;}
*/
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Child operator/(const Condition& cond, Node& b) {
  return Child(&b, cond);
}

Child operator/(const Condition& cond, Node* b) {
  if (!b) {
    Application::error("Args::operator/","null Node argument in / specification");
    return Child(null, cond);
  }
  else return Child(b, cond);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool Args::empty() const {
  return children->empty();
}

bool Args::operator!() const {
  return children->empty();
}

Args::~Args() {
  if (children->refcount > 1) children->refcount--;
  else if (children->refcount == 1) delete children;
}

Args::Args() {
  children = new UArgsChildren;
  children->refcount = 1;
}

Args::Args(Node* b) {
  children = new UArgsChildren;
  children->refcount = 1;
  if (!b) Application::error("Args::Args","null Node argument in arglist");
  else children->push_back(b);
}
  
Args::Args(Node& b) {
  children = new UArgsChildren;
  children->refcount = 1;
  children->push_back(b);
}

Args::Args(const char* s) {
  children = new UArgsChildren;
  children->refcount = 1;
  children->push_back(new String(s));
}

Args::Args(const Child& c) {
  children = new UArgsChildren;
  children->refcount = 1;
  if (!*c) Application::error("Args::Args","Child argument pointing to null node");
  else children->push_back(c);
}

Args::Args(const Args& a) : children(a.children) {
  if (!children) {     // cas Args::none pas encore initialise...
    children = new UArgsChildren;
    children->refcount = 0;
  }
  children->refcount++;
}

Args::Args(const UArgsImpl& a) : children(a.children) {
  if (!children) {     // cas Args::none pas encore initialise...
    children = new UArgsChildren;
    children->refcount = 0;
  }
  children->refcount++;
}

Args& Args::operator+=(const Args& a2) {
  // marche pas car operator* est redefini
  // children->insert(children->end(), a2.children->begin(), a2.children->end());
  for (ChildIter i = a2.children->begin(); i != a2.children->end(); ++i)
    children->push_back(i.child());
  return *this;
}

// ========================================================= [Elc] ===========

UArgsImpl::~UArgsImpl() {
  if (children->refcount > 1) children->refcount--;
  else if (children->refcount == 1) delete children;
}

UArgsImpl::UArgsImpl() {
  children = new UArgsChildren;
  children->refcount = 1;
}

UArgsImpl::UArgsImpl(const UArgsImpl& a) : children(a.children) {
  if (!children) {  // cas Args::none pas encore initialise...
    children = new UArgsChildren;
    children->refcount = 0;
  }
  children->refcount++;
}

// dans ce cas il ne faut pas augmenter a, sinon une expression comme :
//    ubox(args + string)
// aurait pour effet de rajouter string a args
UArgsImpl::UArgsImpl(const Args& a) {
  children = new UArgsChildren;
  *children = *a.children;   // !! la difference importante est ici !!
  children->refcount = 1;
}

UArgsImpl::UArgsImpl(const char* s) {
  children = new UArgsChildren;
  children->refcount = 1;
  children->push_back(new String(s));
}

UArgsImpl::UArgsImpl(Node* b) {
  children = new UArgsChildren;
  children->refcount = 1;
  if (!b) Application::error("Args","null argument in arglist");
  else children->push_back(b);
}

UArgsImpl::UArgsImpl(Node& b) {
  children = new UArgsChildren;
  children->refcount = 1;
  children->push_back(b);
}

UArgsImpl::UArgsImpl(const Child& c) {
  children = new UArgsChildren;
  children->refcount = 1;
  if (!*c) Application::error("Args","null Child argument in arglist");
  else children->push_back(c);
}

const UArgsImpl& operator+(const UArgsImpl& a, const char* s) {
  a.children->push_back(new String(s));
  return a;
}

const UArgsImpl& operator+(const UArgsImpl& a, Node* b) {
  if (!b) Application::error("Args::operator+","null Node in arglist");
  else a.children->push_back(b);
  return a;
}

const UArgsImpl& operator+(const UArgsImpl& a, Node& b) {
  a.children->push_back(b);
  return a;
}

const UArgsImpl& operator+(const UArgsImpl& a, const Child& c) {
  if (!*c) Application::error("Args::operator+","null Child in arglist");
  else a.children->push_back(c);
  return a;
}

// UArgsImpl + Args => recopie children de Args a la suite de ceux de UArgsImpl
// SANS MODIFIER Args
//
const UArgsImpl& operator+(const UArgsImpl& a, const Args& a2) {
  // marche pas car operator* est redefini
  //a.children->insert(a.children->end(), a2.children->begin(), a2.children->end());
  for (ChildIter i = a2.children->begin(); i != a2.children->end(); ++i)
    a.children->push_back(i.child());
  return a;
}

const UArgsImpl& operator+(const _UAttrArgs& attrs, const UArgsImpl& a) {   // !!!!!!
  for (UChildReverseIter i = attrs.children->rbegin(); i != attrs.children->rend(); ++i)
    a.children->push_front(i.child());
  return a;
}

// ========================================================= [Elc] ===========

_UAttrArgs::~_UAttrArgs() {
  if (children->refcount > 1) children->refcount--;
  else if (children->refcount == 1) delete children;
}

_UAttrArgs::_UAttrArgs() {
  children = new UArgsChildren;
  children->refcount = 1;
}

/*
_UAttrArgs::_UAttrArgs(const UArgsImpl& a) : children(a.children) {
  if (!children) {  // cas Args::none pas encore initialise...
    children = new UArgsChildren;
    children->refcount = 0;
  }
  children->refcount++;
}
*/

_UAttrArgs::_UAttrArgs(Attribute* n) {
  children = new UArgsChildren;
  children->refcount = 1;
  if (!n) Application::error("Args","null Attribute argument in arglist");
  else children->push_back(n);
}

_UAttrArgs::_UAttrArgs(Attribute& n) {
  children = new UArgsChildren;
  children->refcount = 1;
  children->push_back(n);
}

_UAttrArgs::_UAttrArgs(UCall* n) {
  children = new UArgsChildren;
  children->refcount = 1;
  if (!n) Application::error("Args","null Child argument in arglist");
  else children->push_back(n);
}

_UAttrArgs::_UAttrArgs(UCall& n) {
  children = new UArgsChildren;
  children->refcount = 1;
  children->push_back(n);
}

/*
_UAttrArgs::_UAttrArgs(const Child& c) {
  children = new UArgsChildren;
  children->refcount = 1;
  if (!*c) Application::error("Args::Args","null Child argument in arglist");
  else children->push_back(c);
}
*/

const _UAttrArgs& operator,(const _UAttrArgs& a, Attribute* n) {
  if (!n) Application::error("Args::operator,","null Attribute argument in arglist");
  else a.children->push_back(n);
  return a;
}

const _UAttrArgs& operator,(const _UAttrArgs& a, Attribute& n) {
  a.children->push_back(n);
  return a;
}

const _UAttrArgs& operator,(const _UAttrArgs& a, UCall* n) {
  if (!n) Application::error("Args::operator,","null UCall argument in arglist");
  else a.children->push_back(n);
  return a;
}

const _UAttrArgs& operator,(const _UAttrArgs& a, UCall& n) {
  a.children->push_back(n);
  return a;
}

/*
const _UAttrArgs& operator+(const _UAttrArgs& a, const Child& c) {
  if (!*c) Application::error("Args::operator,","null Child argument in arglist");
  else a.children->push_back(c);
  return a;
}
*/

}
