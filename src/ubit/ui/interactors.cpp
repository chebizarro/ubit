/*
 *  interactors.cpp
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
#include <ubit/ubit.hpp>
using namespace std;

namespace ubit {


struct SeparatorStyle : public Style {
  SeparatorStyle();
  virtual const Style& getStyle(UpdateContext*) const;
private:
  Style vstyle;
};

const Style& SeparatorStyle::getStyle(UpdateContext* ctx) const {
  Box* parent = null;
  if (ctx && (parent = ctx->getBoxParent()) && parent->isVertical())
    return vstyle; 
  else return *this;
}

SeparatorStyle::SeparatorStyle() {
  orient = Orientation::HORIZONTAL;
  halign = Halign::FLEX;
  valign = Valign::CENTER;
  local.padding.set(0, 0);
  local.border = &Border::shadowIn;
  // len = -1 means "keep initial size"
  // UNRESIZABLE prevents a uhflex/uvflex to resize the widget
  local.size.width = Length(-1, UPX, Size::UNRESIZABLE);

  vstyle.orient = Orientation::VERTICAL;
  vstyle.halign = Halign::CENTER;
  vstyle.valign = Valign::FLEX;
  vstyle.local.padding.set(0, 0);
  vstyle.local.border = &Border::shadowIn;
  // len = -1 means "keep initial size"
  // UNRESIZABLE prevents a uhflex/uvflex to resize the widget
  vstyle.local.size.height = Length(-1, UPX, Size::UNRESIZABLE);
}

Style* Separator::createStyle() {
  return new SeparatorStyle();
}

Separator::Separator() : Box() {
  //setCloseMenuMode(ElementModes::CANNOT_CLOSE_MENU);
}


Style* Label::createStyle() {
  Style* style = new Style();
  style->orient = Orientation::HORIZONTAL;
  style->halign = Halign::LEFT;
  style->valign = Valign::CENTER;
  style->hspacing = 1;
  style->vspacing = 1;
  style->setPadding(2, 2);
  return style;
}

Label::Label(int nbchars, Args a) : Box(a) {
  obtainAttr<Size>().setWidth(nbchars|UEX);
}


Style* TextField::createStyle() {
  Style& s = *new Style();
  s.setColors(Color::inherit, Color::white);
  //s.setBgcolors(Color::none, Color::blue);
  s.setBgcolors(Color::white, Color::blue);
  s.setColor(UOn::DISABLED, Color::disabled);
  s.setBgcolor(UOn::DROP_ENTERED,Color::blue);
  s.orient = Orientation::HORIZONTAL;
  s.halign = Halign::LEFT;
  s.valign = Valign::CENTER;
  s.hspacing = 0;
  s.vspacing = 3;
  s.setPadding(3,2); 
  static Border* border = 
  new RoundBorder(Border::LINE, Color::darkgrey, Color::white,1,1,15,15);
  s.setBorder(border);
  //s.local.border = &Border::shadowIn;
  
  //style->cursor = &Cursor::text;  // done by TextEdit
  s.setSize(Size::INITIAL, Size::INITIAL);
  return &s;
}

TextField::TextField(Args a) : Box(a) {
  obtainAttr<TextEdit>();
  emodes.IS_TEXT_SELECTABLE = true;     // also added by uedit
  emodes.HAS_CURSOR = true;
  disableMenuClosing();
}

TextField::TextField(int nbchars, Args a) : Box(a) {
  obtainAttr<Size>().setWidth(nbchars|UEX);
  obtainAttr<TextEdit>();
  emodes.IS_TEXT_SELECTABLE = true;     // also added by uedit
  emodes.HAS_CURSOR = true;
  disableMenuClosing();
}

TextEdit& TextField::edit() {
  return *getAttr<TextEdit>();
}

TextField& TextField::setEditable(bool state) {
  edit().setEditable(state); return *this;
}

bool TextField::isEditable() const {
  return getAttr<TextEdit>()->isEditable();
}


Style* TextArea::createStyle() {
  Style& s = *new Style();
  s.viewStyle = &FlowView::style;
  s.textSeparator = new String("\n");
  s.orient = Orientation::HORIZONTAL;
  s.halign = Halign::FLEX;
  s.valign = Valign::CENTER;
  
  s.setColors(Color::inherit, Color::white);
  s.setBgcolors(Color::white, Color::blue);
  s.setColor(UOn::DISABLED, Color::disabled);
  s.setBgcolor(UOn::DROP_ENTERED,Color::blue);
  s.hspacing = 0;
  s.vspacing = 3;
  s.local.padding.set(3,2);
  s.local.border = &Border::shadowIn;
  //style->cursor = &Cursor::text;  // fait par TextEdit
  
  // width does not depend on content, but height does! otherwise the box
  // could not grow vertically when children are added or resized (for instance
  // the height of a text flow could not grow)
  //s.local.size.width = -1;   // len = -1 means "keep initial size"
  s.setSize(Size::INITIAL, UAUTO);
  return &s;
}


class ButtonStyle : public Style {
public:
  Style menu, bar, tab;
  ButtonStyle();
  virtual const Style& getStyle(UpdateContext*) const;
};

ButtonStyle::ButtonStyle() {
  orient = Orientation::HORIZONTAL;
  halign = Halign::LEFT;
  valign = Valign::CENTER;
  hspacing = 2;
  vspacing = 2;
  setPadding(2, 2);
  local.border = &Border::shadowOut;
  /*
  setAttributes(UOn::select/Color::grey
                  + UOn::enter/Color::lightgrey
                  + UOn::arm/Color::grey);
   */
  setBgcolors(Color::none, Color::grey);
  setBgcolor(UOn::ENTERED, Color::lightgrey);
  setBgcolor(UOn::ARMED, Color::grey);
  setBgcolor(UOn::DROP_ENTERED, Color::grey);

  tab = *this;
  tab.halign = Halign::CENTER;
  tab.valign = Valign::CENTER;
  tab.setPadding(2, 4);
  tab.setColors(Color::black, Color::white);
  tab.setBgcolors(Color::none, Color::blue);
  
  menu = *this;
  menu.setPadding(10,4);
  menu.local.border = null;
  menu.setBgcolors(Color::none, Color::grey);
  menu.setColor(UOn::ENTERED, Color::white);
  menu.setBgcolor(UOn::ENTERED, Color::blue);
  menu.setColor(UOn::ARMED, Color::white);  
  menu.setBgcolor(UOn::ARMED, Color::blue);
  menu.setBgcolor(UOn::DROP_ENTERED, Color::grey);
  
  bar = *this;
  bar.orient = Orientation::VERTICAL;
  bar.halign = Halign::CENTER;
  bar.valign = Valign::CENTER;
  bar.setPadding(5,3);
  bar.local.border = null;
  bar.setBgcolors(Color::none, Color::grey);
  bar.setBgcolor(UOn::ENTERED, Color::lightgrey);
  bar.setBgcolor(UOn::ARMED, Color::grey);
  bar.setBgcolor(UOn::DROP_ENTERED, Color::grey);
};

