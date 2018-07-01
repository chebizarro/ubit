/************************************************************************
 *
 *  uHardwinGLFW.cpp: GLFW Windows
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

#include <iostream>
#include <cstdio>
#include <ubit/uappli.hpp>
#include <ubit/ucall.hpp>
#include <ubit/uon.hpp>
#include <ubit/ustr.hpp>
#include <ubit/ucursor.hpp>
#include <ubit/uwin.hpp>
#include <ubit/umsproto.hpp>
#include <ubit/nat/udispGLFW.hpp>
#include <ubit/nat/uglcontext.hpp>
using namespace std;

namespace ubit {

//void GLFWIconifyWindow(void);
//void GLFWFullScreen(void)
/*
void UHardwinGLFW::makeCurrent() {
  GLFWSetWindow(sys_win);
}
  
void UHardwinGLFW::swapBuffers() {
  if (sys_win <= 0) return;
  
  int cur_win = GLFWGetWindow();
  if (cur_win != sys_win) GLFWSetWindow(sys_win);
  //cerr << "*** swapBuffers win: " << sys_win << " *** " << endl;
  GLFWSwapBuffers();
  if (cur_win != sys_win) GLFWSetWindow(cur_win);
}
*/
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//NB: move fait par programme (PAS par l'utilisateur: cf UDisp::on_configure())
void UHardwinGLFW::setPos(const UPoint& p) {
  if (sys_win <= 0) return;
  
  int cur_win = GLFWGetWindow();
  if (cur_win != sys_win) GLFWSetWindow(sys_win);
  GLFWPositionWindow(int(p.x), int(p.y));
  if (cur_win != sys_win) GLFWSetWindow(cur_win);
}


UPoint UHardwinGLFW::getPos() const {
  if (sys_win <= 0) return UPoint(0,0);
  
  int cur_win = GLFWGetWindow();
  if (cur_win != sys_win) GLFWSetWindow(sys_win);
  UPoint pos;
  
  if (wintype != SUBWIN) {  // toplevel window
    pos.set(GLFWGet(GLFW_WINDOW_X), GLFWGet(GLFW_WINDOW_Y));
  }
  else {  // subwindow
    int glob_x = GLFWGet(GLFW_WINDOW_X);
    int glob_y = GLFWGet(GLFW_WINDOW_Y);    
    //int parwin = GLFWGet(GLFW_WINDOW_PARENT);
    //if (parwin == 0) GLFWPositionWindow(int(p.x), int(p.y));
    pos.set(glob_x, glob_y);                                    // FAUX &&&&&&!!!!!
  }
  
  if (cur_win != sys_win) GLFWSetWindow(cur_win);
  return pos;
}


