/*
 * UDispGLFW.hpp : Native Layer: platform dependent implementation
 * Ubit GUI Toolkit - Version 8.0
 * (C) Copyright 2018 Chris Daley <chebizarro@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#ifndef UBIT_UDISPGLFW_HPP_
#define	UBIT_UDISPGLFW_HPP_

#include <ubit/udisp.hpp>
#include <ubit/ugl.hpp>
#include <ubit/nat/uhardwinGLFW.hpp>
namespace ubit {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/** GLFW cursor.
  */
class UCursorImpl {
public:
  int cursor;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/** GLFW display.
*/
class UDispGLFW : public UDisp {
public:
  UDispGLFW(const UStr&);
  virtual ~UDispGLFW();
  
  virtual void   setPointerPos(const UPoint& screen_pos);
  virtual UPoint getPointerPos() const;
  virtual int    getPointerState() const;
  virtual bool   grabPointer(const UCursor*);
  virtual void   ungrabPointer();
  virtual bool   pickWindow(int& x_in_win, int& y_in_win, UHardwinImpl* window,
                            UCursor* cursor = null, UCall* callback = null);

  virtual unsigned long createColorPixel(const URgba&);  // not used with GLFW
  virtual UHardwinGLFW* createWinImpl(UWin*);
  virtual UCursorImpl*  createCursorImpl(int curtype);
  virtual void          deleteCursorImpl(UCursorImpl*);
   
  int getSysFrame() const;
  UWin* retrieveWin(int sys_win);
  void initWinCallbacks(int sys_win);

protected:
  virtual void startAppli();
  virtual void quitAppli();
  virtual void startLoop(bool main_loop);   
  virtual void quitLoop(bool main_loop); 
};

}
#endif // UBIT_UDISPGLFW_HPP_
