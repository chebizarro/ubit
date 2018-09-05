/*
 *  box.h: Box Elements (elements that manage 1 or N screen View(s))
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

#ifndef UBIT_UI_BOX_H_
#define	UBIT_UI_BOX_H_

#include <memory>

#include <ubit/core/element.h>

namespace ubit {
  
  /**
   * Box container.
   * 
   * Box is the base class for widgets:
   * - interactors: Button, Item, TextField, TextArea...
   * - containers: Hbox, Vbox, MenuBar...
   * - windows: Window, Dialog, Menu...
   * Most widgets' methods are inherited from Box and Element.
   *
   *
   * IMPORTANT NOTE ON RECURSIVE DESTRUCTION AND SMART POINTERS:
   * When a Box is destroyed, its direct and indirect children are ALSO destroyed
   * EXCEPT if they have other parents OR if they are pointed by a std::unique_ptr<> Smart Pointer.
   * @see class Node for details.
   *
   * Multiple parents and multiple views:
   * - Box objects can have several parents (that can also have multiple parents).
   *   They are then replicated on the screen(s), each replication corresponding to a View.
   *   These views are synchronized when the Box content is modified.
   *   A Box controls as many UViews as there are paths from the root of the instance 
   *   graph to this Box. @see: View.
   *
   * Transparency:
   *  - boxes (and their subclasses) can be made fully transparent by adding
   *    the Background::none property, or partially transparent by adding a 
   *    Alpha property to them.
   *
   *  Layout:
   *  - ubit::Box, ubit::Hbox, ubit::Bar : horizontal layout by default
   *    (add ubit::Orient::vertical for vertical layout). Children are adjusted
   *    in the vertical direction (@see: uvflex() for details)
   *  - ubit::Vbox : vertical layout by default. Children are adjusted in horizontal
   *    direction adjusted (@see: uhflex() for details)
   *  - ubit::Flowbox : "flow" layout (similar to the layout of an HTML page)
   *    Typically used from creating text areas or hypermedia widgets.
   *
   *  AutoResize:
   *  - Box, Hbox, Vbox, Bar are resized when children are added, removed or
   *    resized
   *  - Textbox, Textfield, etc. are not resized when their children are modified.
   *    For this reason, they are well adapted for displaying text
   *  - Flowbox, Textarea implement a specific behavior: their height changes 
   *    but their width remains constant
   */
  class Box: public Element {
  public:
    
    /**
     * creates a new Box; 
     * The argument can either be a single object pointer or reference, or a list 
     * of object pointers or references separated by + operators, example:
     * <pre>
     *   Button* b = new Button(UPix::disquette + "Save" + ucall(obj, saveFunc));
     * </pre>
     *
     * @see also the ubox() shortcut, that returns: *new Box(arglist)
     */
    Box(Args args = Args::none);  
    
    /**
     * recursive destructor.
     * children are automatically destroyed if not pointed elsewhere
     * (in the instance graph or by a std::unique_ptr<>). see class Node for details.
     */
    virtual ~Box() {destructs();}
    
    virtual Box* toBox() {return this;}
    virtual const Box* toBox() const {return this;}
    
    static Style* createStyle();
    /**< static function that returns the style of this class (see details!).
     * this function MUST be redefined by subclasses if they use another Style.
     */
    
    virtual int getDisplayType() const {return BLOCK;} // redefined
    
    void repaint() {update(Update::paint);}           // redefined    
    void update() {update(Update::layoutAndPaint);}   // redefined
    virtual void update(const Update& update_options, Display* = null);  // redefined
    virtual void doUpdate(const Update&, Display* = null);  // redefined
    

    virtual int getViewCount() const;
    ///< returns the number of views of this box.
    
    virtual int getViews(std::vector<View*>&) const;
    /**< returns the views of this box (see DETAILS).
     * the vector is emptied then filled with the views that are controlled by this box.
     * The view count is returned
     *
     * REMARK: the views can be obtained in a more efficient way as follows:
     * <pre>
     *   for (View* v = obj->getView(); v!=null; v = v->getNext())
     *      ....;
     * </pre>
     */
    
    virtual View* getView(int n = 0) const;
    /**< returns the Nth view of this box (see DETAILS).
     * returns the first view if 'n' = 0 and the last view if 'n' = -1.
     *
     * REMARK: getView() can also be used to access all views:
     * <pre>
     *   for (View* v = obj->getView(); v!=null; v = v->getNext())
     *      ....;
     * </pre>
     */
    
    virtual View* getView(const InputEvent&) const;
    ///< returns the view of this box that contains the event location.
      
    virtual View* getView(Window&, const Point& pos_in_win, Display* = null) const;
    ///< returns the view of this box that contains this point in this window.

    virtual View* getViewContaining(const View& child_view) const;
    /**< returns the view of this box that contains 'child_view'.
     * return null if 'child_view' is not a (direct or indirect) child of a view
     * of this box ('child_view' can also be one of the views of this box)
     */
    
    virtual View* getFirstViewInside(const View& parent_view) const;
    /**< returns the first box view that is contained in 'parent_view'.
     * return null if 'parent_view' is not a (direct or indirect) parent of a view
     * of this box
     */
    
    virtual int retrieveRelatedViews(std::vector<View*>& collating_vector) const;
    /* returns the views that are related to this box.
     * retrieves all the views that are currently controlled by this Box.
     *
     * - this function does the same as getViews() except that 'collating_vector' 
     *   is not emptied: the views are added to the end of what it already contains.
     * - the function returns the number of views that were added (which may thus differ
     *   from the total size of 'collating_vector')
     *
     * Note that elements and boxes can have several parents, so that multiple wiews
     * can be associated to the same widget (see Element and Box classes for details)
     */
    
    virtual View* getViewInImpl(View* parent_view);
    
    virtual View* getViewInImpl(View* parent_view, Dimension&);
    /* returns the object's view that is contained in this DIRECT parent view.
     * 'parent_view' will contain several views of this object if it has been added
     * several times to this parent.
     */
    
    void setViewModes(int view_modes, bool);
    
  protected:
    friend class Element;
    friend class Window;
    friend class View;
    friend class AppImpl;
    View* views;

    virtual void addViewImpl(View*);
    virtual void initView(View* parent_view);
    virtual void initChildViews(Element*);
    virtual void deleteViewsInside(const std::vector<View*>& parent_views);
  };
  
  inline Box& ubox(const Args& args = Args::none) {return *new Box(args);}
  /**< shortcut that returns: *new Box().
   * Note: watch the case: Box is a class while ubox() is a function!
   */
  
  Box& uvbox(Args args = Args::none);
  ///< shortcut that creates a vertical Box.
  
  Box& uhbox(Args args = Args::none);
  ///< shortcut that creates a horizontal Box.

}
#endif // UBIT_UI_BOX_H_
