/* **********************************************************************
*
*  uima.hpp: a simple image viewer
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

#ifndef _ima_hpp_
#define _ima_hpp_  1
#include <ubit/ubit.hpp>
using namespace ubit;

class Ima : public UCardbox {
public:
  Ima();
  
protected:
  UBackground background;
  UColor     foreground;
  UFont      font;
  UFilebox   openbox;
  UScrollpane scrollpane;
  UIma       image;
  UStr       current_file;
  UScale     imascale;
  UStr       scale_str;
  UAlertbox  alertbox;
  UData *File, *Tools, *Settings;
  
  // Callback METHODS (can have 0 to 3 arguments) :
  
  // loads the file designed by 'fname'
  void openFile(const UStr* fname);
  
  // loads the file designed by the 'filebox'
  void openFile();
  
  // zoom/dezoom when + - = are pressed
  void keyPressCB(UKeyEvent&);
  
  void customizeScrollButtons();
};

#endif

