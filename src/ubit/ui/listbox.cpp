/*
 *  listbox.cpp: List Boxes
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

#include <ubit/ubit_features.h>
#include <iostream>
#include <ubit/ubit.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT


URadiobox::URadiobox(const Args& a) 
: pchoice(new UChoice()) {
  addAttr(*pchoice);
  add(a);
}

Box* URadiobox::getSelectedItem() const {
  return choice().getSelectedItem();
}

int URadiobox::getSelectedIndex() const {
  return choice().getSelectedIndex();
}

URadiobox& URadiobox::setSelectedItem(Box& i) {
  choice().setSelectedItem(i);  // c'est title qui est selected !
  return *this;
}

URadiobox& URadiobox::setSelectedItem(Box* i) {
  choice().setSelectedItem(i);  // c'est title qui est selected !
  return *this;
}

URadiobox& URadiobox::setSelectedIndex(int i) {
  choice().setSelectedIndex(i);
  return *this;
}

UListbox& UListbox::setSelectedItem(Box& i) {
  choice().setSelectedItem(i);  // c'est title qui est selected !
  return *this;
}

UListbox& UListbox::setSelectedIndex(int i) {
  choice().setSelectedIndex(i);
  return *this;
}


struct UListStyle : public UStyle {
  UStyle transp_style;
  UListStyle();
  
  virtual const UStyle& getStyle(UpdateContext* ctx) const {
    UListbox* l = null; 
    if (ctx && ctx->obj 
        && (l = dynamic_cast<UListbox*>(ctx->obj)) && l->isTransparent())
      return transp_style; 
    else return *this;
  }
};

UListStyle::UListStyle() {
  transp_style.orient = UOrient::VERTICAL;
  transp_style.halign = Halign::FLEX;
  transp_style.valign = Valign::TOP;
  transp_style.hspacing = 1;
  transp_style.vspacing = 1;
  
  setColors(Color::inherit, Color::white);
  setColor(UOn::DISABLED, Color::disabled);
  setBgcolors(Color::white, Color::black);
  orient = UOrient::VERTICAL;
  halign = Halign::FLEX;
  valign = Valign::TOP;
  hspacing = 1;
  vspacing = 1;
  // il faut de l'espace autour des listes pour le scroll
  local.padding.set(4,5);
  local.border = &Border::shadowIn;
}


UStyle* UListbox::createStyle() {
  return new UListStyle;
}

UListbox::UListbox(const Args& a) : transp_style(false) {
  add(a);
}

UListbox& UListbox::addItem(const String& s) {
  add(uitem(ustr(s)));
  return *this;
}

UListbox& UListbox::addItem(String& s, bool dup) {
  if (dup) add(uitem(ustr(s))); else add(uitem(s));
  return *this;
}

UListbox& UListbox::addItems(const char* tab[]) {
  for (unsigned int k = 0; tab[k] != null ; k++)
    addItem(tab[k]);
  return *this;
}

UListbox& UListbox::addItems(const std::vector<String*>& tab, bool dup) {
  for (unsigned int k = 0; k < tab.size(); k++)
    if (tab[k]) addItem(*tab[k], dup);
  return *this;
}

UListbox& UListbox::addItems(const String& item_list, const String& sep) { 
  vector<String*> tab;
  unsigned int count = item_list.tokenize(tab, sep);
  for (unsigned int k = 0; k < count; k++)
    if (tab[k]) add(uitem(tab[k]));
  return *this;
}

UListbox& UListbox::addItems(const Args& prefix, const char* tab[]) {
  for (unsigned int k = 0; tab[k] != null ; k++)
    add(uitem(prefix + ustr(tab[k])));
  return *this;
}

UListbox& UListbox::addItems(const Args& prefix,
                             const std::vector<String*>& tab, bool dup) {
  for (unsigned int k = 0; k < tab.size(); k++)
    if (tab[k]) {
      if (dup) add(uitem(prefix + ustr(*tab[k])));
      else add(uitem(prefix + tab[k]));
    }
  return *this;
}

UListbox& UListbox::addItems(const Args& prefix, const String& item_list, 
                             const String& sep) { 
  vector<String*> tab;
  unsigned int count = item_list. tokenize(tab, sep);
  for (unsigned int k = 0; k < count; k++)
    if (tab[k]) add(uitem(prefix + tab[k]));
  return *this;
}


static void adjustSize(UMenu* m, int i) {
  // inutile (et dengereux) si l'appli est terminee)
  if (Application::isExiting()) return;
  m->update(Update::ADJUST_WIN_SIZE);
}

UCombobox::UCombobox() :
UTextfield(), plist(null), text_only(false) {
  constructs(Args::none);
}

UCombobox::UCombobox(UListbox& l, const Args& a) :
UTextfield(a), plist(l), text_only(false) {
  constructs(a);
}

void UCombobox::constructs(const Args& _a) {
  pmenu = upopmenu(*plist);
  
  setAttr(*new UCompositeBorder
          (Valign::flex + Halign::right
           + uitem(USymbol::down + UOn::action /ucall(this, &UCombobox::openMenuCB))
           + *pmenu
           ));
  addAttr(uvcenter());
  pentry = new Element(new String(""));  // il faut un "" sinon on ne peut pas entre de texte!
  add(*pentry);
  
  plist->addAttr(Border::none);   // virer le border: horrible dans un menu
  plist->addAttr(UOn::action / ucall(this, &UCombobox::actionCB));
  plist->addAttr(UOn::change / ucall(this, &UCombobox::changedCB));
  plist->addAttr(UOn::addChild / ucall(pmenu(), 1, adjustSize));
  plist->addAttr(UOn::removeChild / ucall(pmenu(), -1, adjustSize));
  
  // erreur: ca appelle les callbacks  !!!
  plist->choice().setSelectedIndex(0); 
}

UStyle* UCombobox::createStyle() {
  return UTextfield::createStyle();
}


void UCombobox::actionCB(Event& e) {  //UElemEvent
  //cerr << "UCombobox::actionCB"<< typeid(e).name()  << endl;
  //UEventFlow* flow = source_event.getFlow();
  // View* hardwv = ce n'est pas la meme!
  syncText();
  //Event e2(UOn::action, null, flow);
  //e2.setSource(this);
  //e2.setTarget(source_event.getTarget());
  Event e2(UOn::action, this, e.getSource());  //UElemEvent
  fire(e2);
}


void UCombobox::changedCB(Event& e) {  //UElemEvent
  //cerr << "UCombobox::changedCB" << typeid(e).name() <<endl;
  //UEventFlow* flow = source_event.getFlow();
  //Event e2(UOn::change, null, flow);
  //e2.setSource(this);
  //e2.setTarget(source_event.getTarget());
  Event e2(UOn::change, this, e.getSource());    //UElemEvent
  fire(e2);
}


void UCombobox::syncText() {
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


void UCombobox::openMenuCB(UInputEvent& e) {
  //cerr << "UCombobox::openMenuCB" << typeid(e).name() << endl;
  View* combo_view = e.getView() ? e.getView()->getParentView() : null;
  if (combo_view) {
    pmenu->setPos(*combo_view, Point(0, combo_view->getHeight()));
    pmenu->show();
  }
}


UChoice& UCombobox::choice() {return plist->choice();}
//const UChoice& UCombobox::choice() const {return plist->choice();}

UCombobox& UCombobox::setTextMode(bool st) {
  text_only = st;
  syncText();
  return *this;
}

}


