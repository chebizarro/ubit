/*
 * ukeyGLFW.cpp: key symbols for GLFW
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

#include <ubit/ubit_features.h>
#if UBIT_WITH_GLFW

#include <ubit/ugl.hpp>
#include <ubit/ukey.hpp>
namespace ubit {

const int 
  UModifier::LeftButton  = 1<<8, 
  UModifier::MidButton   = 1<<9, 
  UModifier::RightButton = 1<<10,
  UModifier::ShiftDown   = GLFW_ACTIVE_SHIFT, // 1<<0
  UModifier::ControlDown = GLFW_ACTIVE_CTRL,  // 1<<1
  &UModifier::MetaDown   = 1<<3,              // NOT defined by GLFW
  &UModifier::AltDown    = GLFW_ACTIVE_ALT,   // 1<<2
  UModifier::AltGraphDown= 0x2000;            // ????

  /*
const int
  UModifier::MButton1 = UModifier::LeftButton, 
  UModifier::MButton2 = UModifier::MidButton, 
  UModifier::MButton3 = UModifier::RightButton ,
  UModifier::ShiftButton =   UModifier::ShiftDown,
  UModifier::ControlButton = UModifier::ControlDown;
*/
  
const int 
  UKey::BackSpace = 127,
  UKey::Tab    = 9,
  UKey::Clear  = 0,
  UKey::Enter  = 13,  // comme F13 ?
  UKey::Pause  = 0,
  UKey::ScrollLock = 0,
  UKey::Escape = 27,
  UKey::Delete = 8,
  UKey::Print  = 0,
  UKey::Insert = GLFW_KEY_INSERT,
  UKey::Undo   = 0,
  UKey::Again  = 0,
  UKey::Menu   = 0,
  UKey::Find   = 0,
  UKey::Cancel = 0,
  UKey::Help   = 0,
  
  UKey::NumLock  = 0,
  UKey::CapsLock = 0,
  UKey::Shift    = 0,
  UKey::Control  = 0,
  UKey::Meta     = 0,
  UKey::Alt      = 0,

  UKey::Home = GLFW_KEY_HOME,
  UKey::End  = GLFW_KEY_END,
  UKey::Left = GLFW_KEY_LEFT,
  UKey::Up   = GLFW_KEY_UP,
  UKey::Right= GLFW_KEY_RIGHT,
  UKey::Down = GLFW_KEY_DOWN,
  UKey::PageUp   = GLFW_KEY_PAGE_UP,
  UKey::PageDown = GLFW_KEY_PAGE_DOWN,
   
  // function keys
  UKey::F1 = GLFW_KEY_F1,
  UKey::F2 = GLFW_KEY_F2,
  UKey::F3 = GLFW_KEY_F3,
  UKey::F4 = GLFW_KEY_F4,
  UKey::F5 = GLFW_KEY_F5,
  UKey::F6 = GLFW_KEY_F6,
  UKey::F7 = GLFW_KEY_F7,
  UKey::F8 = GLFW_KEY_F8,
  UKey::F9 = GLFW_KEY_F9,
  UKey::F10 = GLFW_KEY_F10,
  UKey::F11 = GLFW_KEY_F11,
  UKey::F12 = GLFW_KEY_F12,
  UKey::F13 = 13,  // comme Enter ?
  UKey::F14 = 14,
  UKey::F15 = 15,
  UKey::F16 = 16,
  UKey::F17 = 17,
  UKey::F18 = 18,
  UKey::F19 = 19,
  UKey::F20 = 20,
  UKey::F21 = 21,
  UKey::F22 = 22,
  UKey::F23 = 23,
  UKey::F24 = 24;

void UModifier::mapKeys(UDisp* nd) { }
void UKey::mapKeys(UDisp* nd) { }

}
#endif
/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */

