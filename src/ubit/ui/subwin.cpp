/*
 *  subwin.cpp: window inside a window
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
#include <algorithm>
#include <ubit/ucall.hpp>
#include <ubit/usubwin.hpp>
#include <ubit/ui/uwinImpl.hpp>
#include <ubit/uview.hpp>
#include <ubit/ustyle.hpp>
#include <ubit/uappli.hpp>
#include <ubit/core/uappliImpl.hpp>
#include <ubit/uon.hpp>
#include <ubit/ucolor.hpp>

#include <ubit/udispX11.hpp>  // !!!@@@

using namespace std;
namespace ubit {



UStyle* USubwin::createStyle() {
  UStyle* s = Window::createStyle();
  // necessaire, surtout pour UGlcanvas
  s->setBgcolors(Color::none);
  return s;
}

USubwin::USubwin(Args a) : Window(a) {
  // Contrairement aux Window, sont affichees par defaut et sont en mode BOX (= tiled)
  emodes.IS_SHOWABLE = true;
  wmodes.IS_HARDWIN = true;
  //emodes.IS_SUBWIN = true; (remplacé par virtual method isSubWin())
}

// must be redefined to init parent view
// void USubwin::initView(Child *selflink, View *parview) {
  void USubwin::initView(View* parview) {
  Window::initViewImpl(/*selflink,*/ parview, parview->getDisp());
  
  // as the subwin is laid out as a normal Box, it must have
  // a parent view (in constrast with other Wins that do not have one)
  // (see getViewInside())
  View* winview = getWinView(parview->getDisp());
  
  // le parent de la vue doit etre mis a jour (pour les softwins only)
  // MAIS PAS LE CHAMP win !!
  if (winview) winview->parview = parview;
  if (isShowable()) show(true);
}

bool USubwin::realize() {
  if (! wmodes.IS_HARDWIN) {
    Application::error("USubwin::realize","can't be a SOFTWIN: can't realize object %p",this);
    return false;
  }
  
  UHardwinImpl* hw = hardImpl();
  if (!hw) return false;
  if (hw->isRealized()) return true;  // deja realize'
  if (!realizeHardwin(UHardwinImpl::SUBWIN)) return false;
  
  //cerr << "SUBWIN win " <<((UHardwinImplX11*)hw)->getSysWin() <<endl;
  addAttr(UOn::motion / ucall(this, &USubwin::motionImpl));
  addAttr(UOn::resize / ucall(this, &USubwin::resizeImpl));
    
#if UBIT_WITH_X11
  UHardwinX11* _hw = (UHardwinX11*)hw;
  // this makes it possible for the GL window to receive events sent by the UMS
  unsigned char winid[2] = {UHardwinImpl::SUBWIN, 0};
  Atom UMS_WINDOW = ((UDispX11*)_hw->getDisp())->getAtoms().UMS_WINDOW;
  XChangeProperty(_hw->getSysDisp(), _hw->getSysWin(), UMS_WINDOW,
                  XA_STRING, 8, PropModeReplace, winid, 2);
#endif
  return true;
}

void USubwin::resizeImpl(UResizeEvent& e) {
  View* v = e.getView();
  UHardwinImpl* hw = hardImpl();
  if (hw) {
    // setSize first because pos depends on size for GLUT
    hw->setSize(v->getSize());
    hw->getPos();
  }
}
  
void USubwin::motionImpl(UViewEvent& e) {
  View* v = e.getView();
  UHardwinImpl* hw = hardImpl();
  if (hw) {
    hw->setPos(v->getHardwinPos());
    //Point p = hw->getPos(p);
    //cerr << "motionCB pos: " << v->getWinPos() << " p= " << p<<  endl; 
  }
}

}
