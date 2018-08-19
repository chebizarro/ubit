/*
 *  HardwinGLUT.cpp: GLUT Windows
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
#if UBIT_WITH_GLUT

#include <iostream>
#include <cstdio>
#include <ubit/core/application.h>
#include <ubit/core/call.h>
#include <ubit/core/on.h>
#include <ubit/core/string.h>
#include <ubit/ui/cursor.h>
#include <ubit/ui/window.h>
#include <ubit/umsproto.hpp>
#include <ubit/nat/udispGLUT.hpp>
#include <ubit/nat/uglcontext.hpp>
using namespace std;

namespace ubit {


//void glutIconifyWindow(void);
//void glutFullScreen(void)
/*
void UHardwinGLUT::makeCurrent() {
  glutSetWindow(sys_win);
}
  
void UHardwinGLUT::swapBuffers() {
  if (sys_win <= 0) return;
  
  int cur_win = glutGetWindow();
  if (cur_win != sys_win) glutSetWindow(sys_win);
  //cerr << "*** swapBuffers win: " << sys_win << " *** " << endl;
  glutSwapBuffers();
  if (cur_win != sys_win) glutSetWindow(cur_win);
}
*/

//NB: move fait par programme (PAS par l'utilisateur: cf Display::on_configure())
void UHardwinGLUT::setPos(const Point& p) {
  if (sys_win <= 0) return;
  
  int cur_win = glutGetWindow();
  if (cur_win != sys_win) glutSetWindow(sys_win);
  glutPositionWindow(int(p.x), int(p.y));
  if (cur_win != sys_win) glutSetWindow(cur_win);
}


Point UHardwinGLUT::getPos() const {
  if (sys_win <= 0) return Point(0,0);
  
  int cur_win = glutGetWindow();
  if (cur_win != sys_win) glutSetWindow(sys_win);
  Point pos;
  
  if (wintype != SUBWIN) {  // toplevel window
    pos.set(glutGet(GLUT_WINDOW_X), glutGet(GLUT_WINDOW_Y));
  }
  else {  // subwindow
    int glob_x = glutGet(GLUT_WINDOW_X);
    int glob_y = glutGet(GLUT_WINDOW_Y);    
    //int parwin = glutGet(GLUT_WINDOW_PARENT);
    //if (parwin == 0) glutPositionWindow(int(p.x), int(p.y));
    pos.set(glob_x, glob_y);                                    // FAUX &&&&&&!!!!!
  }
  
  if (cur_win != sys_win) glutSetWindow(cur_win);
  return pos;
}


Point UHardwinGLUT::getScreenPos() const {
  if (sys_win <= 0) return Point(0,0);
  
  int cur_win = glutGetWindow();
  if (cur_win != sys_win) glutSetWindow(sys_win);
  
  Point pos(glutGet(GLUT_WINDOW_X), glutGet(GLUT_WINDOW_Y)); // OK with subwindows????%%%%
  
  if (cur_win != sys_win) glutSetWindow(cur_win);
  return pos;
}

//NB: resize fait par programme (PAS par l'utilisateur: cf Display::on_configure())

void UHardwinGLUT::setSize(const Dimension& size) {
  if (sys_win <= 0) return;
  
  int cur_win = glutGetWindow();
  if (cur_win != sys_win) glutSetWindow(sys_win);

  glutReshapeWindow(int(size.width), int(size.height));
  //if (wintype != SUBWIN) Application::getDisp()->setGLViewportOrtho(size);  // fait dans Graph
  
  if (cur_win != sys_win) glutSetWindow(cur_win);
}


