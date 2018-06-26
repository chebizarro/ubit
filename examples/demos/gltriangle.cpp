/* ***********************************************************************
 *
 *  gltriangle.cpp: shows how to draw with OpenGL in a standard Ubit widget
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

#include "gltriangle.hpp"
using namespace std;
using namespace ubit;

#ifndef UBIT_WITH_GL

GLTriangle::GLTriangle(UArgs args) : UBox(args) 
{
  add(uhcenter() + uvcenter() + UFont::xx_large + UColor::red
      "This demo requires OpenGL");
}

#else

#include <ubit/ugl.hpp>

GLTriangle::GLTriangle(UArgs args) : UBox(args) 
{
  UBox& menubar = 
  umenubar(ubutton("X-Rot" + createSliderMenu(xrot))
           + ubutton("Y-Rot" + createSliderMenu(yrot))
           + ubutton("Z-Rot" + createSliderMenu(zrot))
           );
  
  UBox& controls = 
  uvbox(upadding(UIGNORE,8)
        + ulabel(uhcenter() + UColor::navy 
                 + "Press the mouse in the canvas to open the contextual menu")
        + uhbox(uhflex()
                + createSliderBox(" X ", xrot)
                + "  " + createSliderBox(" Y ", yrot)
                + "  " + createSliderBox(" Z ", zrot)
                + "  ")
        );
  
  add(UOrient::vertical 
      + utop() + menubar
      + uvflex() + canvas
      + ubottom() + controls
      );

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // canvas 
  
  // paintCanvasCB() will be called when the canvas is repainted
  canvas.addAttr(UBackground::black
                 + UOn::paint / ucall(this, &GLTriangle::paintCanvasCB));
  
  // the canvas is fully repainted when xrot, yrot or zrot changes
  // NOTE: repaint() is a method of UBox that fully repaints the widget and thus
  // calls UOn::paint callbacks, such as paintCanvasCB(), in this process. 
  // Beware that paintCanvasCB() cannot be called directly and must be used as a
  // UOn::paint callback to work properly.
  
  xrot.onChange( ucall(&canvas, &UBox::repaint) );
  yrot.onChange( ucall(&canvas, &UBox::repaint) );
  zrot.onChange( ucall(&canvas, &UBox::repaint) );
  
  // this popup menu will be opened when the mouse is pressed in the canvas
  UPopmenu& menu = upopmenu(UBackground::white + ualpha(0.15) 
                            + UColor::white + UBorder::none 
                            + GLTriangle::createSliderBox(" X ", xrot)
                            + GLTriangle::createSliderBox(" Y ", yrot)
                            + GLTriangle::createSliderBox(" Z ", zrot)
                            );
  menu.softwin();  // this menu is a 'soft window' to allow transparency

  canvas.add(menu + UOn::mpress / uopen(menu)); //opened on a mouse press
  
  // a button in the top left area of the canvas that opens this menu
  canvas.add(uflatbutton(upos(25, 25)
                         + UOn::idle/UColor::orange
                         + UOn::enter/UBackground::orange 
                         + UOn::mpress / uopen(menu)
                         + "Menu")
             );
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 'val' is the slider model (see USlider constructor).

UBox& GLTriangle::createSliderBox(const char* name, UFloat& val) {
  return uhbox(uleft() + ulabel(UFont::bold + name) + uhflex() + uslider(val));
}

UBox& GLTriangle::createSliderMenu(UFloat& val) {
  return umenu(UBackground::white + ualpha(0.15) 
               + UColor::white + UBorder::none 
               + uslider(val, UOrient::vertical + UBackground::none)
               ).setSoftWin();  // the menu must be a softwin to be transparent
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void GLTriangle::paintCanvasCB(UPaintEvent& e) {
  // This object:
  // - sets the viewport to the appropriate view
  // - sets the clipping planes
  // - calls glPushAttrib(GL_ALL_ATTRIB_BITS) / glPopAttrib() if second argument is true
  // These settings are done twice:
  // - when the object is created to allow custom GL drawing
  // - when the object is destroyed (when the function returns) to display the GUI properly
  //
  UGraph::Glpaint glp(e, false);
    
  glPushAttrib(GL_ENABLE_BIT);
  glEnable(GL_DEPTH_TEST);	
  glDepthFunc(GL_LEQUAL);	
  
  glMatrixMode(GL_PROJECTION);      // Select The Projection Matrix
  glLoadIdentity();                 // Reset The Projection Matrix
  
  // Calculate The Aspect Ratio Of The Window
  gluPerspective(45.0, e.getView()->getWidth() / e.getView()->getHeight(), 0.1, 50);
  
  glMatrixMode(GL_MODELVIEW);       // Select The Modelview Matrix
  glLoadIdentity();				          // Reset The Modelview Matrix

  // !NOTICE: do not do GL_COLOR_BUFFER_BIT because this would clear the color
  // of the whole window (including the background color of the widgets)
  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear Screen And Depth Buffer
  glClear(GL_DEPTH_BUFFER_BIT); 
  
  glLoadIdentity();                 // Reset The Current Modelview Matrix
  glTranslatef(-1.5, 0., -6.);      // Move Left 1.5 Units And Into The Screen 6.0
  
  glRotatef(xrot*3.6, 1., 0., 0.);
  glRotatef(yrot*3.6, 0., 1., 0.);
  glRotatef(zrot*3.6, 0., 0., 1.);
  
  glBegin(GL_TRIANGLES);			      // Drawing Using Triangles
  glColor3f(1.,0.,0.);              // Set The Color To Red
  glVertex3f(0., 1., 0.);           // Top
  glColor3f(0., 1., 0.);            // Set The Color To Green
  glVertex3f(-1.,-1., 0.);          // Bottom Left	
  glColor3f(0., 0., 1.);            // Set The Color To Blue	
  glVertex3f(1., -1., 0.);          // Bottom Right
  glEnd();							            // Finished Drawing The Triangle
  
  glTranslatef(3., 0., 0.);         // Move Right 3 Units
  
  glBegin(GL_QUADS);			          // Draw A Quad
  glColor3f(1., 0., 0.);            // Set The Color To Red
  glVertex3f(-1., 1., 0.);          // Top Left
  glColor3f(0., 1., 0.);            // Set The Color To Green
  glVertex3f(1., 1., 0.);           // Top Right
  glColor3f(0., 0., 1.);            // Set The Color To Blue	
  glVertex3f( 1., -1., 0.);         // Bottom Right
  glColor3f(1, 1., 0.);             // Set The Color To Yellow	
  glVertex3f(-1., -1., 0.);         // Bottom Left
  glEnd();							            // Done Drawing The Quad
  
  glPopAttrib();
}

#endif // UBIT_WITH_GL

