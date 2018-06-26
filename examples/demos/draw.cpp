/*************************************************************************
 *
 *  draw.cpp
 *  Ubit Demos - Version 6
 *  (C) 2009 Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
 *
 **************************************************************************
 * COPYRIGHT NOTICE :
 * THIS PROGRAM IS DISTRIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE
 * IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION;
 * EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 * SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
 **************************************************************************/

#include <vector>
#include <list>
#include <cmath>
#include "draw.hpp"
using namespace std;
using namespace ubit;

class DrawCanvas : public UBox {
public:
  DrawCanvas(UArgs args = UArgs::none);
  virtual ~DrawCanvas();
  
  void setSelectionMode(bool);
  
  UPolygon* getCurrentPoly() const {return current_poly;}
  void setCurrentPoly(UPolygon* p) {current_poly = p;}
  
  UPolygon* getSelectedPoly() const {return selected_poly;}
  void setSelectedPoly(UPolygon* p) {selected_poly = p;}

  UPoint getPreviousMousePos() const {return prev_mouse_pos;}

  void addPoly(UPolygon*);
  void deleteSelection();
  void clear();
  void undo();
  void redo();
  
  UPolygon* hit(float x, float y) const;
  UPolygon* hit(const UPoint& p) const {return hit(p.x, p.y);}  
  
  void clearUndos();

protected:
  enum Mode {SELECT, DRAW};
  typedef std::list<UPolygon*> Polys;

  Mode mode;
  UPoint prev_mouse_pos;
  UPolygon *current_poly, *selected_poly;
  Polys polys, undos;
  void mousePressed(UMouseEvent&);
  void mouseReleased(UMouseEvent&);
  void mouseDragged(UMouseEvent&);
  void paintContent(UPaintEvent&);
};

// =============================================================================

void DrawCanvas::setSelectionMode(bool sel) {
  mode = sel ? SELECT : DRAW;
}

DrawCanvas::DrawCanvas(UArgs args) : 
UBox(args), mode(DRAW), current_poly(null), selected_poly(null) 
{
  addAttr
  (UBackground::white
   + UOn::mpress   / ucall(this, &DrawCanvas::mousePressed)
   + UOn::mdrag    / ucall(this, &DrawCanvas::mouseDragged)
   + UOn::mrelease / ucall(this, &DrawCanvas::mouseReleased)
   + UOn::paint    / ucall(this, &DrawCanvas::paintContent)
   );  
}

DrawCanvas::~DrawCanvas() {
  clearUndos();
  for (Polys::iterator p = polys.begin(); p != polys.end(); ++p) 
    delete *p;
  polys.clear();
}

void DrawCanvas::clear() {
  clearUndos();
  for (Polys::const_iterator p = polys.begin(); p != polys.end(); ++p) 
    undos.push_front(*p);  // inverser!
  polys.clear();
  selected_poly = null;
  repaint();
}

void DrawCanvas::clearUndos() {
  for (Polys::iterator p = undos.begin(); p != undos.end(); ++p) 
    delete *p;
  undos.clear();  
}

void DrawCanvas::addPoly(UPolygon* p) {
  clearUndos();
  polys.push_back(p);
  repaint();
}

void DrawCanvas::deleteSelection() {
  if (!selected_poly) return;
  /*
  Polys::iterator p = polys.begin();
  for (; p != polys.end(); ++p) {if (*p == selected_poly) break;}
  if (p == polys.end()) return;
  */
  undos.push_back(selected_poly);
  polys.remove(selected_poly);
  selected_poly = null;
  repaint();
}


void DrawCanvas::undo() {
  if (!polys.empty()) {
    if (polys.back() == selected_poly) selected_poly = null;
    undos.push_back(polys.back());
    polys.pop_back();
  }
  repaint();
}

void DrawCanvas::redo() {
  if (!undos.empty()) {
    polys.push_back(undos.back());
    undos.pop_back();
  }
  repaint();
}

UPolygon* DrawCanvas::hit(float x, float y) const {
  //cerr <<"hit " << x << " " << y << endl;
  for (Polys::const_reverse_iterator p = polys.rbegin(); p != polys.rend(); ++p) {
    bool found = (*p)->intersects(URect(x-2, y-2, 5, 5));
    //bool found = (*p)->contains(UPoint(x, y));
    if (found) return *p;
  }
  return null;
}

void DrawCanvas::paintContent(UPaintEvent& e) {
  UGraph g(e);
  g.setWidth(1);
  
  for (Polys::const_iterator p = polys.begin(); p != polys.end(); ++p) {
    if ((*p) == selected_poly) g.setColor(UColor::red);
    else g.setColor(UColor::black);
    (*p)->draw(g);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void DrawCanvas::mousePressed(UMouseEvent& e) {
  if (mode == SELECT) {
    setSelectedPoly(hit(e.getPos()));
  }
  else if (mode == DRAW) {
    UPolygon* p = new UPolygon(false); // not closed, ie. Polyline
    p->addPoint(e.getPos());
    this->addPoly(p);
    setCurrentPoly(p);
  }
  repaint();
  prev_mouse_pos = e.getPos();
}

void DrawCanvas::mouseDragged(UMouseEvent& e) {
  if (mode == SELECT) {
    UPolygon* p = getSelectedPoly();
    if (p) p->translate(e.getPos().x - prev_mouse_pos.x, 
                        e.getPos().y - prev_mouse_pos.y);
  }
  else if (mode == DRAW) {
    UPolygon* p = getCurrentPoly();
    if (p) p->addPoint(e.getPos());
  }
  repaint();
  prev_mouse_pos = e.getPos();
}

void DrawCanvas::mouseReleased(UMouseEvent& e) {
  if (mode == SELECT) {
  }
  else if (mode == DRAW) {
    setCurrentPoly(null);
  }
  repaint();
  prev_mouse_pos = e.getPos();
}

// =============================================================================

DrawDemo::DrawDemo(UArgs args) : UBox(args), canvas(new DrawCanvas)
{  
  UBox& mode_box = uradiobox
  (ubutton(" Select " + ucall(canvas, true, &DrawCanvas::setSelectionMode))
   + ubutton(" Draw " + ucall(canvas, false, &DrawCanvas::setSelectionMode))
   ).select(1);

  UBox& controls = uhbox
  (uleft()
   + mode_box
   + "    " 
   + (undo_btn  = ubutton(" Undo " + ucall(canvas, &DrawCanvas::undo)))
   + (redo_btn  = ubutton(" Redo " + ucall(canvas, &DrawCanvas::redo)))
   + (clear_btn = ubutton(" Clear "+ ucall(canvas, &DrawCanvas::clear)))
   + "  "
   + (del_btn = ubutton(" Del " + ucall(canvas, &DrawCanvas::deleteSelection)))
   //+ ubutton(" Test " + ucall(this, &DrawCanvas::test))
  );
  
  canvas->addAttr(usize(1000, 1000));
  
  add
    (UOrient::vertical + upadding(5,5)
   + utop() + controls
   + uvflex() + uscrollpane(canvas)
   );
}
