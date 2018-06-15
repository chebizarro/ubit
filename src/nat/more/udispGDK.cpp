/************************************************************************
 *
 *  UDispGDK.cpp: GDK implementation
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
#include <errno.h>
#include <ubit/uconf.hpp>
#include <ubit/uappli.hpp>
#include <ubit/uappliImpl.hpp>
#include <ubit/ueventflow.hpp>
#include <ubit/uon.hpp>
#include <ubit/uwin.hpp>
#include <ubit/ucursor.hpp>
#include <ubit/umsproto.hpp>
#include <ubit/nat/udispGDK.hpp>
#include <ubit/nat/unatwin.hpp>
using namespace std;
namespace ubit {

NOTE: this port cannot work beacuse it has not been fully updated
  
// ==================================================== [Ubit Toolkit] =========

UCursor UCursor::pointer(GDK_LEFT_PTR, UCONST);
UCursor UCursor::crosshair(GDK_CROSS, UCONST);
UCursor UCursor::text(GDK_XTERM, UCONST);
UCursor UCursor::hand(GDK_HAND2, UCONST);
UCursor UCursor::pencil(GDK_PENCIL, UCONST);
UCursor UCursor::wait(GDK_WATCH, UCONST);
UCursor UCursor::question(GDK_QUESTION_ARROW, UCONST);
UCursor UCursor::hresize(GDK_SB_H_DOUBLE_ARROW, UCONST);
UCursor UCursor::vresize(GDK_SB_V_DOUBLE_ARROW, UCONST);
UCursor UCursor::move(GDK_FLEUR, UCONST);
UCursor UCursor::dnd(GDK_GUMBY, UCONST);
//UCursor UCursor::plus(GDK_PLUS, UCONST);

// ==================================================== [Ubit Toolkit] =========

void UGraph::createRenderContext(XXXX* _nd) {
  UNatDispGDK* nd = (UNatDispGDK*)_nd;  // !!!@@@@

  xor_mode = false;
  font_styles = 0;
  font = null;
  sys_disp = nd->getSysDisp();
  color    = nd->getBlackPixel();
  bgcolor  = nd->getWhitePixel();
  gc = null;
#if UBIT_WITH_GL
  initGL();
  if (UAppli::isUsingGL()) {width = 1.; return;}  
#endif
  width = 0.;
  gc = gdk_gc_new(nd.getFrameWin());  // gc = gdk_gc_new_with_values()
  // gcval.graphics_exposures = false;
  gdk_gc_set_exposures(gc, FALSE);
}

/*
#elif UBIT_WITH_GDK
 GdkDisplay* sys_disp;
 guint32 color, bgcolor;
 GdkGC* gc;
#endif
  
 UNatGraph::UNatGraph(UNatDisp* _nd) : natdisp(_nd) {
   xor_mode = false;
   font_styles = 0;
   font = null;
#if WITH_2D_GRAPHICS
   UNatDispX11* nd = (UNatDispX11*)_nd;  // !!!@@@@
   sys_disp = nd->getSysDisp();
   color    = nd->getBlackPixel();
   bgcolor  = nd->getWhitePixel();
   gc = null;
#endif
   
#ifdef UBIT_WITH_GL
   color_rgba[0] = color_rgba[1] = color_rgba[2] = 0; color_rgba[3] = 255;
   bgcolor_rgba[0] = bgcolor_rgba[1] = bgcolor_rgba[2] = 255; bgcolor_rgba[3] = 255;
   if (UAppli::isUsingGL()) {
     width = 1.;
     return;
   }  
#endif
   
#if UBIT_WITH_GDK
   width = 0.;
   gc = gdk_gc_new(nd.getFrameWin());
   // gc = gdk_gc_new_with_values()
   // gcval.graphics_exposures = false;
   gdk_gc_set_exposures(gc, FALSE);
#elif UBIT_WITH_X11
   width = 0.;
   XGCValues gcval;
   gcval.function = GXcopy;
   gcval.foreground = color;
   gcval.background = bgcolor;
   // graphics_exposures = true means that events are generated when
   // XCopyArea or XCopyPlanes is called and a part of the view is obscured
   // This is useless here except when scrolling data (the last argument 
   // of the UGraph::copyArea function can be used to change this
   // behavior (see fct. copyArea() in ugraph.hpp for details)
   gcval.graphics_exposures = false;
   
   // Note: The GC can be used with any destination drawable having 
   // the same root and depth as the specified drawable. 
   gc = XCreateGC(sys_disp, nd->getSysFrame(),
                  GCFunction | GCForeground | GCBackground | GCGraphicsExposures,
                  &gcval);
#endif
}
*/
// ==================================================== [Ubit Toolkit] =========

UDispGDK::UDispGDK(const UStr& _dname) : UDisp(_dname),
sys_disp(null),
sys_screen(null),
sys_visual(null),
sys_cmap(null),
main_gimpl(None), client_gimpl(None)
{
  if (UAppli::isUsingGL()) {
    UAppli::error("UDispGDK","GDK implementation does not support OpenGL mode");
  }
  
  sys_disp = gdk_display_open(getDisplayName());
  const char* dname = d->getDisplayName().empty() ? "''" : d->getDisplayName().c_str();

  if (!sys_disp || !(sys_screen = gdk_display_get_default_screen(sys_disp))) {
    UAppli::error("UDispGDK","could not open display '%s' (is the X11 server running ?)",dname);
    return;
  }
  //if (nd->xsync) XSynchronize(sys_disp, True);
  //nd->xconnection = -1; //XConnectionNumber(GDK_SCREEN_XDISPLAY(xscreen));
  screen_width = gdk_screen_get_width(sys_screen);
  screen_height = gdk_screen_get_height(sys_screen);

  if (XWidthMMOfScreen(sys_screen) == 0)
    UAppli::error("UDispX11","the horizontal resolution is unknow (display %s)",dname);
  else w_mm_to_px = double(screen_width) / XWidthMMOfScreen(sys_screen);
  
  //if (XHeightMMOfScreen(sys_screen) == 0)
  //  UAppli::error("UDispX11","the vertical resolution is unknow (display %s)",dname);
  //else h_mm_to_px = double(screen_height) / XHeightMMOfScreen(sys_screen);
  
  MM_TO_PX = w_mm_to_px;
  CM_TO_PX = w_mm_to_px * 10;
  IN_TO_PX = w_mm_to_px * 25.4;  // generally close to 72 
  PT_TO_PX = w_mm_to_px * 25.4 / 72;       //  pt = 1/72 in
  PC_TO_PX = 12 * w_mm_to_px * 25.4 / 72;  // 1 pc = 12 pt
  
  sys_visual = gdk_screen_get_rgb_visual(sys_screen);
  sys_cmap = gdk_screen_get_rgb_colormap(sys_screen);

  if (sys_cmap == 0 || sys_visual == 0) {
    UAppli::warning("UDispGDK","GDK: could not find visual; searching for default visual on display '%s'", dname);
    sys_visual = gdk_screen_get_system_visual(sys_screen);
    sys_cmap = gdk_screen_get_system_colormap(sys_screen);
  }
  // ATT: prend TJRS la valeur par defaut de bpp sans tenir compte du nd->bpp demande
  if (sys_visual) bpp = sys_visual->depth;

  if (!(default_rc = UGraph::createRC(this))) {
    UAppli::error("UDispGDK","could not create Graphics Context on display '%s'", dname);
    return;
  }
  
  GdkColor c;
  gdk_color_black(sys_cmap, &c);
  black_pixel = c.pixel;
  gdk_color_white(sys_cmap, &c);
  white_pixel = c.pixel;
  red_mask   = sys_visual->red_mask;
  green_mask = sys_visual->green_mask;
  blue_mask  = sys_visual->blue_mask;
  countBits(red_mask,   red_bits,   red_shift);
  countBits(green_mask, green_bits, green_shift);
  countBits(blue_mask,  blue_bits,  blue_shift);
      
  atoms.PRIMARY_SELECTION   = GetAtom(sys_disp, "PRIMARY");
  atoms.SECONDARY_SELECTION = GetAtom(sys_disp, "SECONDARY");
  atoms.WM_PROTOCOLS    = GetAtom(sys_disp, "WM_PROTOCOLS");
  atoms.WM_DELETE_WINDOW= GetAtom(sys_disp, "WM_DELETE_WINDOW");
  atoms.WM_TAKE_FOCUS   = GetAtom(sys_disp, "WM_TAKE_FOCUS");
  atoms.UMS_WINDOW      = GetAtom(sys_disp, UMS_WINDOW_PROPERTY);
  atoms.UMS_MESSAGE     = GetAtom(sys_disp, UMS_MESSAGE_PROPERTY);
  //atoms.USELECTION    = GetAtom(sys_disp, _USELECTION);

  // NB: requires: sys_visual, default_pixmap, atoms, and possibly, white/black pixels
  mainframe = new UNatWin(this, 10, 10, UNatWin::MAINFRAME);  
  if (mainframe->sys_win == null) {
    UAppli::error("UDispGDK","could not create the main frame window for display '%s'", dname);
    return;
  }
  natgraph = new UNatGraph(this);
  UModifier::mapKeys(this);
  UKey::mapKeys(this);
  
  default_gc->makeCurrent(mainframe);
  is_opened = true;
}  
  
