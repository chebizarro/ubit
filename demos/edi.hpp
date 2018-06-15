/*************************************************************************
 *
 *  edi.hpp: a simple text editor
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

#ifndef _edi_hpp_
#define _edi_hpp_  1
#include <ubit/ubit.hpp>
using namespace ubit;

class Edi : public UBox {
public:
  Edi();
  void openFile(const UStr& path);
  void openCloneOnDisplay(const UStr& display_name);
  void quit();

private:
  friend struct ZoomAction;
  UBox mainbox;
  UStr path;            // complete filename of the page
  UFilebox openbox;     // file box for selecting the page
  UStr text;            // edited text
  UScale scale;         // text scale
  UFont font;           // text font
  UTextarea textarea;   // widget that contains the text
  UScrollpane spane;    // scrollpane that contains the textarea widget
  USpinbox spinbox;     // controls the text scale
  UCtlmenu cmenu;       // contextual menu
  UDialog* fontDialog();
  void openFileCB(UStr* fpath);
  void initCMenu();
};

#endif

