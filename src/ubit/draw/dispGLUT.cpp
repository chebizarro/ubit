/*
 *  dispGLUT.cpp: GLUT implementation
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2009 | Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
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
#if UBIT_WITH_GLUT

#include <iostream>
#include <errno.h>
#include <ubit/ugraph.hpp>
#include <ubit/core/application.h>
#include <ubit/uappliImpl.hpp>
#include <ubit/ueventflow.hpp>
#include <ubit/core/config.h>
#include <ubit/ui/window.h>
#include <ubit/ucursor.hpp>
#include <ubit/nat/udispGLUT.hpp>
#include <ubit/nat/urendercontext.hpp>
using namespace std;

namespace ubit {


Cursor Cursor::pointer(GLUT_CURSOR_LEFT_ARROW, UCONST);
Cursor Cursor::crosshair(GLUT_CURSOR_CROSSHAIR, UCONST);
Cursor Cursor::text(GLUT_CURSOR_TEXT, UCONST);
Cursor Cursor::hand(GLUT_CURSOR_INFO, UCONST);
Cursor Cursor::pencil(GLUT_CURSOR_SPRAY, UCONST);  // ???GLUT_CURSOR_LEFT_ARROW
Cursor Cursor::wait(GLUT_CURSOR_WAIT, UCONST);
Cursor Cursor::question(GLUT_CURSOR_HELP, UCONST);
Cursor Cursor::hresize(GLUT_CURSOR_LEFT_RIGHT, UCONST);
Cursor Cursor::vresize(GLUT_CURSOR_UP_DOWN, UCONST);
Cursor Cursor::move(GLUT_CURSOR_INFO, UCONST);  // same sa hand
Cursor Cursor::dnd(GLUT_CURSOR_CYCLE, UCONST);  // ???
//Cursor Cursor::plus(GDK_PLUS, UCONST);
  

UDispGLUT::UDispGLUT(const String& _dname) : Display(_dname)
{
  const char* dname = getDisplayName().empty() ? "''" : getDisplayName().c_str();

  // glutInitWindowSize(int width, int height); default: 300x300
  //NB: bpp is not used ?
  
  int display_modes = GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA;
  if (depth_size > 0) display_modes |= GLUT_DEPTH;
  if (stencil_size > 0) display_modes |= GLUT_STENCIL;
  //if (accum_size > 0) display_modes |= GLUT_ACCUM;
 
  glutInitDisplayMode(display_modes);
  
  // ATT: un seul DISP peut etre ouvert!
  int argc = 1;
  char *argv[] = {"Ubit"};
  // ? plutot dans initAppli() ?
  glutInit(&argc, argv);   // cf: -gldebug -sync -display -direct -indirect -geom
  
  screen_width = glutGet(GLUT_SCREEN_WIDTH);
  screen_height = glutGet(GLUT_SCREEN_HEIGHT);
  screen_width_mm = glutGet(GLUT_SCREEN_WIDTH_MM);
  screen_height_mm = glutGet(GLUT_SCREEN_HEIGHT_MM);
  
  if (screen_width_mm == 0 || screen_height_mm == 0) {
    Application::warning("UDispGLUT","the resolution of the diplay is unknow (display %s)",dname);
    setPixelPerInch(72);   // default
  }
  else setPixelPerMM(double(screen_width)/screen_width_mm);
   
  //if (!(default_rc = Graph::createRC(this, null/*sharelists*/))) {
  //  Application::error("UDispGLUT","could not create Rendering Context on display '%s'",dname);
  //  return;
  //}
  
  Modifier::mapKeys(this);
  Key::mapKeys(this);
  
  //default_rc->setDest(mainframe, 0, 0);  n'exsite pas sous GLUT!
  is_opened = true;
}
  
UDispGLUT::~UDispGLUT() {
  cerr << "UDispGLUT::~UDispGLUT() " <<endl;
  // if (sys_disp) ???CloseDisplay(sys_disp);
}

  
unsigned long UDispGLUT::createColorPixel(const Rgba& rgba) {
  return 0;
}

