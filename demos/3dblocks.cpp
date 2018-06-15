/*************************************************************************
 *
 *  moveblocks.cpp
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
#include <ubit/ubit.hpp>
using namespace std;
using namespace ubit;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class GLdemo : public UBox {
public:
  GLdemo(UArgs = UArgs::none);
  
private:
  U3Dcanvas canvas;
  UFloat xtrans, ytrans, ztrans, xrot, yrot, zrot;
  uptr<U3Dbox> obj1, obj2, obj3;
  
  void setTransCB();
  void setRotCB();
    
  // NB: val is the slider model (see USlider constructor).
  UBox& createSliderBox(const char* name, UFloat& val) {
    return uhbox(uleft() + ulabel(UFont::bold + name) + uhflex() + uslider(val));
  }
  
  UBox& createSliderMenu(UFloat& val) {
    return umenu(UBackground::none 
                 + uslider(val, UOrient::vertical + UBackground::black + ualpha(.2))
                 ).setSoftWin();        // the menu must be a softwin to be transparent
  }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int main(int argc, char* argv[]) 
{
  // Application options (must be specified *before* the instanciation of UAppli)
  UAppli::conf.useGL();             // this program requires OpenGL rendering
  //UAppli::conf.setDepthBuffer(16);  // this program requires a depth buffer
  
  UAppli appli(argc, argv);
  
  if (!appli.isUsingGL()) {         // OpenGL is not available
    appli.error("main", "this program requires OpenGL");
    return 1;
  }
  
  GLdemo mainbox;
  UFrame frame(usize(650, 550) + mainbox);
  frame.show();
  return appli.start();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

GLdemo::GLdemo(UArgs args) : UBox(args) 
{
  // main box  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
  UMenubar& menubar = umenubar
  (ubutton("XT" + createSliderMenu(xtrans))
   + ubutton("YT" + createSliderMenu(ytrans))
   + ubutton("ZT" + createSliderMenu(ztrans))
   + ubutton("XR" + createSliderMenu(xrot))
   + ubutton("YR" + createSliderMenu(yrot))
   + ubutton("ZR" + createSliderMenu(zrot))
   );

  UBox& controls = uvbox
  (uvmargin(5) + uhmargin(5)
   + ubox(UOrient::horizontal
          + uvbox(UBorder::etchedIn + "Translations"
                  + createSliderBox(" X ", xtrans)
                  + createSliderBox(" Y ", ytrans)
                  + createSliderBox(" Z ", ztrans)
                  )
          + uvbox(UBorder::etchedIn + "Rotations"
                  + createSliderBox(" X ", xrot)
                  + createSliderBox(" Y ", yrot)
                  + createSliderBox(" Z ", zrot)
                  )
           )
   );
  
  add(UOrient::vertical
      + utop() + menubar 
      + uvflex() + canvas
      + ubottom() + controls
      );
  
  // canvas  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  // the canvas has a black background
  canvas.addAttr(UBackground::black);

  xtrans.onChange( ucall(this, &GLdemo::setTransCB) );
  ytrans.onChange( ucall(this, &GLdemo::setTransCB) );
  ztrans.onChange( ucall(this, &GLdemo::setTransCB) );
  
  xrot.onChange( ucall(this, &GLdemo::setRotCB) );
  yrot.onChange( ucall(this, &GLdemo::setRotCB) );
  zrot.onChange( ucall(this, &GLdemo::setRotCB) );
  
  /* penible!
  // this popup menu will be opened when the mouse is pressed in the canvas
  UPopmenu& menu = upopmenu(UBackground::black + ualpha(0.25) + UBorder::none 
                            + UColor::white
                            + createSliderBox(" X ", xrot)
                            + createSliderBox(" Y ", yrot)
                            + createSliderBox(" Z ", zrot)
                            );
  // this menu...
  menu.setSoftWin();  // is a 'soft window' to allow transparency
  canvas.add(menu + UOn::mpress / ucall(&menu, &UMenu::open)); // is opened on a mouse press
  */
  
  canvas.setFovy(60);
  canvas.setNear(100);
  canvas.setFar(1000.);
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // objects
  
  // U3Dbox or U3Dwin
  
  obj1 = u3dbox(usize(400, 50) + ubox(UBackground::wheat));
  obj2 = u3dbox(usize(400, 50) + ubox(UBackground::orange));
  obj3 = u3dbox(usize(400, 50) + ubox(UBackground::green));
  canvas.add(*obj1 + obj2 + obj3);

  // initial value is 0, range is [0,100]
  xrot = 0.;
  yrot = 0.;
  zrot = 0.;

  // initial value is 50, range is [0,100]
  xtrans = 50.;
  ytrans = 50.;
  ztrans = 50.;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void GLdemo::setTransCB() {
  static const float AX = 10., BX = -200;
  static const float AY = 8., BY1 = -100, BY2 = 0, BY3 = 100;
  static const float AZ = 8., BZ = -600;
  
  // xtrans, ytrans, ztrans are in range [0,100] 
  obj1->setTrans( (xtrans-50)*AX+BX, (ytrans-50)*AY+BY1, (ztrans-50)*AZ+BZ);
  obj2->setTrans( (xtrans-50)*AX+BX, (ytrans-50)*AY+BY2, (ztrans-50)*AZ+BZ);
  obj3->setTrans( (xtrans-50)*AX+BX, (ytrans-50)*AY+BY3, (ztrans-50)*AZ+BZ);
}

void GLdemo::setRotCB() {
  // xrot, yrot, zrot are in range [0,100] 
  obj1->setRot(xrot*3.60, yrot*3.60, zrot*3.60);
  obj2->setRot(xrot*3.60, yrot*3.60, zrot*3.60);
  obj3->setRot(xrot*3.60, yrot*3.60, zrot*3.60);
}

