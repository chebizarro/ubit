/*
 *  scrollpane.cpp
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
#include <iostream>
#include <ubit/core/on.h>
#include <ubit/core/call.h>
#include <ubit/core/boxgeom.h>
#include <ubit/ui/box.h>
#include <ubit/ui/scrollbar.h>
#include <ubit/ui/scrollpane.h>
#include <ubit/draw/style.h>
#include <ubit/core/application.h>
#include <ubit/core/config.h>
using namespace std;

namespace ubit {



Style* ScrollPane::createStyle() {
  Style& s = *new Style();
  s.viewStyle = &UPaneView::style;
  s.orient = Orientation::HORIZONTAL;
  s.halign = HAlign::FLEX;
  s.valign = VAlign::FLEX;
  s.hspacing = 1;
  s.vspacing = 1;
  // keep initial size
  s.local.size.width  = -1;
  s.local.size.height = -1;
  return &s;
}


ScrollPane::~ScrollPane() {
  //!!Att: scrollbar pas forcement enfnats du Pane !
  // (seront automatiquement detruit via le DAG inutile de s'en charger ici)
  if (vscrollbar) vscrollbar->setPane(null); vscrollbar = null;
  if (hscrollbar) hscrollbar->setPane(null); hscrollbar = null;
}

ScrollPane::ScrollPane(Args a) : 
Box(a), hscrollbar(null), vscrollbar(null) {
  addAttr(UOn::resize / ucall(this, &ScrollPane::resizeCB));
  constructs(true, true, a);
}

ScrollPane::ScrollPane(int vs_mode, int hs_mode, Args a) :
Box(a), hscrollbar(null), vscrollbar(null) {
  addAttr(UOn::resize / ucall(this, &ScrollPane::resizeCB));
  constructs(vs_mode, hs_mode, a);
}

void ScrollPane::constructs(int vs_mode, int hs_mode, const Args&) {
  Args ba;

  if (vs_mode) {
    vscrollbar = &uvscrollbar();
    if (Application::conf.transp_scrollbars) vscrollbar->setTransparent(true);
    ba += VAlign::flex + HAlign::right + vscrollbar;
    vscrollbar->setPane(this);
  }

  if (hs_mode) {
    hscrollbar = &uhscrollbar();
    if (Application::conf.transp_scrollbars) hscrollbar->setTransparent(true);
    ba += VAlign::bottom + HAlign::flex + hscrollbar;
    hscrollbar->setPane(this);
  }

  Border* border = new UCompositeBorder(ba);
  setAttr(*border);
  if (Application::conf.transp_scrollbars) border->setOverlaid(true);
  
  // the mouse wheel scrolls the pane
  observeChildrenEvents(UOn::wheel / ucall(this, &ScrollPane::wheelCB));
}


void ScrollPane::setTracking(bool state) {
  if (hscrollbar) hscrollbar->setTracking(state);
  if (vscrollbar) vscrollbar->setTracking(state);
}

ScrollPane& ScrollPane::showHScrollbar(bool state) {
  if (hscrollbar) hscrollbar->show(state);
  return *this;
}

ScrollPane& ScrollPane::showVScrollbar(bool state) {
  if (vscrollbar) vscrollbar->show(state);
  return *this;
}
  
ScrollPane& ScrollPane::showHScrollButtons(bool state) {
  ScrollBar* sb = hscrollbar;
  if (sb) {
    if (sb->getLessButton()) sb->getLessButton()->show(state);
    if (sb->getMoreButton()) sb->getMoreButton()->show(state);
  }
  return *this;
}

ScrollPane& ScrollPane::showVScrollButtons(bool state) {
  ScrollBar* sb = vscrollbar;
  if (sb) {
    if (sb->getLessButton()) sb->getLessButton()->show(state);
    if (sb->getMoreButton()) sb->getMoreButton()->show(state);
  }
  return *this;
}

void ScrollPane::unsetHScrollbar() {
  hscrollbar = null;
}

void ScrollPane::unsetVScrollbar() {
  vscrollbar = null;
}

Box* ScrollPane::getScrolledBox() {
  for (ChildIter c = cbegin(); c != cend(); ++c) {
    Box* box = (*c)->toBox();
    if (box) return box;
  }
  return null;    // not found
}

View *ScrollPane::getScrolledView(View* pane_view) {
  Box* child = getScrolledBox();
  return (child ? child->getViewInImpl(pane_view /*,null*/) : null);
}

void ScrollPane::wheelCB(UWheelEvent& e) {
  if (vscrollbar) {
    if (e.getWheelDelta() > 0) vscrollbar->pressScrollButton(e,-1);
    else if (e.getWheelDelta() < 0) vscrollbar->pressScrollButton(e,+1);
  }
}

// Set the Vertical and Horizontal Scrollbars
// Notes:
// - if 'add_to_pane' is true (default case), the scrollbar is added to
//   the Pane child list and it will appear inside the Pane.
// - otherwise, the scrollbar is NOT added to the Pane and MUST be added
//   as a child of another box. This makes it possible to CONTROL a Pane
//   by a scrollbar that is NOT included in this Pane.

/*
void UPane::setVScrollbar(ScrollBar *sc, bool add_to_pane) {
  // enleve le scrollbar du Pane (s'il y est: sinon ne fait rien)
  // mais NE LE DETRUIT PAS (car on ne sait pas qui a cree ce scrollbar:
  // il n'a pas forcement ete cree par le UPane ou le ScrollPane)
  if (vscrollbar) {
    vscrollbar->setPane(null);
    remove(vscrollbar, false);
  }
  vscrollbar = sc;
  if (sc) {
    sc->setPane(this);
    if (add_to_pane) {
      add(VAlign::flex);
      add(HAlign::right);
      add(sc);
      // pour que les add() suivants mettent les enfants en zone centrale
      add(HAlign::flex);
    }
  }
}
*/

void ScrollPane::makeVisible(Box& child) {
  bool move_x = false, move_y = false;
  
  for (View* v = child.getView(0); v != null; v = v->getNext()) {
    Box* list = getScrolledBox();
    View* listv = list ? list->getViewContaining(*v) : null;
    View* panev = getViewContaining(*v);
    
    if (listv && panev
        && listv->getHeight() > 0 && listv->getWidth() > 0 //ICI isShown() serait utile!!!
        && panev->getHeight() > 0 && panev->getWidth() > 0
        ) {
      Point pane_pos = View::convertPosTo(*panev, *v, Point(0,0));
      
      if (pane_pos.x + v->getWidth() > panev->getWidth()) move_x = true;
      else if (pane_pos.x < 0) move_x = true;
      
      if (pane_pos.y + v->getHeight() > panev->getHeight()) move_y = true;
      else if (pane_pos.y < 0) move_y = true;
      
      if (move_x || move_y) {
        Point list_pos = View::convertPosTo(*listv, *v, Point(0,0));
        if (move_x) {
          float _xscroll = (float)list_pos.x / listv->getWidth() * 100;
          setScrollImpl(_xscroll, yscroll);
        }
        if (move_y) {
          float _yscroll = (float)list_pos.y / listv->getHeight() * 100;
          setScrollImpl(xscroll, _yscroll);
        }
      }
    }
  }
}

// synchronizes the scrollbars (att aux loops!)

ScrollPane& ScrollPane::setScroll(float _xscroll, float _yscroll) {
  // NO_DELAY necessaire pour update immediat par appel de setScroll()
  //update(Update::LAYOUT | Update::NO_DELAY);
  doUpdate(Update::LAYOUT);
  setScrollImpl(_xscroll, _yscroll);
  return *this;
}

void ScrollPane::setScrollImpl(float _xscroll, float _yscroll) {
  if (_xscroll < 0) _xscroll = 0; else if (_xscroll > 100) _xscroll = 100;
  if (_yscroll < 0) _yscroll = 0; else if (_yscroll > 100) _yscroll = 100;
  //cerr << _xscroll <<" " << xscroll  <<" " <<_yscroll  <<" " << yscroll<<endl;
  
  // 20oct08: attempt to avoid losing a lot of time when limits are reached
  if (xscroll == _xscroll && yscroll == _yscroll) return;
  
  float xoffset_max = 0, yoffset_max = 0;  // pour toutes les Views de 'pane' 
  
  for (View* view = views; view != null; view = view->getNext()) {
    UPaneView* pane_view = dynamic_cast<UPaneView*>(view);  // UPaneView par constr
    View *winview = null;
    
    if (pane_view && (winview = pane_view->getWinView())) {
      View* viewport_view = getScrolledView(pane_view);
      
      if (viewport_view) {
        PaintEvent e(UOn::paint, winview);
        e.setSourceAndProps(viewport_view);
        float xoffset = 0, yoffset = 0;
        //float deltax = 0, deltay = 0;
        
        // horizontal scroll
        float viewport_w = pane_view->getWidth()
        - pane_view->padding.left.val - pane_view->padding.right.val;
        
        xoffset = _xscroll * (viewport_view->getWidth() - viewport_w) / 100.0; // + 0.5;
        if (xoffset >= 0) {
          //deltax = xoffset - pane_view->getXScroll();
          pane_view->setXScroll(xoffset);
        }
        
        // vertical scroll
        float viewport_h = pane_view->getHeight()
        - pane_view->padding.top.val - pane_view->padding.bottom.val;
        
        yoffset = _yscroll * (viewport_view->getHeight() - viewport_h) / 100.0; // + 0.5;
        if (yoffset >= 0) {
          //deltay = yoffset - pane_view->getYScroll();
          pane_view->setYScroll(yoffset);
        }
        
        if (xoffset >= 0 || yoffset >= 0) {
          xoffset_max = std::max(xoffset_max, xoffset);
          yoffset_max = std::max(yoffset_max, yoffset);
          //viewport_view->getBox()->repaint();
        }
      }
    }
  }

  // correction du scroll en fonction des contraintes du pane
  // (typiquement si le viewport est plus petit que le scrollpane)
  if (xoffset_max == 0) xscroll = 0; else xscroll = _xscroll;
  if (yoffset_max == 0) yscroll = 0; else yscroll = _yscroll;

  // NB: verif callbacks OK
  if (hscrollbar) *hscrollbar->pvalue = xscroll;
  if (vscrollbar) *vscrollbar->pvalue = yscroll;
  repaint();
  
  /*
   cette fonction fait n'importe quoi
  // update scrollbars
  if (hscrollbar) hscrollbar->setValueImpl(xscroll, true);
  if (vscrollbar) vscrollbar->setValueImpl(yscroll, true);
*/
}


void ScrollPane::resizeCB(UResizeEvent& e) {
  UPaneView *pane_view  = dynamic_cast<UPaneView*>(e.getView());

    // !!! A COMPLETER !!! prendre en compte les Units 
  
  if (hscrollbar) {
    float viewport_w = pane_view->getWidth()
    - pane_view->padding.left.val - pane_view->padding.right.val;

    View *viewport_view = getScrolledView(pane_view);
    if (!viewport_view) return;

    // corrbug div by 0
    if (viewport_view->getWidth() == viewport_w) hscrollbar->setValue(0);
    else {
      float xoffset 
      = hscrollbar->getValue() * (viewport_view->getWidth()-viewport_w) /100.0;

      float newscroll = (float)xoffset / (viewport_view->getWidth()-viewport_w) *100.0;
      
      hscrollbar->setValue(newscroll);
    }
  }

  if (vscrollbar) {
    float viewport_h = pane_view->getHeight()
    - pane_view->padding.top.val - pane_view->padding.bottom.val;

    View *viewport_view = getScrolledView(pane_view);
    if (!viewport_view) return;
    
    if (viewport_view->getHeight() == viewport_h) vscrollbar->setValue(0);
    else {
      float yoffset = 
      vscrollbar->getValue() * (viewport_view->getHeight()-viewport_h) /100.0;
      
      float newscroll = (float)yoffset / (viewport_view->getHeight()-viewport_h) *100.0;

      vscrollbar->setValue(newscroll);
    }
  }
}


ViewStyle UPaneView::style(&UPaneView::createView, UCONST);

UPaneView::UPaneView(Box* box, View* parview, UHardwinImpl* w) : 
  View(box, parview, w), 
  padding(0,0),
  xscroll(0),
  yscroll(0) {
}

// "static" constructor used by ViewStyle to make a new view
View* UPaneView::createView(Box* box, View* parview, UHardwinImpl* w) {
  return new UPaneView(box, parview, w);
}

}

