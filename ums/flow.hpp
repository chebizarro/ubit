/*************************************************************************
 *
 *  umsflow.hpp: UMS Server: UMS Event flows.
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2003-2008 Eric Lecolinet / ENST Paris / www.enst.fr/~elc/ubit
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

#ifndef _flow_hpp_
#define	_flow_hpp_
#include "umserver.hpp"

/* ==================================================== ======== ======= */
/** UMS event flow.
 */
class EventFlow {
public:
  EventFlow(class UMServer*, int flow_id);
  virtual ~EventFlow();

  int getID() const {return id;}
  virtual WindowID getPointer() const {return 0;}

  class UMServer& ums;
  class Events& events;
  const int id;
};

/* ==================================================== ======== ======= */
/** UMS mouse event flow.
 */
class MouseFlow : public EventFlow {
public:
  MouseFlow(class UMServer*, int flow_id, bool create_pointer);
  virtual ~MouseFlow();

  long getX() const {return mx;}
  long getY() const {return my;}
  bool isPressed(int btn_id) const {return (btn_mask & btn_id) != 0;}
  ///< btn_mask is a combination of Button1Mask, etc and can be ORed.
  
  WindowID getPointer() const {return pointer_win;}
  void movePointer(long x, long y);
  void showPointer(bool);  
  void createPointer();
  void changePointer(const char* fgcolor, const char* bgcolor);
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

  void pressMouse(int btn_id);
  void releaseMouse(int btn_id);
  void pressMouse(const class UMSbutton*);
  void releaseMouse(const class UMSbutton*);
  ///< btn_mask is ONE of Button1Mask, etc and must NOT be ORed.

  void pressKey(unsigned long keysym);
  void releaseKey(unsigned long keysym);

  void releaseAll();
  ///< releases all mouses and keys (when leaving a remote display)
  
  void moveMouse(long x, long y, bool absolute_coords);
  ///< x and y are screen coordinates if 'absolute_coords' is true
  ///< and relative translations from the previous position otherwise. 

  bool moveMouseOutsideDisplay(long x, long y);
  ///< returns false if not outside or no connection.

  void syncMouse(long x, long y);
  ///< synchronizes this flow with the actual pos of the mouse (and send events).

  bool isOutside(long x, long y, long& n_x, long& n_y, class UMService*&);
  ///< returns true if mouse is outside the home screen.

  void keepMouseInsideScreen(long& mx, long& my);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

  friend class UMServer;
  long mx, my; 	            ///< absolute pos of the mouse
  int btn_mask;             ///< current button mask
  int mod_mask;             ///< current modifier mask
  WindowID pointer_win;
  WindowID last_entered_win;
  bool last_entered_in_ubitwin;
  int  last_entered_winsock;
  bool h_out, v_out;
};

#endif
/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */
