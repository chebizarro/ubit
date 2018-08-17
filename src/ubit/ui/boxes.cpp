/*
 *  boxes.cpp : misc containers.
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
#include <ubit/uon.hpp>
#include <ubit/uboxgeom.hpp>
#include <ubit/ui/background.h>
#include <ubit/uboxes.hpp>
#include <ubit/ucolor.hpp>  
#include <ubit/upix.hpp>
#include <ubit/ucall.hpp>
#include <ubit/uborder.hpp>
#include <ubit/uinteractors.hpp>
#include <ubit/ulistbox.hpp>
#include <ubit/uscrollpane.hpp>
#include <ubit/ui/updatecontext.h>
using namespace std;

namespace ubit {


/* UVbox = vertical Box.
 *  Geometry: see class Box for details.
 *
class UVbox: public Box {
public:
  UCLASS("uvbox", UVbox, new UVbox)
  
  UVbox(Args args = Args::none) : Box(args) {}
  ///< creates a new vertical box; @see also shortcut: uvbox().
  
  static Style* createStyle();
};

 * UHbox = horizontal Box.
 *  Geometry: see class Box for details.
 *
class UHbox: public Box {
public:
  UCLASS("uhbox", UHbox, new UHbox)
  
  UHbox(Args args = Args::none) : Box(args) {}
  ///< creates a new horizontal box; @see also shortcut: uhbox().
  
  static Style* createStyle();
};

Style* UHbox::createStyle() {
  Style* style = new Style();
  style->textSeparator  =  &ustr(" ");
  style->orient   = Orientation::HORIZONTAL;
  style->halign   = Halign::LEFT;
  style->valign   = Valign::FLEX;
  style->hspacing = 1;
  style->vspacing = 1;
  style->local.padding.set(0,0);
  return style;
}

Style* UVbox::createStyle() {
  Style* style = new Style();
  style->textSeparator  = &ustr("\n");
  style->orient   = Orientation::VERTICAL;
  style->halign   = Halign::FLEX;
  style->valign   = Valign::TOP;
  style->hspacing = 1;
  style->vspacing = 1;
  //style->local.vmargin  = 0;  style->local.hmargin  = 0;
  style->local.padding.set(0,0);
  return style;
}
*/

Style* UBar::createStyle() {
  Style* s = new Style;
  s->textSeparator = &ustr("\t");
  s->orient = Orientation::HORIZONTAL;
  s->halign = Halign::LEFT;
  s->valign  = Valign::FLEX;
  s->hspacing = 4;
  s->vspacing = 5;
  s->local.padding.set(5,2);
  s->local.border = &Border::shadowOut;
  return s;
}

Style* UStatusbar::createStyle() {
  Style* s = new Style();
  s->textSeparator  = &ustr("\t");
  s->orient = Orientation::HORIZONTAL;
  s->halign = Halign::LEFT;
  s->valign  = Valign::FLEX;
  s->hspacing = 2;
  s->vspacing = 2;
  s->local.padding.set(6,2); //tb,lr
  s->local.border = &Border::shadowIn;
  return s;
}


Style* UFlowbox::createStyle() {
  Style& s = *new Style();
  s.viewStyle = &FlowView::style;
  s.textSeparator = new String("\n");
  s.orient = Orientation::HORIZONTAL;
  s.halign = Halign::FLEX;
  s.valign = Valign::FLEX;
  s.hspacing = 1;
  s.vspacing = 1;
  s.local.padding.set(0,0);
  
  // width does not depend on content, but height does! otherwise the box could
  // not grow vertically when children are added or resized (for instance the
  // height of a text flow could not grow)
  s.local.size.width = -1; // -1 means "keep initial size"
  return &s;
}

UFlowbox::UFlowbox(Args a): Box(a) {
  emodes.IS_TEXT_SELECTABLE = true;
}


Style* UCardbox::createStyle() {
  Style& s = *new Style();
  s.viewStyle = &UPaneView::style;
  s.orient = Orientation::HORIZONTAL;
  s.halign = Halign::FLEX;
  s.valign = Valign::FLEX;
  s.hspacing = 1;
  s.vspacing = 1;
  s.local.padding.set(0,0);
  //surtout ne pas faire cela: les cards n'apparaissent pas a pas celle qui
  //est selectionnee au debut (ou si on est en flex) 
  //s.local.size.width  = -1; // -1 means "keep initial size"
  //s.local.size.height = -1;
  return &s; 
}

UCardbox::UCardbox(Args args) : Box(args), ptabs(new ListBox()) 
{
  ptabs->addAttr(ucall(this, &UCardbox::setSelectedImpl)
                 + Orientation::horizontal + uhflex() + uvflex()
                 + upadding(0,0) + Background::none + Border::none);
  setAttr(*new UCompositeBorder(utop() + uhcenter() + *ptabs));
}

UCardbox::~UCardbox() {}

Choice& UCardbox::choice() {return ptabs->choice();}

void UCardbox::setSelectedImpl() {
  int index = ptabs->choice().getSelectedIndex();
  int ix = 0;
  for (ChildIter i = cbegin(); i != cend(); ++i) {
    Box* card = dynamic_cast<Box*>(*i);
    if (card) {
      if (ix == index) card->show(true);
      else card->show(false);
      ix++;
    }
  }
}

