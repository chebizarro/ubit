/*
 *  ukeyGDK.cpp : key symbols
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
#include <ubit/ubit_features.h>
#if UBIT_WITH_GDK

#include <X11/keysym.h>  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#include <ubit/nat/udispX11.hpp>
#include <ubit/draw/key.h>

namespace ubit {

static int _MetaDown = GDK_META_MASK, _AltDown = GDK_ALT_MASK; //_ModeSwitch = 0;

 const int 
  Modifier::LeftButton  = GDK_BUTTON1_MASK, 
  Modifier::MidButton   = GDK_BUTTON2_MASK, 
  Modifier::RightButton = GDK_BUTTON3_MASK,
  Modifier::ShiftDown   = GDK_SHIFT_MASK,
  Modifier::ControlDown = GDK_CONTROL_MASK,
  &Modifier::MetaDown   = _MetaDown,
  &Modifier::AltDown    = _AltDown,
  Modifier::AltGraphDown= 0x2000;   // ????
   
const int
  Modifier::MButton1 = Modifier::LeftButton, 
  Modifier::MButton2 = Modifier::MidButton, 
  Modifier::MButton3 = Modifier::RightButton ,
  Modifier::ShiftButton =   Modifier::ShiftDown,
  Modifier::ControlButton = Modifier::ControlDown;

const int 
  Key::BackSpace = XK_BackSpace,
  Key::Tab = XK_Tab,
  Key::Clear = XK_Clear,
  Key::Enter = XK_Return,         // ATT: XK_Linefeed, XP_KP_Enter !!!!
  Key::Pause = XK_Pause,
  Key::ScrollLock = XK_Scroll_Lock,
  Key::Escape = XK_Escape,
  Key::Delete = XK_Delete,
  Key::Print = XK_Print,
  Key::Insert = XK_Insert,
  Key::Undo = XK_Undo,
  Key::Again = XK_Redo,
  Key::Menu = XK_Menu,
  Key::Find = XK_Find,
  Key::Cancel = XK_Cancel,
  Key::Help = XK_Help,
  // XK_Sys_Req, XK_Select, XK_Execute, XK_Begin, XK_Break
  
  Key::NumLock = XK_Num_Lock,
  Key::CapsLock = XK_Caps_Lock,
  Key::Shift = XK_Shift_L,               // XK_Shift_R !!!!!!!!!!
  Key::Control = XK_Control_L,           // XK_Control_R !!!!!!!!!!
  // XK_Shift_Lock,
  Key::Meta = XK_Meta_L,                 // XK_Meta_R !!!!!!!!!!
  Key::Alt = XK_Alt_L,                   // XK_Alt_R !!!!!!!!!!!
  //Key::ModeSwitch = XK_Mode_switch,  // touche Alt avec Mac, ModeChange en Java ???
  /*
   XK_Super_L, XK_Super_R, XK_Hyper_L, XK_Hyper_R
   XK_script_switch
   Multi_key = XK_Multi_key,	 
   Codeinput = XK_Codeinput,	 
   SingleCandidate	= XK_SingleCandidate,	 
   MultipleCandidate = XK_MultipleCandidate,	 
   PreviousCandidate = XK_PreviousCandidate;
   */
   
  // cursor
  Key::Home = XK_Home,
  Key::End = XK_End,
  Key::Left = XK_Left,
  Key::Up = XK_Up,
  Key::Right = XK_Right,
  Key::Down = XK_Down,
  Key::PageUp = XK_Page_Up,
  Key::PageDown = XK_Page_Down,
  // XK_Prior, XK_Next
   
  // function keys
  Key::F1 = XK_F1,
  Key::F2 = XK_F2,
  Key::F3 = XK_F3,
  Key::F4 = XK_F4,
  Key::F5 = XK_F5,
  Key::F6 = XK_F6,
  Key::F7 = XK_F7,
  Key::F8 = XK_F8,
  Key::F9 = XK_F9,
  Key::F10 = XK_F10,
  Key::F11 = XK_F11,
  Key::F12 = XK_F12,
  Key::F13 = XK_F13,
  Key::F14 = XK_F14,
  Key::F15 = XK_F15,
  Key::F16 = XK_F16,
  Key::F17 = XK_F17,
  Key::F18 = XK_F18,
  Key::F19 = XK_F19,
  Key::F20 = XK_F20,
  Key::F21 = XK_F21,
  Key::F22 = XK_F22,
  Key::F23 = XK_F23,
  Key::F24 = XK_F24;

void Modifier::mapKeys(UNatDisp* nd) { }
void Key::mapKeys(UNatDisp* nd) { }

}
#endif

