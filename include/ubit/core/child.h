/*
 *  child.hpp [internal implementation]
 *  Ubit GUI Toolkit - Version 6
 *  (C) 2009 | Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
 *
 * ***********************************************************************
 * COPYRIGHT NOTICE : 
 * THIS PROGRAM IS DISTRIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE 
 * IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. 
 * YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER THE TERMS OF THE GNU 
 * GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION; 
 * EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 * SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
 * **********************************************************************/

#ifndef UBIT_CORE_CHILD_H_
#define	UBIT_CORE_CHILD_H_

namespace ubit {

/**
 * [impl] Internal implementation of a child node.
 */
class Child {
public:
  Child(Node& o) : obj(&o), parent(0), cond(0) {}
  Child(Node* o) : obj(o),  parent(0), cond(0) {}
  Child(Node* o, const Condition& c) : obj(o), parent(0), cond(&c) {}
  Child(const char* s);

  Node* operator*() {return obj;}  
  const Node* operator*() const {return obj;}  
  
  Element* getParent() {return parent;}

  const Condition* getCond() const {return cond;}
  void setCond(const Condition& c)  {cond = &c;}
  void setCond(const Child& c) {cond = c.cond;}
  
 private:
  friend class Parent;
  friend class Node;
  friend class Element;
  Node* obj;
  Element* parent;
  const Condition* cond;
};


template <class _Iter>
class _ChildIter : public _Iter {
public:
  _ChildIter() : _Iter() {}
  _ChildIter(const _Iter& i) : _Iter(i) {}
  _ChildIter(const _ChildIter& i) : _Iter(i) {}
  
  Node*  operator*()    {return static_cast<Node*>(*_Iter::operator*());}
  Child& child()        {return _Iter::operator*();}
  const Condition* getCond() {return _Iter::operator*().getCond();}
};


/** forward iterator in a child or attribute list.
* @see: Element::cbegin(), Node::abegin(), Children.
*/
typedef _ChildIter<std::list<Child>::iterator> ChildIter;

/** reverse iterator in a child or attribute list.
* @see: Element::crbegin(), Children.
*/
typedef _ChildIter<std::list<Child>::reverse_iterator> ChildReverseIter;

/** Child (or attribute) list.
 * @see: ChildIter, Element::children(), Element::attributes(), Attribute::attributes().
 */
class Children : public std::list<Child> {
public:
  ChildIter at(int pos);
  ///< returns an iterator pointing to the object at this position; returns the last child if 'pos' = -1 and end() if 'pos' is out of bounds.

  ChildIter find(const Node& child);
  ///< returns an iterator pointing to 'child'; returns end() if 'child' does no belong to the child list.
  
  ChildIter findStr(const String& value, bool ignore_case = true);
  /**< searches a string (String) which is equal to 'value'; returns end() if there is no such child.
    * this function compares the content of strings (not their addresses).
    */
  
  ChildIter findBox(const String& value, bool ignore_case = true);
  /**< searches a box (Box or subclass) which contains a string which is equal to 'value'; returns end() if there is no such child.
    * this function compares the content of strings (not their addresses).
    */
  
  /** returns an iterator to the first child that derives from this class.
    * "derives" means: this class or a direct or indirect subclass. Exemple:
    * <pre>
    *    Color* c = null;
    *    ChildIter i = children.findClass(c);
    * </pre>
    */
  template <class CC>
    ChildIter findClass(CC*& c) {
      c = null;
      for (ChildIter i = begin(); i != end(); ++i) {
        if (dynamic_cast<CC*>(*i)) {c = (CC*)*i; return i;}
      }
      return end();
    }
};

}
#endif // UBIT_CORE_CHILD_H_

