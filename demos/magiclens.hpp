/* ***********************************************************************
 *
 *  magiclens.hpp : magic lens demo
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
 * ***********************************************************************/

#ifndef _magiclens_hpp_
#define _magiclens_hpp_  1
#include <ubit/ubit.hpp>
using namespace ubit;

struct MapElem {
  const char *name;
  const char *title;
  const char *details, *image;
  int x, y;
};

class MagicLensDemo : public UBox { 
public:
  MagicLensDemo();
  UElem& makeElems(MapElem tab[], const UFlag& cond, const UArgs&);
  void addLens(int lenstype);
  
  UBox scene, scene_container;
  UScale scene_scale;
  UScrollpane scene_scroll;
  UBox controls;
};

#endif
