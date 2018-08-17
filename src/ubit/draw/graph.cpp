/*
 *  graph.h: Graphics Layer
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
#include <vector>
#include <iostream>
#include <cstdio>
#include <cmath>
#include <ubit/udefs.hpp>
#include <ubit/uon.hpp>
#include <ubit/ufont.hpp>
#include <ubit/ufontmetrics.hpp>
#include <ubit/uima.hpp>
#include <ubit/ustr.hpp>
#include <ubit/ugraph.hpp>
#include <ubit/uwin.hpp>
#include <ubit/uappli.hpp>
#include <ubit/ui/updatecontext.h>
#include <ubit/uglcanvas.hpp>     // !!!! DEPENDANCE A OPEN GL UTILISER SUBWIN !!!
#include <ubit/udispX11.hpp>
#include <ubit/udispGLUT.hpp>
//#include <ubit/udispGDK.hpp>
#include <ubit/uhardima.hpp>
#include <ubit/uhardfont.hpp>
#if UBIT_WITH_X11
#  include <ubit/ux11context.hpp>
//#elif UBIT_WITH_GDK
//#  include <ubit/urendercontextGDK.hpp>
#endif
#if UBIT_WITH_GL
#  include <ubit/uglcontext.hpp>
#endif

using namespace std;
namespace ubit {


#if UBIT_WITH_GL 
  const int Graph::LINE_STRIP = GL_LINE_STRIP, Graph::LINE_LOOP = GL_LINE_LOOP, Graph::FILLED = GL_POLYGON;
#else
  const int Graph::LINE_STRIP = 0x0003, Graph::LINE_LOOP = 0x0002, Graph::FILLED = 0x0009;
#endif


Graph::Graph(View* v) {
  rc = null; // securite pour ~Graph
  if (!v) {
    Application::fatalError("Graph::Graph(View*)","null View argument: cannot create a Graph");
    return;
  }
  if (!(hardwin = v->getHardwin())) {
    Application::fatalError("Graph:Graph(View*)","null window: cannot create Graph");
    return;
  }
  
  boxview = v;
  disp = hardwin->disp;
  font = null;
  font_styles = 0;
  //alpha = 1.;
  if (Application::isUsingGL()) width = 1; else width = 0.;   // !!!
  color = disp->getBlackPixel();
  bgcolor = disp->getWhitePixel();
  //color_rgba.set(0); color_rgba.components[3] = 255;
  //bgcolor_rgba.set(255);
  blend_mode = OPAQUE;
  in_xor_mode = false;
  in_3d_mode = false;
  is_client = false;

  // cas ou on voudrait creer un rc reutilisable
  // rc_mode = OWN_rc;
  // rc = Graph::createRenderContext(disp, null/*sharelists*/);  sharelists???
  
#if UBIT_WITH_GLUT
  if (hardwin->glcontext) rc = hardwin->glcontext;
  else rc = disp->getDefaultContext();
#else
  // note that getDefaultContext() inits 'default_context' if not already done
  rc = disp->getDefaultContext();
#endif
  
  GLCanvas* glcanvas = dynamic_cast<GLCanvas*>(hardwin->win);
  if (glcanvas) {
    Point pt = hardwin->getPos();
    rc->setDest(hardwin, -pt.x, -pt.y);
  }
  else rc->setDest(hardwin, 0, 0);

  rc->makeCurrent();
    
  // inutile de le refaire a chaque fois sauf si 3D ou si on change de fenetre !!!!
  setViewportOrtho(hardwin);
  rc->setClip(v->x, v->y, v->width, v->height);      // NB: no offset
  //rc->clipbegin.setRect(clip.x /*+ xwin*/, clip.y /*+ ywin*/, clip.width, clip.height);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Graph::Graph(PaintEvent& e) {
  rc = null; // securite pour ~Graph
  UpdateContext* c = e.current_context;
  Graph* g = c ? c->getGraph() : null;
  if (g == null || g->rc == null) {
    Application::fatalError("Graph::Graph(PaintEvent&)","null graphics context: cannot create Graph");
    return;
  }
  View* v = e.getView();  
  boxview = v;
  disp = g->disp;
  hardwin = g->hardwin;
  font = g->font;
  GraphAttributes::operator=(*g);  
  is_client = true;

