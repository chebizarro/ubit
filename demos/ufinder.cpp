/* ***********************************************************************
 *
 *  ufinder.cpp : a demos that shows how to use the UFinder class
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

#include <iostream>
#include <ubit/ubit.hpp>
#include <ubit/ufinder.hpp>
using namespace std;
using namespace ubit;


void openClones(UAppli& appli, UFinder& finder, int argc, char** argv);


int main(int argc, char* argv[]) {
  // glabal configuration options, if any, must be set before creating the UAppli:
  
  // non standard default background
  // UAppli::conf.setDefaultBackground(UBackground::velin);
  
  // transparent scrollbars
  UAppli::conf.setTransparentScrollbars(true);
 
  // - - - - - - - -
  
  // creates the application context
  UAppli appli(argc, argv);
  
  // creates the finder (UFinder is a class of the Ubit library)
  UFinder finder;
  
  // creates the main frame window, which contains a UFinder instance
  UFrame frame(usize(600, 450)  // default size of the widget
               + uhflex()       // what follows is horizontally resizable
               + uvflex()       // what follows is vertically resizable
               + finder    
               + ubottom()      // what follows is NOT vertically resizable
               + ulabel("")     // blank line at the bottom of the main frame
               );
  
  // see below
  openClones(appli, finder, argc, argv);

  //finder.browseHosts();

  // makes the main frame to appear
  frame.show();
    
  // starts the event loop
  return appli.start();
}

// - - - - - - - -

// this function open clones of the finder on alternate displays 
// (display names must be given as arguments of the command line)
// NOTE: this will only work if Ubit was compiled with X11 as a windowing toolkit

void openClones(UAppli& appli, UFinder& finder, int argc, char** argv) {
  for (int k = 1; k < argc; k++) {
    UDisp* disp = appli.openDisp(argv[k]);
    if (disp) {
      UStr t = ustr("Browser on ") & argv[k];
      UFrame& clone = uframe(usize(600, 450) + utitle(t) + finder);
      disp->add(clone);
      clone.show();
    }
  }
}
