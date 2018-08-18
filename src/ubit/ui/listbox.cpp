/*
 *  listbox.cpp: List Boxes
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



RadioBox::RadioBox(const Args& a) 
: pchoice(new Choice()) {
  addAttr(*pchoice);
  add(a);
}

Box* RadioBox::getSelectedItem() const {
  return choice().getSelectedItem();
}

int RadioBox::getSelectedIndex() const {
  return choice().getSelectedIndex();
}

RadioBox& RadioBox::setSelectedItem(Box& i) {
  choice().setSelectedItem(i);  // c'est title qui est selected !
  return *this;
}

RadioBox& RadioBox::setSelectedItem(Box* i) {
  choice().setSelectedItem(i);  // c'est title qui est selected !
  return *this;
}

RadioBox& RadioBox::setSelectedIndex(int i) {
  choice().setSelectedIndex(i);
  return *this;
}

ListBox& ListBox::setSelectedItem(Box& i) {
  choice().setSelectedItem(i);  // c'est title qui est selected !
  return *this;
}

ListBox& ListBox::setSelectedIndex(int i) {
  choice().setSelectedIndex(i);
  return *this;
}


struct UListStyle : public Style {
  Style transp_style;
  UListStyle();
  
  virtual const Style& getStyle(UpdateContext* ctx) const {
    ListBox* l = null; 
    if (ctx && ctx->obj 
        && (l = dynamic_cast<ListBox*>(ctx->obj)) && l->isTransparent())
      return transp_style; 
    else return *this;
  }
};

UListStyle::UListStyle() {
  transp_style.orient = Orientation::VERTICAL;
  transp_style.halign = Halign::FLEX;
  transp_style.valign = Valign::TOP;
  transp_style.hspacing = 1;
  transp_style.vspacing = 1;
  
  setColors(Color::inherit, Color::white);
  setColor(UOn::DISABLED, Color::disabled);
  setBgcolors(Color::white, Color::black);
  orient = Orientation::VERTICAL;
  halign = Halign::FLEX;
  valign = Valign::TOP;
  hspacing = 1;
  vspacing = 1;
  // il faut de l'espace autour des listes pour le scroll
  local.padding.set(4,5);
  local.border = &Border::shadowIn;
}


Style* ListBox::createStyle() {
  return new UListStyle;
}

ListBox::ListBox(const Args& a) : transp_style(false) {
  add(a);
}

ListBox& ListBox::addItem(const String& s) {
  add(uitem(ustr(s)));
  return *this;
}

ListBox& ListBox::addItem(String& s, bool dup) {
  if (dup) add(uitem(ustr(s))); else add(uitem(s));
  return *this;
}

ListBox& ListBox::addItems(const char* tab[]) {
  for (unsigned int k = 0; tab[k] != null ; k++)
    addItem(tab[k]);
  return *this;
}

ListBox& ListBox::addItems(const std::vector<String*>& tab, bool dup) {
  for (unsigned int k = 0; k < tab.size(); k++)
    if (tab[k]) addItem(*tab[k], dup);
  return *this;
}

ListBox& ListBox::addItems(const String& item_list, const String& sep) { 
  vector<String*> tab;
  unsigned int count = item_list.tokenize(tab, sep);
  for (unsigned int k = 0; k < count; k++)
    if (tab[k]) add(uitem(tab[k]));
  return *this;
}

ListBox& ListBox::addItems(const Args& prefix, const char* tab[]) {
  for (unsigned int k = 0; tab[k] != null ; k++)
    add(uitem(prefix + ustr(tab[k])));
  return *this;
}

ListBox& ListBox::addItems(const Args& prefix,
                             const std::vector<String*>& tab, bool dup) {
  for (unsigned int k = 0; k < tab.size(); k++)
    if (tab[k]) {
      if (dup) add(uitem(prefix + ustr(*tab[k])));
      else add(uitem(prefix + tab[k]));
    }
  return *this;
}

ListBox& ListBox::addItems(const Args& prefix, const String& item_list, 
                             const String& sep) { 
  vector<String*> tab;
  unsigned int count = item_list. tokenize(tab, sep);
  for (unsigned int k = 0; k < count; k++)
    if (tab[k]) add(uitem(prefix + tab[k]));
  return *this;
}


static void adjustSize(Menu* m, int i) {
  // inutile (et dengereux) si l'appli est terminee)
  if (Application::isExiting()) return;
  m->update(Update::ADJUST_WIN_SIZE);
}

ComboBox::ComboBox() :
TextField(), plist(null), text_only(false) {
  constructs(Args::none);
}

ComboBox::ComboBox(ListBox& l, const Args& a) :
TextField(a), plist(l), text_only(false) {
  constructs(a);
}

void ComboBox::constructs(const Args& _a) {
  pmenu = upopmenu(*plist);
  
  setAttr(*new UCompositeBorder
          (Valign::flex + Halign::right
           + uitem(Symbol::down + UOn::action /ucall(this, &ComboBox::openMenuCB))
           + *pmenu
           ));
  addAttr(uvcenter());
  pentry = new Element(new String(""));  // il faut un "" sinon on ne peut pas entre de texte!
  add(*pentry);
  
  plist->addAttr(Border::none);   // virer le border: horrible dans un menu
  plist->addAttr(UOn::action / ucall(this, &ComboBox::actionCB));
  plist->addAttr(UOn::change / ucall(this, &ComboBox::changedCB));
  plist->addAttr(UOn::addChild / ucall(pmenu(), 1, adjustSize));
  plist->addAttr(UOn::removeChild / ucall(pmenu(), -1, adjustSize));
  
  // erreur: ca appelle les callbacks  !!!
  plist->choice().setSelectedIndex(0); 
}

Style* ComboBox::createStyle() {
  return TextField::createStyle();
}


void ComboBox::actionCB(Event& e) {  //UElemEvent
  //cerr << "ComboBox::actionCB"<< typeid(e).name()  << endl;
  //EventFlow* flow = source_event.getFlow();
  // View* hardwv = ce n'est pas la meme!
  syncText();
  //Event e2(UOn::action, null, flow);
  //e2.setSource(this);
  //e2.setTarget(source_event.getTarget());
  Event e2(UOn::action, this, e.getSource());  //UElemEvent
  fire(e2);
}


void ComboBox::changedCB(Event& e) {  //UElemEvent
  //cerr << "ComboBox::changedCB" << typeid(e).name() <<endl;
  //EventFlow* flow = source_event.getFlow();
  //Event e2(UOn::change, null, flow);
  //e2.setSource(this);
  //e2.setTarget(source_event.getTarget());
  Event e2(UOn::change, this, e.getSource());    //UElemEvent
  fire(e2);
}


void ComboBox::syncText() {
  bool auto_up = pentry->isAutoUpdate();
  pentry->setAutoUpdate(false);
  
  edit().setCaretStr(null, 0);  // eviter plantages!
  String* first_s = null;
  pentry->removeAll(false);
  Box* selection = choice().getSelectedItem();
  
  if (selection) {
    for (ChildIter i = selection->cbegin(); i!=selection->cend(); ++i) {
      String* s = dynamic_cast<String*>(*i);
      if (s) {
        String* scop = &ustr(*s);
        if (!first_s) first_s = scop;
        if (scop) pentry->add(*scop);
      }
      else if (!text_only && *i) pentry->add(*i);
    }
  }
  
  // retablir curseur  ICI IL Y A UN TRUC QUI DECONNE: pas de curseur au depart!
  if (first_s) edit().setCaretStr(first_s, 0);
  pentry->setAutoUpdate(auto_up);
  pentry->update();
}


void ComboBox::openMenuCB(InputEvent& e) {
  //cerr << "ComboBox::openMenuCB" << typeid(e).name() << endl;
  View* combo_view = e.getView() ? e.getView()->getParentView() : null;
  if (combo_view) {
    pmenu->setPos(*combo_view, Point(0, combo_view->getHeight()));
    pmenu->show();
  }
}


Choice& ComboBox::choice() {return plist->choice();}
//const Choice& ComboBox::choice() const {return plist->choice();}

ComboBox& ComboBox::setTextMode(bool st) {
  text_only = st;
  syncText();
  return *this;
}

}


