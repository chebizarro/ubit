/*
 *  border.cpp: Border Attribute for Box containers
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2008 | Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
 *
 * ***********************************************************************
 * COPYRIGHT NOTICE :
 * THIS PROGRAM IS DISTRIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE
 * IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION;
 * EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 * SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
 * ***********************************************************************/

#include <iostream>
#include <ubit/ubit_features.h>
#include <ubit/ucall.hpp>
#include <ubit/ucolor.hpp>
#include <ubit/uborder.hpp>
#include <ubit/ugraph.hpp>
#include <ubit/uview.hpp>
#include <ubit/ui/uviewImpl.hpp>
#include <ubit/uupdate.hpp>
#include <ubit/ui/updatecontext.h>
#include <ubit/ubox.hpp>
#include <ubit/uwin.hpp>
#include <ubit/uon.hpp>
using namespace std;
namespace ubit {


#define DARK  Color::darkgrey
#define LIGHT Color::lightgrey

Border Border::none(NONE, Color::inherit, Color::inherit, 0, 0, UCONST);
Border Border::empty(NONE, Color::inherit, Color::inherit, 1, 1, UCONST);
Border Border::line(LINE, DARK, LIGHT, 1, 1, UCONST);
Border Border::shadowOut(+SHADOW, DARK, LIGHT,1,1,UCONST);
Border Border::shadowIn(-SHADOW, DARK, LIGHT, 1,1,UCONST);
Border Border::etchedOut(+ETCHED, DARK, LIGHT, 2,2,UCONST);
Border Border::etchedIn(-ETCHED, DARK, LIGHT, 2,2,UCONST);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Border::Border(int d) : padding(1,1) {
  constructs(d, Color::white, Color::black);
}

Border::Border(int d, const Color& c, const Color& bgc, float horiz, float vert) 
: padding(horiz,vert) {
  constructs(d, c, bgc);
}

Border::Border(int d, const Color& c, const Color& bgc, float horiz, float vert, UConst m)
: Attribute(m), padding(horiz,vert) {
  constructs(d, c, bgc);
}

Border::Border(const Border& b) : padding(b.padding) {
  constructs(b.decoration, *b.pcolor, *b.pbgcolor);
}

Border::~Border() {}

void Border::constructs(int d, const Color& c, const Color& bgc) {
  decoration = d;
  is_overlaid = is_rounded = false;
  pcolor = c;
  pbgcolor = bgc;
}

Border& Border::operator=(const Border& b) {
  if (checkConst()) return *this;
  // !! tester egalite !!
  constructs(b.decoration, *b.pcolor, *b.pbgcolor);
  padding = b.padding;
  changed(true);
  return *this;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

URoundBorder::URoundBorder(int d) : Border(d), arc_w(5), arc_h(5) {
  is_rounded = true;
}

URoundBorder::URoundBorder(int d, const Color& c, const Color& bgc,
             float w, float h, float aw, float ah)
: Border(d, c, bgc, w, h), arc_w(aw), arc_h(ah) {
  is_rounded = true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class UBorderContent : public Element {
public:
  UCLASS(UBorderContent)
  UBorderContent(const Args& a = Args::none) : Element(a) {}
  virtual int getDisplayType() const {return BORDER;}   // redefined
};

UCompositeBorder::UCompositeBorder() 
: Border(+SHADOW, Color::none, Color::none, 1,1) {
}

UCompositeBorder::UCompositeBorder(const Args& a) 
: Border(+SHADOW, Color::none, Color::none, 1,1) {
  pchildren = new UBorderContent(a);
}

/*
UCompositeBorder& UCompositeBorder::addChildren(const Args& a) {
  if (!pchildren) pchildren = new UBorderContent(a);
  else pchildren->add(a);
  return *this;
}

UCompositeBorder& UCompositeBorder::addChildren(bool superimposed, const Args& a) {
  is_overlaid = superimposed;
  if (!pchildren) pchildren = new UBorderContent(a);
  else pchildren->add(a);
  return *this;
}
*/
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Border& Border::setDecoration(int decor) {
  if (checkConst()) return *this;
  decoration = decor;
  switch(decor) {
    case NONE: padding.set(0,0); break;
    case ETCHED: padding.set(2,2); break;
    default: padding.set(1,1); break;
  }
  return *this;
}

Border& Border::setColor(const Color& c) {
  if (checkConst() || pcolor==&c || *pcolor==c) return *this;
  pcolor = c;
  return *this;
}

Border& Border::setBgcolor(const Color& c) {
  if (checkConst() || pbgcolor==&c || *pbgcolor==c) return *this;
  pbgcolor = c;
  return *this;
}

/*
 Border& Border::setThickness(float t) {
 thickness = t;
 return *this;
 }
 */

Border& Border::setPadding(const PaddingSpec& p) {
  if (checkConst()) return *this;
  padding = p;
  return *this;
}

Border& Border::setOverlaid(bool state) {
  if (checkConst()) return *this;
  is_overlaid = state; 
  return *this;
}

void Border::update() {
  updateAutoParents(Update::LAYOUT_PAINT);
}

void Border::putProp(UpdateContext* props, Element&) {
  // a defaut de mieux : ne pas ecraser une definition de border !!
  // s'il contient des elements (typiquement un scrollpane)
  if (!props->local.border || !props->local.border->getSubGroup())
    props->local.border = this;
}

void UCompositeBorder::putProp(UpdateContext* props, Element&) {
  props->local.border = this;  // ecrase si existe
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Border::getSize(const UpdateContext&, PaddingSpec& _m) const {
  _m = padding;
}

void Border::paint(Graph& g, const UpdateContext& parp, const Rectangle& r) const {
  // new: fait avant:
  // vd.margin = margin;
  // if (!vd.can_paint) return;   // si rien a peindre on s'arrete la
  
  // !ATT: il peut y avoir des scrollbars meme si border == none
  if (decoration == NONE) return;
  
  // invert is_active when the btn is pressed
  bool is_active;
  if (parp.obj->isSelected())
    is_active = (parp.obj->getInterState() != UOn::ARMED);
  else 
    is_active = (parp.obj->getInterState() == UOn::ARMED);
  
  const Color *_color;
  if (pcolor->equals(Color::inherit)) _color = parp.color;
  else _color = pcolor;
  
  const Color *_bgcolor;
  if (pbgcolor->equals(Color::inherit)) _bgcolor = parp.bgcolor;
  else _bgcolor = pbgcolor;
  
  const Color *bg = null, *fg = null;
  
  if (decoration > NONE) {  // OUT (normal case)
    if (is_active) {bg = _color; fg = _bgcolor;}
    else {bg = _bgcolor; fg = _color;}   // cas normal
  }
  else {                  // IN (inverted shadows)
    if (is_active) {bg = _bgcolor; fg = _color;}
    else {bg = _color; fg = _bgcolor;}
  }

  paintDecoration(g, r, *parp.obj, *fg, *bg);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Border::paintDecoration(Graph& g, const Rectangle& r, Element& obj, 
                              const Color& fg, const Color& bg) const {
  float x1 = r.x;
  float y1 = r.y;
  float x2 = r.x + r.width - 1;
  float y2 = r.y + r.height -1;
  
  switch (abs(decoration)) {
    case SHADOW:
      // out: fg = bottom+right and bg = top+left
      // in:  invert
      g.setColor(fg);
      g.drawLine(x1+1, y2, x2, y2); // bottom
      g.drawLine(x2, y1+1, x2, y2); // right
      g.setColor(bg);
      g.drawLine(x1, y1, x2-1, y1); // top
      g.drawLine(x1, y1, x1, y2-1); // left
      break;
      
    case ETCHED:
      g.setColor(fg);
      g.drawRect(x1+1, y1+1, r.width-2, r.height-2);
      g.setColor(bg);
      g.drawRect(x1, y1, r.width-2,  r.height-2);
      break;
      
    case LINE:
      g.setColor(fg);
      g.drawRect(x1, y1, r.width-1, r.height-1);
      break;
      
      /*
    case BIGSHADOW:{
      float prev_thickness = g.getWidth();
      g.setColor(*fg);
      g.setWidth(thickness);
      g.drawLine(x1 + 10+4, y2, x2, y2); // bottom
      g.drawLine(x2, y1 + 10+4, x2, y2 + 4); // right
      g.setWidth(prev_thickness);
    } break;
       */
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void URoundBorder::paintDecoration(Graph& g, const Rectangle& r, Element& obj, 
                                   const Color& fg, const Color& bg) const {
  float x1 = r.x;
  float y1 = r.y;
  float x2 = r.x + r.width - 1;
  float y2 = r.y + r.height -1;
  
  switch (abs(decoration)) {
    case SHADOW:
      // out: fg = bottom+right and bg = top+left
      // in:  invert
      g.setColor(fg);
      g.drawLine(x1+1, y2, x2, y2); // bottom
      g.drawLine(x2, y1+1, x2, y2); // right
      g.setColor(bg);
      g.drawLine(x1, y1, x2-1, y1); // top
      g.drawLine(x1, y1, x1, y2-1); // left
      break;
      
    case ETCHED:
      g.setColor(fg);
      g.drawRoundRect(x1+1, y1+1, r.width-2, r.height-2, arc_w, arc_h);
      g.setColor(bg);
      g.drawRoundRect(x1, y1, r.width-2,  r.height-2, arc_w, arc_h);
      break;
      
    case LINE:
      g.setColor(fg);
      g.drawRoundRect(x1, y1, r.width-1, r.height-1, arc_w, arc_h);
      break;
  }
}

}

