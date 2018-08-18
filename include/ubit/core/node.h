/*
 *  node.h: base class for objects that can be added to the scene graph
 *  Ubit GUI Toolkit - Version 8
 * 
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

#ifndef UBIT_CORE_NODE_H_
#define	UBIT_CORE_NODE_H_

#include <ubit/core/object.h>

#include <ubit/udefs.hpp>
#include <ubit/core/child.h>
#include <ubit/core/parent.h>

namespace ubit {

/**
 * Base class of objects that can be added to the UBIT scene graph.
 *
 * A Ubit scene graph can be seen as a XML document:
 * - ubit::Node corresponds to "Node", the base class of XML nodes in the DOM model
 * - subclasses ubit::Attribute and ubit::Element respectively correspond to
 *  "Atrribute" and "Element"
 * - ubit::Box, which derives from ubit::Element, is the base class for widgets,
 *   and ubit::Window for windows
 *
 * ubit::Node derives from ubit::Object, the base class for (almost) all Ubit objects. 
 * 
 */
class Node : public Object {

  Node(Node const &) = delete;
  
  Node& operator=(Node const &) = delete; 
    
public:
  /// XML object types.
  enum NodeType {
    ELEMENT_NODE                   = 1,
    ATTRIBUTE_NODE                 = 2,
    TEXT_NODE                      = 3,
    CDATA_SECTION_NODE             = 4,
    ENTITY_REFERENCE_NODE          = 5,
    ENTITY_NODE                    = 6,
    PROCESSING_INSTRUCTION_NODE    = 7,
    COMMENT_NODE                   = 8,
    DOCUMENT_NODE                  = 9,
    DOCUMENT_TYPE_NODE             = 10,
    DOCUMENT_FRAGMENT_NODE         = 11,
    NOTATION_NODE                  = 12
  };
  
  /// Ubit display types.
  enum DisplayType {
    WINLIST			=-1,
    UNDISPLAYBLE	= 0,
    ATTRIBUTE,
    DATA,
    INLINE,
    BLOCK,
    BORDER,
    HARDWIN,
    SOFTWIN
  };

protected:
  /// constructor, can anly be called by subclasses because Node is abstract.
  explicit Node();
  
  /// destructor, note that children are recursively destroyed except if pointed elsewhere (@see class Node).
  virtual ~Node() {destructs();}
  
public:
  virtual int get_node_type() const {return 0;}
  ///< returns the XML node type.
  
  virtual const String& get_node_name() const;
  ///< return the XML node name.

  virtual String get_node_value() const;
  ///< return the XML node value.

  virtual DisplayType get_display_type() const {return UNDISPLAYBLE;}
  virtual int is_displayable() const {return get_display_type() > 0;}

  /**
   * returns true if this object a child of 'parent'.
   *  'indirect' specifies if indirect childhood is taken into account. 
   */ 
  virtual bool is_child_of(const Element& parent, bool indirect = true) const;
  
  ParentIter pbegin() const {return parents().begin();}
  /**< returns an iterator to the beginning of the list of direct parents.
    * UParentIter is a parent iterator. It is compatible with standard STL algorithms. 
    * This example prints the class name of the direct parents of an object :
    * <pre>
    *    Node* obj = ...;
    *    for (UParentIter p = obj->pbegin(); p != obj->pend(); ++p)
    *        cout << (*p)->getClassName() << endl;
    * </pre>
    * @see also: parents(), abegin(), aend().
    */
  
  ParentIter pend() const {return parents().end();}
  ///< returns an iterator to the end of the parent list (@see pbegin()).

  virtual Parents& parents() const {return _parents;}
  ///< returns the list of direct parents (@see pbegin()).
  
  virtual Element* get_parent(int pos = 0) const;
  /**< returns the Nth direct parent.
    * returns the first parent if 'pos' = 0 and the last parent if 'pos' = -1.
    * returns null and issues an error message if 'pos' is out of range.
    * Notes:
    * - objects can have several parents
    * - getParents() is more efficient for accessing multiple parents.
    */
    
  virtual bool hasSceneGraphParent() const;   // redifined from Object
  /**< this object has at least one parent in the scene graph.
    * objects can have non scene graph parents in some cases. Such parents are not
    * taken into account for deciding whether objects can be automatically destructed. 
    */
  
  template <class CC>
    CC* findParent() const {
      for (UParentIter p = pbegin(); p != pend(); ++p) {
        if (dynamic_cast<CC*>(*p)) {return (CC*)*p;}
        else return ((Node*)(*p))->findParent<CC>();
      }
      return null;
    }
  ///< returns first the direct or indirect parent with that class.

  virtual int getBoxParents(std::vector<Box*>& parvect) const;
  ///< retrieves direct and indirect parents that are boxes.

  virtual Box* getParent(const View*) const;
  /**< returns the direct or indirect parent that contains this view.
    * intermediate Element parents are ignored (ie. traversed) by this function.
    */
  
  virtual View* getParentView(const View*) const;
  /**< returns the direct or indirect parent's view that contains this view.
    * intermediate Element parents are ignored (ie. traversed) by this function.
    */
  
  virtual Box*  getParent(const InputEvent&) const;
  
  
  virtual View* getParentView(const InputEvent&) const;
  /* returns the direct or *indirect* Box parent (resp. parent's view) that contains this event.
    * intermediate Element parents are ignored (ie. traversed) by this function.
    */
     
  virtual void removeFromParents(bool update_parents = true);
  /**< [impl] removes this object from all direct parents.
    * notes:
    * - objects can have several parents!
    * - this function is potentially dangerous: Element::remove() methods
    *   should be prefered when possible.
    */
   
  // - - - misc.  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  friend Child operator/(const Condition&, Node&);  
  /**< conditional expression for callback functions and conditionnal objects.
    * example:
    * <pre>
    *    struct Demo {
    *       void saveFile();
    *       void showMsg(const char*);
    *       ....
    *    };
    *    Demo* d = new Demo;
    *    // NB: ubutton(...) returns *new Button(...)
    *    Box& b = ubutton(" Save..." + UOn::action / ucall(d, &Demo::saveFile));
    *    ....
    *    b.addAttr(UOn::enter / ucall(d, "Saves the file", &Demo::showMsg)
    * </pre>
    * @see: addAttr(), UCall, UOn, Flag for more details.
    */
  
  friend Child operator/(const Condition&, Node*);
  ///< see operator/(const Condition&, Node&).
  
  virtual bool fire(Event&) const;
  ///< fires callback functions that match this event.

  virtual Node& removeAllAttrs(bool autodel = true);
  ///< removes all attributes (if any, redefined by subclasses).
      
  virtual void update() {}
  ///< updates object graphics.

  bool isAutoUpdate() const {return !omodes.DONT_AUTO_UPDATE;}
  ///< return current update policy: see setAutoUpdate().
  
  Node& setAutoUpdate(bool state = true);
  /**< changes the update policy of this object when its is modified.
    * - if 'state' is true, the object if automatically updated when modified
    *   (either by adding/removing children for Element and subclasses,
    *   or by setting new values for Data, String, Attribute ...)
    * - if 'state' is false, the update() function must be called
    *   explicitely to update the graphics. This can be useful in certain cases
    *   to avoid intermediate updates in order to to speed up rendition
    */

protected:
  virtual void addingTo(Child&, Element& parent) {}
  ///< called when this object is added to a parent.
  
  virtual void removingFrom(Child&, Element& parent) {}
  /**< called when this object is removed from a parent (SEE DETAILS IF YOU REDEFINE THIS METHOD).
   * !BEWARE: subclasses that redefine removingFrom() MUST HAVE A DESTRUCTOR that
   * calls destructs().
   * <br>DETAILS: this is because of a C++ subtletly: as polymorphism is deactivated
   * in destructors, a redefined removingFrom() method won't be called except if 
   * destructs() - that calls removingFrom() - is called in the destructor of the
   * class that redefines removingFrom()). 
   */
  
  virtual void destructs();
  ///< unlinks the object from its parents and destroys its children.
    
public:
  void fireParents(const Condition& c, Node* n) const {_parents.fireParents(c,n);}
  
  void updateAutoParents(const Update& m) {_parents.updateAutoParents(m);}    
  virtual void addChangeCall(UCall&);
  virtual Element* getSubGroup() const {return null;}
  
  void _addAttr(const Child& attribute);
  ///< [impl] to be removed @@@.

  ChildIter _abegin() const {return _attributes.begin();}
  ///< [impl] to be removed @@@.

  ChildIter _aend() const {return _attributes.end();}
  ///< [impl] to be removed @@@.
  
protected:
  friend class Child;
  friend class Args;
  friend class Parents;
  friend class Element;
  mutable Parents _parents;
  mutable Children _attributes;  // attributes (including callbacks)

};

}
#endif //UBIT_CORE_NODE_H_
