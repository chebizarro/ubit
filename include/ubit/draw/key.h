/*
 *  key.hpp : key symbols
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


#ifndef _ukey_hpp_
#define _ukey_hpp_ 1

#include <ubit/udefs.hpp>

namespace ubit {
  
  /** Modifier masks.
   * modifier masks are returned by UInputEvent::getModifiers(), UMouseEvent::getButton(),
   * UMouseEvent::getButtons().
   * @see also class UKey for key codes.
   */
  class UModifier {
  public:
    static const int
    LeftButton, RightButton, MidButton,
    ShiftDown, ControlDown, &MetaDown, &AltDown, AltGraphDown;
    
    static void mapKeys(Display*);
    ///< impl: init keys mapping.
  };
  
  /** Key codes.
   * key codes are returned by UKeyEvent::getKeyCode()
   * Note: do not confuse UKey::Shift (key code) with UMod::ShiftMod (modifier mask)
   * @see also class UMod for modifier mask.
   */      
  class UKey {
  public:    
    static const int
    BackSpace, Tab, Clear, Enter, Pause, ScrollLock, Escape, Delete,
    Undo, Again, Print, Insert, Menu, Find, Cancel, Help,
    NumLock, CapsLock, Shift, Control, Meta, Alt,
    Home, End, Left, Up, Right, Down, PageUp, PageDown,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, 
    F18, F19, F20, F21, F22, F23, F24;
    
    static void mapKeys(Display*);
    ///< impl: init keys mapping.
  };
  
}
#endif

