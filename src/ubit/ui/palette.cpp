/*
 *  palette.cpp: movable palette box (internal frame)
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
#include <ubit/core/boxgeom.h>
#include <ubit/ui/interactors.h>
#include <ubit/ui/palette.h>
#include <ubit/draw/color.h>
#include <ubit/draw/font.h>
#include <ubit/core/on.h>
#include <ubit/core/call.h>
#include <ubit/core/event.h>
#include <ubit/ui/border.h>
#include <ubit/ui/symbol.h>
#include <ubit/draw/style.h>
#include <ubit/ui/background.h>
using namespace std;

namespace ubit {


UPalette::UPalette(Args a) :
ppos(new Position),
ppos_ctrl((new PositionControl)->setModel(ppos)),
psize(new Size),
psize_ctrl((new SizeControl)->setModel(psize)),
pcontent_scale(new Scale),
pcontent(new Box(a)),
ptitle(new Element),
pcontrols(new Element),
//pminmaxbtn(new Box),
ptitle_bar(new Box),
presize_btn(new Box)
{
  /*
  pminmaxbtn->addAttr(Color::navy + Font::bold 
                     + ucall(this, &UPalette::minmaxCB));
  pminmaxbtn->add(UOn::select / ustr(" - ") + UOn::deselect / ustr(" + "));
  + UOn::doubleClick / ucall(this, &UPalette::minmaxCB));
   */
  
  ptitle->ignoreEvents();
 
  presize_btn->addAttr(*psize_ctrl //+ Background::metal
                       + uhcenter() + uvcenter()
                       + upos(Length(0,UPX,Position::RIGHT), Length(0,UPX,Position::BOTTOM)));
  presize_btn->add(Symbol::circle); //square
  presize_btn->show(false);
  
  //pcontrols->add(Font::small + ubutton("x"));
  
  ptitle_bar->addAttr(Orientation::horizontal + *ppos_ctrl);
  ptitle_bar->add(uleft() + *ptitle 
                  + uhflex() + ulabel().ignoreEvents() 
                  + uright() + *pcontrols);
  
  pcontent->addAttr(Orientation::vertical+ *pcontent_scale);

  addAttr(Orientation::vertical + *ppos + *psize);
  add(utop() + *ptitle_bar 
      + uvflex() + *pcontent 
      + *presize_btn);
}


Style* UPalette::createStyle() {
  Style* s = Box::createStyle();
  static Border* b = new RoundBorder(Border::LINE,Color::navy,Color::white,2,2,15,15);
  s->local.border = b;
  return s;
}

/*
void UPalette::minmaxCB(MouseEvent&) {
  pcontent->show(! pcontent->isShown());
}
void UPalette::resizeCB(MouseEvent&) {
  pcontent->show(! pcontent->isShown());
}
*/

void UPalette::setPosModel(Position* p) {
  if (ppos == p) return;
  if (ppos) removeAttr(*ppos);
  if (p) addAttr(p);
  ppos = p;
  if (ppos && ppos_ctrl) ppos_ctrl->setModel(ppos);
}

void UPalette::setPosControlModel(PositionControl* pc) {
  if (ppos_ctrl == pc) return;
  if (ppos_ctrl) ptitle_bar->removeAttr(*ppos_ctrl);
  if (pc) ptitle_bar->addAttr(pc);
  ppos_ctrl = pc;
}


}

