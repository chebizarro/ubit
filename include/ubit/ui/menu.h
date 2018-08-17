/*
 *  menu.h: menu bars, pulldown and popup menus
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

#ifndef _umenu_hpp_
#define	_umenu_hpp_ 1

#include <ubit/uboxes.hpp>
#include <ubit/uwin.hpp>

namespace ubit {
  
  class UMenuManager;
  
  /** Menu bar.
   */
  class UMenubar: public UBar {
  public:
    UCLASS(UMenubar)
    
    UMenubar(const Args& a = Args::none);
    ///< create a new Menu Bar; see also shortcut function umenubar().
    
    static  UStyle* createStyle();
    
    // - - -impl - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
    //virtual Element* getBrowsingGroup() {return this;}  
  protected:
    virtual void menuChildCB(UInputEvent&);
#endif
  };
  
  inline UMenubar& umenubar(const Args& args = Args::none) {return *new UMenubar(args);}
  ///< shortcut function that return *new UMenubar(args).
  
  
    /** UMenu: pulldown and cascaded menux. 
   *
   * A UMenu is automatically opened when the mouse is pressed in one of its parents.
   * Most methods are inherited from Window. Example:
   * <pre>
   *   umenubar( ubutton("Menu 1" + umenu( ubutton("aaa") + ubutton("bbb") ))
   *            + ubutton("Menu 2" + umenu( ulabel("xxx") + ucheckbox("ccc") ))
   *           )
   * </pre>
   *
   * Note that the UPopmenu class must be used to create contextual menus (and that
   * popmenus are not automatically opened to allow more programming control)
   * 
   * @see: Window, UMenubar, UPopmenu.
   */
  class UMenu : public Window {
  public:
    UCLASS(UMenu)
    
    UMenu(const Args& a = Args::none);
    ///< creates a new pulldown (or cascaded) Menu; see also shortcut function umenu().
    
    virtual ~UMenu();
    
    static  UStyle* createStyle();
    virtual UMenu* toMenu() {return this;}
    virtual const UMenu* toMenu() const {return this;}
    
    virtual void show(bool state = true) {UMenu::show(state, null);}
    ///< shows/hides the menu at its current location.
    
    virtual void show(bool state, Display*);
    ///< shows/hides the menu at its current location on the specified display.

    void closeAllMenus(UInputEvent&);
    ///< closes all menus that are currently opened.
      
    virtual void setPlacement(const UWinPlacement&);
    /**< specifies automatic placement rules (see UPlacement).
     * update() must be called to enforce new rules
     */
    
    virtual bool realize();
    
    // - - - Impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
    virtual void addingTo(Child&, Element& parent);
    virtual void removingFrom(Child&, Element& parent);
    ///< NOTE that this function require a specific destructor.
  private:
    friend class Application;
    friend class UMenuManager;
    uptr<UCall> menu_opener_cb;
    class UWinPlacement *placement;
  protected:
    virtual void openImpl(UMenuManager&, View* opener, bool auto_place, Display*);
    virtual void menuOpenerCB(UInputEvent&);
    virtual void menuChildCB(UInputEvent&);
#endif
  };
  
  inline UMenu& umenu(const Args& args = Args::none) {return *new UMenu(args);}
  ///< shortcut function that return *new UMenu(args).
  
  
    /** UPopmenu: Contextual Menu.
   *
   * Unlike UMenu(s) which are automaticcally opened, UPopmenu(s) must be opened 
   * by calling their open() or show() methods:
   * <pre>
   *   // a popmenu containing 3 buttons
   *   UPopmenu& menu = upopmenu(ubutton(...) + ubutton(...) + ubutton(...));
   *
   *   // any widget deriving from Box
   *   Box& box = ...;
   *
   *   // pressing the mouse in the box opens the menu
   *   box.add(UOn::mpress / uopen(menu));
   *
   *   // the previous piece of code if equivalent to:
   *   box.add(UOn::mpress / ucall(menu, &UPopmenu::open));
   * </pre>
   *
   * open() opens the menu at the mouse press location. show() would open it at
   * its prior location (a behavior that is rarely needed).
   * 
   * A callback function must be used when different actions must be performed
   * depending on which mouse button (or keyboard key) was pressed. In the following
   * example, the menu is onlmy opened when the right mouse button is pressed:
   * <pre>
   *   void openMenuCB(UMouseEvent& e, UPopmenu* m) {
   *      if (e.getButton() == e.RightButton) m->open(e);
   *   }
   *
   *   box.add(UOn::mpress / ucall(&menu, openMenuCB));
   * </pre>
   *
   * See also: UMenu, Window.
   */
  class UPopmenu : public UMenu {
  public:
    UCLASS(UPopmenu)
    static UStyle* createStyle();
    
    UPopmenu(const Args& a = Args::none);
    ///< create a new contextual menu; see also shortcut function upopmenu().
    
    virtual void open(UMouseEvent&);
    /** opens the menu at the mouse event location.
     * this method shows the menu at a mouse event location so that it will appear 
     * where the mouse was pressed. Example:
     * <pre>    
     *   Box* canvas = ...;
     *   UPopmenu* popmenu = new UPopmenu(....);
     *   canvas->add(UOn::mpress / ucall(popmenu, &UMenu::open));
     * </pre>
     *
     * Notes:

     * - this function grabs the mouse. Launching the application with the --no-grab
     *   option on the command line disables this behavior (this can be useful for debug)
     */                      
  };
  
  inline UPopmenu& upopmenu(const Args& args = Args::none) {return *new UPopmenu(args);}
  ///< shortcut function that returns *new UPopmenu(args).
  
}
#endif