const Style& ButtonStyle::getStyle(UpdateContext* ctx) const {
  if (!ctx) return *this;
  
  Button* btn = dynamic_cast<Button*>(ctx->getObj());
  if (btn->button_type != Button::AUTO) {
    switch (btn->button_type) {
      case Button::TAB:  return tab; break;
      case Button::MENU: return menu; break;
      case Button::BAR:  return bar; break;
      default: return *this; break;
    }
  }
  else {
    Box* parent = null;
    if ((parent = ctx->getBoxParent())) {
      // faudrait separer les 2 cas et rajouter listbox
      if (dynamic_cast<const Menu*>(parent) || dynamic_cast<const MenuBar*>(parent))
        return menu;
      else if (dynamic_cast<const UBar*>(parent))
        return bar;
    }
  }
  return *this;   // tous les autres cas
}


Style* Button::createStyle() {return new ButtonStyle;}

Button::Button(Args a) {
  button_type = AUTO;
  emodes.IS_ARMABLE = true;
  emodes.IS_CROSSABLE = true;
  // this object auto opens dialog children because it is ARMable
  // !att: pour que ca marche faut ajouter a APRES emodes.IS_ARMABLE = true !
  add(a);
}

Button::Button(int btype, Args a) {
  button_type = btype;
  emodes.IS_ARMABLE = true;
  emodes.IS_CROSSABLE = true;
  // this object auto opens dialog children because it is ARMable
  // !att: pour que ca marche faut ajouter a APRES emodes.IS_ARMABLE = true !
  add(a);
}

