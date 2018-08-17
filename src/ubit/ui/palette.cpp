/*
 *  palette.cpp: movable palette box (internal frame)
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2009 | Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
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

#include <ubit/ubit_features.h>
#include <iostream>
#include <ubit/uboxgeom.hpp>
#include <ubit/uinteractors.hpp>
#include <ubit/upalette.hpp>
#include <ubit/ucolor.hpp>
#include <ubit/ufont.hpp>
#include <ubit/uon.hpp>
#include <ubit/ucall.hpp>
#include <ubit/core/event.h>
#include <ubit/uborder.hpp>
#include <ubit/usymbol.hpp>
#include <ubit/ustyle.hpp>
#include <ubit/ubackground.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

UPalette::UPalette(Args a) :
ppos(new UPos),
ppos_ctrl((new UPosControl)->setModel(ppos)),
psize(new USize),
psize_ctrl((new USizeControl)->setModel(psize)),
pcontent_scale(new UScale),
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
                       + upos(Length(0,UPX,UPos::RIGHT), Length(0,UPX,UPos::BOTTOM)));
  presize_btn->add(USymbol::circle); //square
  presize_btn->show(false);
  
  //pcontrols->add(Font::small + ubutton("x"));
  
  ptitle_bar->addAttr(UOrient::horizontal + *ppos_ctrl);
  ptitle_bar->add(uleft() + *ptitle 
                  + uhflex() + ulabel().ignoreEvents() 
                  + uright() + *pcontrols);
  
  pcontent->addAttr(UOrient::vertical+ *pcontent_scale);

  addAttr(UOrient::vertical + *ppos + *psize);
  add(utop() + *ptitle_bar 
      + uvflex() + *pcontent 
      + *presize_btn);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UStyle* UPalette::createStyle() {
  UStyle* s = Box::createStyle();
  static Border* b = new URoundBorder(Border::LINE,Color::navy,Color::white,2,2,15,15);
  s->local.border = b;
  return s;
}

/*
void UPalette::minmaxCB(UMouseEvent&) {
  pcontent->show(! pcontent->isShown());
}
void UPalette::resizeCB(UMouseEvent&) {
  pcontent->show(! pcontent->isShown());
}
*/

void UPalette::setPosModel(UPos* p) {
  if (ppos == p) return;
  if (ppos) removeAttr(*ppos);
  if (p) addAttr(p);
  ppos = p;
  if (ppos && ppos_ctrl) ppos_ctrl->setModel(ppos);
}

void UPalette::setPosControlModel(UPosControl* pc) {
  if (ppos_ctrl == pc) return;
  if (ppos_ctrl) ptitle_bar->removeAttr(*ppos_ctrl);
  if (pc) ptitle_bar->addAttr(pc);
  ppos_ctrl = pc;
}


}