#if UBIT_WITH_GLUT
  if (hardwin->glcontext) rc = hardwin->glcontext;
  else rc = disp->getDefaultContext();
#else
  // note that getDefaultContext() inits 'default_context' if not already done
  rc = disp->getDefaultContext();
#endif
  
  glPushAttrib(GL_ALL_ATTRIB_BITS);    // a faire par le client ?????
  //glPushAttrib(GL_LINE_BIT); suffirait   
      
  PaintEvent e2(UOn::paint, v->getWinView(), null/*flow*/);
  e2.setSourceAndProps(v);
  //rc->setOffset(v->x, v->y); //remplacé avantageusement par glTranslatef()
  
  //if (e2.isClipSet()) {
    // CLIP FAUX ? SERT A QQ CHOSE ?
    // rc->setClip(e2.redraw_clip.x, e2.redraw_clip.y,  // att: en absolu!
    //            e2.redraw_clip.width, e2.redraw_clip.height);
    // rc->clipbegin = e2.redraw_clip;
  //}
  
  glPushMatrix();  // permet que le clip precedent ne soit pas translate
  glTranslatef(v->x, -v->y, 0);
  //cerr << "v " << v->x << " " << v->y << endl;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Graph::~Graph() {
  if (rc == null) return;
  
  if (is_client) {
    // BUG: rc->setOffset(0, 0); faux car l'offset precedent est perdu, en particulier
    // dans le cas d'un dessin dans un widget dans un canvas: l'offset est mis a 0
    // apres le dessin alors que ce widget est decale par rapport au canvas
    glPopMatrix();
    glPopAttrib();    
  }
  else {
    rc->swapBuffers();
  }
  /*
   ... case OWN_RC: delete rc; break;
   */
}


int Graph::GLPaint::count = 0;

Graph::GLPaint::GLPaint(PaintEvent& e, bool _push_attrib) {
  begin(e.getView(), _push_attrib);
}

Graph::GLPaint::GLPaint(View* v, bool _push_attrib) {
  begin(v, _push_attrib);
}

Graph::GLPaint:: ~ GLPaint() {   
  end();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Graph::GLPaint::begin(View* v, bool _push_attrib) {
  no = ++count;
  push_attrib = _push_attrib;
  hardwin = null;

  View* winview = null;
  if (!v 
      || !(winview = v->getWinView())
      || !(hardwin = v->getHardwin())) {
    Application::error("Graph::GLpaint","null or invalid view");
    return;
  }
  
  if (push_attrib) glPushAttrib(GL_ALL_ATTRIB_BITS);
  
  if (hardwin->getWinType() != UWinImpl::SUBWIN) 
    glViewport(int(v->x), int(winview->height - v->height - v->y), 
               int(v->width), int(v->height));  
  /*
   cerr << "> GLSection " << no << " " << v << " / "
   << " / glcoord: "<< int(v->x) << " "<< int(winview->height - v->height - v->y)
   << " / " << int(v->width) << " " << int(v->height)
   << " / hw: "<< v << " " << hardwin 
   << endl;
   */
  glDisable(GL_CLIP_PLANE0);
  glDisable(GL_CLIP_PLANE1);
  glDisable(GL_CLIP_PLANE2);
  glDisable(GL_CLIP_PLANE3);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Graph::GLPaint::end() {   
  if (!hardwin) return;  
  //cerr << "< GLSection " << no << " : " <<hardwin <<endl;
  
  if (push_attrib) glPopAttrib();
  
  // reset to normal GUI rendering 
  Graph::setViewportOrtho(hardwin);
  
  //Dimension size = hardwin->getSize();
  glEnable(GL_CLIP_PLANE0);
  glEnable(GL_CLIP_PLANE1);
  glEnable(GL_CLIP_PLANE2);
  glEnable(GL_CLIP_PLANE3);
  
  hardwin = null;   // dont do it again if called
}

/*
void PaintEvent::beginGL() {
#if UBIT_WITH_GL
  View* winview = null;
  UHardwinImpl* hardwin = null;
  
  if (!source_view 
      || !(winview = source_view->getWinView())
      || !(hardwin = source_view->getHardwin())
      ) {
    Application::internalError("PaintEvent::beginGL","Null window view, function failed");
    return;
  }
  
  //cerr << "beginGL: "
  //<< int(redraw_clip.x)<<" "<<int(winview->height - redraw_clip.height - redraw_clip.y)
  //<<" "<< int(redraw_clip.width)<<" "<< int(redraw_clip.height)<< endl;
  
  if (hardwin->wintype != UWinImpl::SUBWIN) 
    glViewport(int(redraw_clip.x), int(winview->height - redraw_clip.height - redraw_clip.y),
               int(redraw_clip.width), int(redraw_clip.height));
  
  //glPushAttrib(GL_ALL_ATTRIB_BITS); responsabilité du client
  
  glDisable(GL_CLIP_PLANE0);
  glDisable(GL_CLIP_PLANE1);
  glDisable(GL_CLIP_PLANE2);
  glDisable(GL_CLIP_PLANE3);
#endif
}

void PaintEvent::endGL() {
#if UBIT_WITH_GL
  View* winview = null;
  UHardwinImpl* hardwin = null;
  
  if (!source_view || !(winview = source_view->getWinView())
      || !(hardwin = source_view->getHardwin())
      ) {
    Application::internalError("PaintEvent::endGL","Null window view, function failed");
    return;
  }
  
  // glPopAttrib();  responsabilité du client
  
  // reset to normal viewport rendering (does nothing if this hardwin is a subwin)
  //cerr << "endGL: "<<source_view<< " " << hardwin<<endl;
  
  Graph::setViewportOrtho(hardwin);
  glEnable(GL_CLIP_PLANE0);
  glEnable(GL_CLIP_PLANE1);
  glEnable(GL_CLIP_PLANE2);
  glEnable(GL_CLIP_PLANE3);
#endif
}
*/
/*
 void Graph::beginBlend(const Rectangle& clip, float _alpha) {
   blend_mode = TRUE_BLEND;
   alpha = _alpha;
 }
void Graph::endBlend() {
   alpha = 1.;
 }

void Graph::fillBackground(const Rectangle& clip, const Color& c, float _alpha) {
  if (_alpha == 1.) {
    setColor(c);
    fillRect(clip.x, clip.y, clip.width, clip.height);
  }
  else {
    setColor(c, _alpha);   // &&& et s'il y a deja un alpha ???
    fillRect(clip.x, clip.y, clip.width, clip.height);    
  }
}

 void Graph::setGLDefaults() {
 glShadeModel(GL_FLAT);
 glDisable(GL_COLOR_MATERIAL);
 glClearDepth(1.0f);
 glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
 }
 */

void Graph::set3Dmode(bool state) {
  in_3d_mode = state;
  rc->set3Dmode(state);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Graph::setViewportOrtho(UHardwinImpl* hw) {
# if UBIT_WITH_GL
  if (! Application::isUsingGL()) return;
    
  // NB SDL: il faudrait rappeler SDL_SetVideoMode mais cette fct detruit le contexte OpenGL!
  // mainframe.sys_win = SDL_SetVideoMode(w,h, sys_visual->BitsPerPixel, SDL_OPENGL|SDL_RESIZABLE);
  Dimension size = hw->getSize();
  
  //cerr << "Graph::setViewportOrtho: syswin "  << ((UHardwinImplX11*)hw)->sys_win
  //  << " size " << int(size.width)<< " " << int(size.height) << endl;
  
  glViewport(0, 0, int(size.width), int(size.height));
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  glOrtho(0, int(size.width), 0, size.height, 0.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
# endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Graph: noter que le Clip est relatif a la View attachee
// - NB: verifier que le Clip est bien inclus dans la View et prendre le min
// - !ATT: on ne tient pas compte dy cas ou r.x ou r.y  < 0  !!!

void Graph::setClip(const Rectangle& r) {
  Rectangle clip;
  if (r.x >= boxview->getWidth() || r.y >= boxview->getHeight()) {
    clip.width = 0; clip.height = 0;
  }
  else {
    // keep the min
    if (r.x + r.width > boxview->getWidth()) clip.width = boxview->getWidth() - r.x;
    else clip.width = r.width;
    
    if (r.y + r.height > boxview->getHeight()) clip.height = boxview->getHeight() - r.y;
    else clip.height = r.height;
  }
  
  // !ATT: on ne tient pas compte dy cas ou r.x ou r.y  < 0 
  rc->setClip(boxview->x + r.x, boxview->y + r.y, clip.width, clip.height);
}

void Graph::setHardwinClip(const Rectangle& r) {
  rc->setClip(r.x + rc->xwin, r.y + rc->ywin, r.width, r.height);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Graph::getClip(Rectangle& r) const {
  r.x = rc->clip.x - rc->xwin - boxview->x;
  r.y = rc->clip.y - rc->ywin - boxview->y;
  r.width  = rc->clip.width;
  r.height = rc->clip.height;
}

void Graph::getHardwinClip(Rectangle& r) const {
  r.x = rc->clip.x - rc->xwin;
  r.y = rc->clip.y - rc->ywin;
  r.width  = rc->clip.width;
  r.height = rc->clip.height;
}

void Graph::getWinOffset(float& x, float& y) const {
  x = rc->xwin;
  y = rc->ywin;
}

void Graph::setWinOffset(float x, float y) const {
  rc->setOffset(x, y);
}

// ============================================================ [Ubit] =========

Box* Graph::getBox() const {return boxview ? boxview->getBox() : null;}

void Graph::setPaintMode() {
  in_xor_mode = false;
  if (rc) rc->setPaintMode(*this);
}

void Graph::setXORMode(const Color& bg) {
  in_xor_mode = true;
  if (rc) rc->setXORMode(*this, bg);
}

//void Graph::setAlpha(float a) {alpha = a;}

void Graph::setColor(const Color& c) {
  if (rc) rc->setColor(*this, c);
}

void Graph::setColor(const Color& c, float a) {
  Color c2(c, a);
  if (rc) rc->setColor(*this, c2);
}

void Graph::setColor(const Color& c, unsigned int a) {
  Color c2(c, a);
  if (rc) rc->setColor(*this, c2);
}

void Graph::setBackground(const Color& c) {
  if (rc) rc->setBackground(*this, c);
}

// for X11 the default value is 0, which sets thickness to 1 pixel
// and optimizes drawing routines
float Graph::getWidth() const {return width;}

void Graph::setWidth(float w) {
  width = w;
  if (rc) rc->setWidth(*this, w);
}

void Graph::getFontMetrics(const Font& f, FontMetrics& fm) const {
  fm.set(f, disp);
}

void Graph::setFont(const Font& f) {
  FontDescription fd(f);
  setFont(fd);
}

void Graph::setFont(const FontDescription& fd) {
  UHardFont* f = disp->getFont(&fd);
  if (!f) {
    Application::error("Graph::setFont","null font");
    return;
  }
  font = f;
  font_styles = fd.styles;
  rc->setFont(*this, fd);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Graph::flush() const {
  rc->flush();
}

// ============================================================ [Ubit] =========
// drawing

void Graph::draw(const Shape& s) const {
  s.draw(*this);
}

void Graph::fill(const Shape& s) const {
  s.fill(*this);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Graph::drawArc(double x, double y, double w, double h, double start, double ext) const {
  rc->drawArc(x, y, w, h, start, ext, false); // not filled
}

void Graph::fillArc(double x, double y, double w, double h, double start, double ext) const {
  rc->drawArc(x, y, w, h, start, ext, true); // filled
}

void Graph::drawEllipse(double x, double y, double w, double h) const {
  rc->drawArc(x, y, w, h, 0., 360., false);
}

void Graph::fillEllipse(double x, double y, double w, double h) const {
  rc->drawArc(x, y, w, h, 0., 360., true); // filled
}

void Graph::drawIma(const Image& ima, double x, double y, double scale) const {
  if (ima.getNatImas().empty()) ima.realize(0,0, getDisp(), true);  // creer l'original
  rc->drawIma(*this, ima, x, y, scale);
}

void Graph::drawLine(double x1, double y1, double x2, double y2) const {
  rc->drawLine(x1, y1, x2, y2);
}

void Graph::drawLine(const Point& p1, const Point& p2) const {
  rc->drawLine(p1.x, p1.y, p2.x, p2.y);
}

// type is one of LINE_STRIP (polyline), LINE_LOOP (polygon), FILLED (filled polygon).
void Graph::drawPolygon(const std::vector<Point>& points, int polytype) const {
  rc->drawPolygon(points, polytype);
}

void Graph::drawPolygon(const float* coords2d, int card, int polytype) const {
  rc->drawPolygon(coords2d, card, polytype);
}

void Graph::drawRect(double x, double y, double w, double h) const {
  rc->drawRect(x, y, w, h, false);
}

void Graph::fillRect(double x, double y, double w, double h) const {
  rc->drawRect(x, y, w, h, true); // filled
}

void Graph::drawRoundRect(double x, double y, double w, double h, 
                           double arc_w, double arc_h) const { 
  rc->drawRoundRect(x, y, w, h, arc_w, arc_h, false);
}

void Graph::fillRoundRect(double x, double y, double w, double h, 
                           double arc_w, double arc_h) const { 
  rc->drawRoundRect(x, y, w, h, arc_w, arc_h, true);
}

void Graph::copyArea(double x, double y, double w, double h, 
                      double delta_x, double delta_y,
                      bool generate_refresh_events_when_obscured) const
{
  rc->copyArea(x, y, w, h, delta_x, delta_y, generate_refresh_events_when_obscured);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// strings

void Graph::drawString(const String& s, double x, double y) const {
  drawString(s.c_str(), s.length(), x, y);
}

void Graph::drawString(const char* str, int str_len, double x, double y) const {
  if (!str || str_len <= 0) return;

  if (str[str_len-1] == '\n') {  // ne pas afficher le \n final
    if (str_len == 1) return;
    else str_len--;
  }
  
  UHardFont* nf = font;     // NB: pourrait etre dans le constructeur
  if (!nf) {
    Graph* g = const_cast<Graph*>(this);
    g->setFont(*Application::conf.default_font);
    nf = font;
    if (!nf) {
      Application::error("Graph::drawString","null font");
      return;
    }
  }
  
  float fontAscent = nf->getAscent();
  float fontDescent = nf->getDescent();
  float textWidth = nf->getWidth(str, str_len);
  
  // draw underline, overline, strike-through
  if (font_styles > Font::BOLD+Font::ITALIC) {
    
    if (font_styles & Font::FILL) {
      ((Graph*)this)->setColor(Color::lightblue);    // !!!!!!!!!!!&&& TEST @@@@
      float h = fontAscent + fontDescent;
      rc->drawRect(x, y, textWidth, h, true); // filled
      ((Graph*)this)->setColor(Color::navy);     // !!!!!!!!!!!!!&&&!!!!!!!!
    }
    
    if (font_styles & Font::UNDERLINE) {
      float ypos = y + fontAscent + fontDescent/2;
      rc->drawLine(x, ypos, x+textWidth, ypos);
    }
    
    if (font_styles & Font::OVERLINE) {
      //float ypos = rc->ywin + y;
      //drawLine(x, ypos, x2, ypos);
      rc->drawLine(x, y, x+textWidth, y);
    }
    
    if (font_styles & Font::STRIKETHROUGH) {
      float ypos = y + 3 * fontAscent / 4;
      rc->drawLine(x, ypos, x+textWidth, ypos);
    }
  }
  
  rc->drawString(nf, str, str_len, x, y + fontAscent);
} 

}