Button& uflatbutton(Args a) {
  Button* b = new Button;
  b->setAttr(Border::line);    // !!! A REVOIR avec styles 
  b->add(a);
  return *b;
}

Button& utabbutton(Args a) {
  return *new Button(Button::TAB, a);
}

/* Tab Button widget (for tabbed panes, @see UCardbox).
class UTabbutton: public Button {
public:
  UCLASS("utabbutton", UTabbutton, new UTabbutton)
  UTabbutton(const Args& a = Args::none);
  static Style* createStyle();
};

 * Flat Button gadget.
 * has a "flat" square border
 * See Button.
 *
class UFlatbutton: public Button {
public:
  UCLASS("uflatbutton", UFlatbutton, new UFlatbutton)
  UFlatbutton(const Args& a = Args::none);
    static Style* createStyle();
};
*/


Style* Item::createStyle() {
  Style& s = *new Style();
  s.orient = Orientation::HORIZONTAL;
  s.halign = Halign::LEFT;
  s.valign = Valign::FLEX;
  s.hspacing = 2;
  s.hspacing = 2;
  s.setPadding(1, 1);
  /*
   s.setAttributes(UOn::select/Color::white
   + UOn::select/Background::blue
   + UOn::arm/Color::white
   + UOn::arm/Background::blue);
   + UOn::drop/Color::white
   + UOn::drop/Background::blue);
   */
  s.setColors(Color::inherit, Color::white);
  s.setBgcolors(Color::none, Color::blue);
  s.setColor(UOn::ARMED, Color::white);
  s.setBgcolor(UOn::ARMED, Color::blue);
  s.setColor(UOn::DROP_ENTERED, Color::white);
  s.setBgcolor(UOn::DROP_ENTERED, Color::blue);
  return &s;
}

Item::Item(Args a): Box() {
  emodes.IS_ARMABLE = true;
  // this object auto opens dialog children because it is ARMable
  // !att: pour que ca marche faut ajouter a APRES!
  add(a);
}


Style* LinkButton::createStyle() {
  Style& s = *new Style;
  s.orient = Orientation::HORIZONTAL;
  s.halign = Halign::LEFT;
  s.valign = Valign::CENTER;
  s.hspacing = 1;
  s.vspacing = 1;
  s.local.padding.set(1, 1);
  s.local.border = null;
  s.cursor = &Cursor::hand;
  s.font = &Font::underline;
  s.setColors(Color::navy, Color::red);
  s.setBgcolors(Color::none, Color::none);  
  s.setColor(UOn::DISABLED, Color::disabled);
  s.setBgcolor(UOn::ENTERED, Color::grey);
  s.setColor(UOn::ARMED, Color::red);
  s.setColor(UOn::ACTIONED, Color::red);
  s.setColor(UOn::DROP_ENTERED, Color::blue);
  return &s;
}

LinkButton::LinkButton(Args a) : Button(a) {
  emodes.HAS_CURSOR = true; // le style suffit pas !
}

String LinkButton::getHRef() const {        // pas tres efficace!
  String val;
  getAttrValue(val,"href"); 
  return val;
}