UHardwinGLUT* UDispGLUT::createWinImpl(Window* win) {
  return new UHardwinGLUT(this, win);
}

UCursorImpl* UDispGLUT::createCursorImpl(int cursor_type) {
  // cas cursor_type == 0 ???
  UCursorImpl* ci = new UCursorImpl();
  ci->cursor = cursor_type;
  return ci;
}

void UDispGLUT::deleteCursorImpl(UCursorImpl* ci) {
  delete ci;
}

// =============================================================================

void UDispGLUT::setPointerPos(const Point&) {
  Application::error("Display::setPointerPos","Not available with GLUT");
}

Point UDispGLUT::getPointerPos() const {
  Application::error("Display::getPointerPos","Not available with GLUT");
  return Point(0,0);
}

int UDispGLUT::getPointerState() const {
  Application::error("Display::getPointerState","Not available with GLUT");
  return 0;
}

bool UDispGLUT::grabPointer(const Cursor* c) {
  //Application::error("Display::grabPointer","Not available with GLUT");
  return false;
}

void UDispGLUT::ungrabPointer() {
  //Application::error("Display::ungrabPointer","Not available with GLUT");
}

bool UDispGLUT::pickWindow(int& x_in_win, int& y_in_win, UHardwinImpl* window,
                           Cursor* cursor, UCall* callback) {
  Application::error("Display::pickWindow","This function is not available when using GLUT");
  return false;
}


void UDispGLUT::startAppli() {}
void UDispGLUT::quitAppli() {}

class UGlutQuit {
public:
  int err;
  UGlutQuit(int _err) : err(_err) {} 
};

void UDispGLUT::startLoop(bool main) {
  try {
    glutMainLoop();
  }
  catch(UGlutQuit& e) {
    cerr << "UGlutQuit was called on:" << e.err <<endl; 
  }
  catch(...) {
    cerr << "Whatever"<<endl; 
  }
}

void UDispGLUT::quitLoop(bool main) {
  //  if (main) mainloop_running = subloop_running = false;
  //  else subloop_running = false;
  //if (main) throw UGlutQuit(1); m:arche pas sous MacOSX. Why ????
  exit(0);
}

Window* UDispGLUT::retrieveWin(int sys_win) {
  HardwinList::iterator c = hardwin_list.begin();  // NB: on pourrait utiliser un VECTOR?
  HardwinList::iterator c_end = hardwin_list.end();
  for ( ; c != c_end; ++c) {
    UHardwinGLUT* hw = static_cast<UHardwinGLUT*>(*c);
    if (hw && hw->sys_win == sys_win && hw->win) return hw->win;
  }
  return null;
}

namespace glut {
  
  static UDispGLUT* _disp = null;   // only one display !!
  static int _state = 0, _mouse_state = 0;
  static int _win_pos_x = 0, _win_pos_y = 0;
  
  static void winMotionCB(int x, int y) {
    Window* win = _disp->retrieveWin(glutGetWindow());
    if (!win) return;
    EventFlow* f = Application::impl.flowlist[0];
    unsigned long time = Application::getTime();  

    f->mouseMotion(win->getWinView(_disp), time, _state, Point(x,y), 
                   Point(x + _win_pos_x, y + _win_pos_y));
    //cerr << "winMotionCB " << (x+_win_pos_x) <<" "<< (y+_win_pos_y) <<endl;

    Application::impl.processPendingRequests();
  }
  
  
  static void winMouseCB(int btn, int up_down, int x, int y) {
    // NB: l'appel du callback positionne glutGetWindow correctement
    int cur_win = glutGetWindow();
    Window* win = _disp->retrieveWin(cur_win);
    if (!win) return;

    EventFlow* f = Application::impl.flowlist[0];
    View* winview = win->getWinView(_disp);
    unsigned long time = Application::getTime();
    
    _win_pos_x = glutGet(GLUT_WINDOW_X);
    _win_pos_y = glutGet(GLUT_WINDOW_Y);
    Point scr_pos(x + _win_pos_x, y + _win_pos_y);
    Point win_pos(x, y);
    //cerr << "winMouseCB " << _win_pos_x<<" "<<_win_pos_y<<" / "<<scr_pos<<endl;
    
    if (up_down == GLUT_DOWN) {
      switch (btn) {
        case GLUT_LEFT_BUTTON: 
          //19jan08: the pressed button must be included!
          _mouse_state |= MouseEvent::LeftButton;
          _state = _mouse_state | glutGetModifiers();
          f->mousePress(winview, time, _state, win_pos, scr_pos, MouseEvent::LeftButton);
          break;
        case GLUT_MIDDLE_BUTTON:
          _mouse_state |= MouseEvent::MidButton;
          _state = _mouse_state | glutGetModifiers();
          f->mousePress(winview, time, _state, win_pos, scr_pos, MouseEvent::MidButton);
          break;
        case GLUT_RIGHT_BUTTON: 
          _mouse_state |= MouseEvent::RightButton;
          _state = _mouse_state | glutGetModifiers();
          f->mousePress(winview, time, _state, win_pos, scr_pos, MouseEvent::RightButton);
          break;
        default:
          Application::warning("glutMousePress", "unknown mouse button %d", btn);
          break;
      }
    }
    else if (up_down == GLUT_UP) {
      switch (btn) {
        case GLUT_LEFT_BUTTON: 
          //19jan08: the pressed button must be excluded!
          _mouse_state &= ~MouseEvent::LeftButton;
          _state = _mouse_state | glutGetModifiers();
          f->mouseRelease(winview, time, _state, win_pos, scr_pos, MouseEvent::LeftButton);
          break;
        case GLUT_MIDDLE_BUTTON:
          _mouse_state &= ~MouseEvent::MidButton;
          _state = _mouse_state | glutGetModifiers();
          f->mouseRelease(winview, time, _state, win_pos, scr_pos, MouseEvent::MidButton);
          break;
        case GLUT_RIGHT_BUTTON: 
          _mouse_state &= ~MouseEvent::RightButton;
          _state = _mouse_state | glutGetModifiers();
          f->mouseRelease(winview, time, _state, win_pos, scr_pos, MouseEvent::RightButton);
          break;
        default:
          Application::warning("glutMouseRelease", "unknown mouse button %d", btn);
          break;
      }
    }
    Application::impl.processPendingRequests();
  }
  
  // -----------------------------------------------------------------------------
  
  static void winSpecialCB(int keycode, int x, int y) {  // modifiers, etc.
    Window* win = _disp->retrieveWin(glutGetWindow());
    if (!win) return;
    EventFlow* f = Application::impl.flowlist[0];
    View* winview = win->getWinView(_disp);
    unsigned long time = Application::getTime();

    //cerr << "glutSpecialCB: code: " << int(keycode) << endl;
    
    //19jan08: the pressed MODIFIERS SHOULD be included!!!@@@
    _state = _mouse_state | glutGetModifiers();
    f->keyPress(winview, time, _state, keycode, 0/*keychar*/);
    
    //19jan08: the pressed MODIFIERS SHOULD be exluded!!!@@@    
    // !!!??? pas sur que le MODIFIER ait été enlevé !!!???
    _state = _mouse_state | glutGetModifiers();
    f->keyRelease(winview, time, _state, keycode, 0/*keychar*/);

    Application::impl.processPendingRequests();
  }
  
