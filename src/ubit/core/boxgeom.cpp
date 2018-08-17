/*
 *  boxgeom.cpp: Attributes for controlling Box geometry
 *  (see also Attribute.hpp for Attribute base class)
 *  Ubit GUI Toolkit - Version 6
 *  (C) 2009 | Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
 *
 * ***********************************************************************
 * COPYRIGHT NOTICE :
 * THIS PROGRAM IS DISTRIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE
 * IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER  THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION;
 * EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 * SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
 * ***********************************************************************/

#include <ubit/ubit_features.h>
#include <iostream>
#include <ubit/uon.hpp>
#include <ubit/uboxgeom.hpp>
#include <ubit/ucursor.hpp>
#include <ubit/ui/updatecontext.h>
#include <ubit/uwin.hpp>
#include <ubit/uscrollpane.hpp>
#include <ubit/uappli.hpp>
#include <ubit/ucall.hpp>
#include <ubit/ueventflow.hpp>
#include <ubit/ucursor.hpp>
using namespace std;

namespace ubit {



Scale::Scale(float v) : value(v) {}
Scale::Scale(const Float& v) : value(v.floatValue()) {}

void Scale::set(float v) {
  if (checkConst()) return;
  if (value == v) return;
  value = v;
  changed();
}

void Scale::mult(float v) {
  if (checkConst()) return; 
  value *= v;
  changed();
}

void Scale::div(float v) {
  if (checkConst()) return;
  value /= v;
  changed();
}

void Scale::putProp(UpdateContext *props, Element&) {
  props->xyscale *= value;
  props->fontdesc.setScale(props->xyscale);
}

void Scale::update() {
  updateAutoParents(Update::LAYOUT_PAINT);
}

/* ==================================================== [Elc] ======= */
// NB: ne marchera pas si inclus dans un proplist car addingTo n'est pas
// execute dans ce cas

const Length::Modes 
UPos::TOP(0), 
UPos::LEFT(0), 
UPos::RIGHT(1), 
UPos::BOTTOM(1);

void UPos::addingTo(Child& c, Element& parent) {
  Attribute::addingTo(c, parent);
  
  if (parent.emodes.IS_FLOATING) {    // !!! ce n'est plus tout a fait vrai: cf plus bas
    warning("UPos::addingTo",
            "This UPos instance is being added to a widget (%s %p) that contains another UPos (a widget should not contain multiple UPos instances)",
            this, &parent.getClassName());
  }
  
  Box* par = parent.toBox();
  if (!par) {
    warning("UPos::addingTo",
            "This UPos instance is being added to a container (%s %p) that does not derive from Box",
            this, &parent.getClassName());
  }
  else par->emodes.IS_FLOATING = isFloating();
}

void UPos::removingFrom(Child& c, Element& parent) {
  Box* par = parent.toBox();
  if (par) par->emodes.IS_FLOATING = false;
  Attribute::removingFrom(c, parent);
}

bool UPos::equals(const Length& _x, const Length& _y) const {
  return _x==x && _y==y;
}

void UPos::update()  {
  //hum, c'est pas le layout qui change mais la position...!
  updateAutoParents(Update::LAYOUT_PAINT);
}

void UPos::putProp(UpdateContext *props, Element&) {
  props->pos = this;  // pos peut etre proportionnelle etc.
}

bool UPos::isFloating() const {
  return (x.unit.type != Unit::AUTO && x.unit.type != Unit::IGNORE
          && y.unit.type != Unit::AUTO && y.unit.type != Unit::IGNORE);
}

UPos& UPos::set(const Length& newx, const Length& newy) {
  if (checkConst()) return *this;

  Length _x = newx, _y = newy;
  bool xpercent = false, ypercent = false;

  if (_x.unit == UPERCENT || _x.unit == UPERCENT_CTR) {
    xpercent = true;
    if (_x.val < 0.) _x.val = 0.; else if (_x.val > 100.) _x.val = 100.;
  }
  if (_y.unit == UPERCENT || _y.unit == UPERCENT_CTR) {
    ypercent = true;
    if (_y.val < 0.) _y.val = 0.; else if (_y.val > 100.) _y.val = 100.;
  }
  
  if (x == _x && y == _y) return *this;

  Update upd(Update::PAINT);
  upd.setMove(_x.val - x.val, _y.val - y.val, xpercent, ypercent);
  x = _x;
  y = _y;
  
  // POS_HAS_CHANGED indique que les coords des views ne sont plus a jour
  // updateAutoParents() fait ensuite la mise a jour (via View::updatePos())
    
  bool is_floating = isFloating();
  for (UParentIter p = pbegin(); p != pend(); ++p) {
    Box* box = *p ? (*p)->toBox() : null;
    if (box) {
      box->emodes.IS_FLOATING = is_floating;
      for (View* v = box->getView(0); v != null; v = v->getNext()) {
        v->addVModes(View::POS_HAS_CHANGED); 
      }
    }
    //Element* grp = *p;
    //if (grp && !grp->omodes.DONT_AUTO_UPDATE) grp->update(upmode);
  }
  updateAutoParents(upd);
  
  changed(false); // false => pas de update()
  return *this;
}



const Length USize::INITIAL(-1);                 // !!!   A REVOIR !!!!!!  
const Length::Modes USize::UNRESIZABLE(1);       // !!!   A REVOIR !!!!!!  


USize& USize::set(const Length& w, const Length& h) {
  if (checkConst()) return *this;
  if (width == w && height == h) return *this;
  width = w;
  height = h;
  changed();
  return *this;
}

bool USize::equals(const Length& w, const Length& h) const {
  return width==w && height==h;
}

void USize::update()  {
  _parents.setParentsViewsModes(View::SIZE_HAS_CHANGED, true);
  updateAutoParents(Update::LAYOUT_PAINT);
}

void USize::putProp(UpdateContext* props, Element& obj) {
  //props->local.size = *this;
  if (width != UIGNORE) {
    props->local.size.width = width;
    obj.emodes.IS_WIDTH_UNRESIZABLE = (width.modes.val & USize::UNRESIZABLE.val);
  }
  if (height != UIGNORE) {
    props->local.size.height = height;
    obj.emodes.IS_HEIGHT_UNRESIZABLE = (height.modes.val & USize::UNRESIZABLE.val);
  }
}

// ==================================================== Ubit Gui Toolkit =======

UPadding& UPadding::setWidth(Length l) {
  if (checkConst() || (val.left == l && val.right == l)) return *this;
  val.left = l;
  val.right = l;
  changed();
  return *this;
}

UPadding& UPadding::setHeight(Length l) {
  if (checkConst() || (val.top == l && val.bottom == l)) return *this;
  val.top = l;
  val.bottom = l;
  changed();
  return *this;
}

UPadding& UPadding::setLeft(Length l) {
  if (checkConst() || val.left == l) return *this;
  val.left = l;
  changed();
  return *this;
}

UPadding& UPadding::setRight(Length l) {
  if (checkConst() || val.right == l) return *this;
  val.right = l;
  changed();
  return *this;
}

UPadding& UPadding::setTop(Length l) {
  if (checkConst() || val.top == l) return *this;
  val.top = l;
  changed();
  return *this;
}

UPadding& UPadding::setBottom(Length l) {
  if (checkConst() || val.bottom == l) return *this;
  val.bottom = l;
  changed();
  return *this;
}

void UPadding::update() {
  updateAutoParents(Update::LAYOUT_PAINT);
}

void UPadding::putProp(UpdateContext* props, Element&) {
 if (val.top != UIGNORE)    props->local.padding.top = val.top;
 if (val.right != UIGNORE)  props->local.padding.right = val.right;
 if (val.bottom != UIGNORE) props->local.padding.bottom = val.bottom;
 if (val.left != UIGNORE)   props->local.padding.left = val.left;
}

// ==================================================== Ubit Gui Toolkit =======

Orientation Orientation::vertical(VERTICAL, UCONST);
Orientation Orientation::horizontal(HORIZONTAL, UCONST);

Orientation::Orientation() : value(VERTICAL) {}
Orientation::Orientation(const Orientation& v) : value(v.value) {}
Orientation::Orientation(char v, UConst c) : Attribute(c), value(v) {}

void Orientation::addingTo(Child& c, Element& parent) {
  Attribute::addingTo(c, parent);
  
  // ca ne va pas: messages absurdes !!
  //if (parent->hasBMode(UMode::HAS_ORIENT)) {
  // Application::warning("Orientation::addingTo","This Orientation object (%p) has a parent (%s %p) that contains another Orientation object", this, parent->getTypeName(), parent);
  //}
  
  parent.emodes.HAS_ORIENT = true;
  if (value == VERTICAL) parent.emodes.IS_VERTICAL = true;
  else parent.emodes.IS_VERTICAL = false;
}

void Orientation::removingFrom(Child& c, Element& parent) {
  //parent.removeBModes(UMode::HAS_ORIENT | UMode::IS_VERTICAL);
  parent.emodes.HAS_ORIENT = false; 
  parent.emodes.IS_VERTICAL = false;
  Attribute::removingFrom(c, parent);
}

Orientation& Orientation::operator=(const Orientation &o) {
  if (checkConst()) return *this;
  if (value == o.value) return *this;
  value = o.value;
  changed();
  return *this;
}

void Orientation::update() {
  updateAutoParents(Update::SHOW);   // pourquoi show ???
}

void Orientation::putProp(UpdateContext* props, Element& par) {
  // nb: inherited => ne fait rien
  //if (value == VERTICAL) par.addBModes(UMode::IS_VERTICAL);
  //else if (value == HORIZONTAL) par.removeBModes(UMode::IS_VERTICAL);
  
  if (value == VERTICAL) par.emodes.IS_VERTICAL = true;
  else if (value == HORIZONTAL) par.emodes.IS_VERTICAL = false;
}

// ==================================================== Ubit Gui Toolkit =======

Valign Valign::top(TOP, UCONST);
Valign Valign::bottom(BOTTOM, UCONST);
Valign Valign::center(CENTER, UCONST);
Valign Valign::flex(FLEX, UCONST);

Valign::Valign() : value(TOP) {}
Valign::Valign(const Valign& v) : value(v.value) {}
Valign::Valign(char v, UConst c) : Attribute(c), value(v) {}

Valign& Valign::operator=(const Valign& obj) {
  if (checkConst()) return *this;
  if (value == obj.value) return *this;
  value = obj.value;
  changed();
  return *this;
}

void Valign::update() {
  // box size won't change but children layout must be updated
  updateAutoParents(Update::LAYOUT_PAINT);
}

void Valign::putProp(UpdateContext *props, Element&) {
  props->valign = value;
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

Halign Halign::left(LEFT, UCONST);
Halign Halign::right(RIGHT, UCONST);
Halign Halign::center(CENTER, UCONST);
Halign Halign::flex(FLEX, UCONST);

Halign::Halign() : value(LEFT) {}
Halign::Halign(const Halign& v) : value(v.value) {}
Halign::Halign(char v, UConst c) : Attribute(c), value(v) {}

Halign& Halign::operator=(const Halign& o) {
  if (checkConst()) return *this;
  if (value == o.value) return *this;
  value = o.value;
  changed();
  return *this;
}

void Halign::update() {
  // box size won't change but children layout must be updated
  updateAutoParents(Update::LAYOUT_PAINT);
}

void Halign::putProp(UpdateContext *props, Element&) {
  props->halign = value;
}

// ==================================================== Ubit Gui Toolkit =======

HSpacing HSpacing::none(0);

HSpacing::HSpacing(float val) : value(val) {}

HSpacing& HSpacing::operator=(float val) {
  if (checkConst()) return *this;
  if (value == val) return *this;
  value = val;
  changed();
  return *this;
}

HSpacing& HSpacing::operator=(const HSpacing& v) {
  return (*this = v.value);
}

void HSpacing::update() {
  updateAutoParents(Update::LAYOUT_PAINT);
}

void HSpacing::putProp(UpdateContext *props, Element&) {
  props->hspacing = value;
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

VSpacing VSpacing::none(0);

VSpacing::VSpacing(float val) : value(val) {}

VSpacing& VSpacing::operator=(float val) {
  if (checkConst()) return *this;
  if (value == val) return *this;
  value = val;
  changed();
  return *this;
}

VSpacing& VSpacing::operator=(const VSpacing& v) {
  return (*this = v.value);
}

void VSpacing::update() {
  updateAutoParents(Update::LAYOUT_PAINT);
}

void VSpacing::putProp(UpdateContext *props, Element&) {
  props->vspacing = value;
}

// ==================================================== Ubit Gui Toolkit =======

UPosControl::UPosControl(UPos* p) :
change_cursor(false), freeze_x(false), freeze_y(false), 
moved_view(null), 
callbacks(ucall(this, &UPosControl::mouseCB)),
posAttr(p) {}

UPosControl::~UPosControl() {}

void UPosControl::addingTo(Child& c, Element& parent) {
  Attribute::addingTo(c, parent);
  //parent.addAttr(UOn::mdrag/callbacks + UOn::mpress/callbacks + UOn::mrelease/callbacks);
  MultiCondition& mcond = *new MultiCondition;
  mcond.add(UOn::mdrag).add(UOn::mpress).add(UOn::mrelease);
  parent.addAttr(mcond / *callbacks);
}

void UPosControl::removingFrom(Child& c, Element& parent) {
  parent.removeAttr(*callbacks);
  Attribute::removingFrom(c, parent);
}

UPosControl& UPosControl::setModel(UPos* p) 
{posAttr = p; return *this;}

UPosControl& UPosControl::changeCursor(bool state) 
{change_cursor = state; return *this;}

UPosControl& UPosControl::freezeX(bool state) 
{freeze_x = state; return *this;}

UPosControl& UPosControl::freezeY(bool state) 
{freeze_y = state; return *this;}

// POINT IMPORTANT: il faut eviter l'"effet memoire" quand on sort du moving_area:
// les drags ne doivent pas bouger l'objet quand on en sort d'ou des calculs
// en absolu entre les x/yev0 et les x/ybox_pos0

void UPosControl::mouseCB(MouseEvent& e) {
  if (e.getCond() == UOn::mdrag) dragCB(e);
  else if (e.getCond() == UOn::mpress) pressCB(e);
  else if (e.getCond() == UOn::mrelease) releaseCB(e);
}

void UPosControl::pressCB(MouseEvent& e) {
  // !! NOTE for multitouch: this function should be locked so that several handles
  // !! cannot change the position simultaneously  
  moved_view = null;

  View* pos_boxview = posAttr->getParentView(e);
  View* paneview = null;
  if (!pos_boxview 
      || !(moved_view = pos_boxview->getParentView())
      || !(paneview = moved_view->getParentView())
      ) return;

  pt_in_pane0 = e.getPosIn(*paneview);
  //xev0 = e.getScreenPos().x; yev0 = e.getScreenPos().y;
  box_pos0.set(posAttr->getX().val, posAttr->getY().val);
  if (change_cursor) e.getFlow()->setCursor(e, &Cursor::move);
}

void UPosControl::releaseCB(MouseEvent& e) {
  moved_view = null;
  if (change_cursor) e.getFlow()->setCursor(e, null);
}

void UPosControl::dragCB(MouseEvent& e) {
  View* paneview = null;
  if (!moved_view || !(paneview = moved_view->getParentView())) return;

  Point pt_in_pane = e.getPosIn(*paneview);  
  if (pt_in_pane.x < MARGIN) pt_in_pane.x = MARGIN;
  else if(pt_in_pane.x > paneview->width - MARGIN) pt_in_pane.x = paneview->width - MARGIN;
  if (pt_in_pane.y < MARGIN) pt_in_pane.y = MARGIN;
  else if(pt_in_pane.y > paneview->height - MARGIN) pt_in_pane.y = paneview->height - MARGIN;
  
  double dx = pt_in_pane.x - pt_in_pane0.x;
  double dy = pt_in_pane.y - pt_in_pane0.y;
  //float dx = e.getScreenPos().x - xev0; float dy = e.getScreenPos().y - yev0;
  
  float xnew, ynew;
  //margin = 0;
    
  if (freeze_x) xnew = posAttr->getX().val;   // ATT: Unit pas pries en compte!!!
  else {
    bool xpercent = posAttr->x.unit == UPERCENT || posAttr->x.unit == UPERCENT_CTR;
    if (xpercent) {
      xnew = box_pos0.x * moved_view->getWidth() / 100 + dx;
      //margin = 0;
    }
    else {
      xnew = box_pos0.x + dx;    // juste que si handle(0,0) correspond avec moved(0,0) !!!
     // margin = MARGIN;
    }
    
    /* View* handle = e.getView();
    if (-xnew > handle->getWidth() -margin) 
      xnew = -(handle->getWidth() -margin);
    else if (xnew > moved_view->getWidth() -margin) 
      xnew = moved_view->getWidth() -margin;
    */
    
    if (xpercent) xnew = xnew / moved_view->getWidth() * 100;
  }
  
  if (freeze_y) ynew = posAttr->getY().val;   // ATT: Unit pas pries en compte!!!
  else {
    bool ypercent = posAttr->y.unit == UPERCENT || posAttr->y.unit == UPERCENT_CTR;
    if (ypercent) {
      ynew = box_pos0.y * moved_view->getHeight() / 100 + dy;
      //margin = 0;
    }
    else {
      ynew = box_pos0.y + dy;
      //margin = MARGIN;
    }
    
    /* View* handle = e.getView();
    if (-ynew > handle->getHeight() -margin) ynew = -(handle->getHeight() -margin);
     else if (ynew > moved_view->getHeight() -margin) ynew = moved_view->getHeight() -margin; 
     */
    
    if (ypercent)  ynew = ynew / moved_view->getHeight() * 100;
  }
  
  //posAttr->set(xnew|pos->x.unit, ynew|pos->y.unit);
  posAttr->set(Length(xnew, posAttr->x.unit), Length(ynew, posAttr->y.unit));
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

//UMagicLensControl::UMagicLensControl(UPos& _pos, Scrollpane& _pane) 
//: UPosControl(_pos), pane(_pane) {}

UMagicLensControl::UMagicLensControl(UPos* p, Scrollpane* spane) 
: UPosControl(p), pane(spane) {}

UMagicLensControl& UMagicLensControl::setModel(UPos* p, Scrollpane* spane) 
{posAttr = p; pane = spane; return *this;}

void UMagicLensControl::dragCB(MouseEvent& e) {
  if (!posAttr || !pane) return;
  UPosControl::dragCB(e);
  
  View* moved = posAttr->getParentView(e);
  UPaneView* pane_view = (UPaneView*)pane->getFirstViewInside(*moved);
  if (pane_view) {
    float xpane_in_moved = pane_view->x - moved->x;
    float ypane_in_moved = pane_view->y - moved->y;
    pane_view->setXScroll(posAttr->getX().val + xpane_in_moved);
    pane_view->setYScroll(posAttr->getY().val + ypane_in_moved);
  }
}

// ==================================================== Ubit Gui Toolkit =======

USizeControl::USizeControl(USize* s) :
freeze_w(false), freeze_h(false), 
callbacks(ucall(this, &USizeControl::mouseCB)), 
psize(s) {}

USizeControl::~USizeControl() {}

void USizeControl::addingTo(Child& c, Element& parent) {
  Attribute::addingTo(c, parent);
  //parent.addAttr(UOn::mdrag/callbacks + UOn::mpress/callbacks + UOn::mrelease/callbacks);
  MultiCondition& mcond = *new MultiCondition;
  ((mcond += UOn::mdrag) += UOn::mpress) += UOn::mrelease;
  parent.addAttr(mcond / *callbacks);
}

void USizeControl::removingFrom(Child& c, Element& parent) {
  parent.removeAttr(*callbacks);
  Attribute::removingFrom(c, parent);
}

USizeControl& USizeControl::setModel(USize* s) 
{psize = s; return *this;}

USizeControl& USizeControl::freezeWidth(bool state)
{freeze_w = state; return *this;}

USizeControl& USizeControl::freezeHeight(bool state) 
{freeze_h = state; return *this;}

// POINT IMPORTANT: il faut eviter l'"effet memoire" quand on sort du moving_area:
// les drags ne doivent pas bouger l'objet quand on en sort d'ou des calculs
// en absolu entre les x/yev0 et les x/ybox_pos0

void USizeControl::mouseCB(MouseEvent& e) {
  if (e.getCond() == UOn::mdrag) dragCB(e);
  else if (e.getCond() == UOn::mpress) pressCB(e);
  else if (e.getCond() == UOn::mrelease) releaseCB(e);
}

void USizeControl::pressCB(MouseEvent& e) {
  xev0 = e.getScreenPos().x;
  yev0 = e.getScreenPos().y;
  w0 = psize->getWidth().val;
  h0 = psize->getHeight().val;
  e.getFlow()->setCursor(e, &Cursor::hresize);
}

void USizeControl::releaseCB(MouseEvent& e) {
  e.getFlow()->setCursor(e, null);
}

void USizeControl::dragCB(MouseEvent& e) {
  float dx = e.getScreenPos().x - xev0;
  float dy = e.getScreenPos().y - yev0;
  
  float w = 0, h = 0;
  
  if (freeze_w) w = psize->getWidth().val;   // ATT: Unit pas pries en compte!!!
  else {
    w = w0 + dx;
    if (w <= 0) w = 1;
  }
  
  if (freeze_h) h = psize->getHeight().val;   // ATT: Unit pas pries en compte!!!
  else {
    h = h0 + dy;
    if (h <= 0) h = 1;
  }
  
  psize->set(w, h);
}

// ==================================================== Ubit Gui Toolkit =======
#if 0

class Shape : public Attribute {
  friend class View;
  const UPix *pix;
public:
  // The shape of the component will adapt to the shape of the UPix argument
  Shape(const UPix&);
  friend Shape& ushape(UPix&);
  
  void set(const UPix*);
  void set(const UPix&);
  
  virtual void update();
  virtual void putProp(UpdateContext*, Element&);
};


Shape& ushape(UPix &pix) {
  return *(new Shape(pix));
}

Shape::Shape(const UPix &_pix, long m) : Attribute(m){
  pix = &_pix;
}

void Shape::set(const UPix *_pix) {
  if (pix == _pix) return;
  pix = _pix;
  changed();
}
void Shape::set(const UPix &_pix) {set(&_pix);}

void Shape::update() {
  updateAutoParents(Update::all);
}

void Shape::putProp(UpdateContext *props, Element&){
  props->local.shape = this;
}

//------------------------------------------------------------------------------
/* Box Width property than can be changed interactively.
 * a handle is added to the parent box that makes it possible to resize it.
 * Note: a box should not contain a UWidthChooser and a UWidth brick simultaneously
 */
class UWidthChooser: public UWidth {
public:
  UBIT_CLASS(UWidthChooser)
  
  UWidthChooser(Length _width);
  virtual ~UWidthChooser();
  
#ifndef NO_DOC
  void addingTo(Child&, Element& parent);
  void removingFrom(Child&, Element& parent);
protected:
  unique_ptr<Box> phandle;
  float curpos;
  virtual void press(MouseEvent&);
  virtual void drag(MouseEvent&);
#endif
};

/* Box Height property than can be changed interactively.
 * a handle is added to the parent box that makes it possible to resize it.
 * Note: a box should not contain a UHeightChooser and a UHeight brick simultaneously
 */
class UHeightChooser: public UHeight {
public:
  UBIT_CLASS(UHeightChooser)
  
  UHeightChooser(Length height);
  virtual ~UHeightChooser();
  
#ifndef NO_DOC
  void addingTo(Child&, Element& parent);
  void removingFrom(Child&, Element& parent);
protected:
  unique_ptr<Box> phandle;
  float curpos;
  virtual void press(MouseEvent&);
  virtual void drag(MouseEvent&);
#endif
};


UWidthChooser::UWidthChooser(Length w) : UWidth(w) {
  phandle = ubox(uwidth(3) + Border::shadowIn + Cursor::hresize);
  phandle->addAttr(UOn::mdrag / ucall(this, &UWidthChooser::drag));
  phandle->addAttr(UOn::mpress / ucall(this, &UWidthChooser::press));
}

// removingFrom() requires a destructor 
UWidthChooser::~UWidthChooser() {destructs();}

void UWidthChooser::addingTo(Child& c, Element& parent) {
  Attribute::addingTo(c, parent);
  Box* box = parent.toBox();
  if (!box) {
    Application::warning("UWidthChooser::addingTo","parent should be a Box");
    return;
  }
  Border* border = box->getBorder(true);
  border->add(uvcenter() + uright() + *phandle);
}

void UWidthChooser::removingFrom(Child& c, Element& p) {
  //Box* box = parent->boxCast();
  //if (box) {                               // completement faux !!!!
  //  Border* border = box->getBorder();
  //  if (border) box->removeAttrNC(border); // !!! faudrait enlever sur les 2 listes
  //}
  Attribute::removingFrom(c, p);
}

void UWidthChooser::press(MouseEvent& e) {
  View *v, *parv;
  if ((v = e.getView()) && (parv = e.getView()->getParentView())) {
    //curpos = e.getWinX();
    curpos = e.getScreenPos().x;
    //set(parv->getWidth() / e._props.xyscale);  // !!!????@@@
    width = (parv->getWidth());
    changed();
  }
}

void UWidthChooser::drag(MouseEvent& e) {
  View *v, *parv;
  if ((v = e.getView()) && (parv = e.getView()->getParentView())) {
    float new_w = width.val + (e.getScreenPos().x - curpos);
    if (new_w < v->getWidth()) return;    // la handle doit rester entierement visible
    width = new_w;
    changed();
    //curpos = e.getWinX();
    curpos = e.getScreenPos().x;
  }
}


UHeightChooser::UHeightChooser(Length h) : UHeight(h) {
  phandle = ubox(uwidth(3) + Border::shadowIn + Cursor::vresize);
  phandle->addAttr(UOn::mdrag / ucall(this, &UHeightChooser::drag));
  phandle->addAttr(UOn::mpress / ucall(this, &UHeightChooser::press));
}

// removingFrom() requires a destructor 
UHeightChooser::~UHeightChooser() {destructs();}

void UHeightChooser::addingTo(Child&c, Element& parent) {
  Attribute::addingTo(c, parent);
  Box* box = parent.toBox();
  if (!box) {
    Application::warning("UHeightChooser::addingTo","parent should be a Box");
    return;
  }
  Border* border = box->getBorder(true);
  border->add(uhcenter() + ubottom() + *phandle);
}

void UHeightChooser::removingFrom(Child& c, Element& p) {
  // ....
  Attribute::removingFrom(c, p);
}

void UHeightChooser::press(MouseEvent& e) {
  View *v, *parv;
  if ((v = e.getView()) && (parv = e.getView()->getParentView())) {
    //curpos = e.getWinY();
    curpos = e.getScreenPos().y;
    //set(parv->getHeight() / e._props.xyscale);  // !!!@@@???
    height = parv->getHeight();
    changed();
  }
}

void UHeightChooser::drag(MouseEvent& e) {
  View *v, *parv;
  if ((v = e.getView()) && (parv = e.getView()->getParentView())) {
    float new_h = height.val + (e.getScreenPos().y - curpos);
    // la handle doit rester entierement visible
    if (new_h < v->getHeight()) return;
    height = new_h;
    changed();
    //curpos = e.getWinX();
    curpos = e.getScreenPos().y;
  }
}

#endif
}
