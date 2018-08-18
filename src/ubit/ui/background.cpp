/*
 *  background.cpp: Widget background and transparency.
 *  (see also uattribute.hpp for UAttribute base class)
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
#include <ubit/ui/background.h>
#include <ubit/ui/updatecontext.h>
#include <ubit/ucolor.hpp>
#include <ubit/upix.hpp>
#include <ubit/uappli.hpp>
using namespace std;

namespace ubit {



UAlpha::UAlpha(float v) : Attribute() {val = v;}

UAlpha& UAlpha::set(float v) {
  if (checkConst() || v == val) return *this;
  val = v;
  changed();
  return *this;
}

void UAlpha::update() {
  updateAutoParents(Update::paint);
}

void UAlpha::putProp(UpdateContext *props, Element&) {
  props->local.alpha = val;
}


//inline Background& ubgcolor(int r, int g, int b);
//obsolete.

Background Background::none(Color::none, UCONST);
Background Background::inherit(Color::inherit, UCONST);

Background Background::velin(UPix::velin, UCONST);
Background Background::metal(UPix::metal, UCONST);

Background Background::black(Color::black, UCONST);
Background Background::white(Color::white, UCONST);
Background Background::lightgrey(Color::lightgrey, UCONST);
Background Background::grey(Color::grey, UCONST);
Background Background::darkgrey(Color::darkgrey, UCONST);
Background Background::navy(Color::navy, UCONST);
Background Background::lightblue(Color::lightblue, UCONST);
Background Background::blue(Color::blue, UCONST);
Background Background::red(Color::red, UCONST);
Background Background::green(Color::green, UCONST);
Background Background::yellow(Color::yellow, UCONST);
Background Background::wheat(Color::wheat, UCONST);
Background Background::teal(Color::teal, UCONST);
Background Background::orange(Color::orange, UCONST);

Background::Background() 
: alpha(1.), tiling(true) {}

Background::Background(const Background& b) :
pcolor(b.pcolor ? new Color(*b.pcolor) : null), 
//pima(new Image(*b.pima)), 
pima(b.pima),                                // !!! A REVOIR (dupliquer)
alpha(b.alpha), tiling(b.tiling) {}

Background::Background(const Color& c) 
: pcolor(new Color(c)), alpha(1.), tiling(true) {}

Background::Background(Image& i) 
: pima(i), alpha(1.), tiling(true) {} // !!! A REVOIR (dupliquer)

Background::Background(Color& c, UConst m) 
: Attribute(m), pcolor(c), alpha(1.), tiling(true) {}    // color is not duplicated

Background::Background(Image& i, UConst m) 
: Attribute(m), pima(i), alpha(1.), tiling(true) {}    // image is not duplicated

Background::~Background() {}  // pour pval

const Color* Background::getColor() const {return pcolor;}
const Image* Background::getIma() const {return pima;}
bool Background::isTiled() const {return tiling;}

bool Background::operator==(const Background& b) {
  return ((pcolor == b.pcolor || (pcolor && b.pcolor && *pcolor == *b.pcolor))
          && pima == b.pima
          && alpha == b.alpha && tiling == b.tiling);
}

Background& Background::operator=(const Background& b) {
  if (checkConst() || *this == b) return *this;
  if (!b.pcolor) pcolor = null;
  else {
    if (pcolor) *pcolor = *b.pcolor; 
    else pcolor = new Color(*b.pcolor); 
  }
  //pima = new Color(*b.pima);
  pima = b.pima;                           // !!! A REVOIR (dupliquer)
  alpha = b.alpha;
  tiling = b.tiling;
  changed();
  return *this;
}

Background& Background::setColor(const Color& c) {
  if (checkConst() || (pcolor && *pcolor == c)) return *this;
  if (!pcolor) pcolor = new Color(c); else *pcolor = c;
  changed();
  return *this;
}

Background& Background::setRgba(const Rgba& comps) {
  if (checkConst() || (pcolor && *pcolor == comps)) return *this;
  if (!pcolor) pcolor = new Color(comps); else pcolor->setRgba(comps);
  changed();
  return *this;
}

Background& Background::setRbgaF(float r, float g, float b, float a) {
  return setRgba(Rgba(r,g,b,a));
}

Background& Background::setRbgaI(unsigned int r, unsigned int g, 
                                   unsigned int b, unsigned int a) {
  return setRgba(Rgba(r,g,b,a));
}
                     
Background& Background::setNamedColor(const String& colname, float a) {
  if (checkConst()) return *this;
  Rgba comps;
  bool found = Color::parseColor(colname.c_str(), comps);
  comps.comps[3] = (unsigned char)a*255; 
  if (found) return setRgba(comps);
  else {
    warning("Background::setNamedColor","color name '%s' not found", 
            (colname.empty() ? "null" : colname.c_str()));
    return *this;
  }
}

Background& Background::setIma(Image& i, bool t) {
  if (checkConst()) return *this;
  if (pima == &i) return *this;         // !!! A REVOIR (il faut dupliquer)
  pima = i;
  tiling = t;
  changed();
  return *this;
}

Background& Background::setTiling(bool t) {
  if (checkConst()) return *this;
  if (tiling == t) return *this;
  tiling = t;
  changed();
  return *this;
}

void Background::update() {
  updateAutoParents(Update::paint);
}

void Background::putProp(UpdateContext *props, Element&){
  props->local.background = this;
}



}
