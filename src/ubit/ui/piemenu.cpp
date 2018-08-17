/*
 *  piemenu.cpp : Pie and Control menus
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
#include <ubit/ubit.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

Style* PieMenu::createStyle() {
  Style* style = Menu::createStyle();
  //style->local.alpha = 0.;
  style->local.background = null;    // !! alpha et background sont lies !!!!
  style->local.border = null;
  style->local.padding.set(1, 1);
  style->setBgcolors(Color::none);
  //style->setColors(Color::white, Color::yellow);
  style->setColors(Color::white, Color::white);
  //style->setColor(UOn::ARMED, Color::yellow);
  return style;
}


PieMenu::PieMenu() :
menu_style(COMPOUND), 
spring_mode(true), ctlmenu_mode(false),
novice_mode(false), in_show_function(false),
//center_radius(6), item_radius(6), item_margin(1),
curitem(-2),
show_delay(200),
pie_size(-1,-1),  // -1 means "keap initial size"
parmed(null),
from_menu(null), to_menu(null),
pie_radius(-1),   // -1 means auto
center_radius(7),
item_radius(7)
{
  // A REVOIR AVEC STYLES ET Background avance !!!!!@@@@@@
  // de plus c'est le backround et le alpha du Piemenu qui devraient etre utilise !
  static uptr<Color> default_pie_color = new Color(Color::black, 100u);
  static uptr<Color> default_pie_border_color = new Color(Color::black, 100u);
  //static uptr<Color> default_slice_color = new Color(Color::black, 75u);  
  static uptr<Color> default_slice_color = new Color(Color::blue);  
  //static uptr<Color> default_center_color = new Color(Color::white, 255u);
  static uptr<Color> default_center_color = new Color(Color::white);
  static uptr<Color> default_center_border_color = new Color(Color::red, 255u);
  
  ppie_color = default_pie_color; 
  ppie_border_color = default_pie_border_color;
  pslice_color = default_slice_color;
  pcenter_color = default_center_color;
  pcenter_border_color = default_center_border_color;
  
  setSoftwin();  // !must be a softwin!
  
  // the menu won't close on a mrelease event
  //this->disableMenuClosing();
  
  add(UOn::mdrag  / ucall(this, &PieMenu::motionCB)   // DRAG dans tous les cas
      + UOn::mpress / ucall(this, &PieMenu::pressCB)
      + UOn::mrelease / ucall(this, &PieMenu::releaseCB)
      + UOn::paint  / ucall(this, &PieMenu::paintCB)
      + UOn::resize / ucall(this, &PieMenu::resizeCB)
      + UOn::hide / ucall(this, &PieMenu::hideCB)
      + pie_size
      + gitems
      );
  
  for (int k = 0; k < 8; ++k) gitems.add("");  // no item
  gitems.ignoreEvents();  // events are redirected by the menu
  
  // this timer starts the novice mode after novice_mode_delay
  ptimer = new Timer();
  ptimer->onAction(ucall(this, &PieMenu::startNoviceMode));
}

PieMenu::~PieMenu() {
  unlinkFromAndToMenus();
}

/* ==================================================== [Elc] ======= */
/*
 int getCenterRadius() const {return center_radius;}
 //returns the radius of the rest area.
 
 //void setCenterRadius(int d) {center_radius = d;}
 * changes the radius of the rest area.
 * items are armed when the cursor crosses the rest zone border.
 *
 
 int getItemRadius() const {return item_radius;}
 // returns the radius of the items from the center.
 
 void setItemRadius(int d) {item_radius = d;}
 // changes the radius of the items from the center.
 */

void PieMenu::setMenuType(int type) {
  menu_style = type;
  for (ChildIter it = cbegin(); it != cend(); ++it) {
    PieMenu* ch = dynamic_cast<PieMenu*>(*it);
    if (ch) ch->setMenuType(type);
  }
}

Scale* PieMenu::getScale() {  // !!!
  Scale* s = null;
  attributes().findClass(s);
  if (!s) children().findClass(s);
  return s;
}

float PieMenu::getScaleValue() {  // !!!
  Scale* s = getScale();
  return s ? float(*s) : 1.;
}

Box* PieMenu::createItem() {
  Box* b = new Button();
  b->addAttr(Background::none); // + UOn::arm/Color::yellow);
  return b;
}

Box* PieMenu::getItem(int n) const {
  if (n < 0 || n >= 7) return null;
  Node* i = gitems.getChild(n);
  return i ? dynamic_cast<Box*>(i) : null;
}

Box& PieMenu::item(int n) {
  if (n < 0 || n >= 7) n = 7;  // take the last one
  Box* i = getItem(n);
  if (!i) {
    gitems.remove(n);
    i = createItem();
    gitems.add(i, n);
  }
  return *i;
}

Box* PieMenu::getSelectedItem() const {return parmed;}
int PieMenu::getSelectedIndex() const {return curitem >= 0 ? curitem : -1;}

/*
 Box* PieMenu::setItem(int n, Box& child, bool auto_del) {
 if (n < 0 || n >= 8) return null;
 gitems.remove(gitems.child(n), auto_del);
 gitems.add(child, n);
 return &child;
 }
 */

void PieMenu::show(bool state, Display* disp) {
  if (state) {
    novice_mode = true;
    gitems.show(novice_mode);
    reset(null);
  }
  // NB: unlinkFromAndToMenus: fait dans hideCB()
  PopupMenu::show(state, disp);
}


void PieMenu::open(MouseEvent& e) {
  // we have a pbm here because of f->boxMousePress(): this may generate an
  // infinite loop if the coordinates are incorrect. in_show_function avoid this
  // pbm in the stange buggy cases when this might happen
  if (in_show_function) return;
  in_show_function = true;
  
  reset(&e);  // reset() inits menu data and its layout
  
  // moves the menu so that it will be centered on the event location
  // !beware: setPos() is relative to the parent which ignores the menu scale
  float scale = getScaleValue();      // !!!
  setPos(e, 
         //(x - pie_size.getWidth().val/2) * scale, 
         //(y - pie_size.getHeight().val/2) * scale);
         -pie_size.getWidth().val/2 * scale, 
         -pie_size.getHeight().val/2 * scale);
  
  novice_mode = (show_delay == 0);  // starts novice mode (and show items) if delay = 0
  gitems.show(novice_mode);
  
  PopupMenu::show(true, e.getDisp());  // shows the menu on the screen
  
  // this will generate a MousePress on the menu (si motionCB est
  // active par des MouseDrag comme dans les CtlMenus)
  if (e.getFlow()) e.getFlow()->redirectMousePress(e, getWinView(e.getDisp()));
  
  // arms the timer that will start the novice mode after a delay
  if (!novice_mode) ptimer->start(show_delay, 1);
  in_show_function = false;
}


void PieMenu::reset(MouseEvent* e) {
  curitem = -2;  
  // il faut 2 appels a update(LAYOUT) :
  // - le 1er update() calcule la taille de chaque item
  // - layout() positionne les items et calcule la taille globale width, height
  // - le 2e update() calcule la taille du menu
  // HIDDEN_OBJECTS est necessaire car le menu n'est pas encore affiche
  // NO_DELAY calcule immediatement (par defaut les update() sont bufferises)
  //Update upd(Update::LAYOUT | Update::HIDDEN_OBJECTS | Update::NO_DELAY);
  //update(upd);
  doUpdate(Update::LAYOUT | Update::HIDDEN_OBJECTS);
  parmed = null;
  armpos.x = armpos.y = 0;
  mousepos.x = mousepos.y = 0;
}

void PieMenu::startNoviceMode() {
  novice_mode = true;
  gitems.show(true);
}

/* ==================================================== [Elc] ======= */

void PieMenu::resizeCB(UResizeEvent& e) {
  float scale = getScaleValue();
  float rad = 0;
  
  if (pie_radius.val > 0) {
    rad = pie_radius.val;
  }
  else {
    float max_iw = 0, max_ih = 0;  // max item width & height
    ChildIter i = gitems.cbegin();
    
    for (int k = 0; i != gitems.cend(); ++i, ++k) {
      
      Box* b = (*i)->toBox();
      if (!b) continue;
      View* v = b->getView();
      if (!v) continue;
      
      switch(k) {
        case 0: // right
        case 4: // left
          if (max_iw < v->getWidth())  max_iw = v->getWidth();
          break;
        case 2: //top
        case 6: //bottom
          if (max_ih < v->getHeight()) max_ih = v->getHeight();
          break;
      }
    }
    
    // iW and iH must be normalized so they dont depend on scale
    max_iw = max_ih = max(max_iw, max_ih) / scale;
    rad = max_iw + item_radius; // + item_margin;
  }
  
  // normalized radius and diameter of the menu 
  float diam = 2 * rad;
  
  // the global width & height of the menu is set to its diameter
  pie_size.set(diam, diam);
  
  {
    ChildIter i = gitems.cbegin();
    for (int k = 0; i != gitems.cend(); ++i, ++k) {
      
      Box* b = dynamic_cast<Box*>(*i);
      if (!b) continue;
      View* v = b->getView();
      if (!v) continue;
      
      float iw = v->getWidth() / scale;  // normalized item width
      float ih = v->getHeight()/ scale;  // normalized item height
      float a = k * M_PI / 4.;
      float ring = rad - item_radius;
      Point p;
      
      switch(k) {
        case 0: // right   
          p.x = (rad + item_radius) + (ring - iw) / 2.; p.y = (diam - ih) / 2;
          break;
          
        case 1: { // top right
          // on positionne par rapport au milieu de la bande selon la direction a
          // et on deduit la moite de la largeure/hauteur de l'item
          float x_mid_ring = rad + fabs((rad + item_radius) /2. * cos(a));
          p.x = x_mid_ring - iw/2; p.y = rad*0.6 -ih/2;
        } break;
          
        case 2: // top
          p.x = (diam - iw) / 2; p.y = ring * 0.3 - ih/2;
          break;
          
        case 3: { // top left  
          float x_mid_ring = rad - fabs((rad + item_radius) /2. * cos(a));
          p.x = x_mid_ring - iw/2; p.y = rad * 0.6 -ih/2;
        } break;
          
        case 4: // left
          p.x = (rad - item_radius - iw) / 2; p.y = (diam - ih) / 2;
          break;
          
        case 5: {// bottom left 
          float x_mid_ring = rad - fabs((rad + item_radius) /2. * cos(a));
          p.x = x_mid_ring - iw/2; p.y = diam - rad*0.6 - ih/2;
        } break;
          
        case 6: // bottom
          p.x = (diam - iw) / 2; p.y = diam - ring*0.3 - ih/2;
          break;
          
        case 7: { // bottom right
          float x_mid_ring = rad + fabs((rad + item_radius) /2. * cos(a));
          p.x = x_mid_ring - iw/2; p.y = diam - rad*0.6 - ih/2;
        } break;
      }
      
      // the position is forced (not changed by the layout manager)
      //v->setAutoPositioning(false);
      //v->setPos(p);
      
      b->obtainAttr<UPos>() = p;
    }
  }
}


void PieMenu::paintCB(PaintEvent& e) {
  View* v = e.getView();
  if (!v || !novice_mode) return;
  Graph g(e);
  
  if (ppie_color && *ppie_color != Color::none) {
    //if (pie_alpha) g.setAlpha(*pie_alpha);
    g.setColor(*ppie_color);
    g.fillEllipse(0, 0, v->getWidth(), v->getHeight());
  }
  
  if (curitem >= 0 && pslice_color && *pslice_color != Color::none) {
    g.setColor(*pslice_color);
    g.fillArc(0, 0, v->getWidth(), v->getHeight(), curitem*45-22, 45);
  }
  
  if (curitem < 0 && pcenter_color && *pcenter_color != Color::none) {
    g.setColor(*pcenter_color);
    g.fillEllipse(v->getWidth()/2 - center_radius, v->getHeight()/2 - center_radius, 
                  center_radius*2, center_radius*2);
  }
  
  if (pcenter_border_color) {
    g.setColor(*pcenter_border_color);
    g.drawEllipse(v->getWidth()/2 - center_radius, v->getHeight()/2 - center_radius, 
                  center_radius*2, center_radius*2);
  }
  
  if (ppie_border_color) {
    g.setColor(*ppie_border_color);
    g.drawEllipse(0, 0, v->getWidth()-1, v->getHeight()-1);
  }  
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PieMenu::forwardToMenu(PieMenu* submenu) {
  if (to_menu != submenu) {
    if (to_menu) to_menu->from_menu = null;
    to_menu = submenu;
    if (to_menu) to_menu->from_menu = this;
  }
}

void PieMenu::receiveFromMenu(PieMenu* supermenu) {
  if (from_menu != supermenu) {
    if (from_menu) from_menu->to_menu = null;
    from_menu = supermenu;
    if (from_menu) from_menu->to_menu = this;
  }
}

void PieMenu::unlinkFromAndToMenus() {
  if (from_menu) {
    from_menu->forwardToMenu(null);
    from_menu = null;
  }    
  if (to_menu) {
    to_menu->receiveFromMenu(null);
    to_menu = null;
  }
}  

void PieMenu::addSubMenu(int N, PieMenu* submenu) {
  Box& btn = item(N);
  if (!submenu) return;
  submenu->setShowDelay(0);
  
  btn.add(submenu
          //+ (UOn::arm/showSubmenu)(&submenu)
          //+ (UOn::disarm/hideSubmenu)(&submenu)
          + UOn::arm / ucall(this, submenu, &PieMenu::armItemCB)
          + UOn::disarm / ucall(this, submenu, &PieMenu::disarmItemCB)
          );
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

float PieMenu::getCentredCoords(MouseEvent& e, Point& p, bool& must_forward) {
  View* v = getView();
  Point c = getCenterInContainingWin();
  p.x = e.getScreenPos().x - e.getWinView()->getScreenPos().x - c.x;
  p.y = e.getScreenPos().y - e.getWinView()->getScreenPos().y - c.y;
  
  float d = ::sqrt(p.x*p.x + p.y*p.y);   // dist from center
  must_forward = false;
  
  if (to_menu) {
    switch (menu_style) {
      case MULTISTROKE:
        //if (mmodes & CTL_BROWSE) {
        //  if (!e.isControlDown()) must_forward = true; 
        //}
        //else 
        must_forward = true;
        break;
      default:
        if (d > v->getWidth()/2) must_forward = true;
        break;
    }
  }
  
  return d;
}

short PieMenu::getItemFromPos(const Point& p) const {
  float ang;     // value in [-PI/2, +PI/2]
  if (p.x == 0) ang = p.y > 0 ? -M_PI/2 : M_PI/2;
  else ang = ::atan(double(-p.y / p.x));
  if (p.x < 0) ang = (p.y < 0 ) ? M_PI + ang : ang - M_PI;
  
  short newitem = -2;
  if (ang >= -M_PI/8.0 && ang <= M_PI/8.0)
    newitem = 0;
  else if (ang > M_PI/8.0 && ang <= 3.0*M_PI/8.0)
    newitem = 1;
  else if (ang > 3.0*M_PI/8.0 && ang <= 5.0*M_PI/8.0)
    newitem = 2;
  else if (ang > 5.0*M_PI/8.0 && ang <= 7.0*M_PI/8.0)
    newitem = 3;
  else if (ang > 7.0*M_PI/8.0 && ang <= 8.0*M_PI/8.0)
    newitem = 4;
  else if (ang > -3.0*M_PI/8.0 && ang <= -1.0*M_PI/8.0)
    newitem = 7;
  else if (ang > -5.0*M_PI/8.0 && ang <= -3.0*M_PI/8.0)
    newitem = 6;
  else if (ang > -7.0*M_PI/8.0 && ang <= -5.0*M_PI/8.0)
    newitem = 5;
  else if (ang > -8.0*M_PI/8.0 && ang <= -7.0*M_PI/8.0)
    newitem = 4;
  else
    newitem = 4;
  
  return newitem;
}

// !!! PBM: rend parentWin pour softwins, mais this pour hardwin !!!
// !!! DE MEME: meme pbm pour View::getWin() : comportement different
//     pour hardwin et softwin!

Window* PieMenu::getContainingWin(Display* disp) const {   // !!BUGGY si hardwin!!
  View* v = getWinView(disp);
  return v ? v->getWin() : null;
}

Point PieMenu::getCenterInContainingWin(Display* disp) const {
  Window* w = getContainingWin(disp);
  Dimension dim = getSize(disp);
  Point p = w ? getPos(*w, disp) : Point(0,0);
  p.x += dim.width / 2;
  p.y += dim.height / 2;
  return p; 
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PieMenu::armItemCB(MouseEvent& e, PieMenu* m2) {
  if (!m2) return;
  
  if (getSelectedIndex() < 0) return;  
  float a = getSelectedIndex() * M_PI / 4.;
  
  // si submenu concentrique, adapter les rayons pour qu'ils soeint inclus les
  // uns dans les autres
  if (menu_style == CONCENTRIC) {
    m2->item_radius = getWidth()/2.;
  }
  
  // sinon le 1er getWidth est pas a jour (a cause de PieMenu::resizeCB)
  m2->doUpdate(Update::LAYOUT | Update::HIDDEN_OBJECTS);
  
  Point c = getCenterInContainingWin();
  Point p;
  
  switch (menu_style) {
    case CONCENTRIC:
      p.x = c.x - m2->getWidth()/2.;
      p.y = c.y - m2->getHeight()/2.;  
      break;
    case MULTISTROKE:
      // centerd on the MousePress
      p.x = e.getScreenPos().x - e.getWinView()->getScreenPos().x - m2->getWidth()/2;
      p.y = e.getScreenPos().y - e.getWinView()->getScreenPos(). y- m2->getHeight()/2.;
      break;
      //case COMPOUND:
    default:
      p.x = c.x + getWidth()/2. * ::cos(a) 
      + m2->getWidth()/2. * ::cos(a) - m2->getWidth()/2.;
      p.y = c.y - getHeight()/2. * ::sin(a) 
      - m2->getHeight()/2. * ::sin(a) - m2->getHeight()/2.;
      break;
  }
  
  Window* win = getContainingWin(null/*DISP!!*/);
  //m2->setPosOnScreen(Point(m1->getPosOnScreen().x + p.x,  not impl!!
  //                   m1->getPosOnScreen().y + p.y));
  
  // positionner et afficher le submenu
  if (win) m2->setPos(*win, p);
  m2->show();
  
  // remettre le parent par dessus  
  //if (submenu_layout == CONCENTRIC) this->toFront();
  toFront();
  
  forwardToMenu(m2);  // forward events
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PieMenu::disarmItem(InputEvent& e, bool is_browsing) {
  if (parmed) {
    parmed->disarmBehavior(e, is_browsing);     // NB: the source is the menu!!!
  }
  parmed = null;
  curitem = -2;
}

void PieMenu::disarmItemCB(InputEvent& e, PieMenu* m2) {
  if (!m2) return;
  m2->disarmItem(e, true);
  m2->show(false);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// att: e.getSource() incorrect si forwarded (toujours le menu principal)

void PieMenu::motionCB(MouseEvent& e) {
  //if ((mmodes & RADIANT) && e.getButtonState() == 0) return;
  
  // dont do anything if no mouse button is pressed
  if (e.getButtons() == 0 && e.getButton() == 0) return;
  
  Point p;
  bool must_forward;
  float d = getCentredCoords(e, p, must_forward);
  if (must_forward) {
    if (to_menu) to_menu->motionCB(e);
    return;
  }
  
  // this mode moves the menu when Control is pressed
  //if (e.isControlDown() && (menu_options & CTL_MOVE)) {
  //  setPos(e, -getWidth()/2, -getHeight()/2);
  //}
  
  if (d <= center_radius) {
    // on repasse par le centre => desarmer l'item (sauf pour ctlmenu ou on ne
    // peut plus changer les items une fois qu'on est entre dedans)
    if (parmed && (novice_mode && !ctlmenu_mode)) {
      disarmItem(e, true); 
      armpos.x = armpos.y = 0;   //  ???
      mousepos.x = mousepos.y = 0;
      repaint();  // to remove the slide paint
    }
    return;
  }
  
  // si c'st le 1er btn qu'on arme OU on est en mode novice (sauf pour ctlmenu 
  // ou on ne peut plus changer les items une fois qu'on est entre dedans)
  if (!parmed || (novice_mode && !ctlmenu_mode)) {
    short newitem = getItemFromPos(p);
    
    if (parmed) {   // already armed => disarm 
      if (parmed == getItem(newitem)) {
        curitem = newitem;
        goto DRAG;
      } 
      disarmItem(e, true);
    }
    
    ptimer->stop();
    parmed = null;
    curitem = newitem;
    armpos = p;
    mousepos.x = mousepos.y = 0;
    
    {  // arm new item
      Box* b = getItem(curitem);
      // !!! IL FAUT TROUVER UN MOYEN DE VIRER LES ITEMS NULLS !!!!
      if (b && b->cbegin() != b->cend()) parmed = b;
    }
    
    // hide items if we are in ctlmenu mode
    if (ctlmenu_mode) gitems.show(false);
    
    if (parmed) {  // if arm btn was found
      parmed->armBehavior(e, UBehavior::MOUSE);  // arm callback first
      parmed->update();      
      e.setFirstDrag(true);
      e.setCond(UOn::mdrag); 
      parmed->fire(e); 
      e.setFirstDrag(false);
      return;     // dont call drag callback twice
    }
  }
  
DRAG:    // send mdrag whatever the mmode
  if (parmed) {
    mousepos.x = p.x - armpos.x;
    mousepos.y = p.y - armpos.y;
    //e.setTarget(parmed);      // NB: the source is menu  !!!@@@@
    e.setCond(UOn::mdrag); 
    parmed->fire(e);
  }
}


void PieMenu::pressCB(MouseEvent& e) {
  Point p;
  bool must_forward;
  getCentredCoords(e, p, must_forward);
  
  if (must_forward) {
    if (to_menu) to_menu->pressCB(e);
    return;
  }
  
  motionCB(e);
}


void PieMenu::releaseCB(MouseEvent& e) {
  ptimer->stop();
  
  //if (!ctlmenu_mode) {
  Point p;
  bool must_forward;
  float d = getCentredCoords(e, p, must_forward);
  if (must_forward && to_menu) {
    to_menu->releaseCB(e);
    return;
  }
  //}
  
  if (!parmed) {  // controls what happens if release outside item 
    if (spring_mode && d <= center_radius) {
      // release takes place in the center => close this menu
      if (from_menu) {
        from_menu->disarmItem(e, false);
      }
      // il semble que disarmItem() puisse mettre from_menu a null
      if (from_menu) from_menu->repaint();
      show(false);
    }
  }
  else {
    // the menu system (= this menu and its parents) is closed if the parmed
    // item is a leaf (ie. if it does not contain a submenu)
    PieMenu* submenu = null;
    parmed->children().findClass(submenu);  // searches if parmed contains a Piemenu
    
    if (!submenu) {
      disarmItem(e, false);
      // chose this menu and its parents
      PieMenu* m = this;
      while (m) {
        // show(false) calls hideCB that calls unlinkFromAndToMenus
        // that sets from_menu to null
        PieMenu* from = m->from_menu; 
        m->show(false);
        m = from;
      }
    }
  }
}


void PieMenu::hideCB(Event& e) {
  //cerr << "hideCB " << this << endl;
  
  // ATT: keep last 'opener' value because the menu is hidden when the 
  // activaction distance is reached: 
  ptimer->stop();
  // deplace unlinkFromAndToMenus();
  
  // !! hideCB devrait recupere un UViewEvent !!!
  
  InputEvent ie(UOn::hide, getView(0), null, 0, 0);
  disarmItem(ie, true);  // BROWSE ?
  
  unlinkFromAndToMenus();  
  novice_mode = false;
}

/* obsolete: for the old WaveMenu subclass:
 
void PieMenu::closeMenus(MouseEvent& e, bool close_submenus) {
  ptimer->stop();
  
  Point p;
  bool must_forward;
  float d = getCentredCoords(e, p, must_forward);
  
  if (must_forward && to_menu) {
    to_menu->closeMenus(e, true);  // close_submenus
    return;
  }
  
  PieMenu* from = from_menu;
  unlinkFromAndToMenus();
  if (parmed) {
    // NB: the source is menu  !!!@@@@
    UBehavior bp(UBehavior::MOUSE);
    parmed->disarmBehavior(*e.getFlow(), e, bp);
    parmed = null;
  }
  curitem = -2;
  novice_mode = false;
  show(false);  // EXBUG: close(0);
  
  if (from && d > center_radius) from->closeMenus(e, false);  // close_supermenus
}
*/

}



