/*
 *  uparent.hpp [internal implementation]
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


#ifndef _uparent_hpp_
#define	_uparent_hpp_ 1

namespace ubit {

/** [impl] Internal implementation of a parent node.
*/
class Parent {
public:
  Parent(ChildIter i) : child_iter(i) {}
  
  Element* operator*() {return child_iter->parent;}
  const Element* operator*() const {return child_iter->parent;}
  
  Child& getChild() {return child_iter.child();}
  
private:
  ChildIter child_iter;
};


template <class _I>
struct _ParentIter : public _I {
  _ParentIter() : _I(null) {}       // !ATT: peut poser probleme si pas g++ !
  _ParentIter(const _I& i) : _I(i) {}
  _ParentIter(const _ParentIter& i) : _I(i) {}
  
  Element* operator*() {return static_cast<Element*>(*_I::operator*());}
  Parent&  parent() {return _I::operator*();}
};

/**
 * forward iterator in a parent list.
 * @see: Node::pbegin(), Parents.
 */
typedef _ParentIter<std::list<Parent>::iterator> UParentIter;

/**
 * Parent list.
 * see also: UParentIter and Element::parents().
 */
class Parents : public std::list<Parent> {
public:
  void removeFirst(Child*);
  /// removes the first element that is pointing to this child.

  void updateAutoParents(const Update&);
  ///< update parents that are in autoUpdate mode.

  void fireParents(const Condition& c, Node* n);
  ///< fire parents callbacks that match this event.
  
  void setParentsViewsModes(int vmodes, bool on_off);
  ///< changes the modes of parents views.
};

}
#endif