UDispGDK::~UDispGDK() {
  //if (sys_disp && nd->default_pixmap != 0) g_object_unref(default_pixmap);
  if (sys_disp) gdk_display_close(sys_disp);
}
  
// =============================================================================
// ==================================================== [Ubit Toolkit] =========

USysWin UDispGDK::getSysRoot() const {return gdk_screen_get_root_window(sys_screen);}
USysWin UDispGDK::getSysFrame() const {return mainframe->sys_win;}

UNatCursor* UDispGDK::createCursor(int cursor_type) {
  if (cursor_type >= 0) xc = gdk_cursor_new_for_display(sys_disp, (GdkCursorType)shape);
  else {   //none cursor
    //xcursor = gdk_cursor_new_from_pixmap();   A COMPLETER !!!
    xc = 0;
  }
  return xc;
}

unsigned long UDispGDK::createColor(const unsigned char rgba[4]) {
  GDKColor c;
  // !ATT XColor.red/green/blue est sur 16 bits et non 8 !
  c.red   = rgba[0]<<8;
  c.green = rgba[1]<<8; 
  c.blue  = rgba[2]<<8;
  // color allocation & conversion to pixel value
  if (XAllocColor(sys_disp, sys_cmap, &c)) {
    return c.pixel;    // the color was allocated in the colormap
  }
  else { // the appropriate color could be allocated => warning + use black or white
    UAppli::warning("UDispGDK::createColor","can't allocate color (colormap is full?)");
    // white if color > 65535, black otherwise
    if (c.red + c.blue + c.green > 65535*2) return getWhitePixel();
    else return getBlackPixel();
  }
}

void UDispGDK::flush() {
#if UBIT_WITH_GL 
  //if (disp.getConf().using_gl) glFlush();
  if (UAppli::isUsingGL()) glFlush();
#endif
  Flush(sys_disp);
}

void UDispGDK::setPointerPos(int x, int y) {
  UAppli::error("UDisp::setPointerPos","Not available with GDK");
}

void UDispGDK::getPointerPos(int& x, int& y) const {
  x = y = 0;
  UAppli::error("UDisp::getPointerPos","Not available with GDK");
}

int UDispGDK::getPointerState() const {
  UAppli::error("UDisp::getPointerState","Not available with GDK");
  return 0;
}

/*
bool UDispGDK::grabPointer(const UCursor* c) {
  USysCursor cursor = c ? getCursor(*c) : None;
  return gdk_pointer_grab(getSysFrame(),
                          true, //owner
                          (GdkEventMask)(ButtonPressMask|ButtonReleaseMask|PointerMotionMask),
                          USysNull,  // confine to window
                          cursor,   // cursor 
                          GDK_CURRENT_TIME)
    == 0;   // GDK_GRAB_SUCCESS
}

void UDispGDK::ungrabPointer() {
  gdk_pointer_ungrab(GDK_CURRENT_TIME);
}

bool UDispGDK::pickWindow(UNatWin& nw, int& x, int& y, UCall* call, UCursor* curs) {
  UAppli::error("UDisp::pickWindow","Not available with GDK");
  return false;
}
*/

// =============================================================================
// ==================================================== [Ubit Toolkit] =========

static GMainLoop *gmainloop = 0, *gsubloop = 0;

void UDispGDK::eventHandler(GdkEvent* e, gpointer appli) {
  if (!e) return;
  UAppliImpl* a = (UAppliImpl*)appli;
  UDispGDK* nd = (UDispGDK*)a->displist[0]->getNatDisp();  // A COMPLETER !!!
  nd->dispatchEvent(e);
  if (a->request_mask) a->processPendingRequests();
}

static void destroyHandler(gpointer) {}

void UDispGDK::startAppli() {
  UAppliImpl& a = UAppli::impl;
  gdk_error_trap_push();    // supprimer les XErrors
  gdk_rgb_init();
  gdk_event_handler_set(eventHandler, &a, destroyHandler);
  gmainloop = g_main_loop_new(NULL, FALSE);
  gsubloop = g_main_loop_new(NULL, FALSE);  // !!!
}

