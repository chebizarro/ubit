/*
 *  element.h: Element Nodes (lightweight containers that can have children)
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

#ifndef UBIT_CORE_ELEMENT_H_
#define	UBIT_CORE_ELEMENT_H_

#include <string.h>		// memset

#include <ubit/core/attribute.h>

#include <ubit/ui/update.h>
#include <ubit/core/args.h>
#include <ubit/core/string.h>

namespace ubit {
  
  /**
   * lightweight general purpose container.
   *
   * This class is a very lightweight and general container. In contrast with ubit::Box 
   * (and its subclasses) ubit::Element objects do not control ubit::Views and
   * do not manage screen rendering directly (they are just genuine containers)
   *
   * Notes:
   * - see ubit::Box for examples showing how to add and manage children and attributes
   * - objects can have several parents and they can be added several times
   *   to the same parent.
   * - many methods are inherited from superclasses.
   */
  class Element: public Node {

    Element& operator=(Element const &) = delete;
    
    explicit Element(Element const &) = delete;
    
  public:
     
    /**
     * creates a new Element; 
     * The argument can either be a single object pointer or reference, or a list 
     * of object pointers or references separated by + operators, example:
     * <pre>
     *   Button* b = new Button(UPix::disquette + "Save" + ucall(obj, saveFunc));
     * </pre>
     *
     * @see also the Element() shortcut, that returns: *new Element(arglist)
     */
    Element(Args node_arglist = Args::none);
    
    virtual ~Element();
    ///< destructor, note that children are recursively destroyed except if pointed elsewhere (@see class Node).
    
    virtual void destructs();
    
    virtual Element* toElem() {return this;}
    virtual const Element* toElem() const {return this;}
    
    static Style* createStyle();
    /**< static function that returns the style of this class.
     * this function MUST be redefined by subclasses if they use another Style.
     */  
    
    const Style& getStyle(UpdateContext*) const;
    /**< virtual function that returns the style of this object.
     * this function calls createStyle() the first time it is called, then it always
     * return the same Style object.
     * In contrast with createStyle(), which that must be redefined for each subclass,
     * there is no need to redefine getStyle().
     */
    
    virtual int getNodeType() const {return ELEMENT_NODE;}
    ///< returns the XML node type.
    
    virtual const String& getNodeName() const {return getClass().getName();}
    ///< return the XML node name (which is the class name).
    
    virtual int getDisplayType() const {return INLINE;}
    
    virtual void initNode(Document* context) {}
    ///< initialises the XML context of this node.
    
        
    Element& setAttr(Node& attribute);
    /**< adds or replaces this attribute in the ATTRIBUTE list.
     * NOTE that the previous attribute object is destroyed if it was created by 'new'
     * and is not referenced elsewhere by a widget or a UPtr smart pointer (@see Node)
     */
    
    Element& addAttr(const Args& attributes);
    /**< adds one or several attributes (deriving from Attribute) to the ATTRIBUTE list.
     * @see add(const Args&) for details (both functions work in the same way).
     */
    
    Element& removeAttr(Node& attribute, bool auto_delete = true);
    /**< removes/deletes an attribute (and its descendants) from the ATTRIBUTE list.
     * @see: remove(Node&, bool) for details (both functions work in the same way).
     * NOTE that the removed attribute object is destroyed if it was created by 'new'
     * and is not referenced elsewhere by a widget or a UPtr smart pointer (@see Node)
     */  
    
    Element& removeAllAttrs(bool auto_delete = true);
    /**< removes/deletes all attributes (and their descendants) in the ATTRIBUTE list. 
     * @see: remove(Node&, bool) for details (both functions work in the same way).
     * NOTE that the removed attributes are destroyed if they were created by 'new'
     * and are not referenced elsewhere by a widget or a UPtr smart pointer (@see Node)
     */
    
    bool getAttrValue(String& value, const String& attr_name) const;
    /**< retrieves the value of the attribute which class name is 'attr_name'.
     * false is returned if there is no such attribute in the  element's ATTRIBUTE list.
     * true is returned otherwise and the attribute value is stored in the 'value' argument.
     */
    
    Attribute* getAttr(const String& attr_name) const;
    /**< returns the attribute node which class name is 'attr_name'.
     * null is returned if there is no such attribute in the element's ATTRIBUTE list.
     * example: Color* col = (Color*) element.getAttr("Color");
     */
    
    template <class ATTR> 
    ATTR* getAttr() const {ATTR* a; _attributes.findClass(a); return a;}
    /**< retrieves the attribute node that derives from the "ATTR" C++ class.
     * null is returned if there is no such attribute in the element's ATTRIBUTE list.
     * example: Color* col = element.getAttr<Color>();
     */
    
    template <class ATTR> 
    ATTR& obtainAttr()  
    {ATTR* a; _attributes.findClass(a); if (!a) addAttr(a = new ATTR); return *a;}
    /**< retrieves or creates the element's attribute that derives from the "ATTR" C++ class.
     * if there is no such attribute in the element's ATTRIBUTE list, it is created
     * and added to the ATTRIBUTE list.
     * example: Color& col = element.getAttr<Color>();
     */

    virtual bool isParentOf(const Node& possible_child) const;
    ///< returns true if this object a direct or indirect parent of 'child'.
    
    Element& add(const Args& children);
    /**< adds one or several objects to the end of the CHILD list of this element.
     * The argument can either be a single object pointer or reference, or a list 
     * of object pointers or references separated by + operators:
     * <pre>
     *   Button* b = new Button();    // creates a Button (that derives from Element)
     *   b->add(UPix::disquette);       // adds a pixmap image to the button
     *   b->add("Save");                // adds a string that serves as a label
     *   b->add(ucall(obj, saveFunc));  // adds a callback function (@see UCall)
     * </pre>
     * is equivalent to:
     * <pre>
     *   Button* b = new Button();
     *   b->add(UPix::disquette + "Save" + ucall(obj, saveFunc));
     * </pre>
     * and also to:
     * <pre>
     *   Button* b = new Button(UPix::disquette + "Save" + ucall(obj, saveFunc));
     * </pre>
     *
     * Objects added to the CHILD list should derive from Element (typically, widgets
     * deriving from Box, Window), Data (viewable objects such as String, Image) or UCall
     * (callback objects). Certain attributes such as Color, Font, HAlign, VAlign
     * can also be added to the child list.
     *
     * Character strings (e.g. "abcd") are implicitely converted to String objects, so 
     * that add("abcd") is equivalent to: add( ustr("abcd") ). 
     *
     * For syntactical reasons, an argument list cannot solely contain pointers or strings
     * separated by + operators: one argument must then be dereferenced or appropriately
     * converted (exple: add(ustr("abcd") + "xyz") is correct, add("abcd" + "xyz") is not)
     *
     * The addAttr() method works as add() excepts that it adds arguments to the 
     * ATTRBITUTE list. Arguments should then derive from Attribute (attributes) or 
     * UCall (callback aobjects).
     *
     * Subclasses that require a specific behavior should not redefine this function
     * but addImpl(), that is called internally by all add functions
     *
     * @see also: add(const Args&, int pos), add(const Args&, ChildIter pos), addAttr(const Args&)
     */
    
    Element& add(const Args& children, int position);
    /**< adds one or several objects in the CHILD list before this position.
     * @see add(const Args& objects) for details: both functions work in the same way 
     * except that this function makes it possible to specify the position instead of
     * adding to the end of the child list.
     * 0 refers to the position before the first argument in the child list, special
     * value -1 refers to the position after the last child.
     */
    
    Element& add(const Args& children, ChildIter iterator);
    /**< adds one or several objects in the CHILD list before this iterator.
     * @see add(const Args& objects) for details: both functions work in the same way 
     * except that this function makes it possible to specify the position (through
     * an iterator, @see ChildIter) instead of adding to the end of the child list.
     */
    
    virtual Element& addImpl(const Args& nodes, ChildIter pos, Children& in_list);
    virtual bool addImpl1(const Child& node, ChildIter pos, Children& in_list);
    
    Element& remove(Node& child, bool auto_delete = true);
    /**< removes/deletes a child (and its descendants) from the CHILD list.
     * This function removes the first occurence of 'child' from the child list.   
     * Moreover, 'child' and its children are DESTROYED if 'auto_delete' is true, 
     * they were created by 'new' and they are not referenced elsewhere by a widget 
     * or a UPtr smart pointer (@see Node).
     * 
     * The same child can appear SEVERAL times in the child list, in which case it
     * is never deleted (the first occurence is just removed from the list)
     *
     * Subclasses that require a specific behavior should not redefine this function
     * but removeImpl(), that is called internally by all remove functions
     */
    
    Element& remove(int pos, bool auto_delete = true);
    /**< removes/deletes a child (and its descendants) at this position in the CHILD list.
     * 'pos' must be a valid position (positions start from 0, -1 indicates the last child)
     * @see: remove(Node&, bool) for important details on removal vs. deletion.
     */
    
    Element& remove(ChildIter pos, bool auto_delete = true);
    /**< removes/deletes a child (and its descendants) at this position in the CHILD list.
     * 'pos' must be an iterator pointing to the child that must be removed or deleted.
     * @see: remove(Node&, bool).
     */
    
    Element& removeAll(bool auto_delete = true);
    /**< removes/deletes all children (and their descendants) in the CHILD list. 
     * @see: remove(Node&, bool).
     */
    
    virtual Element& removeImpl(ChildIter begin, int N, bool autodel, Children& in_list);
    virtual bool removeImpl1(ChildIter pos, int auto_delete, Children& in_list);  
    
    /**
     * return the child list.
     * @see: child(), getChild(), cbegin(), cend(), crbegin(), crend(), attributes().
     */
    Children& getChildren() const {return children();}
    
    /**
     * return the number of children.
     * @see: child(), getChild(), cbegin(), cend(), crbegin(), crend(), attributes().
     */
    int getChildCount() const {return children().size();}
    
    ChildIter child(int pos) const {return children().at(pos);}
    /** returns an iterator pointing to the child at this position.
     * returns the last child if 'pos' = -1 / returns cend() if 'pos' is out of bounds 
     * @see also: Children find() functions
     */
    
    Node* getChild(int pos) const;  
    /** returns the child at this position.
     * returns the last child if 'pos' = -1 / returns null if 'pos' is out of bounds 
     * @see also: Children find() functions
     */
    
    ChildIter cbegin() const {return children().begin();}
    /**< returns a forward iterator pointing to the beginning of the CHILD list.
     * ChildIter is a child iterator. It is compatible with standard STL algorithms. 
     * This example prints the children class names:
     * <pre>
     *    Element* obj = ...;
     *    for (ChildIter i = obj->cbegin(); i != obj->cend(); ++i)
     *        cout << (*i)->getClassName() << endl;
     * </pre>
     * @see also: children(), cend(), crbegin(), crend(), abegin(), aend(), pbegin(), pend().
     */
    
    ChildIter cend() const {return children().end();}
    ///< returns a forward iterator pointing to the end of the child list (@see cbegin()).
    
    ChildReverseIter crbegin() const {return children().rbegin();}
    ///< returns a reverse iterator pointing to the beginning of the child list (@see cbegin()).
    
    ChildReverseIter crend() const {return children().rend();}
    ///< returns a reverse iterator pionting to the end of the child list (@see cbegin()).
    
    // - - - text - - - - - - - - - - - - - - - - - - - - - - - - -
    
    virtual String retrieveText(bool recursive = true) const;
    /**< collates and returns the text that is enclosed in this object.
     *  'recursive' means that the text included in descendants is also collated.
     */
    
    virtual void retrieveText(String& string, bool recursive = true) const;
    /**< collates and returns the text that is enclosed in this object.
     *  'recursive' means that the text included in descendants is also collated.
     */
    
    // - - - graphics and layout - - - - - - - - - - - - - - - - - -
    
    virtual bool isShown() const;
    /**< returns true if this object is shown.
     * isShown() returns true if this object is "showable" (see isShowable())
     * and all its parents are also "showable".
     * Note that "shown" objects are not always visible on the screen as they
     * may be hidden or clipped by other objects such as scroll panes.
     * @see: isShowable(), Element::show()
     */
    
    bool isShowable() const {return emodes.IS_SHOWABLE;}
    /**< returns true if this object can be shown.
     * isShowable() == true means that this object will be shown
     * if its parents can also be shown.
     * Note: this function has the same meaning as isVisible() in Java.
     * @eee: isShown(), Element::show()
     */
    
    void setShowable(bool s) const {emodes.IS_SHOWABLE = s;}
    ///< synonym for show(bool).
    
    virtual void show(bool = true);
    /**< shows/hides this object.
     * @see also: isShown(), isShowable(), update()
     */
    
    static void closeWin(InputEvent&, int status);
    /**< closes the first window (Dialog, Menu...) that contains this element.
     * @see: Window::close() and ucloseWin().
     */
    
    void repaint() {update(Update::paint);}
    ///< indicates that this object will be repainted (when the main loop becomes idle).
    
    void update() {update(Update::layoutAndPaint);}
    ///< indicates that this object will be layed out and repainted (when the main loop becomes idle).
    
    virtual void update(const Update& update_options, Display* = null);
    /**< indicates that the layout and/or the paint of this object will be updated.
     * this will be done when the main loop becomes idle.
     * - the first argument specify what must be updated and misc options (see Update)
     * - the second argument is only taken into account by redefinitions of this function
     * @see also: repaint(), update(), show().
     */
    
    void doUpdate() {doUpdate(Update::layoutAndPaint, null);}
    
    virtual void doUpdate(const Update&, Display* = null);
    /**< updates the layout and/or the paint of this object right now.
     * this function is called when the main loop becomes idle for repainting the object.
     * Usually, a client program should NOT call doUpdate() directly. Instead, it
     * should call update() or update(const Update&, Display*) that delay layout and
     * repaint actions until the main loop becomes idle. This improves performance
     * and makes double buffering possible.
     */
    
    // - - - events - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    Element& ignoreEvents(bool state = true);
    /**< if arg is true, this element and (its children) ignore events.
     * events are then received by the parent(s) of this element
     */
    
    Element& catchEvents(const Child& condition_callback_expr);
    /**< catches certain events before they reach the object's children.
     * Events are not delivered to their normal targets, instead, they are catched
     * by this element if it contains the target. The specified callback is then fired 
     * with this element as a "source" (@see Event::getSource()). If this callback
     * calls the event's propagate() method, this event will then be sent to the
     * normal target. This makes it possible to filter events and decide which of them
     * can reach targets. Also, some some useful functions can be triggered before
     * events reach their final targets and the events can be modified by settings UFlags
     * or by modifiying the event coordinates. 
     *
     * 3 categories of events can be currently catched:
     * - UOn::mpress, UOn::mrelease & UOn::mdrag mouse events. As, these events are 
     *   closely related, catching one of them will also catch the other 2 kinds.
     * - UOn::mmove mouse events.
     * - UOn::wheel events.
     *
     * 'condition_callback_expr' is a construct such as: 
     * <pre>   UOn::mpress / ucall(obj, &Obj::foo)   // (obj type is Obj*)  
     * </pre>
     * The callback method (eg. 'foo') will be fired BEFORE this condition (eg. 'UOn::mpress')
     * occurs in a direct and indirect child of this element. 
     *
     * To learn more about conditions @see classes UOn and Condition. To learn more about
     * callback objects @see class UCall. 
     */  
    
    Element& observeChildrenEvents(const Child& condition_callback_expr);
    /**< observe events that occur in children.
     * 'condition_callback_expr' is a construct such as: 
     * <pre>   UOn::action / ucall(obj, &Obj::foo)   // (obj type is Obj*)  
     * </pre>
     * The callback method (eg. 'foo') will be fired AFTER this condition (eg. 'UOn::action')
     * occurs in a direct and indirect child of this element. 
     *
     * To learn more about conditions @see classes UOn and Condition. To learn more about
     * callback objects @see class UCall. 
     */
    
    bool isIgnoringEvents() const {return emodes.IGNORE_EVENTS;}
    ///< returns true if events are ignored by this object; 
    
    bool hasCallback(long callback_id) const;
    ///< true if thie object has such a callback.
    
    virtual bool fire(Event&) const;
    ///< fires callback functions that match this event.  
    
    // - - - states - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    Element& enable(bool state = true) {return setEnabled(state);} 
    ///< shortcut for setEnabled(bool state = true).
    
    virtual Element& setEnabled(bool state = true, bool call_callbacks = true);
    /**< specifies whether this object is enabled.
     * - enabled means: sensible to high-level events such as UOn::action, UOn::arm, UOn::disarm...
     * - calls related callbacks if 'call_callbacks' is true
     *  @see: isEnabled(), enable(), disable().
     */
    
    bool isEnabled() const;
    ///< returns true if this object is currently enabled; @see setEnabled().
    
    // - - - - -
    
    virtual Element& setSelected(bool state = true, bool call_callbacks = true);
    /**< specifies whether this object is selected.
     * fires related callbacks if 'call_callbacks' is true
     */
    
    bool isSelected() const {return emodes.IS_SELECTED;}  
    
    bool isSelectable() const {return emodes.IS_SELECTABLE;}
    Element& setSelectable(bool state = true);
    
    // - - - - -
    
    bool isArmed() const;
    
    bool isArmable() const {return emodes.IS_ARMABLE;}
    /**< specifies whether this object can be armed.
     * an armable widget behaves as a button (but its appearance depends on its Style).
     * A widget can be made armable by calling its setArmable() methode
     */
    
    Element& setArmable(bool state = true);
    
    // - - - - -
    
    Element& setAutoRepeat(bool state = true);
    bool isAutoRepeat() const {return emodes.IS_AUTO_REPEAT;}
    
    bool isCrossable() const {return emodes.IS_CROSSABLE;}
    Element& setCrossable(bool state = true);
    
    Element& setBrowsable(bool = true);
    bool isBrowsable() const {return emodes.IS_BROWSABLE;}
    
    bool isDragged() const;
    bool isDraggable() const {return emodes.IS_DRAGGABLE;}
    Element& setDraggable(bool = true);
    
    bool isDroppable() const {return emodes.IS_DROPPABLE;}
    Element& setDroppable(bool = true);
    
    // - - - - -
    
    bool isTextEditable() const {return emodes.IS_TEXT_EDITABLE;}
    bool isFloating() const {return emodes.IS_FLOATING;}
    bool isVertical() const {return emodes.IS_VERTICAL;}    // && HAS_ORIENT ?  !!!!
    bool isWidthResizable()  const {return !emodes.IS_WIDTH_UNRESIZABLE;}
    bool isHeightResizable() const {return !emodes.IS_HEIGHT_UNRESIZABLE;}
    virtual bool isSubWin() const {return false;}  // redefined by USunWin => MUST be virtual!
    
    void disableMenuClosing(bool s = true) {emodes.DONT_CLOSE_MENUS = s;}
    ///< this object and its children wont close menus when clicked if argument is true.
    
    bool isMenuClosingDisabled() const {return emodes.DONT_CLOSE_MENUS;}
    ///< true if this object and its children do not close menus when clicked.
        
    struct Modes {
      unsigned IS_SHOWABLE:1;      ///< can be shown if parents are shown (see isShowable() and isShown())
      unsigned IS_BROWSABLE:1;     ///< can browse (as for lists and menus)
      unsigned IS_CROSSABLE:1;     ///< detects (and shows) UOn::enter and UOn::leave conditions
      unsigned IS_ARMABLE:1;       ///< can be armed 
      unsigned IS_AUTO_REPEAT:1;   ///< auto-repeats UOn::arm while armed
      unsigned IS_DRAGGABLE:1;     ///< can be dragged (with button 1 or button2)
      unsigned IS_DROPPABLE:1;     ///< can be dropped
      unsigned IS_SELECTABLE:1;    ///< can be selected (as for a checkbox)
      unsigned IS_SELECTED:1;      ///< this object is currently selected.
      unsigned IS_TEXT_SELECTABLE:1;///< can select included text
      unsigned IS_TEXT_EDITABLE:1; ///< can edit included text (ie. the group contains a TextEdit brick)
      unsigned IS_WIDTH_UNRESIZABLE:1;
      unsigned IS_HEIGHT_UNRESIZABLE:1;
      unsigned IS_FLOATING:1;       ///< has a Position attr in floating mode.
      unsigned IS_VERTICAL:1;       ///< has a vertical orientation (see HAS_ORIENT).
      unsigned HAS_ORIENT:1;        // A REVOIR @@@@ !!!!.
      unsigned HAS_CURSOR:1;        ///< has a cursor attr.
      unsigned HAS_LAYOUT:1;        ///< has a layout attr.
      unsigned HAS_TIP:1;           ///< has a tool tip.
      unsigned DONT_CLOSE_MENUS:1;
      unsigned OBSERVE_EVENTS:1;
      unsigned IGNORE_EVENTS:1;    ///< this object ignores events.
      enum {
        CATCH_MOUSE_MASK = 1<<0, CATCH_MOUSE_MOVE_MASK = 1<<1, 
        CATCH_WHEEL_MASK = 1<<2, CATCH_KEY_MASK = 1<<3
      };
      unsigned CATCH_EVENTS:4;  ///< this object can catch mouse events.
    };
    
    enum {REMOVE_FROM_PARENTS = -1};
    
    bool isBrowsingGroup() {return emodes.IS_BROWSABLE;}       // !!!
    //virtual Element* getBrowsingGroup() {return emodes.IS_BROWSABLE ? this : null;} 
    
    //Element& setConst() {Object::setConst(); return *this;} // redefined from Object.
    
    virtual Children& children() const {return _children;}
    /*[impl] return the child list.
     * @see: child(), getChild(), cbegin(), cend(), crbegin(), crend(), attributes().
     */
    
    virtual Children& attributes() const {return _attributes;}
    /*[impl] returns the attribute list.
     * @see: abegin(), aend(), children().
     */
    
    ChildIter abegin() const {return attributes().begin();}
    /*[impl] returns an iterator to the beginning the ATTRIBUTE list.
     * works in the same was as cbegin() cend() for the CHILD list.
     * @see: aend(), attributes()
     */
    
    ChildIter aend() const {return attributes().end();}
    //[impl] returns an iterator to the end of the attribute list (@see abegin()).
    
    Object::State getInterState() const {return ostate;}
    //[impl] returns the internal interactive state.
    
    void setInterState(Object::State);
    //[impl] changes the internal interactive state.
    
    virtual const String* getTextSeparator() const;
    ///< returns the text separator used by retrieveText() for separating enclosed children
    
    virtual void deleteViewsInside(const std::vector<View*>& parent_views);
    
    virtual void initView(View* parent_view);
    virtual void initChildViews(Element*);
    virtual void highlight(bool state) {};
    
    virtual int retrieveRelatedViews(std::vector<View*>& collating_vector) const;
    /* returns the views that are related to this element.
     * retrieves all the views where this element currently appears. This list is 
     * obtained  by collating the views of all the Box parents of this element
     * (this may be indirect parents as this element may be a child of another element
     * that is a child of a Box, and so on).
     *
     * - 'collating_vector' is not emptied: the views are added to the end of what
     *   it already contains.
     * - the function returns the number of views that were added (which may thus differ
     *   from the total size of 'collating_vector')
     *
     * Note that elements and boxes can have several parents, so that multiple wiews
     * can be associated to the same widget (see Element and Box classes for details)
     */
    
    Element& _setArmableAndSelectable(bool state);
    
    /* [impl] behavioral functions.
     * these functions are called when the object receives the corresponding events.
     * Usually, these functions should not be redefined by clients. Instead, clients
     * should add UOn::xxx conditions that are called by these functions
     * @see class UOn.
     */
    
    virtual void keyPressBehavior(KeyEvent&);
    virtual void keyReleaseBehavior(KeyEvent&);
    virtual void enterBehavior(InputEvent&, bool is_browing);
    virtual void leaveBehavior(InputEvent&, bool is_browing);
    virtual void armBehavior(InputEvent&, bool is_browing);
    virtual void disarmBehavior(InputEvent&, bool is_browing);
    virtual void actionBehavior(InputEvent&);
    
  protected:
    mutable Modes emodes;
    long callback_mask;
    mutable Children _children;
    virtual int   _getTextLength(bool recursive) const;
    virtual char* _getTextData(char *ptr, bool recursive) const;
  };
  
  inline Element& uelem(Args args = Args::none) {return *new Element(args);}
  ///< shortcut function that returns *new Element().
    
}
#endif // UBIT_CORE_ELEMENT_H_
