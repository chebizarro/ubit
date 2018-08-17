/*
 *  choice.h: item chooser in a list
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

#ifndef _uchoice_hpp_
#define	_uchoice_hpp_ 1

#include <ubit/uattr.hpp>

namespace ubit {
  
  /** Makes the children of a widget (exclusively) selectable.
   *
   * This object makes the CHILDREN of a widget selectable. It must be added to
   * the chilsd or attribute list of this widget. Selection is exclusive. 
   * No child is initially selected. 
   * ListBox, Treebox, ComboBox use an internal Choice
   * @see also: URadioSelect.
   */
  class Choice : public Attribute {
  public:
    UCLASS(Choice)
    
    Choice();  
    ///< create a new Choice object; @see also shortcut uchoice().
    
    virtual ~Choice();
    
    virtual void clearSelection();
    ///< deselects the selected object.
    
    virtual Box* getSelectedItem() const;
    ///< returns the current selection; returns null if there is no selection.
    
    virtual Box* setSelectedItem(Box&);
    ///< selects and returns this item if it is in the list, returns null otherwise.
    
    virtual Box* setSelectedItem(Box*);
    ///< selects this item or clear the selection if this item is null.

    virtual int getSelectedIndex() const;
    /**< returns the index of the selection; returns -1 if there is no selection.
     * NOTE: the returned value is the index among the *selectable* objects of
     * the list. A list can contain non-selectable objects, which are ignored 
     * when counting indexes.
     */
    
    virtual Box* setSelectedIndex(int N);
    /**< selects the Nth *selectable* object in the list.
     * the last selectable object is selected if N = -1. The selected object is
     * returned (if any, null is returned otherwise).
     * NOTE: N is the index of the Nth *selectable* object. A list can contain 
     * non-selectable objects, which are ignored when counting indexes.
     */
    
    virtual Box* getSelectableItem(int N) const;
    /**< returns the Nth *selectable* object.
     * the last selectable object is returned if N = -1.
     * NOTE: A list can contain non-selectable objects, which are ignored when
     * counting indexes. Use getChild() to have access to all objects.
     */
    
    virtual Box* getSelectableItem(const String& s, bool ignore_case = true) const;
    /**< returns the 1st *selectable* object which contains a string equal to 's'.
     * this function compares the content of the strings (not their addresses).
     * NOTE: A list can contain non-selectable objects, which are ignored by
     * this function. Use getChild() to have access to all objects.
     */
    
    struct IsSelectable {
      virtual bool operator()(const Box*) const;
      virtual ~IsSelectable() {}
    };
    
    virtual void setSelectionRule(IsSelectable&);
    /**< define which objects can be selected in the list.
     * by default, ARMable objects that derive from Box can be selected, such as
     * Item, Button ... or any box that has been made armable by adding its 
     * setArmable() method.
     */
    
    virtual void update();
    
    // - - - impl - -  - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
    virtual void putProp(UpdateContext*, Element&);
    virtual void addingTo(Child&, Element& parent);
    virtual void removingFrom(Child&, Element& parent);
    // NB: removingFrom() requires a destructor to be defined
    
    /*
    enum {
      NO_SELECTION, SINGLE_SELECTION, 
      SINGLE_INTERVAL_SELECTION, MULTIPLE_INTERVAL_SELECTION
    };
    
    virtual int getSelectionMode() const {return sel_mode;}
    virtual void setSelectionMode(int m) {sel_mode = v;}
     **< gets/sets the current selection mode.
     * no effect with the current version: always in SINGLE_SELECTION mode.
    
    enum {SELECT_ON_DISARM = 1<<0, SELECT_ON_ARM = 1<<1,  SELECT_ON_DRAG = 1<<2};
    
    virtual int getSelectionStyle() const {return sel_style;}
    
     virtual void setSelectionStyle(int);{sel_style = v;}
     **< gets/sets the current selection style.
     * the selection style is an ORed combination of 
     * SELECT_ON_DISARM, SELECT_ON_ARM, SELECT_ON_DRAG
     */
  protected:
    unique_ptr<UCall>  callbacks;
    unique_ptr<Box>   container;
    unique_ptr<Element> last_browsing_group;
    unique_ptr<Element> sel_items;
    IsSelectable*is_selectable;
    //short sel_mode, sel_style;
    virtual void mouseCB(InputEvent&);
    virtual void actionCB(InputEvent*);
    virtual void changeCB(InputEvent*);
    virtual void changed(bool update = true);
    virtual Box* setSelectedItemImpl(Box* item, InputEvent*);
    virtual Box* getSelectedItemImpl() const;
#endif
  };
  
  inline Choice& uchoice() {return *new Choice;}
  ///< shortcut function that returns *new Choice().
  
  /* ==================================================== [Elc] ======= */
  /** Makes widgets (exclusively) selectable.
   *
   * This object makes widgets (such as: Checkbox, RadioButton, Button) selectable.
   * The same URadioSelect must be added to the child or attribute list of all widgets 
   * that must become exclusively selectable. No widget is initially selected.
   *  <pre>
   *    URadioSelect& sel = *new URadioSelect();
   *    ubox( ucheckbox(sel+"One") + ucheckbox(sel+"Two") + ubutton(sel+"Three") ) 
   *    sel.setSelectedItem(0);
   *  </pre>
   * @see also: Choice.
   */
  class URadioSelect : public Attribute {
  public:
    UCLASS(URadioSelect)
    
    URadioSelect();
    ///< create a new URadioSelect object; @see also shortcut uradioselect().
    
    URadioSelect(Int& index);
    ///< create a new URadioSelect object; @see also shortcut uradioselect().
    
    virtual ~URadioSelect();
    
    virtual void setCanUnselectMode(bool);
    virtual bool isCanUnselectMode() const;
    ///< clicking on a selected item will unselect it if true (default is false).
    
    virtual void clearSelection();
    ///< deselects the selected object (if any).
    
    virtual Element* getSelectedItem() const;
    ///< returns the selected object; returns null if there is no selected object.
    
    virtual void setSelectedItem(Element&);
    ///< selects this object.
    
    Int& index() const {return *pindex;}
    virtual int getSelectedIndex() const;
    ///< returns the index of the selected object; returns -1 if there is no selected object.
    
    virtual void setSelectedIndex(int n);
    virtual void setSelectedIndex(const Int& n);
    ///< selects the Nth  object; selects the last selectable object if n = -1.
    
    virtual void update();
    
    // - - impl  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
    virtual void changed(bool update = true);
    virtual void changed(Element* target);
    /**< called when the selection is changed.
     * This function:
     * - updates grahics (if arg is true)
     * - first fires object's UOn::change callbacks
     * - then fires parents' UOn::select callbacks
     */
    
    virtual void putProp(UpdateContext*, Element&);
    virtual void addingTo(Child&, Element& parent);
    virtual void removingFrom(Child&, Element& parent);
    // NB: removingFrom() requires a destructor to be defined
    
  protected:
    bool can_unselect_mode;
    unique_ptr<Int> pindex;
    unique_ptr<UCall> pselect_callback;
    void itemChanged(InputEvent&);
    void setIndexImpl();
    void _selectItem(Element*);
#endif
  };
  
  inline URadioSelect& uradioSelect() {return *new URadioSelect();}
  ///< shortcut function that return *new URadioSelect();
  
}
#endif
