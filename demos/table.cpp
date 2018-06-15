/* ***********************************************************************
 *
 *  table.cpp
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
//#include <cmath>
#include "viewer.hpp"
using namespace std;
using namespace ubit;


class PhotoTable : public U3Dcanvas {
public:
  static const int WIDTH  = 700;
  static const int HEIGHT = 600;
  static const int VIEWER_WIDTH  = 600;
  static const int VIEWER_HEIGHT = 200;

  PhotoTable();
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int main(int argc, char* argv[]) {
  UAppli::conf.setDepthBuffer(16);
  UAppli appli(argc, argv);
  
  PhotoTable table;
  UFrame frame(table);
   
  appli.add(frame);
  frame.show();
  return appli.start();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

PhotoTable::PhotoTable() {
  setFovy(35);
  setNear(200);
  setFar(1000);
  
  addAttr(usize(WIDTH, HEIGHT));

  Fisheye& v = *new Fisheye(usize(VIEWER_WIDTH, VIEWER_HEIGHT));
  
  U3Dwin& w1 = *new U3Dwin(v);
  add(w1);
  w1.pos().translate(-WIDTH/2 + 50, -25, -850);
  w1.pos().rotate(0, 0, 0);

  U3Dwin& w2 = *new U3Dwin(v);
  add(w2);
  w2.pos().translate(WIDTH/2 - 50, +25, -850);
  w2.pos().rotate(-0, 0, 180);
}




