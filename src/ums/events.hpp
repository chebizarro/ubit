/*************************************************************************
 *
 *  events.hpp - UMS Server
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

#ifndef _events_hpp_
#define	_events_hpp_
#include "umserver.hpp"

struct Events {
  UMServer& ums;
  struct MacEvents* macev;
  static bool has_xtest;

  static int Bmasks[];
  static unsigned int Bmask_Count;

  static int Mmasks[];
  static unsigned int Mmask_Count;

  static unsigned int xbuttonToMask(int xbtn_no);
  static unsigned int mbuttonToMask(int xbtn_no);
  static unsigned int maskToXButton(unsigned int btn_mask);
  static unsigned int keySymToModMask(unsigned long mod_keysym);
  static unsigned long modMaskToKeySym(unsigned int mod_mask);

  // ------------------------------------------------------------

  Events(UMServer*);
  ~Events();

  bool init();
  void leaveLocalDisplay();
  void enterLocalDisplay();

  void getNativePointer(long& x, long& y);
  void warpNativePointer(long x, long y);
  ///< get/set the location of the native pointer.
  
  void getPointer(const MouseFlow&, long& x, long& y);
  void warpPointer(MouseFlow&, long x, long y);
  ///< get/set the location of the pointer of this flow.

  bool getPointer(const MouseFlow&, Pos&, bool check_props = false);
  ///< get the location of the pointer of this flow and the pointed window.
    
  // ------------------------------------------------------------

  void sendKey(MouseFlow&, unsigned long keysym, bool press, Pos* use_pos = 0);
  ///< pos : envoyer event a cette position directement sans gerer le
  /// MouseFlow ni afficher le ptr (utilise par sendMousePress(), etc).
  
  void sendButton(MouseFlow&, int btn_id, bool press, Pos* use_pos = 0);
  
  void sendMotion(MouseFlow& outflow, long x, long y, bool move_ptr);
  ///< move sthe pointer if 'move_pointer' is true.

  void sendMessage(USocket*, const char* message);
  
  // ------------------------------------------------------------
  void getXPointer(long& x, long& y);
  bool getXPointer(Pos&);
  void warpXPointer(long x, long y);
  
  void sendXKey(MouseFlow&, unsigned long keysym, bool press, Pos* use_pos);
  void sendXButton(MouseFlow&, int btn_id, bool press, Pos* use_pos);
  void sendXMotion(MouseFlow&, long x, long y, bool move_ptr);
  void sendXMessage(WindowID, const char* message);
};

#endif
/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */


