/*
 *  treebox.cpp
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


Treebox& utreebox(const Args& a) {return *new Treebox(a);}

Treebox::~Treebox() {}

// ATT: TYPES LIES !
static struct IsSelectableByTree : public Choice::IsSelectable {
  virtual bool operator()(const Box* box) const {
    return dynamic_cast<const Item*>(box);   // !!!!!!!!
  }
} is_selectable_by_tree;

Treebox::Treebox(const Args& a) : ListBox(a) {
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
  static Padding& offset = upadding(0, UIGNORE).setLeft(40);
  psubnodes = &uvbox(children);
  psubnodes->addAttr(offset);
  psubnodes->addAttr(VSpacing::none);
  addAttr(VSpacing::none);
  
  pexpander = new Box
    (UOn::select   / Symbol::down
     + UOn::deselect / Symbol::right
     + UOn::select   / ushow(*psubnodes, true)
     + UOn::deselect / ushow(*psubnodes, false)
     );
  pexpander->setSelected(false);
  psubnodes->show(false);
  
  plabel = new Item(*pexpander +  " " + _title);
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


