/*
 *  geometry.h: graphics geometry
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

#ifndef UBIT_CORE_GEOMETRY_H_
#define	UBIT_CORE_GEOMETRY_H_

#include <ubit/core/object.h>

namespace ubit {
    
   /**
    * 2D Point.
    */
  class Point {
  public:
    float x, y;
    
    Point() : x(0), y(0) {}
    Point(float _x, float _y) : x(_x), y(_y) {}
    
    float getX() const {return x;}
    float getY() const {return y;}
    
    void set(float _x, float _y) {x = _x, y = _y;}
    void set(const Point& p) {*this = p;}
    
    friend std::ostream& operator<<(std::ostream&, const Point&);
    ///< prints the point on an output stream (eg: cout << point).
    
    friend std::istream& operator>>(std::istream&, Point&);
    ///< reads the point from an input stream (eg: cin >> point).  
  };
  
  std::ostream& operator<<(std::ostream&, const Point&);
  ///< prints the point on an output stream (eg: cout << point).
  
  std::istream& operator>>(std::istream&, Point&);
  ///< reads the point from an input stream (eg: cin >> point).  
  
  
    /** 2D Dimension.
   */
  class Dimension {
  public:
    float width, height;
    
    Dimension() : width(0), height(0) {}
    Dimension(float _w, float _h) : width(_w), height(_h) {}
    
    float getWidth()  const {return width;}
    float getHeight() const {return height;}
    
    void set(float _w, float _h) {width = _w, height = _h;}
    void set(const Dimension& d) {*this = d;}
    
    friend std::ostream& operator<<(std::ostream&, const Dimension&);
    ///< prints the dimension on an output stream (eg: cout << dimension).
    
    friend std::istream& operator>>(std::istream&, Dimension&);
    ///< reads the dimension from an input stream (eg: cin >> dimension).    
  };
  
  std::ostream& operator<<(std::ostream&, const Dimension&);
  ///< prints the dimension on an output stream (eg: cout << dimension).
  
  std::istream& operator>>(std::istream&, Dimension&);
  ///< reads the dimension from an input stream (eg: cin >> dimension).
  
  
  /** Abstract Base class for geometrical shapes.
   */
  class Shape : public UObject {
  public:
    UABSTRACT_CLASS(Shape)
    
    enum ShapeType {
      LINE    = 1,
      RECT    = 2,
      ELLIPSE = 3,
      ARC     = 4,
      POLYGON = 5
    };
    
    virtual int getShapeType() const  = 0;
    
    virtual Rectangle getBounds() const = 0;
    ///< returns the bounding box of this shape.
    
    virtual bool contains(const Point&) const = 0; 
    ///< tests if a given point is inside the boundary of this Shape.
    
    virtual bool contains(const Rectangle&) const = 0; 
    ///< tests if the interior of this Shape entirely contains the specified Rectangle
    
    virtual bool intersects(const Rectangle&) const = 0; 
    ///< tests if this Shape intersects the interior of a specified Rectangle.
    
    virtual void draw(const Graph&) const = 0;
    virtual void fill(const Graph&) const = 0;
  };
  
    /** 2D Line.
   */
  class Line : public Shape {
  public:
    UCLASS(Line)

    Point p1, p2;
    
    int getShapeType() const {return LINE;}
    
    Line() {}
    Line(const Point& _p1, const Point& _p2) : p1(_p1), p2(_p2) {}
    
    Rectangle getBounds() const;
    Point getP1() const {return p1;}
    Point getP2() const {return p2;}
    
    virtual void setLine(const Point& _p1, const Point& _p2) {p1 = _p1; p2 = _p2;}
    virtual void setLine(const Line& l) {*this = l;}
    
    bool contains(const Point&) const {return false;}    
    bool contains(const Rectangle&) const {return false;}
    
    bool intersects(const Rectangle&) const; 
    ///< tests if this Line intersects the interior of a specified Rectangle.
    
    bool intersectsLine(const Line&) const;
    ///< tests if the specified line segment intersects this Line.
    
    static bool linesIntersect(const Point& a, const Point& b, const Point& c, const Point& d);
    ///< tests if the line segment [A,B] intersects the line segment from [C,D]
    
    static int relativeCCW(Point a, Point b, Point p);
    /**< Returns an indicator of where P lies with respect to the line segment [A,B].
     * The return value can be either 1, -1, or 0 and indicates in which direction 
     * the specified line must pivot around its first endpoint, A, in order to point 
     * at the specified point P.
     */
    
    int relativeCCW(const Point&) const;
    ///< Returns an indicator of where the specified Point2D lies with respect to this line segment.
    
    virtual void draw(const Graph&) const;
    virtual void fill(const Graph&) const {};
  };
  
    /** 2D Rectangle.
   */
  class Rectangle : public Shape {
  public:
    UCLASS(Rectangle)

    float x, y, width, height;
    
    int getShapeType() const {return RECT;}  
    
    Rectangle() : x(0), y(0), width(0), height(0) {}
    Rectangle(float _x, float _y, float _w, float _h) : x(_x), y(_y), width(_w), height(_h) {}
    Rectangle(const Point& p1, const Point& p2) {setRect(p1, p2);}
    
    Rectangle getBounds() const {return *this;}
    bool isEmpty() const {return width <= 0.0 || height <= 0.0;}
    
    void setRect(const Rectangle& r) {*this = r;}
    void setRect(float x, float y, float width, float height);
    void setRect(const Point& p1, const Point& p2);
    void setFrame(const Rectangle& r) {*this = r;}
    
    bool contains(const Point&) const;
    bool contains(const Rectangle&) const;
    
    bool intersects(const Rectangle&) const; 
    ///< tests if this Rectangle intersects the interior of a specified Rectangle.
    
    bool intersectsLine(const Line&) const; 
    ///< tests if the specified line segment intersects the interior of this Rectangle.
    
    bool doIntersection(const Rectangle&);
    /**< performs the intersection with the rectangle that is given as an argument.
     * returns false if there is no intersection (this rectangle is then left unchanged)
     */
    
    void doUnion(const Rectangle&);
    /**< performs the union with the rectangle that is given as an argument.
     * Note: empty regions are not taken into account.
     */
    
    enum {OUT_LEFT = 1, OUT_TOP = 2, OUT_RIGHT = 4, OUT_BOTTOM = 8};
    
    int outcode(const Point& p) const;
    ///< determines where the specified coordinates lie with respect to this Rectangle.
    
    virtual void draw(const Graph&) const;
    virtual void fill(const Graph&) const;
  };
  
    /** 2D Ellipse.
   */
  class Ellipse : public Shape {
  public:
    UCLASS(Ellipse)

    float x, y, width, height;
    
    int getShapeType() const {return ELLIPSE;}  
    
    Ellipse();
    Ellipse(const Rectangle&);
    Ellipse(float x, float y, float weight, float height);
    
    Rectangle getBounds() const;
    bool isEmpty() const {return width <= 0.0 || height <= 0.0;}
    
    void setFrame(const Rectangle&);
    void setFrame(float x, float y, float width, float height);
    
    bool contains(float x, float y) const;
    bool contains(const Point&) const;
    bool contains(const Rectangle&) const;
    bool intersects(const Rectangle&) const; 
    
    virtual void draw(const Graph&) const;
    virtual void fill(const Graph&) const;
  };
  
    /** 2D Polygon or Polyline.
   */
  class Polygon : public Shape {
  public:
    UCLASS(Polygon)

    virtual int getShapeType() const {return POLYGON;}
    
    virtual ~Polygon();
    
    Polygon(bool closed = true);
    ///< creates an empty polygon ('closed'=true) or polyline ('closed'=false).
    
    Polygon(const std::vector<Point>& points, bool closed = true);
    ///< creates and initalizes a polygon ('closed'=true) or polyline ('closed'=false).
    
    Polygon(const Polygon& poly2);
    ///< creates a new polygon (or a polyline) that is a copy of 'poly2'.
    
    Polygon& operator=(const Polygon& poly2);
    ///< copies the content of 'poly2' to this polygon.
    
    Polygon& addPoint(const Point& point);
    ///< adds a point to the polygon.
    
    Polygon& addPoints(const std::vector<Point>& points);
    ///< adds these points to the polygon.
    
    Polygon& addPoints(const float* _points, int _npoints);
    ///< adds these points to the polygon.
    
    Rectangle getBounds() const {return bounds;}
    ///< returns the bounding box of the polygon.
    
    void translate(float dx, float dy);
    ///< translates the polygon. 
    
    void reset();
    ///< resets this Polygon object to an empty polygon.  
    
    bool contains(const Point&) const;
    bool contains(const Rectangle&) const;
    bool intersects(const Rectangle&) const;
    
    void draw(const Graph&) const;
    void fill(const Graph&) const;
    
  private:
    int npoints, memsize;
    bool closed;
    float* points;   // the array contains x0, y0, x1, y1, x2, y2...
    Rectangle bounds;
    int evaluateCrossings(float x, float y, bool useYaxis, float distance) const;
    void augment(int n);
    void updateBounds(float x, float y);
  };
  
}
#endif // UBIT_CORE_GEOMETRY_H_
