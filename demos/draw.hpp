/*************************************************************************
 *
 *  draw.hpp
 *  Ubit Demos - Version 6
 *  (C) 2009 Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
 *
 **************************************************************************
 * COPYRIGHT NOTICE :
 * THIS PROGRAM IS DISTRIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE
 * IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION;
 * EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 * SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
 **************************************************************************/

#ifndef _draw_hpp_
#define _draw_hpp_  1
#include <ubit/ubit.hpp>

class DrawDemo: public ubit::UBox {
public:
  DrawDemo(ubit::UArgs args = ubit::UArgs::none);
private:
  class DrawCanvas* canvas;
  ubit::uptr<ubit::UButton> undo_btn, redo_btn, clear_btn, del_btn;
};

#endif

