/*
 *  natwinGDK.cpp: GDK Windows
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
#if UBIT_WITH_GDK

#include <iostream>
#include <cstdio>
#include <ubit/core/application.h>
#include <ubit/core/call.h>
#include <ubit/core/on.h>
#include <ubit/core/string.h>
#include <ubit/ui/cursor.h>
#include <ubit/ui/window.h>
#include <ubit/umsproto.hpp>
#include <ubit/nat/unatwin.hpp>
#include <ubit/nat/udispGDK.hpp>
//#include <ubit/nat/udefsGDK.hpp>    // CreatePixmap + DestroyPixmap
using namespace std;
namespace ubit {

void UNatWin::swapBuffers() {
  cerr << "UNatWin::swapBuffers: not implemented" << endl; //!!!! @@@@ to be completed
}

//NB: move fait par programme (PAS par l'utilisateur: cf Display::on_configure())
void UNatWin::setPos(int x, int y) {
  if (sys_win != 0) gdk_window_move(sys_win, x, y);
}

//NB: resize fait par programme (PAS par l'utilisateur: cf Display::on_configure())
void UHardWinGDK::setSize(const Dimension& size) {
  if (sys_win != 0) {
    gdk_window_resize(sys_win, int(size.width), int(size.height));
    //if (Application::isUsingGL()) Application::getDisp()->setGLViewportOrtho(size); fait dans Graph
  }
}

void UNatWin::getPos(int& _x, int& _y) {
  if (sys_win == 0) {_x = _y = 0;}
  else gdk_window_get_origin(sys_win, &_x, &_y);
}

void UNatWin::getSize(int& _w, int& _h) {
  if (sys_win == 0) {_w = _h = 0;}
  else {
    gint x, y, w, h, depth;
    gdk_window_get_geometry(sys_win, &x, &y, &w, &h, &depth);
    _w = int(w); _h = int(h);    
  }
}

void UNatWin::show(bool state) {
  if (sys_win != 0) {
    if (state) gdk_window_show(sys_win); else gdk_window_hide(sys_win);
  }
}

void UNatWin::toFront() {
  if (sys_win != 0) gdk_window_raise(sys_win); 
}

void UNatWin::toBack() {
  if (sys_win != 0) gdk_window_lower(sys_win); 
}

void UNatWin::setTitle(const String& s) {
  if (sys_win != null && s.c_str()) gdk_window_set_title(sys_win, s.c_str());  // UTF-8 !!!
}

void UNatWin::getTitle(String& s) const {
  if (sys_win != null) {                                          // !!!!!! A_COMPLETER !!!
    s = "";  // existe ??????????????
  }
}

void UNatWin::setCursor(const Cursor* curs) {
  Cursor xc = (!curs) ? None : curs->getCursorImpl(disp)->cursor;
  XDefineCursor(SYS_DISP, sys_win, xc);
}

void UNatWin::setNames(const String& res_class, const String& res_name, 
                       const String& win_name, const String& icon_name) { 
  // A IMPLEMENTER !!!
}


void UHardWinGDK::realize(WinType wtype, float w, float h) {
  if (isRealized()) return;
  wintype = wtype;
  
  if (wtype == NONE || wtype == SOFTWIN || wtype == EXTWIN) {
    Application::error("UHardWinGDK","wrong type for this function: %d", wtype);
    return;
  }
  
  UDispGDK* nd = (UDispGDK*)_nd;
  GdkWindowAttr attr;
  gint attr_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_CURSOR;
  
  //attr.title = ?;
  attr.event_mask = 0;
  attr.x = attr.y = 0;
  attr.width = w;
  attr.height = h;
  attr.wclass = GDK_INPUT_OUTPUT;  // normal window
  attr.visual = nd->getSysVisual();
  attr.window_type = GDK_WINDOW_TOPLEVEL;
  attr.cursor = nd->getCursor(Cursor::pointer);
  
  if (nd->getSysColormap() != null) {
    attr.colormap = nd->getSysColormap();
    attr_mask |= GDK_WA_COLORMAP;
  }
  
  if (_type == UNatWin::MENU) {
    attr.window_type = GDK_WINDOW_TEMP;
    //window pas geree par le WM (no borders...) car c'est un menu
    attr.override_redirect = TRUE;
    attr_mask |= GDK_WA_NOREDIR;
  }
  else if (_type == UNatWin::SUBWIN) {
    attr.window_type = GDK_WINDOW_CHILD;
    // il faut rendre cette window sensible aux ExposeEvent car sa
    // parent window ne les recevra pas
    attr.event_mask = GDK_EXPOSURE_MASK;
    //attr_mask |= CWEventMask;
  }
  
  // !!! Note: to use this on displays other than the default
  // display, parent must be specified.
  GdkWindow* parent_win = (_type == UNatWin::SUBWIN) ?
    nd->mainframe->sys_win : null; 
  
  GdkWindow* sys_win = gdk_window_new(parent_win, &attr, attr_mask);  
  if (sys_win == null) return null;  // error
  
  if (_type == UNatWin::SUBWIN) {
    gdk_window_show(sys_win);  // rendre visible
  }
  else {
    gdk_window_set_events(sys_win, (GdkEventMask)
                          (GDK_EXPOSURE_MASK
                           | GDK_POINTER_MOTION_MASK
                           // GDK_POINTER_MOTION_HINT_MASK
                           // GDK_BUTTON_MOTION_MASK
                           // GDK_BUTTON1_MOTION_MASK
                           // GDK_BUTTON2_MOTION_MASK
                           // GDK_BUTTON3_MOTION_MASK
                           | GDK_BUTTON_PRESS_MASK
                           | GDK_BUTTON_RELEASE_MASK
                           | GDK_KEY_PRESS_MASK  
                           | GDK_KEY_RELEASE_MASK
                           | GDK_ENTER_NOTIFY_MASK
                           | GDK_LEAVE_NOTIFY_MASK
                           | GDK_FOCUS_CHANGE_MASK
                           | GDK_STRUCTURE_MASK   
                           | GDK_PROPERTY_CHANGE_MASK
                           | GDK_VISIBILITY_NOTIFY_MASK
                           | GDK_PROXIMITY_IN_MASK 
                           | GDK_PROXIMITY_OUT_MASK
                           // GDK_SUBSTRUCTURE_MASK
                           | GDK_SCROLL_MASK)
                          );
    
    // Close/Delete Window Protocol
    // inutile: cf event types GDK_DELETE, GDK_DESTROY
    
    // non-native pointer events are sent by the UMS on UBIT_WINDOW(s)
    // according to the UBIT_EVENT_FLOW Protocol
    // NB: inutile de copier le 0 final de la string: getProperty le rajoutera
    //unsigned char winid[2] = {_type, 0};
    //XChangeProperty(nd->sys_disp, sys_win, nd->atoms._UMS_WINDOW,
    //                XA_STRING, 8/*format*/, PropModeReplace, winid, 2);
  }
  
  // l'iconification du MainFrame entraine l'iconification de ce Dialog
  if (_type == UNatWin::DIALOG) {
    gdk_window_set_transient_for(sys_win, nd->mainframe->sys_win);
  }
  
  return sys_win;
}

void UNatWin::destroyWin() {
  if (wintype == PIXMAP) g_object_unref(sys_win);
  else gdk_window_destroy(sys_win);
}


}
#endif
