/*
 *  menu.cpp: menu bars, pulldown and popup menus
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
#include <ubit/ui/menu.h>
#include <ubit/ui/uwinImpl.hpp>
#include <ubit/core/call.h>
#include <ubit/core/string.h>
#include <ubit/core/on.h>
#include <ubit/ui/box.h>
#include <ubit/ui/window.h>
#include <ubit/ui/view.h>
#include <ubit/ui/uviewImpl.hpp>
#include <ubit/core/event.h>
#include <ubit/draw/style.h>
#include <ubit/draw/color.h>
#include <ubit/ui/border.h>
#include <ubit/draw/graph.h>
#include <ubit/core/application.h>
#include <ubit/core/boxgeom.h>
#include <ubit/ui/eventflow.h>
#include <ubit/ui/umenuImpl.hpp>
#include <ubit/core/config.h>
#include <ubit/ui/background.h>
#include <ubit/ui/timer.h>
using namespace std;
namespace ubit {


Style* PopupMenu::createStyle() {
  return Menu::createStyle();
}

PopupMenu::PopupMenu(const Args& a) : Menu(a) {
  wmodes.IS_AUTO_OPENED = false;
}


Style* MenuBar::createStyle() {
  Style* style = new Style();
  style->textSeparator = new String("\t");
  style->orient = Orientation::HORIZONTAL;
  style->halign = HAlign::LEFT;
  style->valign = VAlign::FLEX;
  style->hspacing = 4;
  style->vspacing = 5;
  style->font = &Font::bold;
  style->local.border = &Border::shadowOut;
  return style;
}

MenuBar::MenuBar(const Args& a): UBar(a) {
  emodes.IS_BROWSABLE = true;
  
  static MultiCondition* cond = null;
  if (!cond) {
    cond = new MultiCondition();
    *cond += UOn::enter;
    *cond += UOn::leave;
    *cond += UOn::mrelease;
    *cond += UOn::arm;  // only by menubar
  }
  // recupere evenements dans les enfants
  observeChildrenEvents(*cond / ucall(this, &MenuBar::menuChildCB));
}

void MenuBar::menuChildCB(InputEvent& e) {
  MenuManager& mc = e.getFlow()->getMenuManager();
  mc.menuChildCB(e, this);
}


Style* Menu::createStyle() {
  Style& s = *new Style();
  s.textSeparator = &ustr("\n");
  s.orient = Orientation::VERTICAL;
  s.halign = HAlign::FLEX;
  s.valign = VAlign::TOP;
  s.hspacing = 1;
  s.vspacing = 1;
  s.setPadding(0, 0);
  s.local.border = &Border::shadowOut;
  s.local.background = &Background::velin;
  return &s;
}


Menu::Menu(const Args& a) : 
Window(a),
menu_opener_cb(null),
placement(null)
{
  wmodes.IS_MENU = true;
  emodes.IS_BROWSABLE = true;
  if (Application::conf.soft_menus) wmodes.IS_HARDWIN = false;
  
  static MultiCondition* cond = null;
  if (!cond) {
    cond = new MultiCondition();
    *cond += UOn::enter;
    *cond += UOn::leave;
    *cond += UOn::mrelease;
  }
  // recupere evenements dans les enfants
  observeChildrenEvents(*cond / ucall(this, &Menu::menuChildCB));
}

// necessaire a cause de removingFrom (car les fct ne sont pas virtuelles
// dans les desctructeurs!)
Menu::~Menu() {
  // att: reset du MenuCtrl si on detruit le menu
  EventFlow* fl = Application::getFlow(0);   // DEFAULT IFLOW : A REVOIR
  if (fl) fl->getMenuManager().closeAllMenus(true);
  if (placement) delete(placement); placement = null;
  destructs();
}

bool Menu::realize() {
  if (wmodes.IS_HARDWIN) return realizeHardwin(UWinImpl::MENU);
  else {
    Application::internalError("Menu::realize",
                          "can't realize the soft window of this Menu (%p)",this);
    return false;
  }
}

void Menu::closeAllMenus(InputEvent& e) {
  e.getFlow()->getMenuManager().closeAllMenus(true);
}

void Menu::setPlacement(const WindowPlacement& pl) {
  if (placement) delete placement;
  placement = new WindowPlacement(pl);
}


void Menu::addingTo(Child& c, Element& parent) {
  Window::addingTo(c, parent);
  
  static MultiCondition* cond = null;
  if (!cond) {
    cond = new MultiCondition();
    *cond += UOn::enter;
    *cond += UOn::leave;
    *cond += UOn::arm;
    *cond += UOn::disarm;
  }
  if (wmodes.IS_AUTO_OPENED) {
    if (!menu_opener_cb) menu_opener_cb = &ucall(this, &Menu::menuOpenerCB);
    parent.addAttr(*cond / *menu_opener_cb);
  }
}

//NB: removingFrom() requires a destructor to be defined
void Menu::removingFrom(Child& c, Element& parent) {
  // don't delete the ucalls as they are shared
  if (menu_opener_cb) parent.removeAttr(*menu_opener_cb);
  
  Window::removingFrom(c, parent);
}

// Cette fonction est appelee par les boutons qui ouvrent un submenu.

void Menu::menuOpenerCB(InputEvent& e) {
  MenuManager& mc = e.getFlow()->getMenuManager();
  View* opener_view = e.getView();
  
  if (e.getCond() == UOn::arm) {
    //cerr<< endl << "Menu::armOpener"<< endl;
    openImpl(mc, opener_view, true, null);
    
    // noter que top_menu est esnuite ecrase par menuChildCB() si le bouton 
    // est dans un menubar
    if (mc.top_menu == null) mc.top_menu = this;
  }
  
  else if (e.getCond() == UOn::disarm) {
    //cerr<< endl<< "Menu::disarmOpener"<< endl;
    mc.active_menu = this;
    mc.active_opener = e.getView(); //verif
  }
  
  // !!! normalement ca devrait sur TOUS les boutosn conetnus dans un menu,
  // !!! pas seulement ceux qui ouvrent des sous menus   
  else if (e.getCond() == UOn::enter) {
    //cerr << "menuOpenerCB: enter1" << mc.top_menu<< endl;
    if (mc.top_menu != null) {
      openImpl(mc, opener_view, true, null);
    }
  }
  
  else if (e.getCond() == UOn::leave) {
    //cerr << "menuOpenerCB: leave1" << mc.top_menu<< endl;
    if (mc.top_menu != null) {
      mc.closeSubMenus(this, false);
    }
  }
}

void Menu::menuChildCB(InputEvent& e) {
  MenuManager& mc = e.getFlow()->getMenuManager();
  mc.menuChildCB(e, this);
}

void MenuManager::menuChildCB(InputEvent& e, Box* menu_or_menubar) {
  if (e.getCond() == UOn::mrelease) {
    if (active_opener != e.getView() && !e.dontCloseMenu()) {
      //cerr << "menuChildCB: release" << endl;
      closeAllMenus(true);
    }
  }
  else if (e.getCond() == UOn::arm) {
    //cerr << "menuChildCB: arm" << endl;
    // forces value set by setTopMenu() when a menubar button is armed
    // (to set the menubar instead of the open menu)
    top_menu = menu_or_menubar;
  }
  else if (e.getCond() == UOn::enter) {
    // !!!!@@@@ BUG: enter and leave events pas observes !!!!!!@@@
    cerr << "menuChildCB: enter" << endl;
  }
  else if (e.getCond() == UOn::leave) {
    cerr << "menuChildCB: leave" << endl;
  }
}  

void MenuManager::openMenuAfterDelay() {
  if (possible_opener && possible_opener_menu) {
    possible_opener_menu->openImpl(*this, possible_opener,
                                   true, //autoplace
                                   null);
    possible_opener = null;
    possible_opener_menu = null;
  }
}

void MenuManager::closeMenuAfterDelay() {
  if (possible_closer && possible_closer_menu) {
    closeSubMenus(possible_closer_menu, false);
    possible_closer = null;
    possible_closer_menu = null;
  }
}


void PopupMenu::open(MouseEvent& e) {
  // the menu will behave as a "spring menu" if x>=0 and y>=0 because it will
  // catch the mouse release event. x or y should be set to a negative value
  // to avoid this behavior.
  setPos(e, 2, -2);
  Menu::show(true, e.getDisp());
}

void Menu::show(bool state, Display* disp) {
  if (Application::isExiting()) return;
  
  if (wmodes.IS_HARDWIN) 
  /*nop*/;     // hardwins have only one shared window
  else if (disp == null) {
    // verifier si actual_view toujours dans la liste
    USoftwinImpl* softw = softImpl();
    View* v = softw ? softw->getActualView(views) : null;
    if (v) disp = v->getDisp();
  }
  
  if (state) {
    if (!disp) disp = Application::getDisp();
    EventFlow* ef = const_cast<Display*>(disp)->obtainChannelFlow(0);
    if (ef) openImpl(ef->getMenuManager(), null, /*null,*/ false, disp);
    else Application::internalError("Menu::show","null event flow; menu: %p",this);
  }
  
  else {  // !state
    EventFlow* f = null;
    if (disp) f = disp->getChannelFlow(0);  // DEFAULT IFLOW : A REVOIR
    else f = Application::getFlow(0);
    
    // fermer les sous-menus de CE menu
    if (f) f->getMenuManager().closeSubMenus(this, true/*including*/);    
    Window::show(false, disp);
  }
}


