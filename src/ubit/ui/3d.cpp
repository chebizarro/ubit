/*
 *  3d.cpp: 3D Widgets
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2008 | Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
 *
 * ***********************************************************************
 * COPYRIGHT NOTICE :
 * THIS PROGRAM IS DISTRIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE
 * IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION;
 * EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 * SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
 * ***********************************************************************/

#include <ubit/ubit_features.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>
#include <ubit/ubit.hpp>
#include <ubit/ugl.hpp>
#include <ubit/core/event.h>
#include <ubit/uupdate.hpp>
#include <ubit/ui/updatecontext.h>
#include <ubit/ui/uviewImpl.hpp>
#include <ubit/u3d.hpp>
#include <ubit/u3dicon.hpp>
#include <ubit/ustyle.hpp>
#include <ubit/core/uappliImpl.hpp>
#include <ubit/ugraph.hpp>
using namespace std;
namespace ubit {


static const float FOVY = 70.;
static const float NEAR_PLANE = 1.;
static const float FAR_PLANE = 1000.;
static const float SUPERPOSE = 1.;
static const float MAG = 0.5;

// =============================================================================
    
U3Dpos::U3Dpos() : UPos(0, 0), 
z(0), x_rot(0), y_rot(0), z_rot(0) {}

U3Dpos::U3Dpos(float _x, float _y, float _z) : UPos(_x, _y), 
z(_z), x_rot(0), y_rot(0), z_rot(0) {}

U3Dpos& U3Dpos::setTrans(float _x, float _y, float _z) {
  setImpl(_x,_y,_z, true); 
  return *this;
}

U3Dpos& U3Dpos::translate(float dx, float dy, float dz) {
  setImpl(x.val+dx, y.val+dy, z+dz, true);       // Unit ignor� !!!
  return *this;
}

void U3Dpos::setImpl(float _x, float _y, bool upd) {
  // faudrait tester si a change !!!
  x = _x; y = _y;     // z inchanged!
  changed(upd);
}

void U3Dpos::setImpl(float _x, float _y, float _z, bool upd) {
  // faudrait tester si a change !!!
  x = _x; y = _y; z = _z;
  changed(upd);
}

U3Dpos& U3Dpos::setRot(float _x_rot, float _y_rot, float _z_rot, bool upd) {
  // faudrait tester si a change !!!
  x_rot = _x_rot; y_rot = _y_rot; z_rot = _z_rot;
  changed(upd);
  return *this;
}

U3Dpos& U3Dpos::rotate(float dx_rot, float dy_rot, float dz_rot) {
  setRot(x_rot + dx_rot, y_rot + dy_rot, z_rot + dz_rot);
  return *this;
}

void U3Dpos::update() {
  updateAutoParents(Update::paint);
}

void U3Dpos::putProp(UpdateContext* ctx, Element&) {
  ctx->pos = this;
}

// =============================================================================

struct U3DposHandle : public Attribute {
  UCLASSDEF("U3DposHandle", U3DposHandle, null)
  
  enum Transform {XYPOS, ZPOS, XROT, YROT, ZROT}; 
  
  U3DposHandle(U3Dpos&, Transform);
  virtual ~U3DposHandle();
  
  // - - - impl - - - - -- - - - - - - - - - - - - - - - - - - - - - - -
  virtual void putProp(UpdateContext*, Element&) {}
  virtual void addingTo(Child&, Element& parent);
  virtual void removingFrom(Child&, Element& parent);
  // NOTE that removingFrom() requires a destructor to be defined.
  
protected:
  Transform transform;
  uptr<U3Dpos> pos;
  UCall* callbacks;
  Point prev;

  virtual void mouseCB(MouseEvent&);
  virtual void pressCB(MouseEvent&);
  virtual void releaseCB(MouseEvent&);
  virtual void dragCB(MouseEvent&);
  virtual void doubleClickCB(MouseEvent&);
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

U3DposHandle::U3DposHandle(U3Dpos& _pos, Transform _t) :
transform(_t), pos(_pos), callbacks(null) {}

U3DposHandle::~U3DposHandle() {}

void U3DposHandle::addingTo(Child& c, Element& parent) {
  Attribute::addingTo(c, parent);
  if (callbacks) {
    Application::error("U3DposHandle::addingTo","a U3DposHandle cannot be added to several parents; U3DposHandle object: %p",this);
    delete callbacks;
  }
  callbacks = &ucall(this, &U3DposHandle::mouseCB);
  parent.addAttr(UOn::mdrag/callbacks + UOn::mpress/callbacks + UOn::mrelease/callbacks
                 + UOn::doubleClick/callbacks);
}

void U3DposHandle::removingFrom(Child& c, Element& parent) {
  delete callbacks;
  Attribute::removingFrom(c, parent);
}

void U3DposHandle::mouseCB(MouseEvent& e) {
  if (e.getCond() == UOn::mdrag) dragCB(e);
  else if (e.getCond() == UOn::mpress) pressCB(e);
  else if (e.getCond() == UOn::mrelease) releaseCB(e);
  else if (e.getCond() == UOn::doubleClick) doubleClickCB(e);
}

void U3DposHandle::pressCB(MouseEvent& e) {
  prev.x = e.getScreenPos().x;
  prev.y = e.getScreenPos().y;
}

void U3DposHandle::releaseCB(MouseEvent& e) {}

void U3DposHandle::dragCB(MouseEvent& e) {
  Point screen_pos = e.getScreenPos();
  float dx = screen_pos.x - prev.x;
  float dy = -screen_pos.y + prev.y;   // !!!
  prev = screen_pos;
   
  switch (transform) {
    case XYPOS:
      pos->translate(dx, dy, 0);
      break;
    case ZPOS:
      pos->translate(0, 0, dx);
      break;
    case XROT:
      pos->rotate(MAG*dx, 0, 0);
      break;
    case YROT:
      pos->rotate(0, MAG*dx, 0);
      break;
    case ZROT :
      pos->rotate(0, 0, MAG*dx);
      break;
  }
}

void U3DposHandle::doubleClickCB(MouseEvent&) {
  cerr << "U3DposHandle::doubleClickCB " << endl;
  switch (transform) {
    case XYPOS:
      pos->setRot(0, 0, 0);
      break;
    case ZPOS:
      pos->setTrans(pos->getX().val, pos->getY().val, -10);
      break;
    case XROT:
      pos->setRot(0, pos->getYRot(), pos->getZRot());
      break;
    case YROT:
      pos->setRot(pos->getXRot(), 0, pos->getZRot());
      break;
    case ZROT :
      pos->setRot(pos->getXRot(), pos->getYRot(), 0);
      break;
  }
}

// =============================================================================

Style* U3Dbox::createStyle() {
  return Box::createStyle();
}

U3Dbox::U3Dbox(Args args) : Box(args), ppos(new U3Dpos) {
  addAttr(*ppos);
}

U3Dbox& U3Dbox::setTrans(float x, float y, float z) {
  ppos->setTrans(x, y, z); return *this;
}

U3Dbox& U3Dbox::setRot(float xrot, float yrot, float zrot) {
  ppos->setRot(xrot, yrot, zrot); return *this;
}

U3Dbox& U3Dbox::translate(float dx, float dy, float dz) {
  ppos->translate(dx, dy, dz); return *this;
}

U3Dbox& U3Dbox::rotate(float dxrot, float dyrot, float dzrot) {
  ppos->rotate(dxrot, dyrot, dzrot); return *this;
}

// =============================================================================
  
Style* U3Dwin::createStyle() {
  Style* style = Box::createStyle();
  style->orient = Orientation::VERTICAL;
  style->local.border = &Border::etchedOut;
  return style;
}
  
U3Dwin::U3Dwin(Args args) : U3Dbox() {  
  add(uhflex() 
      + utop() + createTitleBar("")           // !!!! title !!!!
      + uvflex() + args
      );
}
  
Box& U3Dwin::createTitleBar(const String& title) {
  return 
  uhbox(*new U3DposHandle(*ppos, U3DposHandle::XYPOS) 
        + uleft()
        + ualpha(0.5) + Background::navy + upadding(UIGNORE,2) + Color::white 
        + Font::bold + " " + uelem(ustr(title)) + "         "
        + ubox(*new U3DposHandle(*ppos, U3DposHandle::ZPOS)
               + Background::white + Color::navy + Font::x_large + " zZ ")
        + uelem(uscale(0.6) //+ Background::white
                   + ustr(" ")  // ustr needed by CC for some reason 
                   + ubox(*new U3DposHandle(*ppos, U3DposHandle::YROT) + U3Dicon::y_rot)
                   + ubox(*new U3DposHandle(*ppos, U3DposHandle::XROT) + U3Dicon::x_rot)
                   + ubox(*new U3DposHandle(*ppos, U3DposHandle::ZROT) + U3Dicon::z_rot)
                   )
        );
  /*
   + uhflex() 
   + ubox(Background::none + " ").ignoreEvents()
   + ubox(" ").ignoreEvents()
   + uright() 
   + ubox(Background::none + UOn::arm/Color::red + "X" //+ ucall(this, &Lens::deleteLens))
   */
}

// =============================================================================

Style* U3Dcanvas::createStyle() {
  Style* style = Box::createStyle();
  style->viewStyle = &3DCanvasView::style;
  style->halign = Halign::LEFT;
  style->valign = Valign::TOP;
  return style;
}

U3Dcanvas::U3Dcanvas(Args a) : Box(a),
fovy(FOVY), aspect(1.), near(NEAR_PLANE), far(FAR_PLANE) {
#ifdef UBIT_WITH_GL
  Application::conf.is_using_gl = true;
#else
  Application::error("U3Dcanvas::U3Dcanvas", 
                "U3Dcanvas requires OpenGL and FreeType (see ./configure --help for compilation options of the Ubit toolkit)");
#endif
}

U3Dcanvas::~U3Dcanvas() {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

ViewStyle 3DCanvasView::style(&3DCanvasView::createView, UCONST);

void 3DCanvasView::doUpdate(UpdateContext& ctx, Rectangle r, Rectangle clip, UViewUpdate& vu) {
#ifndef UBIT_WITH_GL
  View::doUpdate(ctx, r, clip, vu);
#else
  int frame_width  = int(getWin()->getWidth());
  int frame_height = int(getWin()->getHeight());
  
  Graph* g = ctx.getGraph();  // g may be null!
  if (g) {
    g->set3Dmode(true);
    // set the viewport for drawing in the 3Dcanvas
    //cerr << "<<3DCanvasView glViewport: "<< int(x)<<" "<<int(frame_height - height - y)<<" "<< int(width)<<" "<< int(height)<< endl;
    
    glViewport(int(x), int(frame_height - height - y), int(width), int(height));
     
    glClear(GL_DEPTH_BUFFER_BIT);
    glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    //glEnable(GL_POINT_SMOOTH);     // utile?
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);   // !!! problematique...
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    
    U3Dcanvas* canvas = (U3Dcanvas*)getBox();
    //cerr << "- persp "<< canvas->fovy <<" "<<(canvas->aspect*width/height) << " "<< canvas->near<< " "<< canvas->far<<endl;
    gluPerspective(canvas->fovy, canvas->aspect*width/height, canvas->near, canvas->far);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }
  
  View::doUpdate(ctx, r, clip, vu);
  
  if (g) {
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glPopAttrib();
    glDisable(GL_DEPTH_TEST);
    
    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_POLYGON_SMOOTH);
    
    // set the viewport for drawing in the whole hard window
    //cerr << ">>3DCanvasView glViewport: 0 0 " << frame_width<< " " << frame_height << endl << endl;
    glViewport(0, 0, frame_width, frame_height);

    g->set3Dmode(false);
  }
#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct MatchView {
public:
  Point p;
  float distance;
  View* view;
  U3Dpos* pos3d;
  bool in;
  
  bool operator<(const MatchView& b) const {
    if (in && !b.in) return true; 
    else if (b.in && !in) return false;
    else return (distance <b.distance);
  }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

View* 3DCanvasView::findInChildren(Element* grp, const Point& winpos,
                                     const UpdateContext& ctx, ViewFind& vf) 
{
  if (!grp->isShowable() || grp->isIgnoringEvents()) return null;
  
#ifndef UBIT_WITH_GL
  return View::findInChildren(grp, e, cond, ctx, vf);
#else  
  UpdateContext ctx2 = ctx; // voir plus loin
  glMatrixMode(GL_MODELVIEW);
  
  GLdouble point1[3], point2[3], modelMatrix[16], projectionMatrix[16];
  GLint viewport[4];
  glPushMatrix();
  
  // !!! load modelview matrix when viewpoint changes will be implemented.
  glLoadIdentity();
  glPushMatrix();   //we will need this matrix multiple times
  
  //get projection matrix
  glLoadIdentity();
  U3Dcanvas* canvas = (U3Dcanvas*)getBox();
  gluPerspective(canvas->fovy, canvas->aspect*width/height, canvas->near, canvas->far);
  glGetDoublev(GL_MODELVIEW_MATRIX, projectionMatrix);
  
  //get viewport
  viewport[0] = 0;
  viewport[1] = 0;
  viewport[2] = int(this->width);
  viewport[3] = int(this->height);
  
  // NB: att aux confusions entre ex,ey et x,y qui sont les coords de View !  
  float ex = winpos.x - this->x;
  float ey = this->height - (winpos.y - this->y);
  
  bool in_softwin_list = grp->getDisplayType() == Element::WINLIST;  // ?????
  
  vector<MatchView> views;
  
  for (UChildReverseIter ch = grp->crbegin(); ch != grp->crend(); ++ch) {
    if (!ch.getCond() || ch.getCond()->verifies(ctx, *grp)) {
      Dimension dim(0,0);
      Element* chgrp = (*ch)->toElem();
      Box* chbox = null;
      View* chview = null;
      
      if (!chgrp || !chgrp->isShowable() || chgrp->isIgnoringEvents())
        continue;
      
      if (! (chbox = chgrp->toBox())) {   // is an Element not a Box
        View* v = findInGroup(chgrp, winpos, ctx, vf);
        if (v) return v; 
      }
      
      else if (chbox && in_softwin_list && chbox->getDisplayType() == Element::SOFTWIN
               && (chview = chbox->getViewInImpl(this /*,null*/))) {   //pas de ch
        // !!! faudrait tester chview->isShown() !!!
        View* v = chview->findInBox(chbox, winpos, ctx, vf);
        if (v) return v; 
      }
      
      else if (chbox && chbox->getDisplayType() == Element::BLOCK    // elimine les Window
               && (chview = chbox->getViewInImpl(this, /*&ch.child(),*/ dim))) {
        // !!! faudrait tester chview->isShown() !!!
        
        U3Dpos* pos3d = null;
        chgrp->attributes().findClass(pos3d);    
        if (!pos3d) chgrp->children().findClass(pos3d);     // lourd !!!
        
        if (!pos3d) {
          View* v = chview->findInBox(chbox, winpos, ctx, vf);  // fait quoi ?????
          if (v) return v; 
        }
        
        else {
          glPopMatrix();
          glPushMatrix(); //get global modelview matrix
          glTranslatef(pos3d->x.val, pos3d->y.val, pos3d->z);
          glRotatef(-pos3d->x_rot, 1,0,0);  // affiche dans partie inferieure
          glRotatef(pos3d->y_rot, 0,1,0); 
          glRotatef(pos3d->z_rot, 0,0,1);
          glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
          
          float ez = 0;
          if (gluUnProject(ex, ey, ez, modelMatrix, projectionMatrix, viewport, 
                           &point1[0], &point1[1], &point1[2]) == GL_FALSE) {
            Application::internalError("U3DBox::findInChildren","glUnproject failed");
            continue;
          }
          
          ez = 0.5;
          if (gluUnProject(ex, ey, ez, modelMatrix, projectionMatrix, viewport, 
                           &point2[0], &point2[1], &point2[2]) == GL_FALSE) {
            Application::internalError("U3DBox::findInChildren","glUnproject failed");
            continue;
          }
          
          //point1 and point2 defines the mouse click line in the plan coordinate system.
          //get the point on this line with z = 0
          if (point1[2] == point2[2]) {
            Application::internalError("U3DBox::findInChildren","no intersection");
            continue;
          }
          
          // c'est p2-p1, mais y'a un moins en plus.
          float k  = point1[2] / (point1[2]-point2[2]);
          float px = point1[0] + k * (point2[0]-point1[0]);
          float py = point1[1] + k * (point2[1]-point1[1]);
          py *= -1.0;   // y is up in openGL
          
          // we don't test if the point is inside the box, we want the hit 
          // even if the cursor is outside.
          MatchView mv;
          mv.view = chview;
          mv.distance = point1[2] * point1[2] + (point1[0]-px) * (point1[0]-px)
          + (point1[1]+py) * (point1[1]+py);
          mv.p.x = px;
          mv.p.y = py;
          mv.pos3d = pos3d;
          
          //if (curp.local.pwidth && *curp.local.pwidth > 0) 
          //  mv.width = *curp.local.pwidth; else mv.width = ww;
          // inutile mv.width = dim.width;
          
          //if (curp.local.pheight && *curp.local.pheight > 0)
          //  mv.height = *curp.local.pheight; else mv.height = hh;
          // inutile mv.height = dim.height;
          
          //mv.in = (px >= 0  &&  py >= 0 &&  px <= mv.width && py <= mv.height);
          // correct avec dim ?
          mv.in = (px >= 0  &&  py >= 0 &&  px <= dim.width && py <= dim.height);
          views.push_back(mv);
        }
      }
    }
  }
  
  sort<vector<MatchView>::iterator>(views.begin(), views.end());  //sort views
  
  // find the first view that matches
  View* source_view = null;
  
  for (vector<MatchView>::iterator it = views.begin(); it!=views.end(); ++it) {
    Box* box = (*it).view->getBox();
    if (box) {
      vf.ref_pos.x = (*it).p.x;
      vf.ref_pos.y = (*it).p.y;
      vf.refpos_in_canvas = (*it).pos3d;
      vf.ref_view = (*it).view;
      source_view = vf.ref_view->findInBox(box, vf.ref_pos, ctx2, vf);
      if (source_view) break;      //stop as soon as a view is found
    }
  } //endfor
  
  // NB: si not found remettre les bonnes valeurs !!
  // cerr << " event not found " << found_view << endl;
  if (source_view) {
    vf.canvas_view = this;
  }
  else {
    vf.canvas_view = null;
    vf.ref_view = null;
    vf.refpos_in_canvas = null;
  }
  
  glPopMatrix();
  glPopMatrix();
  return source_view;
#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool 3DCanvasView::unproject(const U3Dpos* pos3d,
                              const Point& winpos, Point& convpos) {
  glMatrixMode(GL_MODELVIEW);
  
  GLdouble point1[3], point2[3], modelMatrix[16], projectionMatrix[16];
  GLint viewport[4];
  glPushMatrix();
  
  // !!! load modelview matrix when viewpoint changes will be implemented.
  glLoadIdentity();
  glPushMatrix();   //we will need this matrix multiple times
  
  //get projection matrix
  glLoadIdentity();
  U3Dcanvas* canvas = (U3Dcanvas*)getBox();
  gluPerspective(canvas->fovy, canvas->aspect*width/height, canvas->near, canvas->far);
  glGetDoublev(GL_MODELVIEW_MATRIX, projectionMatrix);
  
  //get viewport
  viewport[0] = 0;
  viewport[1] = 0;
  viewport[2] = int(this->width);
  viewport[3] = int(this->height);
  
  // NB: att aux confusions entre ex,ey et x,y qui sont les coords de View !  
  float ex = winpos.x - this->x;
  float ey = this->height - (winpos.y - this->y);
  
  glPopMatrix();
  glPushMatrix(); //get global modelview matrix
  glTranslatef(pos3d->x.val, pos3d->y.val, pos3d->z);
  glRotatef(-pos3d->x_rot, 1,0,0);  // affiche dans partie inferieure
  glRotatef(pos3d->y_rot, 0,1,0); 
  glRotatef(pos3d->z_rot, 0,0,1);
  glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
  
  float ez = 0;
  if (gluUnProject(ex, ey, ez, modelMatrix, projectionMatrix, viewport, 
                   &point1[0], &point1[1], &point1[2]) == GL_FALSE) {
    Application::internalError("U3DBox::findInChildren","glUnproject failed");
    return false;
  }
  
  ez = 0.5;
  if (gluUnProject(ex, ey, ez, modelMatrix, projectionMatrix, viewport, 
                   &point2[0], &point2[1], &point2[2]) == GL_FALSE) {
    Application::internalError("U3DBox::findInChildren","glUnproject failed");
    return false;
  }
  
  //point1 and point2 defines the mouse click line in the plan coordinate system.
  //get the point on this line with z = 0
  if (point1[2] == point2[2]) {
    Application::internalError("U3DBox::findInChildren","no intersection");
    return false;
  }
  
  // c'est p2-p1, mais y'a un moins en plus.
  float k  = point1[2] / (point1[2]-point2[2]);
  float px = point1[0] + k * (point2[0]-point1[0]);
  float py = point1[1] + k * (point2[1]-point1[1]);
  py *= -1.0;   // y is up in openGL
  
  convpos.x = px;
  convpos.y = py;
  
  //cerr << "unproject: in_hardwin: " <<  winpos.x <<" "<< winpos.y
  //<< " / in_3dwin: " << px <<" " << py  <<endl;
  return true;
}

// =============================================================================

void View::beginUpdate3d(UViewUpdateImpl& vd, Element& grp, UpdateContext& curp) {
#ifdef UBIT_WITH_GL
  if (!vd.can_paint) return;
  
  glTranslatef(0,0,SUPERPOSE);    // superposer les plans
  
  U3Dpos* pos3d = dynamic_cast<U3Dpos*>(curp.pos);
  if (pos3d) {  
    glPushMatrix();
    //change to object s local coordinate system.
    //cerr << "beginUpdate3d " << pos3d->x << " " << pos3d->y << endl;
    glTranslatef(pos3d->x.val, pos3d->y.val, pos3d->z);
    glRotatef(-pos3d->x_rot, 1,0,0);    // because affiche dans partie inferieure
    glRotatef(pos3d->y_rot, 0,1,0); 
    glRotatef(pos3d->z_rot, 0,0,1);
  }
#endif
}

void View::endUpdate3d(UViewUpdateImpl& vd, Element& grp, UpdateContext& curp) {
#ifdef UBIT_WITH_GL
  if (!vd.can_paint) return;
  
  glTranslatef(0,0,-SUPERPOSE);    // desuperposer les plans
  
  U3Dpos* pos3d = dynamic_cast<U3Dpos*>(curp.pos);
  if (pos3d) glPopMatrix();
#endif
}


}
