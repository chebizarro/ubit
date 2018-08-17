/*
 *  keyGLUT.cpp: key symbols for GLUT
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2008 | Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
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

#include <ubit/ubit_features.h>
#if UBIT_WITH_GLUT

#include <ubit/ugl.hpp>
#include <ubit/ukey.hpp>

namespace ubit {

const int 
  Modifier::LeftButton  = 1<<8, 
  Modifier::MidButton   = 1<<9, 
  Modifier::RightButton = 1<<10,
  Modifier::ShiftDown   = GLUT_ACTIVE_SHIFT, // 1<<0
  Modifier::ControlDown = GLUT_ACTIVE_CTRL,  // 1<<1
  &Modifier::MetaDown   = 1<<3,              // NOT defined by GLUT
  &Modifier::AltDown    = GLUT_ACTIVE_ALT,   // 1<<2
  Modifier::AltGraphDown= 0x2000;            // ????

  /*
const int
  Modifier::MButton1 = Modifier::LeftButton, 
  Modifier::MButton2 = Modifier::MidButton, 
  Modifier::MButton3 = Modifier::RightButton ,
  Modifier::ShiftButton =   Modifier::ShiftDown,
  Modifier::ControlButton = Modifier::ControlDown;
*/
  
const int 
  Key::BackSpace = 127,
  Key::Tab    = 9,
  Key::Clear  = 0,
  Key::Enter  = 13,  // comme F13 ?
  Key::Pause  = 0,
  Key::ScrollLock = 0,
  Key::Escape = 27,
  Key::Delete = 8,
  Key::Print  = 0,
  Key::Insert = GLUT_KEY_INSERT,
  Key::Undo   = 0,
  Key::Again  = 0,
  Key::Menu   = 0,
  Key::Find   = 0,
  Key::Cancel = 0,
  Key::Help   = 0,
  
  Key::NumLock  = 0,
  Key::CapsLock = 0,
  Key::Shift    = 0,
  Key::Control  = 0,
  Key::Meta     = 0,
  Key::Alt      = 0,

  Key::Home = GLUT_KEY_HOME,
  Key::End  = GLUT_KEY_END,
  Key::Left = GLUT_KEY_LEFT,
  Key::Up   = GLUT_KEY_UP,
  Key::Right= GLUT_KEY_RIGHT,
  Key::Down = GLUT_KEY_DOWN,
  Key::PageUp   = GLUT_KEY_PAGE_UP,
  Key::PageDown = GLUT_KEY_PAGE_DOWN,
   
  // function keys
  Key::F1 = GLUT_KEY_F1,
  Key::F2 = GLUT_KEY_F2,
  Key::F3 = GLUT_KEY_F3,
  Key::F4 = GLUT_KEY_F4,
  Key::F5 = GLUT_KEY_F5,
  Key::F6 = GLUT_KEY_F6,
  Key::F7 = GLUT_KEY_F7,
  Key::F8 = GLUT_KEY_F8,
  Key::F9 = GLUT_KEY_F9,
  Key::F10 = GLUT_KEY_F10,
  Key::F11 = GLUT_KEY_F11,
  Key::F12 = GLUT_KEY_F12,
  Key::F13 = 13,  // comme Enter ?
  Key::F14 = 14,
  Key::F15 = 15,
  Key::F16 = 16,
  Key::F17 = 17,
  Key::F18 = 18,
  Key::F19 = 19,
  Key::F20 = 20,
  Key::F21 = 21,
  Key::F22 = 22,
  Key::F23 = 23,
  Key::F24 = 24;

void Modifier::mapKeys(Display* nd) { }
void Key::mapKeys(Display* nd) { }

}
#endif

