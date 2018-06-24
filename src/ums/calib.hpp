/*************************************************************************
 *
 *  calib.hpp - UMS Server
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

#ifndef _umscalib_hpp_
#define	_umscalib_hpp_
#include <X11/Xlib.h>

/**settings of the calibration window
* (used for absolute positioning devices such as the MIMIO)
*/
struct Calibration {
  enum {POINT_COUNT = 4};
  static const int CROSS_XYPOS;
  static const int CROSS_SIZE;
  static const float SCREEN_SIZE_PERCENT;

  Calibration(class UMServer*);

  bool isCalibrationCompleted() const;
  void startCalibration(const char* title);
  void configureCalibrationWin(int x, int y, int w, int h);
  void setCalibrationPoint(double x, double y);
  void unsetLastCalibrationPoint();
  void drawCalibrationPoint();

  class UMServer& ums;
  WindowID calwin;
  bool is_calibrating, is_completed;
  GC calgc;
  int curpoint;               ///< completed when == POINT_COUNT
  int x, y, width, height;    ///< coords of the calibration window (= its pos. on the screen)
  int xp[POINT_COUNT], yp[POINT_COUNT];   ///< positions of calpoints in pixels on the screen
  double xcm[POINT_COUNT], ycm[POINT_COUNT];   ///< position of calpoints in source coords
  int msg_x, msg_y;
  unsigned long black_pixel, red_pixel;
};

#endif

