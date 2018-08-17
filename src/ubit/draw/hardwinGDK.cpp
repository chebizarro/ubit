/************************************************************************
 *
 *  unatwinGDK.cpp: GDK Windows
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2008 | Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
 *
 * ***********************************************************************
 * COPYRIGHT NOTICE :
 * THIS PROGRAM IS DISTRIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE
 * IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION;
 * EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 * SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
 * ***********************************************************************/

#include <ubit/ubit_features.h>
#if UBIT_WITH_GDK

#include <iostream>
#include <cstdio>
#include <ubit/uappli.hpp>
#include <ubit/ucall.hpp>
#include <ubit/uon.hpp>
#include <ubit/ustr.hpp>
#include <ubit/ucursor.hpp>
#include <ubit/uwin.hpp>
#include <ubit/umsproto.hpp>
#include <ubit/nat/unatwin.hpp>
#include <ubit/nat/udispGDK.hpp>
//#include <ubit/nat/udefsGDK.hpp>    // CreatePixmap + DestroyPixmap
using namespace std;
namespace ubit {

void UNatWin::swapBuffers() {
  cerr << "UNatWin::swapBuffers: not implemented" << endl; //!!!! @@@@ to be completed
}

//NB: move fait par programme (PAS par l'utilisateur: cf UDisp::on_configure())
void UNatWin::setPos(int x, int y) {
  if (sys_win != 0) gdk_window_move(sys_win, x, y);
}

//NB: resize fait par programme (PAS par l'utilisateur: cf UDisp::on_configure())
void UHardWinGDK::setSize(const UDimension& size) {
  if (sys_win != 0) {
    gdk_window_resize(sys_win, int(size.width), int(size.height));
    //if (UAppli::isUsingGL()) UAppli::getDisp()->setGLViewportOrtho(size); fait dans UGraph
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

void UNatWin::setTitle(const UStr& s) {
  if (sys_win != null && s.c_str()) gdk_window_set_title(sys_win, s.c_str());  // UTF-8 !!!
}

void UNatWin::getTitle(UStr& s) const {
  if (sys_win != null) {                                          // !!!!!! A_COMPLETER !!!
    s = "";  // existe ??????????????
  }
}

void UNatWin::setCursor(const UCursor* curs) {
  Cursor xc = (!curs) ? None : curs->getCursorImpl(disp)->cursor;
  XDefineCursor(SYS_DISP, sys_win, xc);
}

void UNatWin::setNames(const UStr& res_class, const UStr& res_name, 
                       const UStr& win_name, const UStr& icon_name) { 
  // A IMPLEMENTER !!!
}

// ==================================================== [Ubit Toolkit] =========

void UHardWinGDK::realize(WinType wtype, float w, float h) {
  if (isRealized()) return;
  wintype = wtype;
  
  if (wtype == NONE || wtype == SOFTWIN || wtype == EXTWIN) {
    UAppli::error("UHardWinGDK","wrong type for this function: %d", wtype);
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
  attr.cursor = nd->getCursor(UCursor::pointer);
  
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
/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */
