/*
 *  ctlmenu.cpp : Control menus (see upiemenu.hpp)
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
#include <cmath>
#include <ubit/ubit.h>
#include <ubit/ui/zoom-pane.h>
#include <ubit/ui/control-menu.h>
//#include <ubit/ueventImpl.hpp>  // TST
using namespace std;

namespace ubit {

  

Style* ControlMenu::createStyle() {
  Style* style = Menu::createStyle();
  style->local.alpha = 0.;
  style->local.border = null;
  style->local.padding.set(1, 1);
  style->local.background = null;
  style->setBgcolors(Color::none);
  return style;
}

ControlMenu::ControlMenu() {
  spring_mode = false;
  ctlmenu_mode = true;
}


void ControlAction::operator()(Event& e) {
  // NB: getSource() renvoie le menu
  ControlMenu* m = dynamic_cast<ControlMenu*>(e.getSource());
  if (m && e.getCond() == UOn::mdrag) mdrag((MouseEvent&)e, *m);
}

void ControlAction::addingTo(Child& child, Element& parent) {
  /*
   static MultiCondition* mc = null;
   if (!mc) {
     mc = new MultiCondition;
     mc->add(UOn::mdrag);
     mc->add(UOn::arm);
     mc->add(UOn::disarm);
   }
   */
  
  // sert a specifier les conditions par defaut
  if (!child.getCond()) child.setCond(UOn::mdrag);
  
  // NB: c'est bien Node::addingTo qu'il faut appeler sinon la cond par
  // defaut serait UOn::action
  Node::addingTo(child, parent);
}


//ZoomAction::ZoomAction(ZoomPane& zbox) 
//: pscale(&zbox.scale()), ppos(&zbox.pos()), pbox(&zbox) {}

ZoomAction::ZoomAction(Box& zoomed_box, float _gain) : 
ControlAction(_gain), 
zbox(zoomed_box),
posAttr(zoomed_box.obtainAttr<Position>()),
scaleAttr(zoomed_box.obtainAttr<Scale>()) {
}

void ZoomAction::mdrag(MouseEvent& e, ControlMenu& m) {
  View* menuview = e.getView();
  if (!menuview) return;
  
  if (e.isFirstDrag()) {
    mouse_delta = 0.;  

    // Unit ignor� : peut poser probleme !!!
    // NB: posAttr est normalisee donc independante du scale du conteneur parent
    zbox_pos0.set(posAttr->getX().val, posAttr->getY().val);
    zbox_scale0 = *scaleAttr;
    
    View* zbox_view = zbox->getView(e);
    if (!zbox_view) {
      zbox_view = zbox->getView(0);
      //cerr << "ZoomAction: null box view"<<endl;
      //return;
    }
    mouse_in_zbox0 = e.getPosIn(*zbox_view);
    
    //cerr <<"PRESS: "<< mouse_in_zbox0.x <<" / : "<< (zbox_pos0.x * zbox_scale0)<< endl;
    /*
    Point view_scrpos = box_view->getScreenPos();
    Point mouse_scrpos = e.getScreenPos();
    mousepos.x = (mouse_scrpos.x - view_scrpos.x);
    mousepos.y = (mouse_scrpos.y - view_scrpos.y);
     */
   }
  else {
    float d = m.getMouseMovement().x * xmag;
    if (d == mouse_delta) return;
    mouse_delta = d;
    
    float scale = 1.;
    if (mouse_delta > 0) scale = zbox_scale0 * (1 + mouse_delta / 100);
    else if (mouse_delta < 0) scale = zbox_scale0 / (1 - mouse_delta / 100);
    else return;

    //cerr <<"drag: "<< (zbox_pos0.x * zbox_scale0 + (1- scale/zbox_scale0) * mouse_in_zbox0.x) / scale << endl;

    zbox->setAutoUpdate(false);
    posAttr->set((zbox_pos0.x + (1- scale/zbox_scale0) * mouse_in_zbox0.x),
                 (zbox_pos0.y + (1- scale/zbox_scale0) * mouse_in_zbox0.y));
    *scaleAttr = scale;
    zbox->setAutoUpdate(true);

    e.getWin()->update(); // pour que l'objet zoom� se retaille correctement
  }
}


PanAction::PanAction(Box& panned_box, float _gain) : 
ControlAction(_gain), 
box(panned_box),
posAttr(panned_box.obtainAttr<Position>()) {
}

void PanAction::mdrag(MouseEvent& e, ControlMenu& m) {
  if (e.isFirstDrag()) {
    // Unit ignor� : peut poser probleme !!!
    pos0.set(posAttr->getX().val, posAttr->getY().val);   // !!! convesrion afaire

    View* box_view = box->getView(e);
    if (!box_view) return;
    // fait le contraire de ce qu'on veut
    //container_scale = box_view->getParentView()->getScale();
  }
  else {
    Point newpos;
    newpos.x = pos0.x + m.getMouseMovement().x * xmag; // / container_scale;
    newpos.y = pos0.y + m.getMouseMovement().y * ymag; // / container_scale;
    if (*posAttr == newpos) return;
    *posAttr = newpos;
  }
}


ScrollAction::ScrollAction(Scrollpane& _pane, float _gain)
: ControlAction(_gain), pane(&_pane) {}

ScrollAction::ScrollAction(float _gain)
: ControlAction(_gain), pane(null) {}

void ScrollAction::setPane(Scrollpane& p) {pane = &p;}

void ScrollAction::mdrag(MouseEvent& e, ControlMenu& m) {
  if (!pane) return;
  
  if (e.isFirstDrag()) {
    arm_xpos = pane->getXScroll();
    arm_ypos = pane->getYScroll();
    xpos = ypos = 0;  
  }
  else {
    float xs = arm_xpos + m.getMouseMovement().x * xmag;
    if (xs > 100) xs = 100.; else if (xs < 0) xs = 0.;
  
    float ys = arm_ypos + m.getMouseMovement().y * ymag;
    if (ys > 100) ys = 100.; else if (ys < 0) ys = 0.;
  
    if (xs == xpos && ys == ypos) return;
    xpos = xs;
    ypos = ys;
    pane->setScrollImpl(xpos, ypos);  //cf. UPane::setScroll()
   // update paint (dont update geometry): necessaire dans ce cas
    pane->update(Update::paint);
  }
}

}