UCardbox& UCardbox::addCard(Box& card) {
  addTab(Args::none, card);
  return *this;
}

UCardbox& UCardbox::addTab(Args title, Box& card) {
  if (&title != &Args::none) ptabs->add(utabbutton(title));
  else ptabs->add(uitem());  
  add(card);
  setSelectedIndex(-1);
  return *this;
}

Box* UCardbox::getCard(int index) const {
  int ix = 0;
  for (ChildIter i = cbegin(); i != cend(); ++i) {
    Box* c = dynamic_cast<Box*>(*i);
    if (c && ix == index) return c;
    ix++;
  }
  return null;
}

int UCardbox::getCardIndex(Box& card) const {
  int ix = 0;
  for (ChildIter i = cbegin(); i != cend(); ++i) {
    Box* c = dynamic_cast<Box*>(*i);
    if (c == &card) return ix;
    ix++;
  }
  return -1;
}

Box* UCardbox::getSelectedCard() const {
  int index = ptabs->choice().getSelectedIndex();
  if (index >= 0) return getCard(index);
  return null;
}

void UCardbox::setSelectedCard(Box& card) {
  int ix = getCardIndex(card);
  if (ix >= 0) setSelectedIndex(ix);
}

int UCardbox::getSelectedIndex() const {
  return ptabs->choice().getSelectedIndex();
}

void UCardbox::setSelectedIndex(int index) {
  ptabs->choice().setSelectedIndex(index);
}

//Box* UCardbox::getTab(int index) const {
//  return ptabs->choice().getSelectableItem(index);
//}
//Box* UCardbox::getSelectedTab() const {
//  return ptabs->choice().getSelectedItem();
//}

//void UCardbox::setSelectedTab(Box* tab) {
//  ptabs->choice().setSelectedItem(tab);
//}
/*
 UCardbox& UCardbox::setComboMode(bool cm) {
 Border* border = getBorder(true); // true => create border if needed
 //cerr <<"border "<<border<<" " <<border->getSubGroup()<<endl;
 
 if (border->getSubGroup()) border->getSubGroup()->removeAll();
 
 if (cm) {
 ptabs->removeAllAttrs();
 ptabs->addAttr(ptabs->choice());
 border->add(utop() + uhcenter() + ucombobox(*ptabs));
 }
 else {
 ptabs->addAttr(Orientation::horizontal);
 ptabs->addAttr(uhspacing(0));
 ptabs->addAttr(uhmargin(1));
 ptabs->addAttr(uvmargin(1));
 ptabs->addAttr(Border::none);
 ptabs->addAttr(UBgcolor::none);
 ptabs->addAttr(uhflex());
 ptabs->addAttr(uvflex());
 border->add(utop() + uhcenter() + *ptabs);
 }
 return *this;
 }
 */
// NB: ex: class UDocbox : public UVbox 

UDocbox::UDocbox(Args args) {
  zoom_quantum = 1.166;
  
  ptitlebar = uhbox();
  pscale = new Scale;
  pcontent = uvbox(args);
  pcontent->addAttr(*pscale);
  
  pspane = uscrollpane(true, false);
  pspane->add(*pcontent);
  
  addAttr(Orientation::vertical + utop());
  add(uhflex() + utop() + *ptitlebar + uvflex() + *pspane);
}

UDocbox::~UDocbox() {}

void UDocbox::zoom(float z) {
  *pscale = z;
}

void UDocbox::zoomIn() {
  pscale->mult(zoom_quantum);
}

void UDocbox::zoomOut() {
  pscale->div(zoom_quantum);
}

void UDocbox::setZoomQuantum(float zq) {
  zoom_quantum = zq;
}

bool UDocbox::isIconified() const {
  return pspane->isShown();
}

void UDocbox::iconify(bool state) {
  pspane->show(!state);
  //piconify_btn->select(state);
}


AlertBox::AlertBox(Args args) {
  add(Orientation::vertical + Background::white + ualpha(0.8)
      + uhcenter() + uvcenter()
      + Font::bold + Font::large
      + args
      + UOn::action / ushow(*this, false)
      
      + uhbox(uscale(1.5) + Font::xx_large
              + UPix::ray + Color::red + " Alert! ")
      + " "
      + " "
      + " "
      + uhflex()  // necessaire sinon affiche un mot par ligne!
      + uflowbox(Font::x_large + Color::navy + uhcenter() + message)
      + " "
      + " "
      + " "
      + uhcenter()
      + ubutton(uscale(1.25) + Font::x_large + ualpha(0.6) 
                + "   OK   " + ushow(*this, false))
      );
}

Style* AlertBox::createStyle() {
  Style*s = Box::createStyle();
  s->orient = Orientation::VERTICAL;
  return s;
}

void AlertBox::showMsg(const String& msg) {
  message.removeAll();
  message.add(ustr(msg));
  update();
  Box::show();
}

//void AlertBox::showMsg(const String* msg) {if (msg) showMsg(*msg);}

void AlertBox::showMsg(const char* msg) {
  message.removeAll();
  message.add(ustr(msg));
  update();
  Box::show();
}

}
