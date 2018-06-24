/* ==================================================== ======== ======= *
 *
 *  mimio.cpp : Unix driver for the MIMIO(TM)
 *  Ubit Project
 *  Author: Eric Lecolinet (adapted from code by from R. Diaz & C. Pillias)
 *  Modified by: Stuart Pook.
 *  Part of the Ubit Toolkit: A Brick Construction Game Model for Creating GUIs
 *  (C) 2003-2005 EricLecolinet / ENST Paris / http://www.enst.fr/~elc/ubit
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
//pragma ident	"@(#)mimio.cpp	ubit:04.03.01"

#include <iostream>
#include <cmath>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/termios.h>
#include <sys/ioctl.h>
#include <sys/timeb.h>
#include <X11/Xlib.h>
#include "mimio.hpp"
#include "mimioImpl.hpp"
#include "../calib.hpp"
using namespace std;

static const char eio_message[] = " (perhaps the Mimio's cable is too long)";
static const double MIMIOSIZE = 53.2;  // for getPos()
static const double FACTOR = 144.75;
static const double OFFSET = 580.0;
static const int THRESHOLD = 15; // for holdPen(): ATT depend resolution ecran!

const StateList MIMIOFrame::Idle = { 
  7, { 0x16, 0xCA, 0x6B, 0xD7, 0x63, 0x1, 0x2 } 
};

const StateList MIMIOFrame::Temp = { 
  3, { 0x32, MIMIOFrame::BYTE, MIMIOFrame::BYTE } 
};

const StateList MIMIOFrame::PressPen =  { 
  7, { 0x28, MIMIOFrame::BYTE, MIMIOFrame::SHORT, MIMIOFrame::SHORT,
       MIMIOFrame::BYTE, MIMIOFrame::BYTE, MIMIOFrame::XOR } 
};

const StateList MIMIOFrame::ReleasePen = { 
  3, { 0x12, 0x55, 0x47 } 
};

const StateList MIMIOFrame::HoldPen15 =  { 
  4, { 0x15, MIMIOFrame::SHORT, MIMIOFrame::SHORT, MIMIOFrame::XOR } 
};

const StateList MIMIOFrame::HoldPen18 = { 
  7, { 0x18, MIMIOFrame::BYTE, MIMIOFrame::SHORT, MIMIOFrame::SHORT,
       MIMIOFrame::BYTE, MIMIOFrame::BYTE, MIMIOFrame::XOR } 
};

const StateList MIMIOFrame::PressCtrlBtn = { 
  3, {0x22, MIMIOFrame::BYTE, MIMIOFrame::XOR } 
};

const StateList MIMIOFrame::Hello = { 
  3, { 0x52, 0xCC, 0x9E } 
};

const StateList MIMIOFrame::Mimio = { 
  6, { 0x25, MIMIOFrame::BYTE, MIMIOFrame::BYTE, MIMIOFrame::BYTE,
       MIMIOFrame::BYTE, MIMIOFrame::XOR } 
};

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */

MIMIOFrame::MIMIOFrame(MimioSource* _msource, MouseFlow* _mflow) : 
  msource(*_msource),
  mflow(*_mflow) {
  reset(-1);
}

void MIMIOFrame::reset(int _fd) {
  calibrated = false;
  fd = _fd;
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */
// set CTS on or off (adapted from code by Roberto Diaz)

bool MIMIOFrame::setcts(int on) {
  int bits = TIOCM_CTS;
  if (ioctl(fd, (on) ? TIOCMBIS : TIOCMBIC, &bits) < 0) {
    cerr << "MIMIOFrame::setcts: could not ioctl the Mimio: " << strerror(errno)
    << ((errno == EIO) ? eio_message : "") << endl;
    exit(1);
  }
  return true;
}

// =====================================================================

int MIMIOFrame::write(unsigned char c) {
   setcts(1);
   int rst = ::write(fd, &c, 1);
   if (rst != 1) {
     cerr << "MIMIOFrame::write: write to the Mimio failed: "
     << rst << " " << strerror(errno)
     << ((errno == EIO) ? eio_message : "") << endl;
     exit(1);
   }

   setcts(0);
   if (tcdrain(fd) < 0) {
     cerr << "MIMIOFrame::write: could not tcdrain the Mimio: " << strerror(errno)
     << ((errno == EIO) ? eio_message : "") << endl;
     exit(1);
   }
   
  return rst;
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */

bool MIMIOFrame::getFrame(int fd, const StateList& slist) {
  const unsigned int* states = slist.states;
  unsigned char xor_val = states[0]; // XORed bits of the bytes read
  int nb_shorts = 0;                 // number of SHORTS read
  int nb_bytes = 0;                  // number of BYTES read 
  bool ok = true;

  for (u_int pos = 1; pos < slist.size; pos++) {

    u_char c;
    ::read(fd, &c, 1);

    xor_val ^= c;

    switch (states[pos]) {
    case BYTE:
      bytes[nb_bytes] = c;
      nb_bytes++;
      break;

    case SHORT:
      shorts[nb_shorts] = c<<8;
      ::read(fd, &c, 1);
      xor_val ^= c;
      shorts[nb_shorts] += c;
      nb_shorts++;
      break;

    case XOR:
      if (xor_val != 0)	ok = false;
      break;

    default: /* normal char */
      if (c != (unsigned char) states[pos]) ok = false;
    }
  }

  //if (!ok) cerr << "MIMIO: invalid frame" << endl;
  return ok;
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */

bool MIMIOFrame::prologue() {
  bool idle_received = false, temp_received = false;

  cout << "Please wait"; cout.flush();

  while (!idle_received || !temp_received) {
    unsigned char c = 0;
    ::read(fd, &c, 1);

    if (c != MIMIOFrame::Idle.states[0]) {
      getFrame(fd, MIMIOFrame::Idle);
      cout << "..."; cout.flush();
      idle_received = true;
    }
    else if (c != MIMIOFrame::Temp.states[0]) {
      getFrame(fd, MIMIOFrame::Temp);
      cout << "..."; cout.flush();
      temp_received = true;
    }
    else {
      //fprintf(stderr, "received %x \n", c);
      cout << "..."; cout.flush();
    }
  }

  // sends the init sequence to the mimio.
  cout << "MIMIO: sending init sequence..." << endl;
  write(0x52);
  write(0x25);
  write(0x10);
  return true;
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */
/* Reception and interpretation of mimio's frames.
   This function returns only if the mimio is disconnected.
 */

bool MIMIOFrame::read() {
  unsigned char c = 0;
  if (::read(fd, &c, 1) < 0) return false;

  switch(c) {
  case 0x16:
    getFrame(fd, MIMIOFrame::Idle);
    write(0x10);
    //cerr << "Idle ";
    break;

  case 0x32:
    getFrame(fd, MIMIOFrame::Temp);
    write(0x10);
    //cerr << "Temp("<< bytes[0] << bytes[1] << ") ";
    break;

  case 0x15:
    // MIMIO sends four 0x15 followed by one 0x18 when a pen is being pressed
    getFrame(fd, MIMIOFrame::HoldPen15);
    //cerr << "d(" << int(bytes[0]) << ") "<< shorts[0] <<" "<< shorts[1] <<endl; 
    holdPen();
    break;
        
  case 0x18:
    // MIMIO sends four 0x15 followed by one 0x18 when a pen is being pressed
    getFrame(fd, MIMIOFrame::HoldPen18);
    geo.e_up  += 15;
    geo.e_low += 15;
    //cerr << "D(" << int(bytes[0]) << ") " << shorts[0] <<" "<< shorts[1]
    //	 << " / " << int(bytes[1]) << " "<< int(bytes[2]) << endl;
    holdPen();
    if (bytes[1] == 0) write(0x10);
    break;

  case 0x28:
    getFrame(fd, MIMIOFrame::PressPen);
    pressPen();
    break;

  case 0x12:
    getFrame(fd, MIMIOFrame::ReleasePen);
    releasePen();
    break;
    
  case 0x22:
    getFrame(fd, MIMIOFrame::PressCtrlBtn);
    pressCtrlBtn();
    break;
    
  case 0x52:
    getFrame(fd, MIMIOFrame::Hello);
    cout << "Hello world..." << endl;
    break;

  case 0x25:
    getFrame(fd, MIMIOFrame::Mimio);
    cout << "MIMIO is ready" << endl;
    break;
    
    //default: fprintf(stderr, "MIMIO frame%x \n", c); break;
  }

  return true;
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */

void MIMIOFrame::pressPen() {
  // supprimer le bit 128 (le bit de "signe"), parfois a 1 parfois a 0
  // suivant les stylos
  u_int pen_id = (bytes[0] & 0x7f);

  geo.reset(shorts[0], shorts[1]);

  double pen_x = 0, pen_y = 0;
  if (geo.getPos(shorts[0], shorts[1], pen_x, pen_y)) {
    if (!calibrated) {
      Calibration* calib = mflow.ums.getCalibration();
      if (!calib) {
        clog << "UMServer::pressPen: Error: no calibration Window!" << endl;
        return;
      }

      if (pen_id >= 1 && pen_id <= 4)  // stylos
	calib->setCalibrationPoint(pen_x, pen_y);
      else
	calib->unsetLastCalibrationPoint();  //erasers et btns de controle

      if (calib->isCalibrationCompleted()) {
        setCoordinates(*calib);
	calibrated = true;
      }
    }
    else {
      double new_x, new_y;
      cm2pixels(pen_x, pen_y, new_x, new_y);
      int x = int(new_x);
      int y = int(new_y);
      //cerr << "pen " << pen_id << " pressed at " << x << " " << y << endl;
      last_x = x; last_y = y;
      mflow.moveMouse(x, y, true);           // true => absolute pos
      mflow.pressMouse(msource.getButton(pen_id));
    }
  }
}

// =====================================================================

void MIMIOFrame::releasePen() {
  // supprimer le bit 128 (le bit de "signe"), parfois a 1 parfois a 0
  // suivant les stylos
  u_int pen_id = (bytes[0] & 0x7f);
  //cerr << "pen " << pen_id << " released" << endl;
  if (calibrated) {
    mflow.releaseMouse(msource.getButton(pen_id));
  }
}

// =====================================================================

void MIMIOFrame::holdPen() {
  double pen_x = 0, pen_y = 0;
  /*
  bis.r_ups = r_ups;
  bis.r_lows = r_lows;
  bis.d_ups = d_ups;
  bis.d_lows = d_lows;
  bis.d2_ups = d2_ups;
  bis.d2_lows = d2_lows;
  bis.av_d_ups = av_d_ups;
  bis.av_d_lows = av_d_lows;
  bis.av_d2_ups = av_d2_ups;
  bis.av_d2_lows = av_d2_lows;
  bis.smooth = smooth;
  bis.e_up = e_up;
  bis.e_low = e_low;
  */
  gsav = geo;

  if (calibrated && geo.getPos(shorts[0], shorts[1], pen_x, pen_y)) {
    double new_x, new_y;
    cm2pixels(pen_x, pen_y, new_x, new_y);
    int x = int(new_x);
    int y = int(new_y);
    //cerr << "holding " << pen_id << " at pos: " << x << " " << y << endl;

    if (abs(x - last_x) > THRESHOLD || abs(y - last_y) > THRESHOLD) {
      //cerr << "** INVALID **;
      /*
      r_ups = bis.r_ups;
      r_lows = bis.r_lows;
      d_ups = bis.d_ups;
      d_lows = bis.d_lows;
      d2_ups = bis.d2_ups;
      d2_lows = bis.d2_lows;
      av_d_ups = bis.av_d_ups;
      av_d_lows = bis.av_d_lows;
      av_d2_ups = bis.av_d2_ups;
      av_d2_lows = bis.av_d2_lows;
      smooth = bis.smooth;
      e_up = bis.e_up;
      e_low = bis.e_low;
      */
      geo = gsav;
      return;
    }

    last_x = x; last_y = y;
    mflow.moveMouse(x, y, true);   // true => absolute pos
  }
}

// =====================================================================
// a control button has been pressed

void MIMIOFrame::pressCtrlBtn() {
  u_int ctrl_id = (bytes[0] & 0x7f);
  //cerr << "ctrl btn " << ctrl_id << " pressed" << endl;
  mflow.pressMouse(msource.getButton(ctrl_id+10));
  mflow.releaseMouse(msource.getButton(ctrl_id+10));
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */

void MIMIOFrame::Geo::reset(u_short r_up, u_short r_low) {
  r_ups[1]  = r_up;
  r_lows[1] = r_low;

  for (int i=0; i<=SMOOTHING; i++) {
    av_d_ups[i] = av_d_lows[i] = av_d2_ups[i] = av_d2_lows[i] = 0;
    d_ups[i] = d_lows[i] = 0;
  }
  e_up = e_low = smooth = 0;
}

// =====================================================================

bool MIMIOFrame::Geo::getPos(u_short r_up, u_short r_low, 
			     double& xpos, double& ypos) {

  if (r_up==0 || r_low==0 || r_up>=36000 || r_low>=36000) return false;

  for(int i=smooth; i>0; i--) {
      r_ups[i] = r_ups[i-1];  r_lows[i] = r_lows[i-1];
      d_ups[i] = d_ups[i-1];  d_lows[i] = d_lows[i-1];
      d2_ups[i]= d2_ups[i-1]; d2_lows[i]= d2_lows[i-1];
      av_d_ups[i] = av_d_ups[i-1];  av_d_lows[i] = av_d_lows[i-1];
      av_d2_ups[i]= av_d2_ups[i-1]; av_d2_lows[i]= av_d2_lows[i-1];
  }

  e_up -= 3;
  e_low-= 3;

  int loop = 2;
  while (loop>0) {

    int loop_inc = 0;

    int d_up = r_up - e_up - r_ups[1];
    int d_low= r_low - e_low- r_lows[1];
    int d2_up = d_up - d_ups[1];
    int d2_low= d_low- d_lows[1];

    if (smooth < SMOOTHING) {
      av_d_ups[0] = (av_d_ups[1] *smooth + (double) d_up) / (double) (smooth+1);
      av_d_lows[0]= (av_d_lows[1]*smooth + (double) d_low)/ (double) (smooth+1);
      av_d2_ups[0] =(av_d2_ups[1] *smooth + (double) d2_up) / (double) (smooth+1);
      av_d2_lows[0]=(av_d2_lows[1]*smooth + (double) d2_low)/ (double) (smooth+1);
      smooth++;
    } 
    else {
      av_d_ups[0] = 
	av_d_ups[1] +((double) (d_up - d_ups [SMOOTHING])) / (double) SMOOTHING;
      av_d_lows[0] = 
	av_d_lows[1]+((double) (d_low- d_lows[SMOOTHING])) / (double) SMOOTHING;
      av_d2_ups[0] =
	av_d2_ups[1] +((double) (d2_up -d2_ups [SMOOTHING])) / (double) SMOOTHING;
      av_d2_lows[0] = 
	av_d2_lows[1]+((double) (d2_low-d2_lows[SMOOTHING])) / (double) SMOOTHING;
    }

    r_ups[0] = r_up - e_up;  r_lows[0] = r_low - e_low;
    d_ups[0] = d_up;  d_lows[0] = d_low;
    d2_ups[0]= d2_up; d2_lows[0]= d2_low;

    loop -= 2;

    if (smooth>=2) {

      // large variation of acceleration : bad point
      if (fabs((double)d2_up - d2_ups[1]) > 175+2*fabs(av_d2_ups[2])
	  && fabs((double)d2_ups[1]-d2_ups[2]) > 50) {

	int diff;

	/* if it is an isolated bad point */
	if (((double) -d2_up)/(double)d2_ups[1] > 1.5) {
	  //cerr << "BAD UP : isolated" << endl;
	  diff = r_ups[1] - (r_ups[2]+r_up-e_up) / 2;
	  //cerr<<"diff:"<<diff<<" moyd:"<<av_d_ups[2]<<endl;
	}
	else {
	  //cerr << "BAD UP : not isolated" << endl;
	  diff = d2_ups[1] - d2_ups[2];
	  e_up += diff;
	}
	av_d_ups[1] -= diff / (double) SMOOTHING;
	av_d2_ups[1] -= diff / (double) SMOOTHING;
	r_ups[1] -= diff;
	d_ups[1] -= diff;
	d2_ups[1] -= diff;
	loop_inc=1;

	//cerr<<" up:" <<r_ups[1] <<" d:"<<d_ups[1] <<" moy:"<<av_d2_ups[1] <<" d2:"<<d2_ups[1] <<endl;
	//cerr <<" low:"<<r_lows[1]<<" d:"<<d_lows[1]<<" moy:"<<av_d2_lows[1]<<" d2:"<<d2_lows[1]<<endl;
      }

      // large variation of acceleration : bad point
      if (fabs((double)d2_low - d2_lows[1]) > 175 + 2 * fabs(av_d2_lows[2])
	  && fabs((double)d2_lows[1] - d2_lows[2]) > 50) {

	int diff;

	/* if it is an isolated bad point */
	if (((double) -d2_low)/(double)d2_lows[1] > 1.5) {
	  //cerr << "BAD DOWN : isolated" << endl;
	  diff = r_lows[1] - (r_lows[2]+r_low-e_low) / 2;
	  //cerr<<"diff:"<<diff<<" moyd:"<<av_d_lows[2]<<endl;
	} 
	else {
	  //cerr << "BAD DOWN : not isolated" << endl;
	  diff = d2_lows[1]-d2_lows[2];
	  e_low += diff;
	}

	av_d2_lows[1] -= diff / (double) SMOOTHING;
	av_d_lows[1] -= diff / (double) SMOOTHING;
	r_lows[1] -= diff;
	d_lows[1] -= diff;
	d2_lows[1] -= diff;
	loop_inc=1;
	
	//cerr<<" up:" <<r_ups[1] <<" d:"<<d_ups[1] <<" moy:"<<av_d2_ups[1] <<" d2:"<<d2_ups[1] <<endl;
	//cerr<<" low:"<<r_lows[1]<<" d:"<<d_lows[1]<<" moy:"<<av_d2_lows[1]<<" d2:"<<d2_lows[1]<<endl;
      }
      
      loop += loop_inc;
    }
  }

  // calculates the (xpos,ypos) position
  int i = 1;
  if (smooth == 1) i = 0;

  double r_up_cm = ((double) r_ups[i] -OFFSET) / FACTOR;
  double r_low_cm= ((double) r_lows[i]-OFFSET) / FACTOR;
  
  ypos = 
    (r_up_cm*r_up_cm - r_low_cm*r_low_cm + MIMIOSIZE*MIMIOSIZE) / (2.0*MIMIOSIZE);
  double ab1 = r_up_cm*r_up_cm - ypos * ypos;
  double ab2 = r_low_cm*r_low_cm - (MIMIOSIZE - ypos) * (MIMIOSIZE - ypos);

  // theoricaly, this can't happend, but there is still imprecisions...
  if (ab1<0.0) ab1=0.0;
  if (ab2<0.0) ab2=0.0;
  xpos = (sqrt(ab1) + sqrt(ab2)) / 2.0;
  
  return true;
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */
// solves a system of linear equations.

template<int size>
void solves(double system[size][size], double res[size]) {

  // Turn the system into an upper triangular matrix
  for(unsigned int i=0; i<size; i++) {

    double temp = system[i][i];

    if (temp == 0.0) {
      
      unsigned int j;
      for(j=i+1; (j<size)&&(system[j][i]==0.0); j++);
      if (j==size) continue;
      
      for(unsigned int k=i; k<size; k++) {
	temp = system[i][k];
	system[i][k] = system[j][k];
	system[j][k] = temp;
      }
      temp = res[i];
      res[i] = res[j];
      res[j] = temp;
      
      temp = system[i][i];
    }

    for(unsigned int j=i; j<size; j++) system[i][j] /= temp;
    res[i] /= temp;
    
    for(unsigned int k=i+1; k<size; k++) {
      temp = system[k][i];
      for(unsigned int j=i; j<size; j++)
	system[k][j] -= temp*system[i][j];
      res[k] -= temp*res[i];
    }
  }

  // Turn the system into an identity matrix
  for (int i=size-1; i>=0; i--)
    for (int k=i-1; k>=0; k--) {
      res[k] -= system[k][i]*res[i];
      //system[k][i] = 0;
    }
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */

CoordConverter::CoordConverter() {}

void CoordConverter::setCoordinates(const Calibration& cal) {
  int x_pixels[4];
  int y_pixels[4];
  const double* x_cm  = cal.xcm;
  const double* y_cm  = cal.ycm;

  for (int k = 0; k < 4; k++) {
    x_pixels[k] = cal.xp[k] + cal.x;
    y_pixels[k] = cal.yp[k] + cal.y;
  }

  // calc the constants A, B, C, D, E, F, G and H.
  double system[8][8], res[8];
  for(int i=0; i<4; i++) {
    // (x+y) + A(x+y)X + B(x+y)Y = C + DX + EY + F + GX + HY
    system[2*i][6] = (x_cm[i]+y_cm[i])*x_pixels[i];
    system[2*i][7] = (x_cm[i]+y_cm[i])*y_pixels[i];
    system[2*i][0] = -1.0;
    system[2*i][1] = -x_pixels[i];
    system[2*i][2] = -y_pixels[i];
    system[2*i][3] = -1.0;
    system[2*i][4] = -x_pixels[i]; 
    system[2*i][5] = -y_pixels[i];
    res[2*i] = -(x_cm[i]+y_cm[i]);
    // (x-y) + A(x-y)X + B(x-y)Y = C + DX + EY - F - GX - HY
    system[1+2*i][6] = (x_cm[i]-y_cm[i])*x_pixels[i];
    system[1+2*i][7] = (x_cm[i]-y_cm[i])*y_pixels[i];
    system[1+2*i][0] = -1.0;
    system[1+2*i][1] = -x_pixels[i];
    system[1+2*i][2] = -y_pixels[i];
    system[1+2*i][3] = 1.0;
    system[1+2*i][4] = x_pixels[i];
    system[1+2*i][5] = y_pixels[i];
    res[1+2*i] = -(x_cm[i]-y_cm[i]);
  }

  solves(system, res);
  A=res[6]; B=res[7];
  C=res[0]; D=res[1]; E=res[2];
  F=res[3]; G=res[4]; H=res[5];
}

/* ==================================================== ======== ======= */

void CoordConverter::pixels2cm(int xp, int yp, 
			       double &xcm, double &ycm) const {
  double d = 1.0 + A*(double)xp + B*(double)yp;
  xcm = (C + D*(double)xp + E*(double)yp) / d;
  ycm = (F + G*(double)xp + H*(double)yp) / d;
}

void CoordConverter::cm2pixels(double xcm, double ycm,
			       double &xp, double &yp) const {
  double d = (B*ycm-H)*(A*xcm-D) - (B*xcm-E)*(A*ycm-G);
  xp = ((B*xcm-E)*(ycm-F) - (B*ycm-H)*(xcm-C)) / d;
  yp = ((A*ycm-G)*(xcm-C) - (A*xcm-D)*(ycm-F)) / d;
}

void CoordConverter::cm2pixels(double xcm, double ycm,
			       double &xres, double &yres,
                               double &rres) const {
  double d = (B*ycm-H)*(A*xcm-D) - (B*xcm-E)*(A*ycm-G);
  xres = ((B*xcm-E)*(ycm-F) - (B*ycm-H)*(xcm-C)) / d;
  yres = ((A*ycm-G)*(xcm-C) - (A*xcm-D)*(ycm-F)) / d;

  //double r = (B*(F+C)+H-E + A*(F-C)+D-G - (xres+yres)*(A*(H+E)+B*(G-D))) / (2.0*d);
  double dxx = (B*F+H - xres*(A*H+B*G));
  double dxy = (B*C-E - xres*(A*E-B*D));
  double dyx = (A*F-G - yres*(A*H+B*G));
  double dyy = (D-A*C - yres*(A*E-B*D));
  rres = (sqrt(dxx*dxx + dxy*dxy) + sqrt(dyx*dyx + dyy*dyy)) / (2.0*d);
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */

MimioSource::MimioSource(MouseFlow* _mflow, const char* port_name)
: frame(*new MIMIOFrame(this, _mflow)) {
  // default mapping of the 5 pens + 1 eraser
  // source_button, source_modifiers ==> flow_button, flow_modifiers

  // attention: il n'y a que 3 Buttons avec X ; 4 & 5 servent au scroll
  setButton(1, MouseLeftID, 0);	        // black pen
  setButton(2, MouseLeftID, ShiftMask);	// blue pen
  setButton(3, MouseRightID, 0); 	        // green pen
  setButton(4, MouseLeftID, ShiftMask|ControlMask); // red pen
  setButton(9, MouseMiddleID, ControlMask);	// big eraser
  setButton(10,MouseMiddleID, 0);		// small eraser

  // boutons de controle du MIMIO (values: 10 + (1, 2, 4, 8, 16))
  setButton(11, MouseRightID, 0);		  // b1 (top)
  setButton(12, MouseRightID, ControlMask);  // b2
  setButton(14, MouseRightID, ShiftMask);    // b3
  setButton(18, MouseRightID, Mod1Mask);	  // b4
  setButton(26, MouseRightID, Mod2Mask);	  // b5 (bottom)

  fd = -1;
  fd = open(port_name);
}

MimioSource::~MimioSource() {
  close();
}

// =====================================================================

void MimioSource::close() {
  if (fd != -1) {
    // tcsetattr(fd, TCSADRAIN,  &old_tty);
    if (::close(fd) < 0)
    	cerr << "could not close the serial port: " << strerror(errno) << endl;;
  }
  fd = -1; 
}

bool MimioSource::read() {
  return frame.read();
}

void MimioSource::calibrate() {
  frame.calibrated = false;
  Calibration* calib =  frame.mflow.ums.getCalibration();
  if (!calib) {
    clog << "UMServer::calibrate: Error: no calibration Window!" << endl;
    return;
  }
  calib->startCalibration("MIMIO Calibration");
}

// =====================================================================
// adapted from code by Roberto Diaz

int MimioSource::open(const char* port_name) {
  frame.reset(-1);

  if (!port_name) {
    cerr << "UMServer:MIMIO: can't open: null port name" << endl;
    return -1;
  }
  if (fd > 0) ::close(fd);

  cout << "UMServer:MIMIO: opening port: " << port_name << " ..." << endl;

  fd = ::open(port_name, O_RDWR | O_NOCTTY | O_NONBLOCK);
  if (fd < 0) {
    cerr << "UMServer:MIMIO: can't open port " << port_name << ": " << strerror(errno) << endl;
    return fd;
  }

  frame.reset(fd);  // !!
  cout << "UMServer:MIMIO: port successfully opened." << endl;

  // save the original settings 
  if (tcgetattr(fd, &old_tty) < 0) 
    cerr << "UMServer:MIMIO: can't save current ioctl settings" << endl;
  
  struct termios options = { 0 };

  /* sometimes the Linux 2.4.23 kernel and the Belkin USB serial adapter F5U103
   * disagree on the current baud rate, so change it twice.
   */
  const int fbaud = (BAUDRATE == B19200) ? B300 : B19200;
  if (cfsetispeed(&options, fbaud) < 0
      || cfsetospeed(&options, fbaud) < 0
      || tcsetattr(fd, TCSAFLUSH, &options) < 0)
  {
    cerr << "UMServer:MIMIO: could not ioctl " << port_name << ": " << strerror(errno) << endl;
    close();
    return -1;
  }

  options.c_cc[VMIN] = 1;
  /* CLOCAL was added for MacOS X and the Keyspan USA-19QW.
   * It needs to be tested on Solaris et Linux 
   */
  options.c_cflag = CREAD | CS8 | CLOCAL;
  if (cfsetispeed(&options, BAUDRATE) < 0
      || cfsetospeed(&options, BAUDRATE) < 0
      || tcsetattr(fd, TCSAFLUSH, &options) < 0
      ) {
    cerr << "UMServer:MIMIO: can't set MIMIO ioctl settings" << endl ;
    close();  
    return -1;
  }
  if (fcntl(fd, F_SETFL, 0) < 0) {
    cerr << "UMServer:MIMIO: could not switch O_NONBLOCK off" << endl;
    close();
    return -1;
  }

  // checks if the MIMIO is coonected and sends the init sequence
  cout << "UMServer:MIMIO: starting connection with the MIMIO..." << endl;
  
  if (!frame.prologue()) {
    cerr << "UMServer:MIMIO: can't open connection with the MIMIO"<< endl;
    close();
  }

  cout << "UMServer:MIMIO: starting calibration" << endl; 
  calibrate();
  return fd;
}

/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */
