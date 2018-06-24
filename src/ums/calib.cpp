/*************************************************************************
 *
 *  calib.cpp - UMS Server
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2008 Eric Lecolinet / ENST Paris / www.enst.fr/~elc/ubit
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

#include <unistd.h>       // darwin
#include <cstdlib>        // getenv, atexit
#include <cstdio>
#include <cstring>
#include <string>
#include <X11/Xlib.h>
#include "umserver.hpp"
#include "calib.hpp"
using namespace std;

const int Calibration::CROSS_XYPOS = 50;
const int Calibration::CROSS_SIZE  = 300 ;
const float Calibration::SCREEN_SIZE_PERCENT = 0.9;

/* ==================================================== ======== ======= */
// CALIBRATION

Calibration* UMServer::getCalibration() {
  if (!calib) calib = new Calibration(this);
  return calib;
}

bool Calibration::isCalibrationCompleted() const {
  return is_completed;
}


Calibration::Calibration(UMServer* _ums) : ums(*_ums) {
  is_calibrating = false;
  is_completed = false;
  curpoint = 0;
  x = y = 0;
  width = height = 0;

  Display* disp = ums.xdisplay;
  Screen* src = (Screen*) ums.xscreen;

  XSetWindowAttributes wattr;
  wattr.background_pixel = WhitePixelOfScreen(src);
  unsigned long wattr_mask = CWBackPixel;

  calwin = XCreateWindow(disp, ums.getRoot(),
                         0, 0, 800, 600, 0,
                         CopyFromParent, //screen_depth,
                         InputOutput,    // Class
                         CopyFromParent,
                         wattr_mask, &wattr);
  XSelectInput(disp, calwin,
               ExposureMask|StructureNotifyMask
	       |ButtonPressMask|ButtonReleaseMask|PointerMotionMask);

  XStoreName(disp, calwin, "Calibration Window");
  //cant destroy this window (nor the X connection)
  XSetWMProtocols(disp, calwin, &ums._WM_DELETE_WINDOW, 1);

  XGCValues gcval;
  gcval.function   = GXcopy;
  gcval.foreground = black_pixel = BlackPixelOfScreen(src);
  gcval.background = WhitePixelOfScreen(src);

  calgc = XCreateGC(disp, calwin,
                    GCFunction | GCForeground | GCBackground, &gcval);

  XColor color, exact_color;
  if (XAllocNamedColor(disp, DefaultColormapOfScreen((Screen*)(ums.xscreen)),
		       "red", &color, &exact_color))
    red_pixel = color.pixel;
  else 
    red_pixel = black_pixel;

  //fs = XLoadQueryFont(getXDisplay(), font_name);
  //gcval.font = font->fid;
  //XChangeGC(natdisp->xdisplay, gc, GCFont, &gcval);
}

/* ==================================================== ======== ======= */

void Calibration::startCalibration(const char* calwin_title) {
  Display* disp = ums.getDisplay();
  long screen_width  = ums.getScreenWidth();    // !! probleme si dual-headed display !!
  long screen_height = ums.getScreenHeight();

  if (calwin_title) XStoreName(disp, calwin, calwin_title);
  curpoint = 0;
  is_calibrating = true;
  is_completed = false;

  width = int(screen_width * SCREEN_SIZE_PERCENT);
  height = int(screen_height * SCREEN_SIZE_PERCENT);
  x = (screen_width - width) / 2;
  y = (screen_height - height) / 2;

  XMoveResizeWindow(disp, calwin, x, y, width, height);
  XMapRaised(disp, calwin);
  XFlush(disp);
  XMoveResizeWindow(disp, calwin, x, y, width, height);
  XFlush(disp);

  configureCalibrationWin(x, y, width, height);
  cout << "*** Starting Calibration" << endl;

}

/* ==================================================== ======== ======= */

void Calibration::configureCalibrationWin(int calwin_x, int calwin_y,
					  int _width, int _height) {
  Display* disp = ums.getDisplay();
  // stores the offset (= the position of the calwin on the screen)
  Window childw;
  XTranslateCoordinates(disp, calwin,
                        RootWindowOfScreen((Screen*)ums.xscreen),
                        0, 0, &x, &y, &childw);
  width  = _width;
  height = _height;

  xp[0] = CROSS_XYPOS;
  yp[0] = CROSS_XYPOS;
  xp[1] = width  - CROSS_XYPOS;
  yp[1] = CROSS_XYPOS;
  xp[2] = width  - CROSS_XYPOS;
  yp[2] = height - CROSS_XYPOS;
  xp[3] = CROSS_XYPOS;
  yp[3] = height - CROSS_XYPOS;

  msg_x = 100; msg_y = height/2 - 50;
  drawCalibrationPoint();
}

/* ==================================================== ======== ======= */

void Calibration::drawCalibrationPoint() {
  Display* disp = ums.getDisplay();

  XClearWindow(disp, calwin);

  if (curpoint < POINT_COUNT) {   // draw a cross
    XDrawLine(disp, calwin, calgc,
	      xp[curpoint]-CROSS_SIZE/2, yp[curpoint],
	      xp[curpoint]+CROSS_SIZE/2, yp[curpoint]);
    XDrawLine(disp, calwin, calgc,
	      xp[curpoint], yp[curpoint]-CROSS_SIZE/2,
	      xp[curpoint], yp[curpoint]+CROSS_SIZE/2);
  }
  
  char step[10];
  const char* msg;
  sprintf(step,"Step %d:",curpoint+1);
  XDrawString(disp, calwin, calgc, msg_x, msg_y, step, strlen(step));

  XGCValues gcval;
  gcval.foreground = red_pixel;
  XChangeGC(disp, calgc, GCForeground, &gcval);
    
  if (curpoint < POINT_COUNT) 
    msg = "Press a MIMIO pen on the CENTER of the cross";
  else
    msg = "Press a MIMIO pen ANYWHERE on the window to complete the calibration";
  XDrawString(disp, calwin, calgc, msg_x, msg_y+30, msg, strlen(msg));
  
  gcval.foreground = black_pixel;
  XChangeGC(disp, calgc, GCForeground, &gcval);

  if (curpoint == 0)
    msg = "Note: the calibration window can be moved or resized BEFORE setting the 1st point";
  else 
    msg = "Note: the previous point can be reset by clicking with the MIMIO eraser";
  XDrawString(disp, calwin, calgc, msg_x, msg_y+60, msg, strlen(msg));
  XFlush(disp);
}

/* ==================================================== ======== ======= */

void Calibration::setCalibrationPoint(double _x, double _y) {
  Display* disp = ums.getDisplay();

  if (curpoint < POINT_COUNT) {
    xcm[curpoint] = _x;
    ycm[curpoint] = _y;
    curpoint++;
    drawCalibrationPoint();
  }
  else {
    is_calibrating = false;
    is_completed = true;
    XUnmapWindow(disp, calwin);
    XFlush(disp);
    cout << " - Calibration completed" << endl<< endl;
  }
}

void Calibration::unsetLastCalibrationPoint() {
  if (curpoint > 0) {
    curpoint--;
    xcm[curpoint] = 0;
    ycm[curpoint] = 0;
  }
  drawCalibrationPoint();
}

/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */


