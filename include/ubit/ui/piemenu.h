/*
 *  piemenu.hpp : Pie and Control menus
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

#ifndef _upiemenu_hpp_
#define _upiemenu_hpp_ 1

#include <ubit/core/boxgeom.h>
#include <ubit/ui/menu.h>

namespace ubit {
  
/** Pie menu.
  * Pie menus can have up to 8 items that are counted from 0 to 7 in anticlockwise
  * order, starting from the right direction of the x axis.
  * Pie menus can be nested by using the addSubMenu() method. 
  *
  * @see: show() methods and PopupMenu class to see how to open menus
  * @see also: ControlMenu subclass for creating control menus.
  *
  * Event management:
  * each menu item can have callbacks that are fired on these conditions:
  * - UOn::action : this item has been activated (the mouse is released)
  * - UOn::arm : this item has been selected (the mouse enters this item)
  * - UOn::disarm : this item has been deselected (the mouse leaves this item)
  * - UOn::mdrag : the mouse is dragged in this item
  *
  * Item callbacks can have a MouseEvent argument.
  * - getSource() returns the menu (NOT the item!)
  * - getView() returns the view of the menu
  * - getTarget() returns the item that is selected
  * - getX(), getY(), etc. are relative to the top/left origin of the menu
  *   (ie. to the menu view returned by getView())
  * - isFirstDrag() returns true at the beginning of a drag sequence
  *   (only once, just after UOn::arm).
  *
  * Example:
  * <pre>
  *   class Demo {
  *   public:
  *       void cut();
  *       void copy();
  *       void paste();
  *       ....
  *   };
  *
  *   Demo* d = new Demo();
  *   PieMenu* pmenu = new PieMenu(); 
  *
  *   pmenu->item(0).add("Cut" + UOn::action / ucall(d, &Demo::cut))
  *   pmenu->item(1).add("Copy" + UOn::action / ucall(d, &Demo::copy))
  *   pmenu->item(2).add("Paste" + UOn::action / ucall(d, &Demo::paste))
  * </pre>
  * @see also: UCall  
 */
class PieMenu : public PopupMenu {
public:
  UCLASS(PieMenu)

  PieMenu();
  ///< creates a new pie menu; @see also ControlMenu and shortcut function upiemenu().
  
  virtual ~PieMenu();

  static Style* createStyle();

  virtual Box& item(int n);
  /**< gets or creates the Nth item in the menu.
    * the last item is returned if N is too large
    * The item is created implicitely if it does not exist.
    * @see also: getItem(int n);
    */
  
  virtual Box* getItem(int n) const;
  /**< returns the Nth item in the menu.
    * N must be a valid value (@see PieMenu), null is returned otherwise. 
    * The item is NOT created if it does not exist.
    * @see also: item(int n);
    */
   
  virtual Box* getSelectedItem() const;
  ///< returns the item that is currently selected (null if none).

  virtual int getSelectedIndex() const;
  ///< returns the index of item that is currently selected (-1 if none).

  virtual void addSubMenu(int N, PieMenu* submenu);
  ///< adds a submenu to this menu.
  
  void addSubMenu(int N, PieMenu& submenu) {addSubMenu(N, &submenu);}
  ///< adds a submenu to this menu.
  
  virtual void open(MouseEvent&);
  /** opens the menu at the mouse event location (after waiting for getShowDelay()); MUST be used in UCall statements.
   * the delay can be changed by setShowDelay().
   */
    
  virtual void show(bool state, Display*);
  
  virtual void show(bool state = true) {PieMenu::show(state,null);}
  ///< shows/hides the menu immediately (@see: Menu::show(bool state)).
   
  unsigned long getShowDelay() const {return show_delay;}
  ///< returns the delay before the menu is opened (default = 300 ms)

  void setShowDelay(unsigned long d) {show_delay = d;}
  ///< changes the delay before the menu is opened (default = 300 ms)

  const Point& getMouseMovement() const {return mousepos;}
  ///< returns the location of the last DRAG event from the location of the ARM event.
  
  Window* getContainingWin(Display* = null) const;  
  ///< returns the window where the menu is currently opened.
  
  Point getCenterInContainingWin(Display* = null) const;
  ///< returns the location of the menu center from the origin of getContainingWin().
  
  bool isSpringMode() const {return spring_mode;}
  void setSpringMode(bool state = true) {spring_mode = state;}

  // properties 
  
  Color& pieColor()       {return *ppie_color;}
  Color& pieBorderColor() {return *ppie_border_color;}
  Color& sliceColor()     {return *pslice_color;}
  Color& centerColor()    {return *pcenter_color;}
  Color& centerBorderColor() {return *pcenter_border_color;}

  Length& pieRadius()     {return pie_radius;}
  Length& centerRadius()  {return center_radius;}
  Length& itemRadius()    {return item_radius;}
    
  // - - callbacks - - 

  virtual void pressCB(MouseEvent&);
  ///< [impl] called when the mouse is pressed.
 
  virtual void releaseCB(MouseEvent&);
  ///< [impl] called when the mouse is released.
  
  virtual void motionCB(MouseEvent&);
  ///< [impl] called when the mouse is dragged.

  virtual void hideCB(Event&);
  ///< [impl] called when the menu is hidden.

  virtual void paintCB(PaintEvent&);
  ///< [impl] called when the menu is repainted.
    
  virtual void resizeCB(UResizeEvent&);
  ///< [impl] called when the menu is resized.
    
  virtual void armItemCB(MouseEvent& e, PieMenu* submenu);
  ///< [impl] called when an item is armed.

  virtual void disarmItemCB(InputEvent& e, PieMenu* submenu);
  ///< [impl] called when an item is disarmed.

  
  enum MenuType {CONCENTRIC, COMPOUND, MULTISTROKE};
  
  void setMenuType(int);
  MenuType getMenuType(int) {return MenuType(menu_style);}
  
  Scale* getScale();
  float getScaleValue();
    
  virtual Box* createItem();
  ///< called internally to create a new pie item (can be redefined).

  virtual void disarmItem(InputEvent&, bool is_browsing);
  ///< [impl] disarms the item that is currently armed (if any).
 
  virtual float getCentredCoords(MouseEvent&, Point& p, bool& must_forward);
  ///< [impl] tranforms events coords to centred coords.

  virtual short getItemFromPos(const Point&) const;

  virtual void reset(MouseEvent*);
  ///< [impl] called when the menu is reset by show().

  virtual void startNoviceMode();
  ///< [impl] starts the novide mode: called by show().
  
  virtual void forwardToMenu(PieMenu* submenu);
  ///< [impl] sets the submenu that is currently active.
 
   virtual void receiveFromMenu(PieMenu* supermenu);
   ///< [impl] sets the supermenu that is currently active.
 
   virtual void unlinkFromAndToMenus();
   ///< [impl] detatches from the current sub and supermenu.
 
protected:
  unsigned char menu_style;
  unsigned char spring_mode:1, ctlmenu_mode:1, novice_mode:1, in_show_function:1; 
  int curitem;
  unsigned long show_delay;
  Point armpos, mousepos;
  Size pie_size;
  unique_ptr<Timer> ptimer;
  //unique_ptr<Box> parmed; plantage: parmed n'appartient pas au menu!
  Box* parmed;
  PieMenu *from_menu, *to_menu;
  Element gitems;
  Length pie_radius, center_radius, item_radius;
  unique_ptr<Color> ppie_color, pcenter_color, pslice_color, ppie_border_color, pcenter_border_color;
};

inline PieMenu& upiemenu() {return *new PieMenu();}
///< shortcut function that creates a new pie menu.

}
#endif

