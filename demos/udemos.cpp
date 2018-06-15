/* ***********************************************************************
 *
 *  udemos.cpp: a program that launches various Ubit demos
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

#include <iostream>
#include <cmath>
#include <ubit/ubit.hpp>
#include <ubit/ufinder.hpp>
#include "viewer.hpp"
#include "gltriangle.hpp"
#include "edi.hpp"
#include "ima.hpp"
#include "draw.hpp"
#include "zoom.hpp"
//#include "zoomlib.hpp"
#include "toolglass.hpp"
#include "piemenus.hpp"
#include "magiclens.hpp"
using namespace std;
using namespace ubit;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class Desktop : public UBox {
public:
  static const int WIN_WIDTH = 300;
  static const int ICON_WIDTH = 100;
  static const int ICON_HEIGHT = 80;
  static const int ICON_SPACING = 5;
  static const float ICON_CONTENT_SCALE;

  enum DockMode {LEFT, RIGHT, TOP, BOTTOM, MATRIX, SLANTED_PLANE};
  
  Desktop(class Demos*);
  void setDockMode(DockMode);
  UPalette& addWidget(const UStr& name, UBox&);
  
private:
  class Demos& demos;
  DockMode dock_mode;
  UElem widgets;
  void desktopResized(UResizeEvent&);
  void widgetPosChanged(UEvent&, UPalette*);
  void widgetSizeChanged(UEvent&, UPalette*);
};

const float Desktop::ICON_CONTENT_SCALE = 0.3;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class ZPane : public UZoompane {
public:
  static const int ITEMS_PER_LINE = 4;
  static const int ITEM_WIDTH  = 250;
  static const int ITEM_HEIGHT = 200;
  static const int SPACING = 5;
  
  ZPane(class Demos*);
  void addWidget(const UStr& name, UBox&);
  
private:
  class Demos& demos;
  int item_count;
  uptr<UBox> last_row;
  uptr<UPosControl> pane_pos_ctrl;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class Demos : public UFrame {
public:
  Demos();
  void addDemo(const UStr& name, UBox& widget);
  void addDemo(const UStr& name, UBox* widget);
  void maximize(UPalette*);
  void minimize(UPalette*);

//private:
  UBar toolbar;
  UBox mainbox;
  Desktop desktop;
  ZPane zpane;
  UPalette& fullbox;
  //Anim anim;
};

// -----------------------------------------------------------------------------

int main(int argc, char* argv[]) 
{  
  // sets the default background of the UAppli
  //UAppli::conf.setDefaultBackground(UBackground::velin);
  
  // requires a DepthBuffer size of 16 when available
  //UAppli::conf.setDepthBuffer(16);

  // use transparent scrollbars
  UAppli::conf.setTransparentScrollbars(true);
    
  // create the application context
  UAppli appli(argc, argv);
  
  appli.setImaPath("../images/covers");    // images for zoomlib
  
  // create the main frame window, add it to the appli and show it
  Demos demos;
  demos.show();
  
  // start the event loop
  return appli.start();
}

// -----------------------------------------------------------------------------

Demos::Demos() : 
desktop(this),
zpane(this),
fullbox(*new UPalette())
//anim(&fullbox, 0.6, 1.0)
{
  fullbox.titleBar().addAttr(ucall(this, &fullbox, &Demos::minimize));
  fullbox.show(false);
  desktop.show(false);
  mainbox.addAttr(UBackground::velin);
  mainbox.add(desktop + zpane + fullbox);
  
  toolbar.
  add(uleft()
      + ubutton("ZUI" + ushow(desktop, false) + ushow(zpane, true))
      + ubutton("Left" + ucall(&desktop, Desktop::LEFT, &Desktop::setDockMode))
      + ubutton("Right" + ucall(&desktop, Desktop::RIGHT, &Desktop::setDockMode))
      + ubutton("Top" + ucall(&desktop, Desktop::TOP, &Desktop::setDockMode))
      + ubutton("Bottom" + ucall(&desktop, Desktop::BOTTOM, &Desktop::setDockMode))
      + ubutton("Matrix" + ucall(&desktop, Desktop::MATRIX, &Desktop::setDockMode))
      + ubutton("SlantedPlane" + ucall(&desktop,Desktop::SLANTED_PLANE,&Desktop::setDockMode))
      + uhflex() + ulabel("")
      + uright() + ubutton("Quit" + ucall(0, &UAppli::quit))
      );
  
  add(utop() + toolbar 
      + uvflex() + mainbox
      + ubottom() + "Press right mouse button (or Ctrl + any mouse button) to open the global menu");
  
  addDemo("Finder", new UFinder);
  addDemo("ZoomMap", new ZoomDemo);
  addDemo("Editor", new Edi);  
  //addDemo("ZoomLib", new ZoomLib);
  addDemo("GLTriangle", new GLTriangle);
  addDemo("ToolGlass", new ToolglassDemo);
  addDemo("Drawing", new DrawDemo);
  addDemo("Images", new Ima);
  addDemo("PieMenus", new PiemenuDemo);
  addDemo("MagicLens", new MagicLensDemo);
  addDemo("FishEye", new Fisheye);
}

void Demos::addDemo(const UStr& name, UBox& demo) {
  desktop.addWidget(name, demo);
  zpane.addWidget(name, demo);
}

void Demos::addDemo(const UStr& name, UBox* demo) {
  desktop.addWidget(name, *demo);
  zpane.addWidget(name, *demo);
}

void Demos::maximize(UPalette* i) {
  cerr << "maximize " << endl;
  //fullbox.setName(i->getName());
  fullbox.content().removeAll();
  fullbox.content().add(*i->content().getChild(0)); // QUE FIRST CHILD !!!
  fullbox.contentScale() = 1.;
  desktop.show(false);
  zpane.show(false);
  fullbox.show(true);
  //anim.run(true);
}

void Demos::minimize(UPalette* i) {
  fullbox.show(false);
  zpane.show(true);
  //anim.run(false);
}

// =============================================================================

Desktop::Desktop(Demos* _demos) :
demos(*_demos), 
dock_mode(LEFT) {
  addAttr(UOn::resize / ucall(this, &Desktop::desktopResized));
  add(widgets);
}

UPalette& Desktop::addWidget(const UStr& name, UBox& box) {
  UPalette& w = *new UPalette(box);
  w.setAttr(UBackground::metal);

  w.title().add(ustr(name));

  w.contentScale() = ICON_CONTENT_SCALE;
  w.pos().onChange(ucall(this, &w, &Desktop::widgetPosChanged));
  w.size().set(ICON_WIDTH, ICON_HEIGHT);
  w.size().onChange(ucall(this, &w, &Desktop::widgetSizeChanged));
  w.resizeBtn().show();

  UZoommenu* zmenu = new UZoommenu(w, w);
  w.add(zmenu);
  // !!! A REVOIR : SPECIFIER QUE C'EST RIGHT BUTTON
  w.catchEvents(UOn::mpress / ucall(zmenu, &UZoommenu::openMenuCB));
  widgets.add(w);
  return w;
}

void Desktop::setDockMode(DockMode m) {
  dock_mode = m;
  float x = ICON_SPACING;
  float y = ICON_SPACING;
  int item_count = 0, items_per_line = 0;

  if (dock_mode == RIGHT)
    x = getView()->getWidth() - ICON_WIDTH - ICON_SPACING;

  if (dock_mode == BOTTOM || dock_mode == SLANTED_PLANE)
    y = getView()->getHeight() - ICON_HEIGHT - ICON_SPACING;
  
  if (dock_mode == MATRIX)
    items_per_line = ::sqrt(double(getChildCount()));

  demos.zpane.show(false);
  show(true);

  for (UChildIter i = widgets.cbegin(); i != widgets.cend(); ++i) {
    UPalette* w = dynamic_cast<UPalette*>(*i);
    if (!w) continue;
    
    w->pos().set(x, y);
    w->contentScale() = ICON_CONTENT_SCALE;
    w->size().set(ICON_WIDTH, ICON_HEIGHT);
    
    switch (dock_mode) {
      case LEFT:
      case RIGHT:   
        y += ICON_HEIGHT + ICON_SPACING;
        if (y + ICON_HEIGHT > getView()->getHeight()) {
          if (dock_mode == LEFT) x += ICON_WIDTH + ICON_SPACING;
          else x -= ICON_WIDTH + ICON_SPACING;
          y = ICON_SPACING;
        }
        break;
        
      case TOP:
      case BOTTOM:
      case SLANTED_PLANE:
        x += ICON_WIDTH + ICON_SPACING;
        if (x + ICON_WIDTH > getView()->getWidth()) {
          x = ICON_SPACING;
          if (dock_mode == TOP) y += ICON_HEIGHT + ICON_SPACING;
          else y -= ICON_HEIGHT + ICON_SPACING;
        }
        break;
        
      case MATRIX:
         x += ICON_WIDTH + ICON_SPACING;
        item_count++;
        if (item_count % items_per_line == 0) {
          x = ICON_SPACING;
          y += ICON_HEIGHT + ICON_SPACING;
        }
        break;
    }
  }
}

void Desktop::desktopResized(UResizeEvent&) {
  setDockMode(dock_mode);
}

void Desktop::widgetSizeChanged(UEvent& e, UPalette* widget) {
  float w = widget->size().getWidth().val;
  float h = widget->size().getHeight().val;
  if (w < ICON_WIDTH) w = ICON_WIDTH;
  if (h < ICON_HEIGHT) h = ICON_HEIGHT;
  widget->size().set(w,h);
  
  float dim = min(w, h);
  float s = (dim - ICON_WIDTH) / (WIN_WIDTH - ICON_WIDTH) + ICON_CONTENT_SCALE;
  widget->contentScale() = s > 1. ? 1. : s;
}

void Desktop::widgetPosChanged(UEvent& e, UPalette* widget) {
  if (dock_mode == SLANTED_PLANE) {
    float ratio = widget->pos().getY().getValue() / (getView()->getHeight() + 100);
    float scale = 5 - 5 * ratio;
    widget->size().set(ICON_WIDTH * scale, ICON_HEIGHT * scale);
    widget->contentScale() = (scale/2. < 1. ? scale/2. : 1);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

ZPane::ZPane(Demos* _demos) :
demos(*_demos), 
item_count(0), 
last_row(null),
pane_pos_ctrl((new UPosControl)->setModel(&viewportPos()))
{
  setAttr(upadding(SPACING,SPACING));
  viewport().setAttr(uvspacing(SPACING));
}

void ZPane::addWidget(const UStr& name, UBox& content) {
  UPalette& item = *new UPalette(uscale(0.5) + content);
  
  item.title().add(ustr(name));
  //item.titlebar().addAttr(ucall(&demos, &item, &Demos::maximize));  // !!!
  item.setPosModel(null);
  item.setPosControlModel(pane_pos_ctrl);
  item.setAttr(usize(ITEM_WIDTH, ITEM_HEIGHT));
  item.setAttr(UBackground::metal);
    
  if (item_count % ITEMS_PER_LINE == 0) {
    last_row = uhbox();
    last_row->setAttr(uhspacing(SPACING));
    viewport().add(*last_row);
  }
  last_row->add(item);
  item_count++;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0

 struct Anim : public UTimer {
 Anim(DemoBox* _demobox, float _min_scale, float _max_scale);
 void run(bool maximize);
 private:
 Icon* demobox;
 float min_scale, max_scale, scale_incr;
 bool in_maximize_mode;
 void timerCB();
 };
 
Anim::Anim(DemoBox* _demobox, float _min_scale, float _max_scale) : 
demobox(_demobox), 
min_scale(_min_scale), max_scale(_max_scale), scale_incr(0.1),
in_maximize_mode(true)
{
  onAction(ucall(this, &Anim::timerCB));
}

void Anim::run(bool _maximize) {
  in_maximize_mode = _maximize;
  if (in_maximize_mode) demobox->scale = min_scale;
  else demobox->scale = max_scale;
  demobox->show(true);
  start(500, -1);
  //oevrviews.show(false);
}

void Anim::timerCB() {
  if (in_maximize_mode) {
    if (demobox->scale >= max_scale) {
      stop();
    }
    else {
      demobox->scale = demobox->scale + scale_incr;
    }
  }
  
  else { // (!maximize_mode) 
    if (demobox->scale <= min_scale) {
      stop();
    }
    else {
      demobox->scale = demobox->scale - scale_incr;
    }
  }
  
  //oevrviews.show(false);
}

#endif