void UDispGDK::quitAppli() {
}

void UDispGDK::startLoop(bool main) {
  UAppliImpl& a = UAppli::impl;
  if (main) {
    a.mainloop_running = true;
    g_main_loop_run(gmainloop);
  }
  else {
    a.subloop_running = true;
    g_main_loop_run(gsubloop);
  }
}

void UDispGDK::quitLoop(bool main) {
  UAppliImpl& a = UAppli::impl;
  if (main) {
    a.mainloop_running = a.subloop_running = false;
    g_main_loop_quit(gmainloop);
  }
  else {
    a.subloop_running = false;
    g_main_loop_quit(gsubloop);
  }
}

// ==================================================== [Elc] ==================
// =============================================================================

//define FLOW(E,T) obtainFlow(E->T.state, E->T.subwindow)
#define FLOW(E,T) obtainFlow(0,0)

static int getButton(GdkEvent* sev) {
  switch (sev->button.button) {
    case 1: return UEvent::LeftButton;
    case 2: return UEvent::MidButton;
    case 3: return UEvent::RightButton;
  }
  return 0;
}

static void getKey(GdkEvent* sev, unsigned int& keycode, unsigned short& keychar) {
  int count = sev->key.length;
  keycode = sev->key.keyval;
  // PAS forcement a jour !!!!!
  keychar = (count > 0 || sev->key.string == 0) ? 0 : sev->key.string[0];
}

/*
static void onSelection(UDisp* nd, UWin* win, UView* winview, GdkEvent* sev) {
  // NB: no time, no state!
  UEventFlow* f = nd->obtainFlow(0,0);   // ????  
  switch (sev->type) {
    case GDK_SELECTION_CLEAR: { 
      ...;
    } break;
      
    case GDK_SELECTION_REQUEST: {
      ...;
    } break;
      
    case GDK_SELECTION_NOTIFY: {
      ...;
    } break;
  }  
  if (win->hasCallback(UMode::SYSWM_CB)) {
    USysWMEvent e(winview, f, sev);
    win->fire(e, UOn::sysWM);
  }  
}
*/
// -----------------------------------------------------------------------------

