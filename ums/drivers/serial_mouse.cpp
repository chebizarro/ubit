/* ==================================================== ======== ======= *
 *
 *  serial_mouse.cpp : serial mouse driver (Logitec 3 btns) for the UMS.
 *  Ubit Project
 *  Author: Eric Lecolinet (adapted from code by Thierry Duval - IRISA Siames)
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

//pragma ident	"@(#)serial_mouse.cpp	ubit:04.03.01"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/termios.h>
#include <sys/ioctl.h>
#include <X11/Xlib.h>
#include "serial_mouse.hpp"
using namespace std;

/* ==================================================== ======== ======= */

MouseSource::MouseSource(MouseFlow* _mflow, const char* mouse_port_name)
: mflow(*_mflow) {
  // 3 Buttons avec X ; 4 & 5 servent au scroll
  setButton(1, MouseLeftID, 0);
  setButton(2, MouseMiddleID, 0);
  setButton(3, MouseRightID, 0);
  setButton(4, WheelUpID, 0);
  setButton(5, WheelDownID, 0);

  fd = -1;
  is_init = false;
  is_b1_pressed = is_b2_pressed = is_b3_pressed = false;
  if (mouse_port_name) fd = open(mouse_port_name);
}

MouseSource::~MouseSource() {
  close();
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */

void MouseSource::close() {
  if (fd != -1) ::close(fd);
  fd = -1; 
}

// returns the openend port or -1 if connection fails
int MouseSource::open(const char* mouse_port) {
  close();
  if (!mouse_port) return -1;

  fd = ::open(mouse_port, O_RDWR | O_NOCTTY | O_NONBLOCK);
  if (fd == -1) {
    //cerr << "Could not open mouse on device " << mouse_port << endl ;
    return fd;
  }

  struct termios options = { 0 };

  // input modes
  options.c_iflag = IGNBRK | IGNPAR;

  // output modes
  options.c_oflag = 0;

  // local modes
  options.c_lflag = 0;

  options.c_cflag = CS7 | CSTOPB | CREAD | CLOCAL;

  // un hack pour tenir compte d'un pbm specifique au Keyspan USA-19QW
  // (l'ancien modele) sinon on perd des events
  // noter que ca ne marche pas avec le nouveau modele (USA-19HS)
  if (strstr(mouse_port, "tty.USA19QW")) options.c_cflag |= PARENB;

  // control chars
  options.c_cc[VTIME] = 0;
  options.c_cc[VMIN] = 1;

  // speed
  if (cfsetispeed(&options, B1200) < 0
      || cfsetospeed(&options, B1200) < 0
      || tcsetattr(fd, TCSAFLUSH, &options) < 0
      ) {
    //cerr << "Could not set mouse ioctl settings " << endl ;
    close(); 
    return -1;
  }

  if (fcntl(fd, F_SETFL, 0) < 0) {
    cerr << "open: could not switch O_NONBLOCK off" << endl;
    close();
    return -1;
  }

  return fd;
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */

bool MouseSource::read() {
  unsigned char ctrl = 0;
  unsigned char data[10];  // bug coorige: char oublie!

  int ctrl_count = 0;
  do {
    if (ctrl_count > 0) {
      /*
	 unsigned char aux = ctrl;
	 cerr << "**aux: "
	      << (aux & 1<<7) << "."
	      << (aux & 1<<6) << "."
	      << (aux & 1<<5) << "."
	      << (aux & 1<<4) << "."
	      << (aux & 1<<3) << "."
	      << (aux & 1<<2) << "."
	      << (aux & 1<<1) << "."
	      << (aux & 1<<0) << endl << endl;
      */
      // initialise et pas un control
      // NB: en fait il faudrait decrypter le prologue, mais on triche
      // et on considere que c'est initialise des que la souris a bouge
      
      if (is_init && (ctrl & 64) == 0) {
	// 32 means B2 is pressed
	if (is_b2_pressed) {
	  if ((ctrl & 32) == 0)  {
	    is_b2_pressed = false;
	    mflow.releaseMouse(getButton(2));
	  }
	}
	else {
	  if ((ctrl & 32) != 0) {
	    is_b2_pressed = true;
	    mflow.pressMouse(getButton(2));
	  }
	}
      }
    }
    
    int n = ::read(fd, &ctrl, 1);
    if (n < 0) return false;
    else if (n > 0) {
      //cerr << "ctrl: "<< int(ctrl) << endl;
      ctrl_count++; 
    }
  } 
  while ((ctrl & 64) == 0);   // marque des chars de controle
  
  int data_count = 0;

  data[0] = 0;
  data[1] = 0;

  while (data_count < 2) {        // lire x et y (2 bytes)
    int n = ::read(fd, data + data_count, 1);
    //cerr << "data: "<< int(data[data_count]) << endl;
    if (n < 0) return false;
    data_count += n;
  }

  
  // le bit 128 est a 1 (le bit de "signe") : le supprimer
  int dx = (data[0] & 0x7f);
  
  // le bit 2 de ctrl est a 1 quand dx negatif
  if (ctrl & 2) {
    // le bit 1 est a 1 si pas de depassement
    if (ctrl & 1) dx = dx - 64;
    else {
      //cerr<< "DEPASSEMENT NEGATIF" << endl;
      dx = dx - 64 - 64;
    }
  }
  else {  // deplacment positif
    // le bit 1 est a 1 si depassement
    if (ctrl & 1) {
      //cerr<< "DEPASSEMENT POSITIF" << endl;
      dx = 64 + dx;
    }
  }
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  // le bit 128 est a 1 (le bit de "signe") : le supprimer
  int dy = (data[1] & 0x7f);
  
  // le bit 8 de ctrl est a 1 quand dy negatif
  if (ctrl & 8) {
    // le bit 4 est a 1 si pas de depassement
    if (ctrl & 4) dy = dy - 64;
    else dy = dy - 64 - 64;
  }
  else {  // deplacment positif
    // le bit 4 est a 1 si depassement
    if (ctrl & 4) dy = 64 + dy;
  }
  /*
  cerr << "ctrl: "   // << ctrl << " " 
	   << (ctrl & 1<<7) << "."
	   << (ctrl & 1<<6) << "."
	   << (ctrl & 1<<5) << "."
	   << (ctrl & 1<<4) << "."
	   << (ctrl & 1<<3) << "."
	   << (ctrl & 1<<2) << "."
	   << (ctrl & 1<<1) << "."
	   << (ctrl & 1<<0);
      unsigned char x = data[0];
      cerr << " / x: "  // << data[0] << ' '<< x << ' '
	   << (x & 1<<7) << "."
	   << (x & 1<<6) << "."
	   << (x & 1<<5) << "."
	   << (x & 1<<4) << "."
	   << (x & 1<<3) << "."
	   << (x & 1<<2) << "."
	   << (x & 1<<1) << "."
	   << (x & 1<<0);
      unsigned char y = data[1];
      cerr << " / y: " // << data[1] << ' '<< y << ' '
	   << (y & 1<<7) << "."
	   << (y & 1<<6) << "."
	   << (y & 1<<5) << "."
	   << (y & 1<<4) << "."
	   << (y & 1<<3) << "."
	   << (y & 1<<2) << "."
	   << (y & 1<<1) << "."
	   << (y & 1<<0);
      cerr << endl;
  */
      //tcflush (fd, TCIFLUSH); inutile

  // false => relative pos
  if (dx != 0 || dy != 0) mflow.moveMouse(dx, dy, false);

  // 32 means B1 is pressed
  if (is_b1_pressed) {
   if ((ctrl & 32) == 0) {
     is_b1_pressed = false;
     mflow.releaseMouse(getButton(1));
   }
  }
  else {
    if ((ctrl & 32) != 0) {
      is_b1_pressed = true;
      mflow.pressMouse(getButton(1));
    }
  }
    
  // 16 means B3 is pressed
  if (is_b3_pressed) {
    if ((ctrl & 16) == 0) {
     is_b3_pressed = false;
     mflow.releaseMouse(getButton(3));
    }
  }
  else {
    if ((ctrl & 16) != 0){
      is_b3_pressed = true;
      mflow.pressMouse(getButton(3));
    }
  }
    
  if (mflow.getX() > 0 || mflow.getY() > 0) is_init = true;

  return true;
}

/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */
