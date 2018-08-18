/**
 *
 *  ugraph.h: Graphics Layer
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


#ifndef UBIT_DRAW_GRAPH_H_
#define	UBIT_DRAW_GRAPH_H_

#include <vector>
#include <ubit/udefs.hpp>
#include <ubit/ucolor.hpp>
#include <ubit/ugeom.hpp>

namespace ubit {
  
  
struct GraphAttributes {
  GraphAttributes() : color_rgba(0u,0u,0u,255u), bgcolor_rgba(255u,255u,255u,255u) {}
  short font_styles;
  float width;
  unsigned long color, bgcolor;      // for 2D_GRAPHICS
  Rgba color_rgba, bgcolor_rgba;    // for GL
  enum {OPAQUE, UNIFORM_BLEND, PSEUDO_BLEND, TRUE_BLEND} blend_mode;
  bool in_xor_mode;
  bool in_3d_mode;      // true when displaying inside a 3Dcanvas
};

  
/** class for drawing on widgets.
  * Widgets inherit from Box. A box can manage one or several views. Graph makes
  * it possible to draw graphic primitives on these views.
  *
  * @see also: Box, View.
 */
class Graph : public GraphAttributes {
public:
  /** for client GL Graphics.
   */
  class GLPaint {
  public:
    GLPaint(PaintEvent&, bool _push_attrib);
    GLPaint(View*, bool _push_attrib);
    virtual ~GLPaint();
    virtual void begin(View*, bool _push_attrib);
    virtual void end();
  protected:
    static int count;
    int no;
    bool push_attrib;
    UHardwinImpl* hardwin;
  };
  
  
  Graph(PaintEvent&);
  /**< constructor for drawing in a UOn::paint callback method.
   * Example:
   * <pre>
   *   void MyObj::paintCB(PaintEvent& e) {
    *       Graph g(e);
    *       g.setColor(Color::red);  // predefined red
    *       g.drawRect(10, 10, 50, 50);
    *       g.setColor(Color(0, 0, 255, 128));  // semi-transparent blue
    *       g.drawLine(10, 10, 10+50, 10+50);
    *   }
    *
    *   int main() {
    *       MyObj* obj = new MyObj();
    *       Box* box = new Box();
    *       box->add(UOn::paint / ucall(obj, &MyObj::paintCB));
    *       .....
    *   }
   * </pre>
   * The last line adds a paint callback to the box. ucall(...) is a callback
   * object that will fire the paintCB() instance method of MyObj each time the box
   * is repainted. paintCB() could also have arguments or be a static function:
   * @see UCall for more details.
   *
   * The paintCB() method has a PaintEvent& parameter that can be passed to 
   * the Graph constructor. This will initialize the Graph object with appropriate
   * attributes and the redraw clip corresponding to this paint event.
   *
   * Note that Graph objects should always be created in this way, so that they 
   * are implicitely destroyed when the function returns. Graph objects may not
   * be copied nor stored for future use.
   *
   * font, colors, and other attributes are undefined when the Graph object
   * is created, they must be specified by using the appropriate functions
   * (such as Graph::setFont(), setColor(), etc.
   *
   * Ubit objects that derive from Box can have multiple View(s). The callback
   * function is then fired once for each view (and the PaintEvent parameter
   * is set accordingly for each view)
   *
   * Note that this constr. raises an exception if the corresponding View is null
   */
  
  virtual ~Graph();
    
  Box* getBox() const;
  /**< returns the widget where this Graph can draw on.
    * this function is a shortcut for: getView()->getBox().
    * @see: getView().
    */
  
  View* getView() const {return boxview;}
  /**< returns the view of the widget where this Graph can draw on.
    * widgets inherit from Box. A box can manage one or several views.
    * view->getBox() returns the box that owns this view (@see View::getBox()).
    */
  
  Display* getDisp() const {return disp;}
  ///< returns the display that is related to this Graph.

  Rectangle getClip() const {Rectangle r; getClip(r); return r;}
  ///< returns the drawing zone relatively to the origin of getView().
  
  void getClip(Rectangle&) const;
  ///< returns the drawing zone relatively to the origin of getView().

  void setClip(const Rectangle&);
  ///< changes the drawing zone relatively to the origin of getView().

  // ==== Attributes ===========================================================

  void setPaintMode();
  /**< sets the drawing mode in Paint mode.
    * "Paint mode" is the default "normal" drawing mode. In Paint Mode, drawing
    * primitives overwrites pixels with the paint color, which must set by calling
    * setColor() and can have an (optional) alpha channel. 
    * @see: setXORMode(), setColor(), setBackground().
    */
    
  void setXORMode(const Color& background_color);
  /** sets the drawing mode in XOR mode.
    * In "XOR mode", drawing primitives alternates the paint and the background 
    * colors, which must be set by calling setColor() and setBackground().
    * NOTES: 
    * - !!! the current OpenGL implementation does NOT support XOR mode !!!
    * - alpha channel cannot be used in in XOR mode
    * @see: setPaintMode(), setColor(), setBackground().
    */
  
  bool isXORMode() const {return in_xor_mode;}
  /**< returns the drawing mode.
    * see setXORMode()
    */
  
  void setColor(const Color&);
  /**< changes the paint color (which can have an alpha component).
    * The paint color is used by all drawing primitives. Example:
    * <pre>
    *   void paintCB(PaintEvent& e) {
    *       Graph g(e);
    *       g.setColor(Color::red);  // predefined red
    *       g.drawRect(10, 10, 50, 50);
    *       g.setColor(Color(0, 0, 255, 128));  // semi-transparent blue
    *       g.drawLine(10, 10, 10+50, 10+50);
    *   }
    * </pre>
    * @see also: setPaintMode(), setXORMode(), setBackground().
    */
    
  void setColor(const Color&, float alpha);
  /**< changes the paint color (forces this alpha component, in range [0,1]).
   * @see setColor(const Color&).
   */
  
  void setColor(const Color&, unsigned int alpha);
  /**< changes the paint color (forces this alpha component, in range [0,255]).
   * @see setColor(const Color&).
    */
    
  void setBackground(const Color&);
  /**< changes the background color.
   */
  
  void setFont(const Font&);
  /**< changes the current Font. 
    */
  
  void setFont(const FontDescription&);
  /**< changes the current Font. 
    */
  
  void getFontMetrics(const Font&, FontMetrics&) const;
  /**< return the corresponding Font Metrics. 
    */
    
  void setWidth(float);
  /**< changes the line width (when drawing lines, etc).
    * in X11 mode the default value is 0, this will draw 1 pixel width lines in an optimized way
    */ 
  
  float getWidth() const;
  /**< returns the current line width.
    * @see: setWidth()
    */
       
  void flush() const;
  ///< flushes all drawing requests.

  // ==== Drawing routines =====================================================
     
  void draw(const Shape&) const;
  ///< draws a shape.
  
  void fill(const Shape&) const;
  ///< fills a shape.

  void drawArc(double x, double y, double width, double height, 
               double start_angle, double ext_angle) const;
  /**< draws a circular or elliptical arc.
    * 'x,y,width,height' are the top/left position and size of the enclosing rectangle
    * 'start_ang' is the beginning angle and 'ext_ang' the extent of the arc relative
    *  to 'start_ang'. Angles are in degrees.
    */
  
  void fillArc(double x, double y, double width, double height, 
               double start_angle, double ext_angle) const;
  ///< fills an arc of a circle (or a complete circle); same arguments as drawArc().
  
  void drawEllipse(double x, double y, double width, double height) const; 
  ///< draws an ellipse or a circle that fits in this rectangle.
        
  void fillEllipse(double x, double y, double width, double height) const; 
  ///< fills an ellipse or a circle that fits in this rectangle.
  
  void drawIma(const Image&, double x, double y, double scale = 1.) const;
  ///< draws this image with this scale.

  void drawLine(double x1, double y1, double x2, double y2) const;
  ///< draws a line.
  
  void drawLine(const Point& p1, const Point& p2) const;
  ///< draws a line.
  