void UDispGDK::dispatchEvent(GdkEvent* sev) {
  USysWin event_win = sev->any.window;
  UWin* win = retrieveWin(event_win);
  if (!win) return;
  UView* v = win->getWinView(&disp);  
  
  switch(sev->type) {
    
    case GDK_MOTION_NOTIFY: {
      UEventFlow* f = FLOW(sev, motion);
      f->winMouseMove(win, sev->motion.time, sev->motion.state, sev->motion.x, sev->motion.y);
    } break;
      
    case GDK_BUTTON_PRESS:
    case GDK_2BUTTON_PRESS:    // ????????
    case GDK_3BUTTON_PRESS: {   // ????????
      UEventFlow* f = FLOW(sev, button);
      cerr << "GDK_BUTTON_PRESS "<< endl;
      f->winMousePress(win, sev->button.time, sev->button.state, 
                       sev->button.x, sev->button.y, getButton(sev));
    } break;
      
    case GDK_BUTTON_RELEASE: {
      UEventFlow* f = FLOW(sev, button);
      f->winMouseRelease(win, sev->button.time, sev->button.state, 
                         sev->button.x, sev->button.y, getButton(sev));
    } break;
      
    case GDK_SCROLL: {
      cerr << "GDK_SCROLL "<< endl;
      // onWheelEvent(...);  !!!!!
    } break;
      
    case GDK_KEY_PRESS: {
      unsigned short keychar;
      unsigned int keycode;
      cerr << "GDK: onKeyPress not implemented " << keycode << " " << keychar << endl;
      UEventFlow* f = FLOW(sev, key);
      getKey(sev, keycode, keychar);
      f->winKeyPress(win, sev->key.time, sev->key.state, 0/*x*/,0/*y*/, keycode, keychar);
    } break;
      
    case GDK_KEY_RELEASE: {
      unsigned short keychar;
      unsigned int keycode;
      cerr << "GDK: onKeyRelease not implemented " << keycode << " " << keychar << endl;
      UEventFlow* f = FLOW(sev, key);
      getKey(sev, keycode, keychar);
      f->winKeyRelease(win, sev->key.time, sev->key.state, 0/*x*/,0/*y*/, keycode, keychar);
    } break;
      
    case GDK_ENTER_NOTIFY: {
    UEventFlow* f = FLOW(sev, crossing);
      f->winEnter(win, sev->crossing.time, sev->crossing.x, sev->crossing.y);
    } break;
    
  case GDK_LEAVE_NOTIFY: {
    UEventFlow* f = FLOW(sev, crossing);
    f->winLeave(win, sev->crossing.time, sev->crossing.x, sev->crossing.y);
  } break;
    
  case GDK_FOCUS_CHANGE:
    //onFocus(win, v, sev);
    break;
    
  case GDK_EXPOSE:
    onPaint(v, sev->expose.area.x, sev->expose.area.y,
            sev->expose.area.width, sev->expose.area.height);
    break;
    
    //case GDK_NO_EXPOSE:    voir cas X11 !!!
    //onPaint(...);
    // break;
    //case GraphicsExpose: see gdk_event_get_graphics_expose()
    
  case GDK_CONFIGURE:
    onResize(v, sev->configure.width, sev->configure.height);
    break;
    
  case GDK_DELETE:
    win->close(0);
    break;    
    
  case GDK_MAP:
    // permet de ne pas prendre en compte les Configure Events
    // tant que window pas visible (voir onResize)
    win->setShowable(true);
    win->setMapped(true);
    if (win->hasCallback(UMode::WIN_STATE_CB)) {
      UWinEvent e(v, /*obtainFlow(0,0),*/ sev->type, 0/*state*/);
      win->fire(e, UOn::winState);
    }
    break;
    
  case GDK_UNMAP:
    win->setShowable(false);
    win->setMapped(false);
    if (win->hasCallback(UMode::WIN_STATE_CB)) {
      UWinEvent e(v, /*obtainFlow(0,0),*/ sev->type, 0/*state*/);
      win->fire(e, UOn::winState);
    }
    break;
  
  case GDK_WINDOW_STATE: {
    GdkWindowState changed_mask = sev->window_state.changed_mask;
    /* A COMPLETER!!
    if (changed_mask & (GDK_WINDOW_STATE_WITHDRAWN | GDK_WINDOW_STATE_ICONIFIED)) {
      win->setCmodes(UMode::CAN_SHOW, false);
      win->setCmodes(UMode::WIN_MAPPED, false);
    }
    if (changed_mask & (GDK_WINDOW_STATE_MAXIMIZED | GDK_WINDOW_STATE_FULLSCREEN)) {
      win->setCmodes(UMode::CAN_SHOW, true);
      win->setCmodes(UMode::WIN_MAPPED, true);
    }
    */
    if (win->hasCallback(UMode::WIN_STATE_CB)) {
      UWinEvent e(v, /*obtainFlow(0,0),*/ sev->type, changed_mask);
      win->fire(e, UOn::winState);
    }
  } break;
    
  /*
   case GDK_SELECTION_CLEAR:
   case GDK_SELECTION_REQUEST:
   case GDK_SELECTION_NOTIFY:
     onSelection(win, v, sev);  A IMPLEMENTER !!!!
       break;
  */
    
  case GDK_CLIENT_EVENT:{
    UEventFlow* f = obtainFlow(0,0);
    UMessagePortMap* messmap = UAppli::getMessagePortMap();
    
    if (messmap && sev->client.message_type == atoms.UMS_MESSAGE) {
      UMessageEvent e(v, f, sev);
      messmap->fireMessagePort(e);
    }
    else if (win->hasCallback(UMode::SYSWM_CB)) {
      USysWMEvent e(v, f, sev);
      win->fire(e, UOn::sysWM);
    }
  } break;
    
  case GDK_PROPERTY_NOTIFY:{
    UEventFlow* f = obtainFlow(0,0);
    UMessagePortMap* messmap = UAppli::getMessagePortMap();
    
    if (messmap && sev->property.atom == atoms.UMS_MESSAGE) {
      UMessageEvent e(v, f, sev);
      messmap->fireMessagePort(e);
    }
    else if (win->hasCallback(UMode::SYSWM_CB)) {
      USysWMEvent e(v, f, sev);
      win->fire(e, UOn::sysWM);
    }
  } break;
      
  case GDK_DESTROY: 
    //cerr<<"GDK_DESTROY"<< endl;
    break;
    
  case GDK_NOTHING:
    //nop: a special code to indicate a null event.
    break;
    
  default:
    if (win->hasCallback(UMode::SYSWM_CB)) {
      UEventFlow* f = obtainFlow(0,0);
      USysWMEvent e(v, f, sev);
      win->fire(e, UOn::sysWM);
    }
    break;
  }
}

}
#endif  // UBIT_WITH_GDK
/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */

