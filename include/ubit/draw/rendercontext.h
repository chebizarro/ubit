/*
 *  URenderContext.h: generic rendering context
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


#ifndef _urendercontext_hpp_
#define	_urendercontext_hpp_ 1

#include <ubit/ui/uwinImpl.hpp>

namespace ubit {

class URenderContext {
public: 
  virtual bool isGlcontext() const = 0;
  virtual UGlcontext* toGlcontext() = 0;
  virtual const UGlcontext* toGlcontext() const = 0;
  
  virtual bool isSharedWith(const URenderContext*) const = 0;
  ///< true if the same internal OpenGL reendering context is shared.
  
  Display* getDisp() const {return disp;}
  UHardwinImpl* getDest() const {return dest;}
  
  virtual void setDest(UHardwinImpl* dest, double xoffset, double yoffset) = 0;
  /* changes the destination drawable and the corresponding coordinate system.
   * Note that this function does not only change the destination but also the coordinate
   * system according to the destination's size. This is especially important when GL 
   * is used for rendering because the GL origin is the bottom left point of the drawable
   * while the Ubit origin is the top left point (as for most GUI toolkits).
   * Hence:
   * - when GL is used, Y coordinates are converted to drawable_height - Y 
   *   before calling drawing functions
   * - besides, setDest() sets the 'logical offset' to 0,0 (@see setOffset())
   */
   
  virtual void setOffset(double x, double y) = 0;
  /* changes the 'logical offset'.
   * this
   * The 'logical offset' if applied to X,Y coordinates before calling drawing functions.
   * This offset is 'logical' in that sense that it is related to the Ubit coordinate
   * system, where the origin is the top left point of the drawable.
   always related to the Ubit 
   */
  
  virtual void setPaintMode(Graph&) = 0;
  /* sets this context in paint mode (the default mode).
   * paint mode is the default (and the only mode available when GL is used)
   * the foreground color is initialized according to Graph values.
   */
  
  virtual void setXORMode(Graph&, const Color& backcolor) = 0;
  /* sets this context in XOR mode (not available with GL).
   * XOR mode is not available when GL is used
   * the foreground color is initialized according to Graph values and the
   * background color according to the second argument.
   */
  
  virtual void set3Dmode(bool state) = 0;
  /// impl.
  
  virtual void setClip(double x, double y, double width, double height) = 0;  
  /// impl.
  
  const Rectangle& getClip() const {return clip;}
  
  virtual void setColor(Graph&, const Color&) = 0;
  virtual void setBackground(Graph&, const Color&) = 0;
  virtual void setFont(Graph&, const UFontDesc&) = 0;
  virtual void setWidth(Graph&, double) = 0;

  virtual void makeCurrent() const = 0;
  /* makes this context the current rendering context.
   * calls glMakeCurrent() if this context is a UGlcontext
   */
  
  virtual void swapBuffers() = 0;
  /* swaps the front and back buffers of the current destination drawable.
   * calls glSwapBuffers() if this context is a UGlcontext
   */
  
  virtual void flush() = 0;
  /* flushes all drawing request.
   * calls glFlush() when GL is used.
   */
  
  virtual void drawArc(double x, double y, double w, double h, 
                       double start, double ext, bool filled) const = 0;

  virtual void drawIma(const Graph&, const Image&, double x, double y, 
                       double scale) const = 0;

  virtual void drawLine(double x1, double y1, double x2, double y2) const = 0;  

  virtual void drawPolygon(const float* points, int card, int polytype) const = 0;
  virtual void drawPolygon(const std::vector<Point>& points, int polytype) const = 0;
  
  virtual void drawRect(double x, double y, double w, double h, bool filled) const = 0;
  virtual void drawRoundRect(double x, double y, double w, double h, 
                             double arc_w, double arc_h, bool filled) const = 0;

  virtual void drawString(const UHardFont*, const char* str, int str_len, 
                          double x, double y) const = 0;
  
  virtual void copyArea(double x, double y, double w, double h, double delta_x, double delta_y,
                        bool generate_refresh_events_when_obscured) const = 0;

protected:
  URenderContext(Display* d) : disp(d), dest(null), xwin(0), ywin(0), own_dest(false) {}  
  virtual ~URenderContext() {if (own_dest) delete dest;}

  friend class Graph;
  friend class Display;
  Display* disp;
  UHardwinImpl* dest;    // where graphics are drawn (can be a Window or a Pixmap)
  Rectangle clip;
  double xwin, ywin; // offset from 'dest' origin
  bool own_dest;     // true if 'dest' is a pixmap that must thus be destroyed with this GC
};

}
#endif
