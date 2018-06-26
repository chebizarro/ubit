/* ***********************************************************************
 *
 *  gltriangle.hpp: shows how to draw with OpenGL in a standard Ubit widget
 *  (see also glcanvas.cpp that shows how to draw in a UGLCanvas)
 *  Ubit Demos - Version 6
 *  (C) 2009 Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
 *
 *  NOTE: this example is based on the first lesson of the NeHe's OpenGL 
 *  tutorial available at: http://nehe.gamedev.net/lesson.asp?index=01
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

#ifndef _gltriangle_hpp_
#define _gltriangle_hpp_  1
#include <ubit/ubit.hpp>
using namespace ubit;

// NOTE that this demo requires OpenGL
class GLTriangle : public UBox {
public:
  GLTriangle(UArgs = UArgs::none);
  
private:
  UFloat xrot, yrot, zrot;
  UBox canvas;
  
  void paintCanvasCB(UPaintEvent&);  
  
  // 'model' is the slider model (see USlider constructor).
  static UBox& createSliderBox(const char* label, UFloat& model);  
  static UBox& createSliderMenu(UFloat& model);
};

#endif

