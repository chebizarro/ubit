/* ==================================================== ======== ======= *
*
*  ukeyGDK.cpp : key symbols
*  Ubit 6.0
*  Author: Eric Lecolinet
*  Part of the Ubit Toolkit: A Brick Construction Game Model for Creating GUIs
*  (C) 1999-2007 Eric Lecolinet / ENST Paris
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
* ==================================================== [(c)Elc] ======= *
* ==================================================== ======== ======= */

#include <ubit/ubit_features.h>
#if UBIT_WITH_GDK

#include <X11/keysym.h>  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#include <ubit/nat/udispX11.hpp>
#include <ubit/ukey.hpp>
namespace ubit {

static int _MetaDown = GDK_META_MASK, _AltDown = GDK_ALT_MASK; //_ModeSwitch = 0;

 const int 
  UModifier::LeftButton  = GDK_BUTTON1_MASK, 
  UModifier::MidButton   = GDK_BUTTON2_MASK, 
  UModifier::RightButton = GDK_BUTTON3_MASK,
  UModifier::ShiftDown   = GDK_SHIFT_MASK,
  UModifier::ControlDown = GDK_CONTROL_MASK,
  &UModifier::MetaDown   = _MetaDown,
  &UModifier::AltDown    = _AltDown,
  UModifier::AltGraphDown= 0x2000;   // ????
   
const int
  UModifier::MButton1 = UModifier::LeftButton, 
  UModifier::MButton2 = UModifier::MidButton, 
  UModifier::MButton3 = UModifier::RightButton ,
  UModifier::ShiftButton =   UModifier::ShiftDown,
  UModifier::ControlButton = UModifier::ControlDown;

const int 
  UKey::BackSpace = XK_BackSpace,
  UKey::Tab = XK_Tab,
  UKey::Clear = XK_Clear,
  UKey::Enter = XK_Return,         // ATT: XK_Linefeed, XP_KP_Enter !!!!
  UKey::Pause = XK_Pause,
  UKey::ScrollLock = XK_Scroll_Lock,
  UKey::Escape = XK_Escape,
  UKey::Delete = XK_Delete,
  UKey::Print = XK_Print,
  UKey::Insert = XK_Insert,
  UKey::Undo = XK_Undo,
  UKey::Again = XK_Redo,
  UKey::Menu = XK_Menu,
  UKey::Find = XK_Find,
  UKey::Cancel = XK_Cancel,
  UKey::Help = XK_Help,
  // XK_Sys_Req, XK_Select, XK_Execute, XK_Begin, XK_Break
  
  UKey::NumLock = XK_Num_Lock,
  UKey::CapsLock = XK_Caps_Lock,
  UKey::Shift = XK_Shift_L,               // XK_Shift_R !!!!!!!!!!
  UKey::Control = XK_Control_L,           // XK_Control_R !!!!!!!!!!
  // XK_Shift_Lock,
  UKey::Meta = XK_Meta_L,                 // XK_Meta_R !!!!!!!!!!
  UKey::Alt = XK_Alt_L,                   // XK_Alt_R !!!!!!!!!!!
  //UKey::ModeSwitch = XK_Mode_switch,  // touche Alt avec Mac, ModeChange en Java ???
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
  UKey::Home = XK_Home,
  UKey::End = XK_End,
  UKey::Left = XK_Left,
  UKey::Up = XK_Up,
  UKey::Right = XK_Right,
  UKey::Down = XK_Down,
  UKey::PageUp = XK_Page_Up,
  UKey::PageDown = XK_Page_Down,
  // XK_Prior, XK_Next
   
  // function keys
  UKey::F1 = XK_F1,
  UKey::F2 = XK_F2,
  UKey::F3 = XK_F3,
  UKey::F4 = XK_F4,
  UKey::F5 = XK_F5,
  UKey::F6 = XK_F6,
  UKey::F7 = XK_F7,
  UKey::F8 = XK_F8,
  UKey::F9 = XK_F9,
  UKey::F10 = XK_F10,
  UKey::F11 = XK_F11,
  UKey::F12 = XK_F12,
  UKey::F13 = XK_F13,
  UKey::F14 = XK_F14,
  UKey::F15 = XK_F15,
  UKey::F16 = XK_F16,
  UKey::F17 = XK_F17,
  UKey::F18 = XK_F18,
  UKey::F19 = XK_F19,
  UKey::F20 = XK_F20,
  UKey::F21 = XK_F21,
  UKey::F22 = XK_F22,
  UKey::F23 = XK_F23,
  UKey::F24 = XK_F24;

void UModifier::mapKeys(UNatDisp* nd) { }
void UKey::mapKeys(UNatDisp* nd) { }

}
#endif
/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */

