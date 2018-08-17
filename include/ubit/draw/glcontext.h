/*
 *  UGlcontext.h: OpenGL rendering context
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


#if UBIT_WITH_GL

#ifndef _UGlcontext_hpp_
#define	_UGlcontext_hpp_ 1

#include <ubit/ugl.hpp>
#include <ubit/urendercontext.hpp>

namespace ubit {

class UHardImaGL;

class UGlcontext : public URenderContext {
public:
#if UBIT_WITH_GLUT
  UGlcontext(Display*, class UHardwinGLUT*);
#else
  UGlcontext(Display*, UGlcontext* sharelists);
#endif
  virtual ~UGlcontext();
  
  virtual bool isGlcontext() const {return true;}
  virtual UGlcontext* toGlcontext() {return this;}
  virtual const UGlcontext* toGlcontext() const {return this;}
  virtual bool isSharedWith(const URenderContext*) const;

  virtual void setDest(UHardwinImpl* destination, double xoffset, double yoffset);
  virtual void setOffset(double x, double y);
  virtual void setPaintMode(Graph& g);
  virtual void setXORMode(Graph& g, const Color& backcolor);
  virtual void set3Dmode(bool state);
  virtual void setClip(double x, double y, double width, double height);
  
  virtual void setColor(Graph& g, const Color&);
  virtual void setBackground(Graph& g, const Color&);
  virtual void setFont(Graph& g, const UFontDesc&);
  virtual void setWidth(Graph& g, double);

  virtual void makeCurrent() const;
  virtual void swapBuffers();
  virtual void flush();
  
  virtual void drawArc(double x, double y, double w, double h, double start, double ext, bool filled) const;
  virtual void drawIma(const Graph&, const Image&, double x, double y, double scale) const;
  virtual void drawLine(double x1, double y1, double x2, double y2) const; 
  virtual void drawPolygon(const float* points, int card, int polytype) const;
  virtual void drawPolygon(const std::vector<Point>& points, int polytype) const;
  virtual void drawRect(double x, double y, double w, double h, bool filled) const;
  virtual void drawRoundRect(double x, double y, double w, double h, 
                             double arc_w, double arc_h, bool filled) const;
  virtual void drawString(const UHardFont*, const char* str, int str_len, double x, double y) const;
  
  virtual void copyArea(double x, double y, double w, double h, 
                        double delta_x, double delta_y,
                        bool generate_refresh_events_when_obscured) const;
  ///< not available with OpenGL.
  
private:
  double win_height, cx, cy;  // offset from 'dest' origin with cy converted to lower bound
#if UBIT_WITH_X11
  friend class UDispX11;
  GLXContext glxcontext;
#elif UBIT_WITH_GLUT
  class UHardwinGLUT* hardwin;
#endif
  void drawTex(const Graph&, const UHardImaGL*, double x, double y, double width, double height) const;
};

}
#endif
#endif

