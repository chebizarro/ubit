/* ***********************************************************************
 *
 *  piemenus.hpp (part of udemo)
 *  A simple example that illustrates how to use Pie menus.
 *  Ubit Demos - Version 6
 *  (C) 2009 Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
 *
 * ***********************************************************************
 * COPYRIGHT NOTICE :
 * THIS PROGRAM IS DISTRIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE
 * IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION;
 * EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 * SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
 * ************************************************************************/

#ifndef _piemenu_demo_hpp_
#define _piemenu_demo_hpp_  1
#include <ubit/ubit.hpp>
using namespace ubit;

class PiemenuDemo: public UBox {
public:
  PiemenuDemo();
private:
  UButton shape, color, thickness;
  UBar toolbar;
  UPiemenu main_menu;
  
  void initToolbar();
  void initMenus();
  
  void setShape(const char* com);
  void setColor(UColor*);
  void setThickness(int);
  void doFunction(const char* com);
};

#endif


