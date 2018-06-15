/* ***********************************************************************
 *
 *  glcanvas.cpp: shows how to draw with OpenGL in a UGlcanvas
 *  (see also gltriangle.cpp that shows how to draw in a standard Ubit widget)
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2008 Eric Lecolinet | ENST Paris | http://www.enst.fr/~elc/ubit
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

#include <iostream>
#include <ubit/ubit.hpp>
using namespace std;
using namespace ubit;

#ifndef UBIT_WITH_GL

int main(int argc, char **argv) {
  cerr << "This demo requires OpenGL" << endl;
}

#else

#include <ubit/ugl.hpp>
#include <ubit/uglcanvas.hpp>

class Canvas : public UGlcanvas {
public:
  Canvas(class GLdemo& _mainbox) : mainbox(_mainbox) {}
private:
  class GLdemo& mainbox;
  virtual void initGL();
  virtual void paintGL(UPaintEvent&);
  virtual void resizeGL(UResizeEvent&, int w, int h);
};


class GLdemo : public UBox {
public:
  GLdemo(UArgs = UArgs::none);
private:
  friend class Canvas;
  UFloat xrot, yrot, zrot;
  Canvas canvas;

  static UBox& createSliderBox(const char* name, UFloat& val) {
    return uhbox(uleft() + ulabel(UFont::bold + name) + uhflex() + uslider(val));
  }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int main(int argc, char **argv) {
  // Application options (must be specified *before* the instanciation of UAppli)
  UAppli::conf.useGL();               // this program requires OpenGL rendering
  //UAppli::conf.setDepthBuffer(16);  // this program requires a depth buffer

  UAppli appli(argc, argv);
  
  // requires OpenGL but does not need to be launched in OpenGL mode
  //if (!appli.isUsingGL()) {         // OpenGL is not available
  //  appli.error("main", "this program requires OpenGL");
  //  return 1;
  //}
  
  GLdemo mainbox;
  UFrame frame(usize(500, 450) + mainbox);
  frame.show();
  return appli.start();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void foo(const char* s) {
  cerr << "msg: " << s << endl;  
}

GLdemo::GLdemo(UArgs args) : UBox(args), canvas(*this)
{  
  UBox& menubar = 
  umenubar(ubutton("X-Rot" + umenu( uslider(xrot, UOrient::vertical)) )
           + ubutton("Y-Rot" + umenu( uslider(yrot, UOrient::vertical)) )
           + ubutton("Z-Rot" + umenu( uslider(zrot, UOrient::vertical)) )
           + uhflex() + ubox()
           + uright() + ubutton("Quit" + ucall(0, &UAppli::quit))
           );
  
  UBox& controls = 
  uvbox(upadding(5,UIGNORE)
        + ulabel(uhcenter() + UColor::navy 
                 + "Press the mouse in the canvas to open the contextual menu")
        + createSliderBox(" X ", xrot)
        + createSliderBox(" Y ", yrot)
        + createSliderBox(" Z ", zrot)
        + " " // blank line
        );
  
  add(UOrient::vertical
      + utop() + menubar
      + uvflex() + canvas
      + ubottom() + controls
      );
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // canvas 
  
  // the canvas is fully repainted when xrot, yrot or zrot changes
  // NOTE: repaint() is a method of UBox that fully repaints the widget and thus
  // calls Canvas::paintGL()
  xrot.onChange( ucall(&canvas, &UBox::repaint) );
  yrot.onChange( ucall(&canvas, &UBox::repaint) );
  zrot.onChange( ucall(&canvas, &UBox::repaint) );
  
  // this popup menu will be opened when the mouse is pressed in the canvas
  UPopmenu& menu = upopmenu(UBackground::white + ualpha(0.15) 
                            + UColor::white + UBorder::none 
                            + GLdemo::createSliderBox(" X ", xrot)
                            + GLdemo::createSliderBox(" Y ", yrot)
                            + GLdemo::createSliderBox(" Z ", zrot)
                            );
  menu.softwin();  // this menu is a 'soft window' to allow transparency

  UPiemenu& menu2 = upiemenu();
  //menu2.setPieColor(*new UColor(UColor::grey, 70u));
  //menu2.setSliceColor(*new UColor(UColor::red, 100u));
  menu2.pieColor() = *new UColor(UColor::grey, 70u);
  menu2.sliceColor() = UColor::red;

  menu2.item(0).add("aaaa"+ ucall("aaa", foo));
  menu2.item(2).add("bbbb"+ ucall("bbb", foo));
  menu2.item(4).add("cccc"+ ucall("ccc", foo));
  menu2.item(6).add("dddd"+ ucall("ddd", foo));
  
  canvas.add(menu + menu2 + UOn::mpress / uopen(menu2));
    
  // just for fun, a button in the top left area of the canvas that opens this menu
  canvas.add(uflatbutton(upos(25, 25)
                         + UOn::idle/UColor::orange
                         + UOn::enter/UBackground::orange 
                         + UOn::mpress / uopen(menu)
                         + "Menu")
             );
}

// =============================================================================

void Canvas::initGL() {
  glShadeModel(GL_SMOOTH);			      // Enable Smooth Shading
  glClearColor(0.0f, 0.0f, 0.0f, 0.5f);	  // Black Background
  glClearDepth(1.0f);				      // Depth Buffer Setup
  glEnable(GL_DEPTH_TEST);			      // Enables Depth Testing
  glDepthFunc(GL_LEQUAL);                 // The Type Of Depth Testing To Do
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Really Nice Perspective Calculations
}

void Canvas::resizeGL(UResizeEvent&, int width, int height) {
  if (height==0) height=1;          // Prevent A Divide By Zero By Making Height Equal One
    
  glViewport(0,0, width, height);   // Reset The Current Viewport
  glMatrixMode(GL_PROJECTION);      // Select The Projection Matrix
  glLoadIdentity();                 // Reset The Projection Matrix
  
  // Calculate The Aspect Ratio Of The Window
  gluPerspective(45.0f, (float)width/height, 0.1f, 100.0f);
  
  glMatrixMode(GL_MODELVIEW);      // Select The Modelview Matrix
  glLoadIdentity();                // Reset The Modelview Matrix
}


void Canvas::paintGL(UPaintEvent& e) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear Screen And Depth Buffer
  
  glLoadIdentity();                 // Reset The Current Modelview Matrix
  glTranslatef(-1.5, 0., -6.);      // Move Left 1.5 Units And Into The Screen 6.0
  
  glRotatef(mainbox.xrot*3.6, 1., 0., 0.);
  glRotatef(mainbox.yrot*3.6, 0., 1., 0.);
  glRotatef(mainbox.zrot*3.6, 0., 0., 1.);
  
  glBegin(GL_TRIANGLES);            // Drawing Using Triangles
  glColor3f(1.,0.,0.);              // Set The Color To Red
  glVertex3f(0., 1., 0.);           // Top
  glColor3f(0., 1., 0.);            // Set The Color To Green
  glVertex3f(-1.,-1., 0.);          // Bottom Left	
  glColor3f(0., 0., 1.);            // Set The Color To Blue	
  glVertex3f(1., -1., 0.);          // Bottom Right
  glEnd();                          // Finished Drawing The Triangle
  
  glTranslatef(3., 0., 0.);         // Move Right 3 Units
  
  glBegin(GL_QUADS);                // Draw A Quad
  glColor3f(1., 0., 0.);            // Set The Color To Red
  glVertex3f(-1., 1., 0.);          // Top Left
  glColor3f(0., 1., 0.);            // Set The Color To Green
  glVertex3f(1., 1., 0.);           // Top Right
  glColor3f(0., 0., 1.);            // Set The Color To Blue	
  glVertex3f( 1., -1., 0.);         // Bottom Right
  glColor3f(1, 1., 0.);             // Set The Color To Yellow	
  glVertex3f(-1., -1., 0.);         // Bottom Left
  glEnd();                          // Done Drawing The Quad
}

#endif // UBIT_WITH_GL
