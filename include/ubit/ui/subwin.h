/*
 *  subwin.hpp
 *  Ubit GUI Toolkit - Version 8
 *  (C) 2018 Chris Daley
 *  (C) 2009 | Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
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
 * 
 */

#ifndef _usubwin_hpp_
#define	_usubwin_hpp_ 1

#include <ubit/uwin.hpp>

namespace ubit {
  
  /** Subwindow: a hard window that is embedded inside another window.
   * This class should not be used in most applications. It creates a non-toplevel 
   * system window (typically a X11 "child window") that is embbeded into another window.
   * It can be useful in some specific cases (for instance for drawing GL graphics:
   * see GLCanvas) but should be avoided in the general case because it breaks
   * certain graphical properties of the toolkit, such as the possibility to have
   * transparent windows and menus (they must then be created as "hard windows",
   * see Window, otherwise they won't appear if they are on top of a Subwindow)
   *
   * @see also subclass: GLCanvas.
   */
  class USubwin : public Window {
  public:
    UCLASS(USubwin)
    
    USubwin(Args = Args::none);
    
    virtual bool isSubWin() const {return true;}
    
    static Style* createStyle(); // redefined
    virtual int getDisplayType() const {return BLOCK;} // redefined
    
    // - impl - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    virtual bool realize();
    
  protected:
    virtual void initView(View* parent_view);
    // must be redefined to init parent view properly.
    
    virtual void motionImpl(UViewEvent&);
    
    virtual void resizeImpl(UResizeEvent&);
    // redefined by GLCanvas.
  };
  
}
#endif
