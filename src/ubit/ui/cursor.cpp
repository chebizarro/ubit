/*
 *  cursor.cpp: Cursor Attribute
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
#include <ubit/draw/color.h>
#include <ubit/ui/view.h>
#include <ubit/core/event.h>
#include <ubit/ui/updatecontext.h>
#include <ubit/ui/cursor.h>
#include <ubit/core/application.h>

namespace ubit {

Cursor Cursor::none(-1, UCONST);
  
  /* les autres definis dans udispX11.cpp :
  *     Cursor Cursor::pointer(XC_left_ptr, UCONST);
  *     Cursor Cursor::crosshair(XC_tcross, UCONST);
  * ...etc...
  */
  

Cursor::Cursor(const Cursor& c) : cursor_type(c.cursor_type) {}
Cursor::Cursor(int ctype) : cursor_type(ctype) {}
Cursor::Cursor(int ctype, UConst m): Attribute(m), cursor_type(ctype) {}

Cursor& Cursor::set(const Cursor& c) {
  if (checkConst()) return *this;
  if (cursor_type == c.cursor_type) return *this;
  cursor_type = c.cursor_type;
  changed(true);
  return *this;
}

bool Cursor::equals(const Cursor &c) const {
  return (cursor_type == c.cursor_type);
}


void Cursor::addingTo(Child& c, Element& parent) {
  Attribute::addingTo(c, parent);
  //if (parent->isDef(0, UMode::HAS_CURSOR)) 
  //  Application::warning("Cursor::addingTo","multiple Cursor bricks in object:", parent->cname());
  // rendre parent sensitif aux events ad hoc
  parent.emodes.HAS_CURSOR = true;
}

void Cursor::removingFrom(Child& c, Element& parent) {
  // tant pis s'il y a plusieurs Cursors: de tt facon c'est une erreur
  parent.emodes.HAS_CURSOR = false;
  Attribute::removingFrom(c, parent);
}

void Cursor::putProp(UpdateContext* ctx, Element&) {
  ctx->cursor = this;
}

Cursor::~Cursor() {
  if (Application::isExiting()) return;
  for (int k = 0; k < (int)cimpl.size(); ++k) {
    Display* d = Application::getDisp(k);
    if (d) d->deleteCursorImpl(cimpl[k]);
  }
  destructs();
}

UCursorImpl* Cursor::getCursorImpl(Display* d) const {
  int id = d->getID();
  if (id < (int)cimpl.size() && cimpl[id] != null) return cimpl[id];
  else {
    if (id >= (int)cimpl.size()) {    // agrandir la table;
      for (int k = cimpl.size(); k <= id; ++k) cimpl.push_back(null);
    }
    return cimpl[id] = d->createCursorImpl(cursor_type);
  }
}


}
