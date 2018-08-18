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
#include <ubit/ui/view.h>
#include <ubit/draw/style.h>
#include <ubit/core/application.h>
#include <ubit/core/appimpl.h>
#include <ubit/uon.hpp>
#include <ubit/draw/color.h>

#include <ubit/udispX11.hpp>  // !!!@@@

using namespace std;
namespace ubit {



Style* SubWindow::createStyle() {
  Style* s = Window::createStyle();
  // necessaire, surtout pour GLCanvas
  s->setBgcolors(Color::none);
  return s;
}

SubWindow::SubWindow(Args a) : Window(a) {
  // Contrairement aux Window, sont affichees par defaut et sont en mode BOX (= tiled)
  emodes.IS_SHOWABLE = true;
  wmodes.IS_HARDWIN = true;
  //emodes.IS_SUBWIN = true; (remplacé par virtual method isSubWin())
}

// must be redefined to init parent view
// void SubWindow::initView(Child *selflink, View *parview) {
  void SubWindow::initView(View* parview) {
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

bool SubWindow::realize() {
  if (! wmodes.IS_HARDWIN) {
    Application::error("SubWindow::realize","can't be a SOFTWIN: can't realize object %p",this);
    return false;
  }
  
  UHardwinImpl* hw = hardImpl();
  if (!hw) return false;
  if (hw->isRealized()) return true;  // deja realize'
  if (!realizeHardwin(UHardwinImpl::SUBWIN)) return false;
  
  //cerr << "SUBWIN win " <<((UHardwinImplX11*)hw)->getSysWin() <<endl;
  addAttr(UOn::motion / ucall(this, &SubWindow::motionImpl));
  addAttr(UOn::resize / ucall(this, &SubWindow::resizeImpl));
    
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

void SubWindow::resizeImpl(UResizeEvent& e) {
  View* v = e.getView();
  UHardwinImpl* hw = hardImpl();
  if (hw) {
    // setSize first because pos depends on size for GLUT
    hw->setSize(v->getSize());
    hw->getPos();
  }
}
  
void SubWindow::motionImpl(UViewEvent& e) {
  View* v = e.getView();
  UHardwinImpl* hw = hardImpl();
  if (hw) {
    hw->setPos(v->getHardwinPos());
    //Point p = hw->getPos(p);
    //cerr << "motionCB pos: " << v->getWinPos() << " p= " << p<<  endl; 
  }
}

}
