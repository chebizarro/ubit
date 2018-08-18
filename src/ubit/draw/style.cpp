/*
 *  style.cpp: Element and Box styles
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
#include <ubit/ubit_features.h>
#include <ubit/ucall.hpp>
#include <ubit/ui/box.h>
#include <ubit/uboxgeom.hpp>
#include <ubit/draw/font.h>
#include <ubit/draw/color.h>
#include <ubit/ui/uborder.h>
#include <ubit/draw/style.h>
#include <ubit/ui/view.h>
#include <ubit/core/application.h>
#include <ubit/core/appimpl.h>
#include <ubit/uon.hpp>
using namespace std;
namespace ubit {



ULocalProps::ULocalProps() :
size(UAUTO, UAUTO),
padding(0, 0),
border(null), 
background(null), 
alpha(1.0),
content(null) {
}

Style::Style() {
  viewStyle = &View::style;
  textSeparator = null;    // text separator between two children when printing
  orient = Orientation::INHERIT;
  halign = Halign::LEFT;
  valign = Valign::TOP;
  hspacing = HSpacing::INHERIT;
  vspacing = VSpacing::INHERIT;
  font = null;
  cursor = null;
  //edit = null;
  //color = null;
  //bgcolor = null;
  //attrs = null;
  setColors(Color::inherit);
  setColor(UOn::DISABLED, Color::disabled);
  setBgcolors(Color::none);
  setBgcolor(UOn::DROP_ENTERED, Color::grey);
}

Style::~Style() {
  //on ne peut pas detruire colors & bgcolors car partagees
  //delete colors;
  //delete bgcolors;
}


void Style::setSize(Length w, Length h) {
  if (w != UIGNORE) local.size.width = w;
  if (h != UIGNORE) local.size.height = h;
}

void Style::setPadding(Length horiz, Length vert) {
  if (horiz != UIGNORE) local.padding.left = local.padding.right = horiz;
  if (vert != UIGNORE) local.padding.top = local.padding.bottom = vert;
}

void Style::setHorizPadding(Length left, Length right) {
  if (left != UIGNORE) local.padding.left = left;
  if (right != UIGNORE) local.padding.right = right;
}

void Style::setVertPadding(Length top, Length bottom) {
  if (top != UIGNORE) local.padding.top = top;
  if (bottom != UIGNORE) local.padding.bottom = bottom;
}


const Color* Style::getColor(const Element& e) const {
  return colors[e.isSelected() * UOn::ACTION_COUNT + (int)e.getInterState()];
}

const Color* Style::getBgcolor(const Element& e) const {
  return bgcolors[e.isSelected()*UOn::ACTION_COUNT + (int)e.getInterState()];
}

void Style::setColors(Color& c) {setColors(c, c);}

void Style::setColors(Color& c_unselect, Color& c_select) {
  for (int k = 0; k < UOn::ACTION_COUNT; ++k) {
    colors[k] = &c_unselect;
    colors[UOn::ACTION_COUNT + k] = &c_select;
  }
}

void Style::setBgcolors(Color& c) {setBgcolors(c, c);}

void Style::setBgcolors(Color& c_unselect, Color& c_select) {
  for (int k = 0; k < UOn::ACTION_COUNT; ++k) {
    bgcolors[k] = &c_unselect;
    bgcolors[UOn::ACTION_COUNT + k] = &c_select;
  }
}

void Style::setColor(int action, Color& c) {
  colors[action] = &c;
  colors[UOn::ACTION_COUNT + action] = &c;
}

void Style::setBgcolor(int action, Color& c) {
  bgcolors[action] = &c;
  bgcolors[UOn::ACTION_COUNT + action] = &c;
}

}