// a generaliser avec Element::closeWinCB
static Menu* getMenuParent(View* opener) {
  for (View *v = opener; v != null; v = v->getParentView()) {
    Box* box = v->getBox();
    if (box) {
      if (dynamic_cast<MenuBar*>(box)) return null;
      Menu* menu = dynamic_cast<Menu*>(box);
      if (menu) return menu;
    }
  }
  return null;
}

void Menu::openImpl(MenuManager& mc, View* opener, 
                     bool auto_place, Display* disp) {
  View* winview = getWinView(opener ? opener->getDisp() : null);
  if (!winview)  {
    Application::error("Menu::openImpl","the window of this Menu (%p) is not realized; check if this menu has a valid parent",this);
    return;
  };
  
  Menu* parent_menu = null;
  bool is_cascaded = false;   // sert a positionner le menu en dessous ou a droite
  
  // s'il y a deja un menu ouvert et qu'un parent (indirect) de l'opener
  // est aussi un menu alors this est un menu cascade
  // -> dans ce cas il ne faut fermer que les sous-menus deja eventuellement
  //    ouverts (et sinon il faut fermer tous les menus)
  if (//!mc.empty()
      mc.top_menu != null
      && opener && ((parent_menu = getMenuParent(opener)))
      ) {
    is_cascaded = true;
    // si ce menu est deja affiche fermer ses menus cascades
    // sinon fermer ceux du menu parent
    if (mc.contains(this)) mc.closeSubMenus(this, false);
    else mc.closeSubMenus(parent_menu, false);
  }
  
  else {
    is_cascaded = false;
    // 1aug06: si show(true) est appele, il n'y a pas d'opener, et pourtant il
    // n'y a pas de raison de fermer les menus parents
    mc.closeSubMenus(this, false);
  }
  
  // cas d'erreur: le menu ne peut etre realize
  if (wmodes.IS_HARDWIN) {
    if (!this->realize()) return;
  }
  
  // affecter le browsingGroup de Application:
  // -- c'est celui herite du graphe d'instanciation
  //    (en particulier le cas des menubars ou des menus cascades)
  // -- sauf s'il est nul auquel cas le browsingGroup sera l'opener
  //    qui a provoque l'ouverture du menu (cad: opener_view->getBox())
  // -- et si l'opener est nul ce sera le menu lui-meme
  
  //menuBrowsingGroup = menugroup;
  //if (!menuBrowsingGroup) menuBrowsingGroup = this; // le menu lui-meme
  
  //positionner le menu
  if (opener) {
    if (placement) setPos(*opener, *placement);
    else if (auto_place) {	 // default rules
      WindowPlacement pl;
      if (is_cascaded) {
        pl.halign = &HAlign::right;
        pl.hoppositeBorder = true;
        pl.hdist = 1;
      }  
      else {
        pl.valign = &VAlign::bottom;
        pl.voppositeBorder = true;
        pl.vdist = 1;
      }
      setPos(*opener, pl);
    }
  }
  
  mc.openMenu(opener, this, /*menuBrowsingGroup,*/ disp);
  
  // le grab physique fait en sorte que le menu se ferme automatiquement
  // si on clique la souris (ou si ou tape une touche du clavier)
  // qunad la souris est en dehors de l'application
  // (ie. si le grab physique etait supprime les menus resteraient ouverts
  //  en permanence qunad la souris est dans une autre appli, ce qui n'est
  //  pas completement delirant mais n'est pas conforme a l'usage)
  
  // NB: ne pas faire de grab si plusieurs display: comme il est toujours
  // fait sur les 1er UDips il bloquerait les autres
  
  if (wmodes.IS_HARDWIN && Application::getDispList().size() == 1
      && Application::conf.menu_grab) {
    
    // EX-BUG: ne rien faire quand disp est null sinon ca bloque
    //if (disp) disp->grabPointer();+
    // mod:5fev6: il faut grabber sinon les menus ne marchent plus correctement
    // le pbm venait d'ailleurs: ungrab n'etait pas correctement appele
    
    if (!disp) disp = Application::getDisp();
    const_cast<Display*>(disp)->grabPointer(); 
  }
}


MenuManager::~MenuManager() {}

MenuManager::MenuManager(EventFlow* _eflow) : eflow(*_eflow) {
  disp = null;
  // grabbed menus
  menu_count = 0;
  active_menu = null;
  active_opener = null;
  possible_opener = null;
  possible_opener_menu = null;
  possible_closer = null;
  possible_closer_menu = null;
  //open_timer = new Timer();
  //open_timer->onAction(ucall(this, &MenuManager::openMenuAfterDelay));
  //close_timer = new Timer();
  //close_timer->onAction(ucall(this, &MenuManager::closeMenuAfterDelay));
  top_menu = null;
}

//bool MenuManager::empty() const {return (menu_count == 0);}

bool MenuManager::contains(Menu* menu) const {
  if (!menu) return false;
  for (int k = 0; k < menu_count; k++) {
    if (menustack[k] == menu) return true;
  }
  return false;   // not found
}

bool MenuManager::contains(View* v) const {
  while (v && !v->getBox()->toMenu()) v = v->getParentView();
  if (!v) return false;
  return contains(v->getBox()->toMenu());
}

void MenuManager::openMenu(View* opener, Menu* menu, Display*_disp) {
  active_menu = menu;
  active_opener = opener;
  disp = _disp;
  
  // ajouter ce menu a la liste des grabbedMenus sauf s'il y est deja
  if (!contains(menu)) {
    if (menu_count < int(menustack.size())) menustack[menu_count] = menu;
    else menustack.push_back(menu);
    menu_count++;
  }
  menu->Window::show(true, disp);
}

