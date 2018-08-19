/*
 *  winImpl.cpp: window implementation
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
#include <ubit/ui/uwinImpl.hpp>
#include <ubit/ui/uviewImpl.hpp>
#include <ubit/draw/display.h>
#include <ubit/uglcontext.hpp>
using namespace std;

namespace ubit {

UHardwinImpl::UHardwinImpl(Display* d, Window* w) :
wintype(NONE), must_update(false), 
disp(d), win(w), softwin_list(null), glcontext(null) {
}
  
UHardwinImpl::~UHardwinImpl() {
  if (disp) {
    // enlever cette window de la liste des win managees par le disp
    disp->removeHardwin(win);
    // disp.getNatDisp()->showNotify(getSysWin(), false);    // warn the UMS server
    
    if (disp->default_context == glcontext) disp->default_context = null;
    if (disp->current_glcontext == glcontext) disp->current_glcontext = null;
  }
  delete glcontext;  // if any
}


Children* UHardwinImpl::getSoftwinList() {return softwin_list;}
  
UWinList* UHardwinImpl::getSoftwins() {
  if (softwin_list && softwin_list->begin() != softwin_list->end()) 
    return static_cast<UWinList*>(**softwin_list->begin());
  // other cases
  return null;
}

UWinList* UHardwinImpl::obtainSoftwins() {
  UWinList* softwins = getSoftwins();
  // recuperer ou creer la softlist de la hardwin
  if (!softwins) {
    if (!softwin_list) softwin_list = new Children();
    // UWinList elements are not counted has parents when auto deleting children
    // they are considered as genuine lists, not viewable elements when managing views
    softwins = new UWinList();
    softwin_list->push_back(softwins);
    softwins->setShowable(true);
  }
  return softwins;
}

void UHardwinImpl::addSoftwin(Window* softwin, Window* hardwin, Display* d, bool add_to_the_end) {
  if (!softwin || !hardwin) return;
  
  // s'il ya  une vue courante l'enlever (ca devrait etre fait uniquement si meme disp !!!)
  //View* softwin_view = softwin->getActualSoftwinView();
  USoftwinImpl* softw = softwin->softImpl();
  View* softwin_view = softw ? softw->getActualView(softwin->views) : null;
  
  if (softwin_view) {
    UHardwinImpl* old_hardwin_impl = softwin_view->getHardwin();
    if (old_hardwin_impl) old_hardwin_impl->removeSoftwin(softwin);
  }
  
  softwin_view = softwin->realizeSoftwin(hardwin, hardwin->getWinView(d), 
                                         d, add_to_the_end);
  if (softw) softw->setActualView(softwin_view);
}

// seul addSoftWin() doit appeller cette fonction, de maniere a enlever de la liste cournate
void UHardwinImpl::removeSoftwin(Window* softwin) {
  if (!softwin) return;
  USoftwinImpl* swi = softwin->softImpl();
  if (swi) swi->setActualView(null);
  
  UWinList* softwins = getSoftwins();
  if (softwins) {
    softwins->setAutoUpdate(false);
    softwins->remove(*softwin, false);
    softwins->setAutoUpdate(true);
  }
}


}