  static const int LINE_STRIP, LINE_LOOP, FILLED;

  void drawPolygon(const std::vector<Point>& points, int type = LINE_LOOP) const;
  /**< draws a polyline, polygon or a filled polygon depending on 'type'.
    * type is one of LINE_STRIP (polyline), LINE_LOOP (polygon), FILLED (filled polygon).
    */
  
  void drawPolygon(const float* points, int npoints, int type = LINE_LOOP) const;
  /**< draws a polyline, polygon or a filled polygon depending on 'type'.
    * 'points' format is x0, y0, x1, y1, x2, y2... it must contains 2*npoints values
    * type is one of LINE_STRIP (polyline), LINE_LOOP (polygon), FILLED (filled polygon).
    */
     
  void drawPolyline(const std::vector<Point>& points) const {drawPolygon(points, LINE_STRIP);}
  ///< draws a polyline.
     
  void fillPolygon(const std::vector<Point>& points) const {drawPolygon(points, FILLED);}
  ///< fills a polygon (that must be convex).
  
  void drawRect(double x, double y, double width, double height) const;
  ///< draws a rectangle.
 
  void fillRect(double x, double y, double width, double height) const;
  ///< fills a rectangle.
    
  void drawRoundRect(double x, double y, double width, double height, 
                     double arc_w, double arc_h) const;
  /**< draws a round-cornered rectangle.
    * arc_w, arc_h are the horiz/vert diameters of the arc at the 4 corners
    */
  
  void fillRoundRect(double x, double y, double width, double height, 
                     double arc_w, double arc_h) const;
  ///< fills a round-cornered rectangle; same arguments as fillRect(), drawRoundRect().
  
  void drawString(const String& string, double x, double y) const;
  ///< draw a character string (the current Font is used: @see setFont()).

  void drawString(const char* string, int length, double x, double y) const;
  ///< draw a character string (the current Font is used: @see setFont()).

  void copyArea(double x, double y, double width, double height,
                double delta_x, double delta_y, 
                bool paint_events_when_obscured) const;
  /** copies a rectangular area (X11 only, does not work with OpenGL).
    *  copies a rectangular area of size (width,height) at location (x,y) 
    *  to a location shifted by delta_x, delta_y
    * <br>Notes: 
    *  - does nothing if the toolkit is using OpenGL as its graphics lib
    *  - CopyArea can't copy obscured parts of the component but will generate refresh
    *    events for repainting the missing parts if the last argument is true 
    */
    

  Graph(View*);
  /**< constructor for drawing in a view.
   * see Graph(PaintEvent&) for details.
   * This constr. raises an exception if the View is null
   */
  
  UHardwinImpl* getHardwin() const {return hardwin;}
  /* [Impl] returns the hard window where this Graph can draw on.
   * Ubit Windows (Window and subclasses) can either be hard (system windows) or
   * soft (simulated), in which case they are drawn on the top of a hard window.
   * getHardwin() always returns the hard window.
   */
  
  void getHardwinClip(Rectangle&) const; 
  ///< [Impl] returns the drawing zone relatively to the current hard window.
  
  void setHardwinClip(const Rectangle&);
  ///< [Impl] changes the drawing zone relatively to the current hard window.
  
  void set3Dmode(bool state);
  bool in3Dmode() const {return in_3d_mode;}
    
  void getWinOffset(float& x, float& y) const;  
  void setWinOffset(float x, float y) const;  
  static void setViewportOrtho(UHardwinImpl*);

private:
  Graph(Graph&);
  Graph& operator=(const Graph&);
protected:
  friend class View;
  friend class U3DCanvasView;
  friend class PaintEvent;
  friend class FontMetrics;
  friend class RenderContext;
  friend class GLContext;
  friend class UX11context;
  Display* disp;         // display 
  View* boxview;      // view where the graphics are drawn
  UHardwinImpl* hardwin;   // corresponding hard window 
  UHardFont* font;     // native font that is currently used
  RenderContext* rc;  // corresponding rendering context
  bool is_client;
};

}
#endif // UBIT_DRAW_GRAPH_H_