UPoint UHardwinGLFW::getScreenPos() const {
  if (sys_win <= 0) return UPoint(0,0);
  
  int cur_win = GLFWGetWindow();
  if (cur_win != sys_win) GLFWSetWindow(sys_win);
  
  UPoint pos(GLFWGet(GLFW_WINDOW_X), GLFWGet(GLFW_WINDOW_Y)); // OK with subwindows????%%%%
  
  if (cur_win != sys_win) GLFWSetWindow(cur_win);
  return pos;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//NB: resize fait par programme (PAS par l'utilisateur: cf UDisp::on_configure())

void UHardwinGLFW::setSize(const UDimension& size) {
  if (sys_win <= 0) return;
  
  int cur_win = GLFWGetWindow();
  if (cur_win != sys_win) GLFWSetWindow(sys_win);

  GLFWReshapeWindow(int(size.width), int(size.height));
  //if (wintype != SUBWIN) UAppli::getDisp()->setGLViewportOrtho(size);  // fait dans UGraph
  
  if (cur_win != sys_win) GLFWSetWindow(cur_win);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UDimension UHardwinGLFW::getSize() const {
  if (sys_win <= 0) return UDimension(0,0);

  int cur_win = GLFWGetWindow();
  if (cur_win != sys_win) GLFWSetWindow(sys_win);
  
  UDimension dim(GLFWGet(GLFW_WINDOW_WIDTH), GLFWGet(GLFW_WINDOW_HEIGHT));
  
  if (cur_win != sys_win) GLFWSetWindow(cur_win);
  return dim;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UHardwinGLFW::show(bool state) {
  if (sys_win <= 0) return;
  
  int cur_win = GLFWGetWindow();
  if (cur_win != sys_win) GLFWSetWindow(sys_win);
  if (state) GLFWShowWindow(); 
  else GLFWHideWindow();
  if (cur_win != sys_win) GLFWSetWindow(cur_win);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UHardwinGLFW::toFront() {
  if (sys_win <= 0) return;
  
  int cur_win = GLFWGetWindow();
  if (cur_win != sys_win) GLFWSetWindow(sys_win);
  GLFWPopWindow();
  if (cur_win != sys_win) GLFWSetWindow(cur_win);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UHardwinGLFW::toBack() {
  if (sys_win <= 0) return;
  
  int cur_win = GLFWGetWindow();
  if (cur_win != sys_win) GLFWSetWindow(sys_win);
  GLFWPushWindow();
  if (cur_win != sys_win) GLFWSetWindow(cur_win);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UHardwinGLFW::setCursor(const UCursor* curs) {
  if (sys_win <= 0) return;
  
  int cur_win = GLFWGetWindow();
  if (cur_win != sys_win) GLFWSetWindow(sys_win);
  int xc = (!curs) ? GLFW_CURSOR_INHERIT : curs->getCursorImpl(disp)->cursor;
  GLFWSetCursor(xc);
  if (cur_win != sys_win) GLFWSetWindow(cur_win);  
 }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UHardwinGLFW::setTitle(const UStr& s) {
  if (sys_win <= 0 || !s.c_str()) return;
  
  int cur_win = GLFWGetWindow();
  if (cur_win != sys_win) GLFWSetWindow(sys_win);
  GLFWSetWindowTitle(s.c_str());
  if (cur_win != sys_win) GLFWSetWindow(cur_win);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UStr UHardwinGLFW::getTitle() const {
  if (sys_win <= 0) return "";
  
  //int cur_win = GLFWGetWindow();
  //if (cur_win != sys_win) GLFWSetWindow(sys_win);
  //title = ???;                                                // existe ???  !!!&&&
  //if (cur_win != sys_win) GLFWSetWindow(cur_win);

  return "";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UHardwinGLFW::setIconTitle(const UStr& s) {
  if (sys_win <= 0 || !s.c_str()) return;
  
  int cur_win = GLFWGetWindow();
  if (cur_win != sys_win) GLFWSetWindow(sys_win);
  GLFWSetIconTitle(s.c_str());
  if (cur_win != sys_win) GLFWSetWindow(cur_win);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UStr UHardwinGLFW::getIconTitle() const {
  if (sys_win <= 0) return "";
  
  //int cur_win = GLFWGetWindow();
  //if (cur_win != sys_win) GLFWSetWindow(sys_win);
  //title = ???;                                                // existe ???  !!!&&&
  //if (cur_win != sys_win) GLFWSetWindow(cur_win);

  return "";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// no effect with GLFW

void UHardwinGLFW::setClassProperty(const UStr& instance_name, const UStr& class_name) {
}

// ==================================================== [Ubit Toolkit] =========

UHardwinGLFW::UHardwinGLFW(UDispGLFW* d, UWin* w)
: UHardwinImpl(d, w), sys_win(0) {
  glcontext = new UGlcontext(d, this);
}

UHardwinGLFW::~UHardwinGLFW() {
  if (sys_win < 1) return;
  //else if (wintype == PIXMAP) DestroyPixmap(SYS_DISP, sys_win);  //A_COMPLETER!!!
  
  GLFWDestroyWindow(sys_win);    // detruit MAINFRAME ???
  // fait par ~UHardwin: delete glcontext;
}

// ==================================================== [Ubit Toolkit] =========

void UHardwinGLFW::realize(WinType wtype, float w, float h) {
  if (isRealized()) return;  
  wintype = wtype;  // must be done first!

  if (wtype == PIXMAP) {  // first!
    UAppli::error("UHardwinGLFW","type PIXMAP not implemented for GLFW");
    return;
  }
  
  if (!isHardwin()) {
    UAppli::error("UHardwinGLFW","wrong type for this function: %d", wtype);
    return;
  }
  
  if (wtype != SUBWIN) {
    sys_win = glfwCreateWindow(w, h, " ");
    GLFWReshapeWindow(int(w), int(h));
    getDispGLFW()->initWinCallbacks(sys_win);
  }
  
  else {       // !!! ATTENTION: il faut retrouver le bon parent!
    /*
    int parent_win = 0;
    UView *win_v, *parent_v;
    UHardwinGLFW* parent_hw;
    if ((win_v = win->getWinView(/disp/))
        && (parent_v = win_v->getParentView())
        && (parent_hw = (UHardwinGLFW*)parent_v->getHardwin())
        ) {
      parent_win = parent_hw->sys_win;
    }
    */
    int parent_win = 0;
    UElem* par = win->getParent(0);
    UWin* parw = dynamic_cast<UWin*>(par);
    if (parw) parent_win = ((UHardwinGLFW*)parw->getHardwin())->sys_win;
    
    if (parent_win == 0) {
      UAppli::error("UHardwinGLFW::realize","the parent of the USubwin is not realized, using the main frame as a parent");
      parent_win = 1;
    }

    sys_win = GLFWCreateSubWindow(parent_win, 0, 0, int(w), int(h));
    getDispGLFW()->initWinCallbacks(sys_win);   // ???
  }
}


}
