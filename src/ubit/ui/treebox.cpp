/*
 *  treebox.cpp
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
namespace ubit {


Treebox& utreebox(const Args& a) {return *new Treebox(a);}

Treebox::~Treebox() {}

// ATT: TYPES LIES !
static struct IsSelectableByTree : public UChoice::IsSelectable {
  virtual bool operator()(const Box* box) const {
    return dynamic_cast<const UItem*>(box);   // !!!!!!!!
  }
} is_selectable_by_tree;

Treebox::Treebox(const Args& a) : Listbox(a) {
  choice().setSelectionRule(is_selectable_by_tree);
  addAttr(Border::none);
}

Treenode* Treebox::getSelectedNode() const {
  Element* title = choice().getSelectedItem();
  Element* node = title ? title->getParent(0) : null; // c'est title qui est selected !
  return node ? dynamic_cast<Treenode*>(node) : null;
}

void Treebox::setSelectedNode(Treenode& n) {
  choice().setSelectedItem(*n.plabel);  // c'est title qui est selected !
}

int Treebox::getSelectedIndex() const {
  return choice().getSelectedIndex();
}

Treebox& Treebox::setSelectedIndex(int i) {
  choice().setSelectedIndex(i);
  return *this;
}


Treenode& utreenode(const Args& title) {
  return *new Treenode(title);
}

Treenode& utreenode(const Args& title, const Args& children) {
  return *new Treenode(title, children);
}

Treenode::Treenode(const Args& title) {
  constructs(title, Args::none);
}

Treenode::Treenode(const Args& title, const Args& children) {
  constructs(title, children);
}

void Treenode::constructs(const Args& _title, const Args& children) {
  static UPadding& offset = upadding(0, UIGNORE).setLeft(40);
  psubnodes = &uvbox(children);
  psubnodes->addAttr(offset);
  psubnodes->addAttr(UVspacing::none);
  addAttr(UVspacing::none);
  
  pexpander = new Box
    (UOn::select   / USymbol::down
     + UOn::deselect / USymbol::right
     + UOn::select   / ushow(*psubnodes, true)
     + UOn::deselect / ushow(*psubnodes, false)
     );
  pexpander->setSelected(false);
  psubnodes->show(false);
  
  plabel = new UItem(*pexpander +  " " + _title);
  Box::add(*plabel + *psubnodes);
}

void Treenode::expand(bool s) {
  pexpander->setSelected(s);
}

/*
void Treenode::setOffset(UHmargin& margin) {
  UHmargin* m = null;
  ChildIter i = psubnodes->attributes().findClass(m);
  if (m) remove(i);
  psubnodes->add(margin);
}
*/

void Treenode::addNode(Treenode& n) {
  psubnodes->add(n);
}
 
void Treenode::removeNode(Treenode& n) {
  psubnodes->remove(n);
}
 
}