Style* Checkbox::createStyle() {
  ButtonStyle& s = *new ButtonStyle;
  s.orient = s.menu.orient = s.bar.orient = Orientation::HORIZONTAL;
  s.local.border = s.menu.local.border = s.bar.local.border = null;
  s.local.content = s.menu.local.content = s.bar.local.content = new Element(Symbol::check);
  s.setBgcolors(Color::none, Color::none);
  s.setBgcolor(UOn::DROP_ENTERED, Color::grey);
  s.bar.setBgcolors(Color::none, Color::none);
  s.bar.setBgcolor(UOn::DROP_ENTERED, Color::grey);
  s.menu.setBgcolors(Color::none, Color::none);
  s.menu.setBgcolor(UOn::DROP_ENTERED, Color::grey);
  return &s;
}

Style* RadioButton::createStyle() {
  ButtonStyle& s = *new ButtonStyle;
  s.orient = s.menu.orient = s.bar.orient = Orientation::HORIZONTAL;
  s.local.border = s.menu.local.border = s.bar.local.border = null;
  s.local.content = s.menu.local.content = s.bar.local.content = new Element(Symbol::radio);
  s.setBgcolors(Color::none, Color::none);
  s.setBgcolor(UOn::DROP_ENTERED, Color::grey);
  s.bar.setBgcolors(Color::none, Color::none);
  s.bar.setBgcolor(UOn::DROP_ENTERED, Color::grey);
  s.menu.setBgcolors(Color::none, Color::none);
  s.menu.setBgcolor(UOn::DROP_ENTERED, Color::grey);
  return &s;
}

Checkbox::Checkbox(Args a) : Button(a) {
  emodes.IS_SELECTABLE = true;
  emodes.IS_CROSSABLE = false;
}

RadioButton::RadioButton(Args a) : Button(a) {
  emodes.IS_SELECTABLE = true;
  emodes.IS_CROSSABLE = false;
}


SpinBox::SpinBox(Args a) : 
pvalue(*new Int(0)),
pstr(*new String),
increment(1) {
  constructs(a);
}

SpinBox::SpinBox(Int& _value, Args a) : 
pvalue(_value),
pstr(*new String),
increment(1) {
  constructs(a);
}

Style* SpinBox::createStyle() {
  return Box::createStyle();
}

void SpinBox::constructs(const Args& a) {
  ptextfield = new TextField;
  ptextfield->addAttr(UOn::action / ucall(this, 0, &SpinBox::updateValue)
                      + UOn::leave  / ucall(this, 0, &SpinBox::updateValue));
  ptextfield->add(a); // ??
  ptextfield->add(*pstr);

  pstr->setInt(*pvalue);
  pstr->append("        ");

  // value change -> mise a jour de la string
  pvalue->onChange(ucall(this, &SpinBox::changed));

  Box& up_btn = ubox
    (Symbol::up + UOn::arm/ucall(this,+1,&SpinBox::updateValue));

  Box& down_btn = ubox
    (Symbol::down + UOn::arm/ucall(this,-1,&SpinBox::updateValue));

  //up_btn.setAutoRepeat(true);
  //down_btn.setAutoRepeat(true);

  add(Orientation::horizontal 
      + uhflex() + ptextfield
      + uright() + uvbox(uscale(0.95) + uvcenter() + uhcenter() + up_btn + down_btn)
      );
}


void SpinBox::setValue(int v) {*pvalue = v;}
void SpinBox::setIncrement(int v) {increment = v;}

void SpinBox::updateValue(int dir) {
  if (dir == 0) *pvalue = *pstr;  // transform string to num value
  // ajouter ou retrancher increment 
  else if (dir > 0) *pvalue += increment;
  else if (dir < 0) *pvalue -= increment;
}

void SpinBox::changed() {
  pstr->setInt(*pvalue);  // reset string format
  Event e1(UOn::change, this, pvalue);  //UElemEvent
  fire(e1);

  Event e2(UOn::action, this, pvalue);  //UElemEvent
  fire(e2);
}

}
