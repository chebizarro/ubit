/*
 *  GLContext.cpp: OpenGL rendering context
 *  Ubit GUI Toolkit - Version 8
 *  (C) 2018 Chris Daley
 *  (C) 2009 | Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */


#include <ubit/ubit_features.h> 
#if UBIT_WITH_GL

#include <iostream>
#include <cmath>
#include <ubit/udefs.hpp>
#include <ubit/draw/font.h>
#include <ubit/ui/fontmetrics.h>
#include <ubit/draw/ufontImpl.hpp>
#include <ubit/draw/color.h>
#include <ubit/draw/image.h>
#include <ubit/core/application.h>
#include <ubit/ugl.hpp>
#include <ubit/draw/graph.h>
#include <ubit/uglcontext.hpp>
#include <ubit/uhardima.hpp>
#include <ubit/uhardfont.hpp>
#if UBIT_WITH_X11
#  include <ubit/udispX11.hpp>
#elif UBIT_WITH_GLUT
#  include <ubit/udispGLUT.hpp>
#endif

using namespace std;
namespace ubit {


// ! checks that the current context is the default context of the display !
#define MAKE_CURRENT if (disp->current_glcontext != this) makeCurrent()

#if UBIT_WITH_X11

GLContext::GLContext(Display* d, GLContext* sharelists) :
RenderContext(d), win_height(0), cx(0), cy(0) 
{
  glxcontext = ((UDispX11*)d)->createGlcontext(sharelists);
}

GLContext::~GLContext() {
  ((UDispX11*)disp)->destroyGlcontext(glxcontext);
}

void GLContext::makeCurrent() const {
  if (!dest) {
    Application::error("GLContext::makeCurrent","null drawable");
    return;
  }
  
  // glFlush() must be called when displaying on the same window with another
  // glcontext, otherwise what was drawn before glXMakeCurrent() may be lost: 
  //if (disp->current_gc && disp->current_gc->dest == dest) 
  glFlush();
  
  disp->current_glcontext = this;
  
  // always called because destination or glcontext may have changed
  glXMakeCurrent(((UHardwinX11*)dest)->getSysDisp(), ((UHardwinX11*)dest)->getSysWin(), 
                 glxcontext);
}

void GLContext::swapBuffers() {
  MAKE_CURRENT;
  glXSwapBuffers(((UHardwinX11*)dest)->getSysDisp(), ((UHardwinX11*)dest)->getSysWin());
}

// used by Display::getFont() to create fonts in the default context
bool GLContext::isSharedWith(const RenderContext* c) const {
  const GLContext* glc = c->toGlcontext();
  return glc && glc->glxcontext == glxcontext; // !!! A COMPLETER : FAUT VERIFIER SHAREDLISTS !!!
}


#elif UBIT_WITH_GLUT

GLContext::GLContext(Display* d, UHardwinGLUT* hw) :
RenderContext(d), win_height(0), cx(0), cy(0) 
{
  hardwin = hw;
}

GLContext::~GLContext() {}

void GLContext::makeCurrent() const {
  if (!dest) {
    Application::error("GLContext::makeCurrent","null destination drawable");
    return;
  }

  glFlush();     // see X11 version for comments
  
  disp->current_glcontext = this;
  
  // always called because destination may have changed
  if (hardwin->sys_win)
    glutSetWindow(hardwin->sys_win);
  else Application::error("GLContext::makeCurrent","destination drawable is not initialized");
}

void GLContext::swapBuffers() {
  MAKE_CURRENT;
  glutSwapBuffers();
}

// used by Display::getFont() to create fonts in the default context
bool GLContext::isSharedWith(const RenderContext* c) const {
  return false;                // !!! A COMPLETER : FAUT VERIFIER SHAREDLISTS !!!
}

#endif

void GLContext::setDest(UHardwinImpl* d, double x, double y) {
  dest = d;
  xwin = x;
  ywin = y;
  win_height = dest->getSize().height;
  cx = x;
  cy = win_height - y;
}

void GLContext::setOffset(double x, double y) {
  xwin = cx = x;
  ywin = y;
  cy = win_height - ywin;
}

void GLContext::set3Dmode(bool state) {
  if (state) {
    win_height = 0;
    cy = -ywin;
  }
  else {
    win_height = dest->getSize().height;
    cy = win_height - ywin;
  }
}

void GLContext::flush() {
  MAKE_CURRENT;
  glFlush();
}

void GLContext::setPaintMode(Graph& g) {
  MAKE_CURRENT;
  glLogicOp(GL_COPY);
  glColor4ubv(g.color_rgba.comps);
}

void GLContext::setXORMode(Graph& g, const Color& bg) {
  MAKE_CURRENT;
  glLogicOp(GL_XOR);
  //setGLColor(color ^ bgcolor); // color_rgba !!!
  Application::warning("Graph::setXORMode","XOR mode not implemented in OpenGL; color %p", &bg); //  !!!!  &&&&
  g.bgcolor_rgba = bg.getRgba();
  glColor4ubv(g.bgcolor_rgba.comps);
}


void GLContext::setColor(Graph& g, const Color& c) {
  MAKE_CURRENT;
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  if (g.in_xor_mode) {
    Application::warning("Graph::setColor","XOR mode not implemented in OpenGL; color %p", &c); //@@@!!!!
  }
  g.color_rgba = c.getRgba();
  //if (color_rgba.components[3] >= 255) color_rgba.components[3] = int(alpha * 255);  //&&&&&&
  glColor4ubv(g.color_rgba.comps);
}


void GLContext::setBackground(Graph& g, const Color& c) {
  //MAKE_CURRENT;
  g.bgcolor_rgba = c.getRgba();
}


void GLContext::setFont(Graph& g, const FontDescription& fd) {
  //MAKE_CURRENT;
} 


void GLContext::setWidth(Graph& g, double w) { 
  MAKE_CURRENT;
  //get supported line width range and step size
  //glGetFloatv(GL_LINE_WIDTH_RANGE,sizes);
  //glGetFloatv(GL_LINE_WIDTH_GRANULARITY,&step);
  glLineWidth(w);
}


void GLContext::copyArea(double x, double y, double w, double h, double delta_x, double delta_y,
                           bool generate_refresh_events_when_obscured) const 
{
  MAKE_CURRENT;
  Application::error("Graph::copyArea","This function is not available when OpenGL is used");
}


void GLContext::setClip(double x, double y, double width, double height) {
  MAKE_CURRENT;
  clip.setRect(x, y, width, height);  
  /* !!!WARNING!!!
   * glClipPlane() makes it impossible to use glTranslate(), glRotates()... beacuse
   * the cliping plane applies to the MODIFIED matrix, whichs means that it is realtive
   * the last transformation and cannot be set globally.
   * => functions using glTranslate(), etc. WON'T WORK AS EXPECTED after setGLClipPlanes!
   *
   * Notes:
   * - FTGL Render() functions were modified for this reason
   * - paint callbacks should disable GL_CLIP_PLANEi when they use geometrical operations
   */
  GLdouble left[] = {1, 0, 0, -clip.x+1};  // il faut +1
  glClipPlane(GL_CLIP_PLANE0, left);
  glEnable(GL_CLIP_PLANE0);
  
  GLdouble right[] = {-1, 0, 0, clip.x + clip.width};
  glClipPlane(GL_CLIP_PLANE1, right);
  glEnable(GL_CLIP_PLANE1);
  
  GLdouble top[] = {0, -1, 0, win_height - clip.y+1};  // il faut +1
  glClipPlane(GL_CLIP_PLANE2, top);
  glEnable(GL_CLIP_PLANE2);
  
  GLdouble bottom[] = {0, 1, 0, -win_height + clip.y + clip.height}; 
  glClipPlane(GL_CLIP_PLANE3, bottom);
  glEnable(GL_CLIP_PLANE3);
}


void GLContext::drawArc(double x, double y, double w, double h, 
                         double start, double ext, bool filled) const {
  MAKE_CURRENT;
  double rw = w / 2.;  // width radius
  double rh = h / 2.;  // height radius
  double center_x = cx+x +rw; 
  double center_y = cy-y -rh;
  double a1 = start * M_PI / 180.;
  if (ext >= 360) ext = 361;  // to avoid discontinuties
  double a2 = (start+ext) * M_PI / 180.;
  double incr = M_PI / max(w,h);  // resolution
  
  if (filled) {
    /*
     GLUquadric* quad = gluNewQuadric();
     glTranslatef( 10, -30, 0.0f);
     gluDisk(quad, 99, 100, 5, 5);
     gluDeleteQuadric(quad);
     */

    glEnable(GL_POLYGON_SMOOTH);   // marche pas ?    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_POLYGON);
    for (double a = a1; a <= a2; a += incr)
      glVertex2d(center_x + rw*::cos(a), center_y + rh*::sin(a));
    if (ext < 360) glVertex2d(center_x, center_y);  // add central point to get a quadrant
    glEnd();
    glDisable(GL_POLYGON_SMOOTH);
  }
  else {
    //si c'est un cercle (pas un arc)
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glBegin(GL_POLYGON);
    
    glEnable(GL_LINE_SMOOTH);
    glBegin(GL_LINE_STRIP);
    for (double a = a1; a <= a2; a += incr)
      glVertex2d(center_x + rw*::cos(a), center_y + rh*::sin(a));
    glEnd();
    glDisable(GL_LINE_SMOOTH);
  }
}


void GLContext::drawLine(double x1, double y1, double x2, double y2) const {
  MAKE_CURRENT;
  glBegin(GL_LINE_STRIP);
  glVertex2d(cx+x1, cy-y1);
  glVertex2d(cx+x2, cy-y2);
  glEnd();
}


void GLContext::drawRect(double x, double y, double w, double h, bool filled) const {
  MAKE_CURRENT;
  if (filled) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // dont draw on right, bottom borders (cx+x+w, cy-y-h)
    glRectd(cx+x, cy-y, cx+x +w-1, cy-y -h+1);
  }
  else {
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  marche pas
    //glRectf(xwin + x    , -(ywin + y), xwin + x + w, -(ywin + y + h));
    glBegin(GL_LINE_LOOP);
    glVertex2d(cx+x   , cy-y);
    glVertex2d(cx+x +w, cy-y);
    glVertex2d(cx+x +w, cy-y -h);
    glVertex2d(cx+x   , cy-y - h);
    glEnd();
  }
}


#define RECT_ARC(center_x, center_y, aww, ahh, from, to, incr) { \
  for (double a = (from); a >= (to); a -= (incr))  \
    glVertex2d(center_x + (aww)*::cos(a), center_y + (ahh)*::sin(a));  \
}

void GLContext::drawRoundRect(double x, double y, double w, double h, 
                               double arc_w, double arc_h, bool filled) const {
  MAKE_CURRENT;
  double x2 = x+w, y2 = y+h;
  double aww = arc_w/2., ahh = arc_h/2.; // arc radius
  double incr = M_PI / max(aww,ahh);  // resolution
  //double incr = 0.2;

  if (filled) {
    glEnable(GL_POLYGON_SMOOTH);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_POLYGON);
  }
  else {
    //pose pbm avec FTGL
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glBegin(GL_POLYGON);  
    glEnable(GL_LINE_SMOOTH);
    glBegin(GL_LINE_LOOP);   
  }
  
  glVertex2d(cx+x+aww, cy-y); glVertex2d(cx+x2-aww, cy-y); //top line
  RECT_ARC(cx+x2-aww, cy-y-ahh, aww, ahh, M_PI/2, 0, incr); // top right arc

  glVertex2d(cx+x2, cy-y-ahh); glVertex2d(cx+x2, cy-y2+ahh); //right line
  RECT_ARC(cx+x2-aww, cy-y2+ahh, aww, ahh, 0, -M_PI/2, incr); // bottom right arc    

  glVertex2d(cx+x2-aww, cy-y2); glVertex2d(cx+x+aww, cy-y2); //bottom line
  RECT_ARC(cx+x+aww, cy-y2+ahh, aww, ahh, -M_PI/2, -M_PI, incr); // bottom left arc

  glVertex2d(cx+x, cy-y2+ahh); glVertex2d(cx+x, cy-y-ahh); //left line
  RECT_ARC(cx+x+aww, cy-y-ahh, aww, ahh, M_PI, M_PI/2, incr); // top left arc
  
  glEnd();
  if (filled) glDisable(GL_POLYGON_SMOOTH);
  else glDisable(GL_LINE_SMOOTH);

  /*
   if (!filled) {
     drawArc(x, y, aw, ah, 90, 90, false); // top left arc
     drawArc(x2-aw, y, aw, ah, 0, 90, false); // top right arc
     drawArc(x, y2-ah, aw, ah, 180, 90, false); // bottom left arc
     drawArc(x2-aw, y2-ah, aw, ah, 270, 90, false); // bottom right arc    
     drawLine(x+aww, y, x2-aww, y); // top line
     drawLine(x2, y+ahh, x2, y2-ahh); // right line
     drawLine(x+aww, y2, x2-aww, y2); // bottom line
     drawLine(x, y+ahh , x, y2-ahh); // left line
  }
  else { // filled
     drawRect(x+aww, y, w-2*aww, ahh, true); // filled top 
     drawRect(x, y+ahh, w, h-2*ahh, true); // filled middle 
     drawRect(x+aww, y2-ahh, w-2*aww, ahh, true); // filled bottom 
     
     drawArc(x, y, arc_w, arc_h, 90, 90, true); // filled top left arc
     drawArc(x2-arc_w, y, arc_w, arc_h, 0, 90, true); // filled top right arc
     drawArc(x, y2-arc_h, arc_w, arc_h, 180, 90, true); // filled bottom left arc
     drawArc(x2-arc_w, y2-arc_h, arc_w, arc_h, 270, 90, true); // filled top right arc
  }
  */
}



void GLContext::drawString(const UHardFont* nf, const char* str, int str_len, 
                             double x, double y) const {
  MAKE_CURRENT;
  int charpos_begin, charpos_end;
  float xpos_begin, xpos_end;
  
  if (!FontMetrics::getClippedText(nf, clip.x, clip.width, str, str_len, cx+x, cy-y,
                                    charpos_begin, charpos_end, xpos_begin, xpos_end))
    return;
  
  if (charpos_begin < 0 || charpos_end < 0 ||charpos_end < charpos_begin) 
    return;    
  
  nf->drawString(str+charpos_begin, charpos_end - charpos_begin+1, xpos_begin, cy-y);  
}

// type is one of LINE_STRIP (polyline), LINE_LOOP (polygon), FILLED (filled polygon).

void GLContext::drawPolygon(const float* coords2d, int card, int polytype) const {
  if (card <= 0 || coords2d == null) return;
  MAKE_CURRENT;
  glPushMatrix();
  //glTranslatef(cx, -ywin, 0.);
  glTranslatef(cx, cy, 0.);  // ????
  glScalef(1., -1., 1.);  // !!!
  
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(2, GL_FLOAT, 0/*stride*/, coords2d);
  glDrawArrays((GLenum)polytype, 0, card); 
  glDisableClientState(GL_VERTEX_ARRAY);
  
  glPopMatrix();
}


void GLContext::drawPolygon(const std::vector<Point>& points, int polytype) const {
  int card = points.size();
  if (card <= 0) return;
  MAKE_CURRENT;
  glBegin((GLenum)polytype);
  for (int k = 0; k < card; ++k) glVertex2f(cx+points[k].x, cy-points[k].y);
  glEnd();  
}


void GLContext::drawIma(const Graph& g, const Image& ima, double x, double y, double scale) const {
  // une seule natima dans ce cas: la premiere
  if (!ima.getNatImas().empty()) {
    UHardIma* ni = *(ima.getNatImas().begin());
    //if (ni) drawNatIma((UHardImaGL*)ni, x, y, scale);
    if (ni) drawTex(g, (UHardImaGL*)ni, x, y, ima.getWidth()*scale, ima.getHeight()*scale);
  }
}

void GLContext::drawTex(const Graph& g, const UHardImaGL* ni,
                          double x, double y, double width, double height) const
{
  MAKE_CURRENT;
  
  if (ni->texid == 0) {
    if (ni->pixels) ni->createTexFromPixels(); //??? pouquoi pas fait dans setRaster?
    if (ni->getTexID() == 0) return;
  }
  
  GLfloat x1 = cx+x;
  GLfloat y1 = cy-y;
  GLfloat x2 = x1 + width;
  GLfloat y2 = y1 - height;
  
  glPushAttrib(GL_CURRENT_BIT);
  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
  glEnable(GL_BLEND);
  //glColor4f(1.0, 1.0, 1.0, g.alpha);
  glColor4f(1.0, 1.0, 1.0, 1.); // g.color_rgba.getAlphaI());   // !!!!!??????
  
  glBindTexture(GL_TEXTURE_2D, ni->texid);
  glBegin(GL_POLYGON);
  glTexCoord2f(0.0, 0.0); glVertex2f(x1, y1);
  glTexCoord2f(1.0, 0.0); glVertex2f(x2, y1);
  glTexCoord2f(1.0, 1.0); glVertex2f(x2, y2);
  glTexCoord2f(0.0, 1.0); glVertex2f(x1, y2);
  glEnd();
  //if (alpha >= 1.) glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  glPopAttrib();
}

/* without textures
 void GLContext::drawTex(RenderContext* ge, const UHardIma& ima,
 float x, float y, float width, float height) {
 #ifndef UBIT_WITH_GL
 Application::internalError("Graph::drawTex","invalid function in GL mode");
 #else
 if (!ima->glpix) {
 // TRANSITION: cas ou ou a d'abord creee une XImage
 if (ima.xima) ima->createGLTexFromXImages();
 if (!ima.glpix) return;
 }
 
 MAKE_CURRENT;
 
 // ima rect: rectangle de la location de l'ima dans target view
 // si l'image n'etait pas clippee
 Rectangle imarect(x, y, width, height);
 
 // intersection d'imarect avec le clip courant
 bool clipstat = imarect.setInter(ge->cliprect.x - ge->xwin,
 ge->cliprect.y - ge->ywin,
 ge->cliprect.width,
 ge->cliprect.height);
 // rien a afficher
 if (!clipstat) return;
 
 //glPushAttrib(GL_CURRENT_BIT);
 glEnable(GL_TEXTURE_2D);
 glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 glEnable(GL_BLEND);
 glColor4f(1.0, 1.0, 1.0, 1.0);
 
 glRasterPos2i(imarect.x+ ge->xwin,
 -(imarect.y+ ge->ywin + imarect.height));
 
 glPixelStorei(GL_UNPACK_SKIP_PIXELS, imarect.x - x);
 int skip_rows = height - imarect.height -(imarect.y - y);
 glPixelStorei(GL_UNPACK_SKIP_ROWS, skip_rows);
 glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
 //fait en soft donc tres long et en plus ca plante
 //glPixelZoom(1., -1.);
 glDrawPixels(imarect.width, imarect.height,
 GL_RGBA, GL_UNSIGNED_BYTE, ima->pixels);
 
 if (alpha > 1.) glDisable(GL_BLEND);
 glDisable(GL_TEXTURE_2D);
 //glPopAttrib();
 }
 */

}
#endif