Dimension UHardwinGLUT::getSize() const {
  if (sys_win <= 0) return Dimension(0,0);

  int cur_win = glutGetWindow();
  if (cur_win != sys_win) glutSetWindow(sys_win);
  
  Dimension dim(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
  
  if (cur_win != sys_win) glutSetWindow(cur_win);
  return dim;
}


void UHardwinGLUT::show(bool state) {
  if (sys_win <= 0) return;
  
  int cur_win = glutGetWindow();
  if (cur_win != sys_win) glutSetWindow(sys_win);
  if (state) glutShowWindow(); 
  else glutHideWindow();
  if (cur_win != sys_win) glutSetWindow(cur_win);
}


void UHardwinGLUT::toFront() {
  if (sys_win <= 0) return;
  
  int cur_win = glutGetWindow();
  if (cur_win != sys_win) glutSetWindow(sys_win);
  glutPopWindow();
  if (cur_win != sys_win) glutSetWindow(cur_win);
}


void UHardwinGLUT::toBack() {
  if (sys_win <= 0) return;
  
  int cur_win = glutGetWindow();
  if (cur_win != sys_win) glutSetWindow(sys_win);
  glutPushWindow();
  if (cur_win != sys_win) glutSetWindow(cur_win);
}


void UHardwinGLUT::setCursor(const Cursor* curs) {
  if (sys_win <= 0) return;
  
  int cur_win = glutGetWindow();
  if (cur_win != sys_win) glutSetWindow(sys_win);
  int xc = (!curs) ? GLUT_CURSOR_INHERIT : curs->getCursorImpl(disp)->cursor;
  glutSetCursor(xc);
  if (cur_win != sys_win) glutSetWindow(cur_win);  
 }


void UHardwinGLUT::setTitle(const String& s) {
  if (sys_win <= 0 || !s.c_str()) return;
  
  int cur_win = glutGetWindow();
  if (cur_win != sys_win) glutSetWindow(sys_win);
  glutSetWindowTitle(s.c_str());
  if (cur_win != sys_win) glutSetWindow(cur_win);
}


String UHardwinGLUT::getTitle() const {
  if (sys_win <= 0) return "";
  
  //int cur_win = glutGetWindow();
  //if (cur_win != sys_win) glutSetWindow(sys_win);
  //title = ???;                                                // existe ???  !!!&&&
  //if (cur_win != sys_win) glutSetWindow(cur_win);

  return "";
}


void UHardwinGLUT::setIconTitle(const String& s) {
  if (sys_win <= 0 || !s.c_str()) return;
  
  int cur_win = glutGetWindow();
  if (cur_win != sys_win) glutSetWindow(sys_win);
  glutSetIconTitle(s.c_str());
  if (cur_win != sys_win) glutSetWindow(cur_win);
}


String UHardwinGLUT::getIconTitle() const {
  if (sys_win <= 0) return "";
  
  //int cur_win = glutGetWindow();
  //if (cur_win != sys_win) glutSetWindow(sys_win);
  //title = ???;                                                // existe ???  !!!&&&
  //if (cur_win != sys_win) glutSetWindow(cur_win);

  return "";
}

// no effect with GLUT

void UHardwinGLUT::setClassProperty(const String& instance_name, const String& class_name) {
}


UHardwinGLUT::UHardwinGLUT(UDispGLUT* d, Window* w)
: UHardwinImpl(d, w), sys_win(0) {
  glcontext = new GLContext(d, this);
}

UHardwinGLUT::~UHardwinGLUT() {
  if (sys_win < 1) return;
  //else if (wintype == PIXMAP) DestroyPixmap(SYS_DISP, sys_win);  //A_COMPLETER!!!
  
  glutDestroyWindow(sys_win);    // detruit MAINFRAME ???
  // fait par ~UHardwin: delete glcontext;
}


void UHardwinGLUT::realize(WinType wtype, float w, float h) {
  if (isRealized()) return;  
  wintype = wtype;  // must be done first!

  if (wtype == PIXMAP) {  // first!
    Application::error("UHardwinGLUT","type PIXMAP not implemented for GLUT");
    return;
  }
  
  if (!isHardwin()) {
    Application::error("UHardwinGLUT","wrong type for this function: %d", wtype);
    return;
  }
  
  if (wtype != SUBWIN) {
    sys_win = glutCreateWindow(" ");
    glutReshapeWindow(int(w), int(h));
    getDispGLUT()->initWinCallbacks(sys_win);
  }
  
  else {       // !!! ATTENTION: il faut retrouver le bon parent!
    /*
    int parent_win = 0;
    View *win_v, *parent_v;
    UHardwinGLUT* parent_hw;
    if ((win_v = win->getWinView(/disp/))
        && (parent_v = win_v->getParentView())
        && (parent_hw = (UHardwinGLUT*)parent_v->getHardwin())
        ) {
      parent_win = parent_hw->sys_win;
    }
    */
    int parent_win = 0;
    Element* par = win->getParent(0);
    Window* parw = dynamic_cast<Window*>(par);
    if (parw) parent_win = ((UHardwinGLUT*)parw->getHardwin())->sys_win;
    
    if (parent_win == 0) {
      Application::error("UHardwinGLUT::realize","the parent of the SubWindow is not realized, using the main frame as a parent");
      parent_win = 1;
    }

    sys_win = glutCreateSubWindow(parent_win, 0, 0, int(w), int(h));
    getDispGLUT()->initWinCallbacks(sys_win);   // ???
  }
}


}
#endif

