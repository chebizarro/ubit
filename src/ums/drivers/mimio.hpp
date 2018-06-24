/* ==================================================== ======== ======= *
*
*  mimio.hpp : Unix driver for the MIMIO(TM).
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
//pragma ident	"@(#)mimio.hpp	ubit:04.03.01"

#ifndef _mimio_hh_
#define _mimio_hh_
#include "../source.hpp"
#include "../flow.hpp"
#include <termios.h>
#include <cstdio>

/* ==================================================== ======== ======= */
/** Mimio event source.
 */
class MimioSource : public EventSource {
public:
  MimioSource(class MouseFlow*, const char* port_name);
  virtual ~MimioSource();

  virtual int open(const char* port_name);  
  ///< returns filedesc ; can be used to reopen the port

  virtual void close();
  virtual bool read();

  virtual bool canCalibrate() const {return true;}
  virtual void calibrate();
  ///< calibrates (or re-calibrates) the MIMIO.

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

private:
  friend class MIMIOFrame;
  static const speed_t BAUDRATE = B19200;
  struct termios old_tty;  // old state of the serial port.
  class MIMIOFrame& frame;
};

#endif
/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */
