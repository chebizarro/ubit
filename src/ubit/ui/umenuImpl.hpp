/*
 *  menuImpl.hpp
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


#ifndef _umenuImpl_hpp_
#define	_umenuImpl_hpp_ 1

#include <vector>

namespace ubit {

/** [Implementation] manages the stack of menus that are currently opened on a given EventFlow.
*/
class MenuManager {
public:
  MenuManager(EventFlow*);
  ~MenuManager();

  Menu* getDeepestMenu() const {return active_menu;}
  ///< returns the deepest menu in the menu stack (if any, null otherwise).

  bool contains(Menu* menu) const;
  // returns true if this menu is in the menu stack.

  bool contains(View*) const;
  // returns true if this view is contained in a menu in the menu stack.

  void closeAllMenus(bool clear_top_menu);
  ///< closes all menus in the menu stack.   
  
  void closeSubMenus(Menu* from_menu, bool including_this_menu);
  ///< closes the submenus of this menu (calls closeAllMenus if menu is not found).

  void openMenu(View* opener, Menu*, Display*);
  void openMenuAfterDelay();
  void closeMenuAfterDelay();

  void menuChildCB(InputEvent&, Box* menu_or_menubar);
 
protected:
  friend class Menu;
  friend class MenuBar;
  friend class EventFlow;
  std::vector<Menu*> menustack;  // stack of menus that are currently being grabbed
  int menu_count;
  Display* disp;
  EventFlow& eflow; 
  View *active_opener, *possible_opener, *possible_closer;
  Menu *active_menu, *possible_opener_menu, *possible_closer_menu;
  unique_ptr<Timer> kk_open_timer, kk_close_timer;  // unique_ptr: dont destroy Timers  $$$
  Box* top_menu;
};

}
#endif

