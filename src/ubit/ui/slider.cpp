/*
 *  slider.cpp
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

#include <iostream>
#include <ubit/ubit_features.h>
#include <ubit/ui/slider.h>
#include <ubit/ui/scrollbar.h>
#include <ubit/draw/style.h>
#include <ubit/ui/update.h>
#include <ubit/ui/updatecontext.h>
#include <ubit/upix.hpp>
#include <ubit/uon.hpp>
#include <ubit/ucall.hpp>
#include <ubit/core/application.h>
#include <ubit/uboxgeom.hpp>
using namespace std;

namespace ubit {



class USliderRail : public Box {
public:
  UCLASS(USliderRail)  
  USliderRail(const Args& a = Args::none) : Box(a) {}
  static Style* createStyle();
};
  
class USliderKnob : public Box {
public:
  UCLASS(USliderKnob)  
  USliderKnob(const Args& a = Args::none) : Box(a) {}
  static Style* createStyle();
};


USliderStyle::USliderStyle() {
  // fix size and avoid useless updates 
  //vstyle.local.size.set(16,UPX, 150,UPX);
  vstyle.local.size.width  = 16|UPX;
  vstyle.local.size.height = 150|UPX;
  vstyle.hspacing = 0;
  vstyle.vspacing = 0;
  vstyle.orient = Orientation::VERTICAL;
  vstyle.halign = Halign::CENTER;
  vstyle.valign = Valign::FLEX;
  //vstyle.local.padding.left = 1; //sinon impression rail decentre
  
  //local.size.set(150,UPX, 16,UPX);
  local.size.width  = 150|UPX;
  local.size.height = 16|UPX;
  hspacing = 0;
  vspacing = 0;
  orient = Orientation::HORIZONTAL;
  halign = Halign::FLEX;
  valign = Valign::CENTER;
  //local.padding.top = 1;
}

const Style& USliderStyle::getStyle(UpdateContext* ctx) const {
  //att: par defaut, si pas de ctx, renvoyer HORIZ
  if (ctx && ctx->obj && ctx->obj->isVertical()) return vstyle; 
  else return *this;
}


USlider& uslider(const Args& a) {return *new USlider(a);}

USlider& uslider(Float& v, const Args& a) {return *new USlider(v, a);}

USlider& uhslider(const Args& a) {return *new USlider(a);}

USlider& uvslider(const Args& a) {
  return *new USlider(Orientation::vertical + a);
}

USlider::USlider(const Args& a) : Box(a), pvalue(*new Float) {
  // Box(a) avant constructs() pour que isVertical() soit correct
  constructs();
}

USlider::USlider(Float& v, const Args& a) : Box(a), pvalue(v) {
  constructs();
}

void USlider::constructs() {
  // action doit etre appele a la fin sur le release
  pvalue->onChange(ucall(this, &USlider::actionCB));

  // the position is a percent of the parent size
  pknob_pos = new UPos(0,0);
  //pknob_pos->setPercentMode(true, true);  // adjusted
  pknob_pos->onChange(ucall(this, &USlider::changeCB));

  pknob_ctrl = (new UPosControl)->setModel(pknob_pos);
  // sinon appel incorrect de changeCB() a cause des setX(), setY() ci-dessous !
  pknob_pos->ignoreChangeCallbacks(true);
  if (isVertical()) {
    pknob_pos->setX(50.|UPERCENT_CTR);
    pknob_ctrl->freezeX(true);
  }
  else {
    pknob_pos->setY(50.|UPERCENT_CTR);
    pknob_ctrl->freezeY(true);
  }
  pknob_pos->ignoreChangeCallbacks(false); // retablir
  
  add(*(prail = createRail()));
  prail->ignoreEvents();

  add(*(pknob = createKnob()));
  pknob->addAttr(*pknob_pos);
  pknob->addAttr(*pknob_ctrl);
  
  addAttr(UOn::mpress / ucall(this, &USlider::gotoPosCB));
}


Box* USlider::createKnob() {return new USliderKnob();}
Box* USlider::createRail() {return new USliderRail();}


void USlider::gotoPosCB(MouseEvent& e) {
  View* v = e.getView();
  if (!v) return;

  if (isVertical()) {
    float y = e.getY() * 100.0 / v->getHeight();
    pknob_pos->setY(y | UPERCENT_CTR);
  }
  else {
    float x = e.getX() * 100.0 / v->getWidth();
    pknob_pos->setX(x | UPERCENT_CTR);
 }
}

void USlider::actionCB(Event& notused) {  //UNodeEvent
  // cette verif doit etre dans setCB, pas dans set() car pvalue peut etre
  // modifiee directement si partagee par d'autres objets (via value() etc)
  if (*pvalue < 0.) *pvalue = 0.;
  else if (*pvalue > 100.) *pvalue = 100.;  // entre 0 et 100
  
  if (isVertical()) pknob_pos->setY(*pvalue | UPERCENT_CTR); 
  else pknob_pos->setX(*pvalue | UPERCENT_CTR);
  
  Event e2(UOn::action, this, pknob);    //UElemEvent
  fire(e2);
}

void USlider::changeCB(Event& notused) {  //UNodeEvent
  if (isVertical()) *pvalue = pknob_pos->getY().val; 
  else *pvalue = pknob_pos->getX().val;
  
  Event e2(UOn::change, this, pknob);  //UElemEvent
  fire(e2);
}


struct USliderRailStyle : public Style {
  Style vstyle;
  USliderRailStyle();
  virtual const Style& getStyle(UpdateContext* ctx) const {
    //att: par defaut, si pas de ctx, renvoyer HORIZ
    if (ctx && ctx->parent_ctx && ctx->parent_ctx->obj
        && ctx->parent_ctx->obj->isVertical())
      return vstyle; 
    else return *this;
  }
};

USliderRailStyle::USliderRailStyle() {
  vstyle.orient = Orientation::INHERIT;
  //vstyle.local.size.setWidth(5);
  vstyle.local.size.width = 5|UPX;
  vstyle.local.border = &Border::shadowIn;
  
  orient = Orientation::INHERIT;
  //local.size.setHeight(5);
  local.size.height = 5|UPX;
  local.border = &Border::shadowIn;
}

Style* USliderRail::createStyle() {
  return new USliderRailStyle;
}


struct USliderKnobStyle : public Style {
  Style vstyle;
  USliderKnobStyle();
  virtual const Style& getStyle(UpdateContext* ctx) const {
  //att: par defaut, si pas de ctx, renvoyer vertical
    if (ctx && ctx->parent_ctx && ctx->parent_ctx->obj
        && ctx->parent_ctx->obj->isVertical())
      return vstyle; 
    else return *this;
  }
};

USliderKnobStyle::USliderKnobStyle() {
  orient = Orientation::INHERIT;
  halign = Halign::LEFT;
  valign = Valign::TOP;
  hspacing = 0;
  vspacing = 0;
  local.content = new Element(UPix::hslider);

  vstyle.orient = Orientation::INHERIT;
  vstyle.halign = Halign::LEFT;
  vstyle.valign = Valign::TOP;
  vstyle.hspacing = 0;
  vstyle.vspacing = 0;
  vstyle.local.content = new Element(UPix::vslider);
}

Style* USliderKnob::createStyle() {
  return new USliderKnobStyle();
}

#if EXXXX

/* URangeSlider gadget.
*  UOn::change callbacks are activated when the scale value is changed.
*/
 Class* URangeSlider::getClass() const {
   static Class* c = Application::addElementClass("urangeslider", createStyle);
   return c;
 }

class URangeSlider: public USlider {
public:
  URangeSlider(const Args& = Args::none);
  URangeSlider(const Orientation&, const Args& = Args::none);
  URangeSlider(Float& value1, Float& value2, 
               const Orientation&, const Args& = Args::none);
  /**< constructors.
    * - default orientation is Orientation::Vertical
  * - 'orient' can be Orientation::horizontal or Orientation::vertical.
    */
  
  Float& value2() {return *pvalue2;}
  virtual float getValue2() const;
  ///< returns the 2nd value (a float between 0 and 100).
  
  virtual void setValue2(float percent);
  /**< changes the 2nd value (a float between 0 and 100).
    * Note: UOn::change callbacks are activated when the value is changed.
    */
  
  // implementation
  
  virtual Class* getClass() const;
  static Style* createStyle();
  
  virtual Box* createSlider(bool vertical);
  virtual Box* createSlider2(bool vertical);
  
protected:
    virtual float getPercent(Event& e, 
                             View *slider_view, View *railbox_view,
                             int _delta_mouse);
  virtual void setValueImpl(float newval, bool upd);
  
  virtual void pressSlider2(Event&);
  virtual void releaseSlider2(Event&);
  virtual void dragSlider2(Event&);
  
private:
  unique_ptr<Float> pvalue2; 
  unique_ptr<Box> pslider2;
  UPos slider_pos2;
  int delta_mouse2;
  void constructs2();
};
#endif

}
