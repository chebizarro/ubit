/* ==================================================== ======== ======= *
*
*  toolglass.cpp a simple toolglass that draws lines, rectangles and ellipses.
*  Ubit Demos - Version 6
*  (C) 2009 Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
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

#include <vector>
#include <cmath>
#include <ubit/ubit.hpp>
#include "toolglass.hpp"
using namespace std;
using namespace ubit;


struct GraphShape {
  GraphShape();
  void setType(int shape_type) {type = shape_type;}
  void setColor(const UColor* c) {color = c;}
  void setWidth(double w) {width = w;}
  void setBounds(const UPoint& p1, const UPoint& p2);
  void createShape();
  void draw(UGraph&);
  
  int type;
  float width;
  uptr<const UColor> color;
  uptr<UShape> shape;
};

typedef std::list<GraphShape> GraphShapes;

// =============================================================================

class Canvas : public UBox {
public:
  Canvas(UArgs = UArgs::none);
  
  void clear();  
  void createShape(const UPoint&);
  void mofifyShape(const UPoint&);
  void paintCB(UPaintEvent&);
  
  GraphShapes gshapes;
  GraphShape next_gshape;
  UPoint first_point;
};

// =============================================================================

GraphShape::GraphShape()
: type(UShape::LINE), width(1), color(UColor::black), shape(null) {}

void GraphShape::createShape() {
  switch (type) {
    case UShape::LINE: 
      shape = new ULine();
      break;
    case UShape::RECT: 
      shape = new URect();
      break;
    case UShape::ELLIPSE: 
      shape = new UEllipse();
      break;
    default:
      UAppli::warning("GraphShape::createShape","Invalid shape type");
      break;
  }
}

void GraphShape::setBounds(const UPoint& p1, const UPoint& p2) {
  if (shape == null) createShape();
  UShape* s = shape;
  switch (type) {
    case UShape::LINE: 
      ((ULine*)s)->setLine(p1, p2);
      break;
    case UShape::RECT: 
      ((URect*)s)->setFrame(URect(p1, p2));
      break;
    case UShape::ELLIPSE: 
      ((UEllipse*)s)->setFrame(URect(p1, p2));
      break;
    default:
      UAppli::warning("GraphShape::setBounds","Invalid shape type");
      break;
  }  
}

void GraphShape::draw(UGraph& g) {
  if (shape) {
    g.setColor(*color);
    g.setWidth(width);
    g.draw(*shape);
  }
} 

// =============================================================================

Canvas::Canvas(UArgs args) : UBox(args) {
  addAttr(UBackground::white);
  addAttr(UOn::paint / ucall(this, &Canvas::paintCB));
}

void Canvas::createShape(const UPoint& pt) {
  first_point = pt;
  gshapes.push_back(next_gshape);
}

void Canvas::mofifyShape(const UPoint& pt) {
  if (gshapes.begin() != gshapes.end()) {
    gshapes.back().setBounds(first_point, pt);
    repaint();
  }
}

void Canvas::paintCB(UPaintEvent& e) {
  UGraph g(e);
  for (GraphShapes::iterator p = gshapes.begin(); p != gshapes.end(); ++p) 
    (*p).draw(g);
}

void Canvas::clear() {
  gshapes.clear();
  repaint();   // repaint the canvas 
}

// =============================================================================

class GlassButton : public UItem {
public:
  GlassButton(Canvas& c, UArgs args) : canvas(c) {
    addAttr(ualpha(0.5) 
            + UOn::mpress / ucall(this, &GlassButton::pressCB)
            + UOn::mdrag  / ucall(this, &GlassButton::dragCB)
            );
    add(uhcenter() + uvcenter() + args);
  }
  
  virtual void pressCB(UMouseEvent& e) {
    canvas.createShape(e.getPosIn(canvas));
  }
  virtual void dragCB(UMouseEvent& e) {
    canvas.mofifyShape(e.getPosIn(canvas));
  }
  
protected:
  Canvas& canvas;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct ShapeButton : public GlassButton {
  int shape_type;
  
  ShapeButton(Canvas& c, int t, UArgs args) : GlassButton(c, args), shape_type(t) {
    addAttr(UOn::select / UBackground::orange + UOn::select / UColor::white);
  }
  
  virtual void pressCB(UMouseEvent& e) {
    canvas.next_gshape.setType(shape_type);
    GlassButton::pressCB(e);   
  }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct WidthButton : public GlassButton {
  float width;
  
  WidthButton(Canvas& c, float w, UArgs args) : GlassButton(c, args), width(w) {
    addAttr(UOn::select / UBackground::orange + UOn::select / UColor::white);
  }
  
  virtual void pressCB(UMouseEvent& e) {
    canvas.next_gshape.setWidth(width);
    GlassButton::pressCB(e);   
  }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct ColorButton : public GlassButton {
  UColor* color;
  
  ColorButton(Canvas& c, UColor& col, UArgs args) : GlassButton(c, args), color(&col) {
    static UBorder* b = new UBorder(UBorder::LINE, UColor::white, UColor::blue);
    addAttr(*b);
  }
  
  virtual void pressCB(UMouseEvent& e) {
    canvas.next_gshape.setColor(color);
    GlassButton::pressCB(e);   
  }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class Toolglass : public UPalette {
public:
  Toolglass(Canvas& canvas) {  
    UArgs lprops = UBorder::none + uvspacing(1) + upadding(0,0);
    
    UListbox& shapebox = ulistbox
      (lprops + UFont::small 
       + new ColorButton(canvas, UColor::black, UBackground::black + "     ")
       + new ColorButton(canvas, UColor::yellow,UBackground::yellow + UFont::small + "     ")
       + new ColorButton(canvas, UColor::red, UBackground::red + UFont::small + "     ")
       + new ColorButton(canvas, UColor::green, UBackground::green + UFont::small + "     ")
       );
    shapebox.setTransparent();
    shapebox.setSelectedIndex(0);
    
    UListbox& widthbox = ulistbox
      (lprops
       + new WidthButton(canvas, 1, "  1  ")
       + new WidthButton(canvas, 3, "  3  ")
       + new WidthButton(canvas, 5, "  5  ")
       + new WidthButton(canvas, 7, "  7  ")
       );
    widthbox.setTransparent();
    widthbox.setSelectedIndex(0);
    
    UListbox& colorbox = ulistbox
      (lprops
       + new ShapeButton(canvas, UShape::LINE, "Line")
       + new ShapeButton(canvas, UShape::RECT, "Rect")
       + new ShapeButton(canvas, UShape::ELLIPSE, "Ell.")
       );
    colorbox.setTransparent();
    colorbox.setSelectedIndex(0);
    
    title().addAttr(UColor::blue).add("Palette");
    content().addAttr(UOrient::horizontal + uhspacing(1)
                      + UBackground::grey + ualpha(0.15));
    content().add(shapebox + widthbox + colorbox);
  }
};
  
// =============================================================================

ToolglassDemo::ToolglassDemo() {
  Canvas* canvas = new Canvas();
  glass = new Toolglass(*canvas);
  glass->pos() = UPoint(10, 10);
  canvas->add(glass);
  
  UBox& toolbox = uhbox(uleft() + ubutton("Clear" + ucall(canvas, &Canvas::clear)));
  
  add(UOrient::vertical + uhflex() + upadding(5,5)
      + utop() + toolbox 
      + uvflex() + canvas
      );
}


