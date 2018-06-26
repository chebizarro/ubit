/*************************************************************************
 *
 *  3Dworkspace.cpp: shows how to display Ubit widgets in perspective
 *  Ubit Demos - Version 6
 *  (C) 2009 Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
 *
 * ************************************************************************
 * COPYRIGHT NOTICE :
 * THIS PROGRAM IS DISTRIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE
 * IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION;
 * EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 * SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
 * ************************************************************************/

#include <iostream>
#include <cmath>
#include <ubit/ubit.hpp>
#include <ubit/uxpm.hpp>
#include <ubit/ufinder.hpp>
#include "draw.hpp"
#include "zoom.hpp"
using namespace std;
using namespace ubit;

#ifndef UBIT_WITH_GL

int main(int argc, char **argv) {
  cerr << "This demo requires OpenGL" << endl;
}

#else

class Workspace3D : public UBox {
public:
  Workspace3D(UArgs args = UArgs::none);
private:
  U3Dcanvas canvas;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int main(int argc, char* argv[]) {
  UAppli::conf.useGL();             // this program requires OpenGL rendering
  UAppli::conf.setDepthBuffer(16);  // this program requires a depth buffer
  UAppli appli(argc, argv);
  
  if (!appli.isUsingGL()) {         // OpenGL is not available
    appli.error("main", "this program requires OpenGL");
    return 1;
  }
  
  Workspace3D workspace;
  UFrame frame(usize(750, 650) + workspace);
   
  frame.show();
  return appli.start();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Workspace3D::Workspace3D(UArgs args) {

  UMenu& file_menu = umenu
  (//ubutton(UPix::ofolder  + " Open")
   //+ ubutton(UPix::diskette + " Save")
   //+ usepar()
   ubutton(UPix::stop     + " Quit" + ucall(0, &UAppli::quit))
   );
  
  UMenubar& menubar = umenubar
  (ubutton(UPix::ofolder + "File "+ file_menu)
   + ubutton(upix(UXpm::windows) + "Windows ")
   );
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  U3Dwin& w1 = *new U3Dwin(/*"Zoom",*/usize(500, 350) + new ZoomDemo());
  U3Dwin& w2 = *new U3Dwin(/*"Finder",*/ usize(600, 350) + new UFinder());
  U3Dwin& w3 = *new U3Dwin(/*"Drawing",*/ usize(400, 400) + new DrawDemo());
  //U3Dwin& w4 = *new U3Dwin(/*"Triangle",*/ usize(400, 400) + new GLTriangle());

  canvas.setFovy(70);
  canvas.setNear(100);
  canvas.setFar(1000.);
    
  w1.pos().translate(100, 550, -800);
  w1.pos().rotate(0, -15, -20);
  
  w2.pos().translate(-300, -20, -700);
  w2.pos().rotate(15, 0, 0);
  
  w3.pos().translate(-550, 450, -750);
  w3.pos().rotate(0, 20, 0);
  
  canvas.add(w1 + w2 + w3);

  add(UOrient::vertical + uhflex()
      + utop() + menubar 
      + uvflex() + canvas);
}

#endif // UBIT_WITH_GL