void MenuManager::closeAllMenus(bool clear_top_menu) {
  if (Application::isExiting()) return;
  
  for (int k = 0; k < menu_count; k++) {
    // !ATT: uses Window::show(), not the specific Menu::show() method.
    menustack[k]->Window::show(false, null);
  }
  menu_count = 0;
  
  active_menu = null;
  active_opener = null;
  if (disp) const_cast<Display*>(disp)->ungrabPointer(); 
  else Application::getDisp()->ungrabPointer(); 
  
  if (clear_top_menu) top_menu = null;
}

// closes menus that are after 'menu' in the menustack (ie. does not close 'menu'
// nor the menus that opend 'menu'
//
void MenuManager::closeSubMenus(Menu* menu, bool including_this_menu) {
  int k;  // must be unsigned because k becomes<0 in: for (..; k >=0 ; k--) {
  int found = -1;
  
  for (k = 0; k < menu_count; k++) {
    if (menustack[k] == menu) {
      found = including_this_menu ? k-1 : k;
      break;
    }
  }
  
  // cas ou menu n'est pas encore dans la pile (1jt06)
  // fermer ceux qui sont pas ses parents
  if (found < 0 && menu && menu_count > 0) {
    for (k = menu_count-1; k >=0 ; k--) {
      if (menu->isChildOf(*menustack[k])) {
        found = k;
        break;
      }
    }
  }
  
  if (found < 0) {  // not found : depile tout et ungrab
    closeAllMenus(false);
    return;
  }
  
  // if !included skip this menu
  k++; found++;
  
  for ( ; k < menu_count; k++) {
    // !ATT: uses Window::show(), not the specific Menu::show() method.
    menustack[k]->Window::show(false, null);
  }
  menu_count = found;
}

}