  static void winKeyboardCB(unsigned char keychar, int x, int y) {
    Window* win = _disp->retrieveWin(glutGetWindow());
    if (!win) return;
    EventFlow* f = Application::impl.flowlist[0];
    View* winview = win->getWinView(_disp);
    unsigned long time = Application::getTime();

    //cerr << "glutKeyboardCB: key: " << keychar << " int: "<< int(keychar) << endl;
    
    if (keychar == 127     // backspace
        || keychar == 13   // enter
        || keychar == 8    // delete
        || keychar == 9    // tab
        || keychar == 27   // escape
        ) {
      winSpecialCB(keychar, x, y);  // keychar serves as keycode
    }
    else {
      _state = _mouse_state | glutGetModifiers();
      f->keyPress(winview, time, _state, 0/*keycode*/, keychar);
      f->keyRelease(winview, time, _state, 0/*keycode*/, keychar);
      Application::impl.processPendingRequests();
    }
  }
  
  // -----------------------------------------------------------------------------
  
  static void winEntryCB(int in_out) {
    Window* win = _disp->retrieveWin(glutGetWindow());
    if (!win) return;
    EventFlow* f = Application::impl.flowlist[0];    
    View* winview = win->getWinView(_disp);
    unsigned long time = Application::getTime();

    _win_pos_x = glutGet(GLUT_WINDOW_X);
    _win_pos_y = glutGet(GLUT_WINDOW_Y);
    
    if (in_out == GLUT_ENTERED) f->winEnter(winview, time);
    else if (in_out == GLUT_LEFT) f->winLeave(winview, time);
    Application::impl.processPendingRequests();
  }
  
  static void winDisplayCB() {    
    if (glutLayerGet(GLUT_NORMAL_DAMAGED)) { // glutDisplayCB() caused by window system
      Window* win = _disp->retrieveWin(glutGetWindow());
      if (!win) return;
      
      View* v = win->getWinView();
      //cerr << "winDisplayCB " << win << " size " <<v->width << " " <<  v->height << endl;
      _disp->onPaint(v, v->x, v->y, v->width, v->height);
    }
    // in all cases, even if caused by glutPostRedisplay()
    Application::impl.processPendingRequests();
  }
  
  static void winReshapeCB(int w, int h) {
    Window* win = _disp->retrieveWin(glutGetWindow());
    //cerr << "winReshapeCB " << win << " size " <<w << " " << h << endl;
    if (!win) return;
    
    View* v = win->getWinView();
    _disp->onResize(v, Dimension(w, h));
  }
  
  static void winVisibilityCB(int state) {
    Window* win = _disp->retrieveWin(glutGetWindow());
    if (!win) return;
    //View* v = win->getWinView();
    //cerr << "glutVisibilityCB " << endl;
    // state: GLUT_NOT_VISIBLE or GLUT_VISIBLE
  }
  
  /*
   void glutIdleFunc(void (*func)(void));
   void glutTabletMotionFunc(void (*func)(int x, int y));
   void glutTabletButtonFunc(void (*func)(int button, int state, int x, int y));
   void glutSpaceballMotionFunc(void (*func)(int x, int y, int z));
   void glutSpaceballRotateFunc(void (*func)(int x, int y, int z));
   void glutSpaceballButtonFunc(void (*func)(int button, int state));
   void glutDialsFunc(void (*func)(int dial, int value));
   void glutButtonBoxFunc(void (*func)(int button, int state));   
   */
} // end namespace glutImpl
  
void UDispGLUT::initWinCallbacks(int sys_win) {
  glut::_disp = this;  // static var used by callback functions

  if (sys_win <= 0) {
    Application::error("UDispGLUT::initWinCallbacks","Wrong window ID: %d", sys_win);
    return; 
  }

  glutSetWindow(sys_win);  
  glutMotionFunc(glut::winMotionCB);
  glutPassiveMotionFunc(glut::winMotionCB);
  glutMouseFunc(glut::winMouseCB);
  glutSpecialFunc(glut::winSpecialCB);
  glutKeyboardFunc(glut::winKeyboardCB);
  glutEntryFunc(glut::winEntryCB);
  glutDisplayFunc(glut::winDisplayCB);
  glutReshapeFunc(glut::winReshapeCB);
  glutVisibilityFunc(glut::winVisibilityCB);
}  

}
#endif  // UBIT_WITH_GLUT



