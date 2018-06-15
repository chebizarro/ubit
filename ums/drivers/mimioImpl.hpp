/* ==================================================== ======== ======= *
 *
 *  mimioImpl.hh : MIMIO(TM) driver for the UMS.
 *  Ubit Project
 *  file adapted from code by from R. Diaz & C. Pillias
 *
 *  Part of the Ubit Toolkit: A Brick Construction Game Model for Creating GUIs
 *  (C) 2003-2005 Eric Lecolinet / ENST Paris / http://www.enst.fr/~elc/ubit
 *
 * ***********************************************************************
 * COPYRIGHT NOTICE : 
 * THIS PROGRAM IS DISTRIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE 
 * IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. 
 * YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER THE TERMS OF THE GNU 
 * GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION; 
 * EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 * SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
 * ***********************************************************************
 * ==================================================== [Elc:05] ======= *
 * ==================================================== ======== ======= */

#ifndef _mimioImpl_hpp_
#define _mimioImpl_hpp_

/** Coordinate converter.
 */
struct CoordConverter {

  CoordConverter();

  /** inits the converter from the known positions of 4 points in the
   *  two units (pixels and centimeters). 
   */
  void setCoordinates(const Calibration&);

  /** Converts pixels to cm */
  void pixels2cm(int xp, int yp, double &xcm, double &ycm) const;

  /** Converts cm to pixels */
  void cm2pixels(double xcm, double ycm, double &xp, double &yp) const;
  void cm2pixels(double xcm, double ycm, double &xp, double &yp, double &rp) const;

  // constants used to convert positions.
  double A, B, C, D, E, F, G, H;
};

/* ==================================================== ======== ======= */

struct StateList {
  const unsigned int size;
  const unsigned int states[10];
};

class MIMIOFrame : public CoordConverter {
  friend class MimioSource;

public:
  MIMIOFrame(MimioSource*, MouseFlow*);
  virtual ~MIMIOFrame() {}
  
  void reset(int fd);
  bool read();

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

  bool prologue();
  void pressPen();
  void releasePen();
  void holdPen();
  void pressCtrlBtn();

  /// reads a frame (returns false if error).
  bool getFrame(int fd, const StateList&);

  /// writes a byte on the serial port. Return non-0 value if error.
  virtual int write(unsigned char);

  bool setcts(int on);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

private:
  static const unsigned int BYTE = 1<<8;
  static const unsigned int SHORT= 1<<9;
  static const unsigned int XOR  = 1<<10;

  static const StateList Idle, Temp, PressPen, ReleasePen, 
    HoldPen15, HoldPen18, PressCtrlBtn, Hello, Mimio;

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

  MimioSource& msource;
  MouseFlow& mflow;
  bool calibrated;
  int fd;
  int last_x, last_y;

  // the data read in the frame.
  unsigned short shorts[2];
  unsigned char  bytes[4];

  struct Geo {
    static const int SMOOTHING = 4;

    void reset(unsigned short r_up, unsigned short r_low);

    bool getPos(unsigned short r_up, unsigned short r_low, 
		double& xpos, double& ypos);
    /**< calculate the x an y position of an object.
     * pos is in cm, from the upper microphone, with an y axis from top 
     * to bottom
     */

    // previous positions
    unsigned int r_ups[SMOOTHING+1], r_lows[SMOOTHING+1];

    // previous speeds
    int d_ups[SMOOTHING+1], d_lows[SMOOTHING+1];

    // previous accelerations
    int d2_ups[SMOOTHING+1], d2_lows[SMOOTHING+1];
    double av_d_ups[SMOOTHING+1], av_d_lows[SMOOTHING+1];
    double av_d2_ups[SMOOTHING+1], av_d2_lows[SMOOTHING+1];

    // number of elements in the previous arrays
    int smooth;
    int e_up, e_low;
  } geo, gsav;
};

/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */
#endif
