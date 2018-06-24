/* ==================================================== ======== ======= *
*
*  serial_mouse.hpp : serial mouse driver (Logitec 3 btns) for the UMS.
*  Ubit Project
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

#ifndef _serial_mouse_hpp
#define	_serial_mouse_hpp
#include "../source.hpp"
#include "../flow.hpp"

/* ==================================================== ======== ======= */
/** Serial Mouse event source.
 */
class MouseSource : public EventSource {
public:
  MouseSource(class MouseFlow*, const char* dev_name);
  virtual ~MouseSource();

  virtual int open(const char* dev_name);  
  ///< returns filedesc ; can be used to reopen the port

  virtual void close();
  virtual bool read();

private:
  class MouseFlow& mflow;
  bool is_init;                // mouse reader initialized
  bool is_b1_pressed, is_b2_pressed, is_b3_pressed;
};

#endif
/* ==================================================== [TheEnd] ======= */
