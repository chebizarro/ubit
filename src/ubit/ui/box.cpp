/*
 *  box.cpp: Box Elements (elements that manage 1 or N screen View(s) 
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
#include <ubit/core/boxgeom.h>
#include <ubit/ui/updatecontext.h>
#include <ubit/ui/uviewImpl.hpp>
#include <ubit/draw/style.h>
#include <ubit/core/appimpl.h>
using namespace std;
namespace ubit {


Box& uvbox(Args args) {
  //return *new UVbox(args);
  Box* box = new Box(args);
  box->setAttr(Orientation::vertical);
  return *box;
}

Box& uhbox(Args args) {
  //return *new UHbox(args);
  Box* box = new Box(args);
  box->setAttr(Orientation::horizontal);
  return *box;
}

Box::Box(Args a) : views(null) {
  // faire add() APRES sinon la box est consideree comme un new Element
  // (car on sera dans le constructeur de new Element) ce qui ferait merder
  // l'addition de children qui necessitent une Box comme parent
  add(a);
}

Style* Box::createStyle() {
  Style* style = new Style();
  style->textSeparator = new String(" ");
  style->orient = Orientation::INHERIT;
  // abndonne: marche pas: a la fois hflex et vflex
  style->halign = HAlign::FLEX;
  style->valign = VAlign::FLEX;
  style->hspacing = 1;
  style->vspacing = 1;
  style->local.padding.set(0,0);
  return style;
}

/*
 virtual Box* getChildAt(const Point&, View* box_view = null) const;
 * return the direct child at this location in this box view.
 * a box can control several views. Returns the child at this position
 * in the first view if 'box_view' = null
 
Box* Box::getChildAt(const Point& p, View* view) const {
  if (!view) view = getView(0);
  if (!view) return null;
  return getChildAtImpl(Point(p.x + view->x, p.y + view->y), view);
}

virtual Box* getChildAtImpl(const Point&, View* box_view) const;

 Box* Box::getChildAtImpl(const Point& p, View* view) const {
  //if (!view) return null;
  for (ChildReverseIter ch = crbegin(); ch != crend(); ++ch) {
    // pas possible: context inconnu
    // if (!ch.getCond() || ch.getCond()->verifies(curp, grp))     
    Box* chbox = (*ch)->toBox();
    
    // elimine les Window
    if (chbox && (chbox->getDisplayType() == BLOCK) && chbox->isShowable()) {
      for (View* v = chbox->views; v != null; v = v->getNext()) {
        // !!! faudrait tester v->isShown()                                  !!!
        if (//v->getBoxHolder() == chnode &&     // 9nov08: obsolete
            v->getParentView() == view
            && p.x >= v->x && p.x < v->x + v->width
            && p.y >= v->y && p.y < v->y + v->height)
          return v->getBox();
      }
    }
  }
  return null;
}

virtual Dimension getViewSize(View* box_view = null) const; 
 * returns the size of this box view.
 * a box can control several views. Returns the size of the first view if 
 * 'box_view' = null
 *
virtual Point getViewPos(View* box_view = null) const;  
 * returns the position of the upper left corner of this box view.
 * a box can control several views. Returns the upper left corner of the
 *  first view if 'box_view' = null
 *
Point Box::getViewPos(View* view) const {
  if (!view) view = getView(0);
  if (view) return view->getPos();
  else return Point(0,0);
}

Dimension Box::getViewSize(View* view) const {
  if (!view) view = getView(0);
  if (view) return view->getSize(); 
  else return Dimension(0,0);
}
 */

// IL FAUDRAIT ORDONNER LES VIEWS!
void Box::addViewImpl(View* v) {
  v->setNext(views);
  views = v;
}

int Box::getViewCount() const {
  int n = 0;
  for (View* v = views; v != null; v = v->getNext()) n++;
  return n;
}

int Box::getViews(std::vector<View*>& tab) const {
  // NB: fait tab.clear() contrairement a retrieveViews()
  tab.clear();
  int count = 0;
  for (View* v = views; v != null; v = v->getNext(), count++) {
    tab.push_back(v);
  }
  return count;
}

View* Box::getView(int pos) const {
  if (pos == 0) return views;
  else {
    int n = 0;
    View* last_view = null;
    for (View* v = views; v != null; v = v->next) {
      if (n == pos) return v;
      last_view = v;
      n++;    // no de la vue (pas du parent!)
    }
    
    if (pos == -1) return last_view;
    else return null;	// not found
  }
}

View* Box::getView(const InputEvent& e) const {
  View* eview = e.getView();
  if (!eview) return null;
  
  // this first test won't work if the event source is on a superimposed component
  // such as a menu displayed over this box
  View* found_view = getViewContaining(*eview);
  if (found_view) return found_view;

  MouseEvent* me = ((InputEvent&)e).toMouseEvent();
  View* winview = e.getWinView();
  if (!me || !winview) return null;
    
  Point pos_in_win = View::convertPosTo(*winview, *eview, me->getPos());

  for (View* v = views; v != null; v = v->next) {
    if (v->containsWC(pos_in_win)) return v;
  }
  
  return null;
}

View* Box::getView(Window& win, const Point& pos_in_win, Display* d) const {
  View* winview = win.getWinView(d);
  if (!winview) return null;
  
  for (View* v = views; v != null; v = v->next) {
    // CF SHAPE!!!@@@ and IS_SHOWN !!!@@@
    if (v->getWinView() == winview && v->containsWC(pos_in_win)) return v;
  }
  return null;
}

/*
View* Box::getSubView(View& view, const Point& pos_in_view) const {
  View* winview = view.getWinView();
  if (!winview) return null;
  Point pos_in_win = View::convertPosTo(*winview, view, pos_in_view);

  for (View* v = views; v != null; v = v->next) {
    ViewFind vf(winview, pos_in_win, UBehavior::MOUSE, 0);
    Point source_pos;
    View* found_view = v->findSource(vf, source_pos);
    if (found_view) return found_view;
  }
  return null;
}

View* Box::getSubView(Window& win, const Point& win_pos, Display* d) const {
  View* winview = win.getWinView(d);
  return winview ? getViewAt(*winview, win_pos) : null;
}

View* Box::getSubView(const Point& screen_pos, Display* d = null) const {
  View* winview = Application::getWinView(screen_pos, d);
  if (!winview) return null;
  return getViewAt(winview, conversion);
}
*/

View* Box::getViewContaining(const View& _view) const {
  for (View* v = const_cast<View*>(&_view); v!=null; v = v->getParentView()) {
    if (v->getBox() == this) return v;
  }
  return null;  // not found
}

View* Box::getFirstViewInside(const View& parent_view) const {
  for (View* v = views; v != null; v = v->next) {
    View* pv = v->getParentView();
    if (!pv) ;
    else if (pv == &parent_view) return v;
    else if (pv->getBox()->getFirstViewInside(parent_view)) return v;
  }
  return null;	// not found
}

View* Box::getViewInImpl(View* parview /*,Child* cnode*/) {  
  for (View* v = views; v != null; v = v->next) {
    if (v->getParentView() == parview) return v;
  }
  return null;	// not found
}


View* Box::getViewInImpl(View* parview, /*Child* cnode,*/ Dimension& dim) {
  View* v = getViewInImpl(parview /*, cnode*/);
  if (v) {dim.width = v->width; dim.height = v->height;} 
  else dim.width = dim.height = 0;
  return v;
}

/* debug
 void foo(View* v) {
 for (; v ; v = v->getParentView()) {
 cerr << "--pview: " << v <<" box " << v->getBox() 
 << " "  << v->getBox() ->getTypeName() << endl;
 }
 }
 */

void Box::setViewModes(int vmodes, bool state) {
  for (View* v =getView(0); v != null; v = v->getNext()) {
    if (state) v->addVModes(vmodes); else v->removeVModes(vmodes);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// att: parview n'est pas la view du parent mais du 1er box!

void Box::initView(View* parview) {
  if (!parview) {
    error("Box::initView","[internal error] object has no parent view");
    return;
  }
  
  const ViewStyle* render = null;
  if (emodes.HAS_LAYOUT) {
    // si renderer defini dynamiquement dans la childlist
    attributes().findClass(render);
    if (!render) children().findClass(render);
  }
  else {  // default: rendre le renderer defini par le style de la Win
    render = getStyle(null).viewStyle;
  }

  View *view = null;
  if (render)
    view = (render->createView)(this, parview, parview->getHardwin());
  else {
    error("Box::initView","the Style of this object could not be retreived");
    view = new View(this, parview, parview->getHardwin());
  }
  
  if ((parview && parview->hasVMode(View::NO_DOUBLE_BUFFER)))
    view->addVModes(View::NO_DOUBLE_BUFFER);

  addViewImpl(view);
  
  // propager aux children
  Element::initView(view);  // view, pas parview !
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//void Box::initChildViews(Element* child, Child *childlink) {
void Box::initChildViews(Element* child) {
  // retrouver toutes les vues pour tous les parents de box
  for (View* v = views; v != null; v = v->next) {
    if (v && v->isRealized())   // initialiser b et ses descendants
      // ? QUESTION: init pour tous ou que Element/ubox ?
      //child->initView(childlink, v);
      child->initView(v);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// does the same as getViews() except that tab is not emptied

int Box::retrieveRelatedViews(std::vector<View*>& tab) const {
  int count = 0;
  for (View* v = views; v != null; v = v->next, count++) {
    tab.push_back(v);
  }
  return count;
}

// parent_views may belong to several widgets, which are not necessarily
// the direct parents of this box

//void Box::deleteRelatedViews(View* parview, Child* cnode) {
void Box::deleteViewsInside(const vector<View*>& parent_views) {
  vector<View*> tab;  
  for (View* v = views; v != null; v = v->next) {
    if (v->isChildOf(parent_views)) tab.push_back(v);
  }
  
  Element::deleteViewsInside(tab); 
  
  View *nextv = null, *prevv = null;
  for (View* v = views; v!=null; v = nextv) {
    nextv = v->next;
    if (v->isChildOf(parent_views)) {
      if (prevv) prevv->next = nextv; else views = nextv;
      delete v;  // NB: delete est redefini
      v = null;
    }
    if (v) prevv = v;
  }
}

// NOTE: a la difference des Box, les Views des Window sont partagees sauf
// dans le cas ou elles appartiennent a des Display diffrents
// La regle de destruction est donc differente: on ne detruit les vues
// que si cette Window est en cours de destruction (UMode::DESTRUCTING)
// ou si c'est une softwin qui depend d'une hardwin en cours de destr.

//void Window::deleteRelatedViews(View* parview, Child* cnode) {
void Window::deleteViewsInside(const std::vector<View*>& parent_views) {
  vector<View*> tab;
  Window* hw = null;

  for (View* v = views; v != null; v = v->next) {
    if (isHardwin()? omodes.IS_DESTRUCTING: ((hw=v->getWin()) && hw->omodes.IS_DESTRUCTING))
      tab.push_back(v);
  }
  
  Element::deleteViewsInside(tab); 
  
  View *nextv = null, *prevv = null;
  for (View* v = views; v!=null; v = nextv) {
    nextv = v->next;
    if (isHardwin()? omodes.IS_DESTRUCTING: ((hw=v->getWin()) && hw->omodes.IS_DESTRUCTING)){
      if (prevv) prevv->next = nextv; else views = nextv;
      delete v;  // NB: delete est redefini
      v = null;
    }
    if (v) prevv = v;
  }
}

// performs the actual update when the main loop is idle

void Box::update(const Update& upd, Display*) {
  if (Application::isExiting()) return;
  
  // do not update now, add to the update list
  Application::impl.addUpdateRequest(this, upd);

  /*
  if (Application::impl.isProcessingUpdateRequests()) {
    // do not update now, add to the update list
    Application::impl.addUpdateRequest(this, upd);
  }
  else if ((upd.modes & Update::NO_DELAY) || Application::conf.usync) {
    // update objets already in the list, then proceed
    Application::impl.processUpdateRequests();
    doUpdate(upd, null);
  }
  else {
    // do not update now, add to the update list
    Application::impl.addUpdateRequest(this, upd);
  }
 */
}

// performs the actual update now

void Box::doUpdate(const Update& upd, Display*) {
  if (Application::isExiting()) return;

  // update objets already in the list, then proceed
  if (!Application::impl.isProcessingUpdateRequests()) 
    Application::impl.processUpdateRequests();

  // REMARQUE IMPORTANTE: les hide/show ne sont pas traites de la meme facon
  // si l'objet est floating ou non:
  // - dans le 1er cas, pas besoin de recalculer le layout, un repaint suffit
  // - dans le 2nd cas, il faut recalculer le Layout - et actuellement -
  //   c'est meme celui DE LA FENETRE qu'on recalcule ce qui est A REVOir

  Window* updated_win = null;
  for (View* view = views; view != null; view = view->getNext()) {
    Window*  hardwin = null;
    View* hardwin_view = null;
    
    if (view && view->isRealized()   // check views!=null (some may have been deleted)
        && (hardwin = view->getWin())
        && (hardwin_view = hardwin->getWinView(view->getDisp()))
        // ne rien faire si la window n'est pas visible (sauf si always)
        // ce qui implique de ne pas appliquer directement cette version
        // de update() sur une window                           !!POSE PBM AVEC SOFTWINS!!
        && (hardwin->isShowable() || upd.isHiddenObjectsMode())
        ) {
      
      PaintEvent e(UOn::paint, hardwin_view, null/*flow*/);
    
      // MOVE
      
      if (upd.modes & Update::MOVE) {
        // on veut reafficher la zone minimale du parent de view
        // (= la reunion de l'ancienne et de la nouvelle position de view)
        e.setSourceAndProps(view);
        view->updatePaintData(&e.redraw_clip);
      }
      
      // PAINT_ONLY (NO LAYOUT)
      
      else if ((upd.modes & Update::PAINT) && !(upd.modes & Update::LAYOUT)) {
        if (!isShowable() && !upd.isHiddenObjectsMode()) break;
        e.setSourceAndProps(view);
        
        // si e.getView() == null cette view n'est pas visible
        // a l'ecran actuellement car cachee par un ScrollPane, etc.
        if (!e.getView()) return;

        // si data est specifie, essayer de ne reafficher QUE la region
        // contenant elem (au lieu de la box entiere)
        // NB: data n'est pas necessairement situe sous la souris 
        // (ie. ce n'est pas toujours e->getData())

        if (!(upd.modes & Update::STR_DATA)) {
          // NB: updateViewImpl utilise e.redrawClip et redrawStatus pour ne
          // reafficher que le clip concerne (ou juste upd.region si definie)
          if (!isShowable()) view->updatePaintData(&e.redraw_clip);
          else view->updatePaint(&e.redraw_clip);
        }
        else {
          UDataContext dc;
          const Update::StrData& sd = upd.props.sd;

          if (sd.data && e.getView()) {
            // !ATT: ca marche pas vraiment car e devrait etre un MouseEvent
            // avec (xmouse,ymouse) correctement intialises. Mais c'est pas
            // completement inutile dans le cas des textarea avec des plusieurs
            // lignes: on ne reaffichera que cette ligne (mais en entier)
            Data* d = e.getView()->findData(dc, Point(1,1), sd.data, sd.pos1, sd.pos2);
            
            // NB: on modifie sd->region alors que upd est cense etre const
            // ceci dit dans ce cas c'est pas un pbm car il est jamais const
            if (d && d == sd.data) sd.region = &dc.win_clip;
          }
          if (!isShowable()) view->updatePaintData(sd.region);
          else if (upd.props.sd.region) view->updatePaint(sd.region);
        }
      }

      // SHOW/HIDE
      
      else if (upd.modes & (Update::SHOW | Update::HIDE)) {
        if (upd.modes & Update::SHOW) emodes.IS_SHOWABLE = true;
        else emodes.IS_SHOWABLE = false;
        
        // Cas Floating
        if (isFloating() || getDisplayType() == SOFTWIN) {
          View* layout_view = view->getParentView();
          if (!layout_view) layout_view = hardwin_view;
          // keep the same size (un show/hide ne doit pas retailler le parent!)
          Dimension size = layout_view->getSize();
          layout_view->updateLayout(&size);
          hardwin_view->updatePaint(view);
        }
        else {
          // devrait etre comme LAYOUT mais le pbm c'est que e.firstLayoutView
          // est indetermine
          if (hardwin != updated_win) {
            hardwin->doUpdate(Update::layoutAndPaint, null);
            updated_win = hardwin;
          }
        }
      }

      // LAYOUT_ONLY or LAYOUT_AND_PAINT (et SCROLL mais passe jamais par la)

      else {
        // ici il y a un pbm si la region est ponctuelle sans epaisseur
        // (par ex une ubox vide) car locateSource renverra null
        // (du fait d'une intersection vide) => on met a 1,1
        if (view->width == 0)  view->width = 1;
        if (view->height == 0) view->height = 1;

        e.setSourceAndProps(view);
        
        // fromview = from which view we must perform the lay out
        // (de telle sorte que les parents se reajustent correctement
        // quand c'est necessaire)
        // typiquement fromview est window vie ou bien la vue d'un
        // floating ou d'une box qui a une width ET height imposee
        View* layout_view = e.layout_view;
        if (!layout_view) layout_view = hardwin_view;

        // showview : la vue a partir de laquelle on paint
        // generalement : meme view que layoutview (cf ci-apres)
        View* showview = null;
        Dimension size(0,0);
        // est-ce qu'on impose la meme taille? normalement oui
        bool has_size = true;

        // cas ou la box a une width OU height imposee mais elle vient
        // de changer => il faut donc la recalculer (getSize() renverrait
        // l'ancienne taille). De plus, comme cette taille a peut etre diminue
        // il faut reafficher le parent
        if (layout_view->hasVMode(View::SIZE_HAS_CHANGED)) {
          size.width = size.height = -1; 
          has_size = false;
          if (layout_view != hardwin_view) showview = layout_view->getParentView();
        }
        else {
          // !!! cet algo est completement FAUX: on ne peut pas savoir
          // a priori a partir d'ou il faut refaire le layout ni le paint
          // - AVANT on prenait tjrs showview = fromview; ce qui a pour effet
          //   de reafficher trop de choses dans la plupart des cas (eg.
          //   les scrollbars qui reaffichent le viewport!) 
          // - MAINTENANT: on a rajoute la "rustine" Update::ADD_REMOVE
          //   pour ne prendre showview = fromview que dans ce cas
          // mais de toute facon, il faudrait s'y prendre autrement, avec
          // des damaged et de la factorisation
          
          size = layout_view->getSize();     // keep the same size
          if (upd.modes & (Update::ADD_REMOVE)) showview = layout_view;
          else showview = view;
        }

        if (isShowable() && e.getView()) {
          if ((upd.modes & Update::LAYOUT) && !(upd.modes & Update::PAINT))
            // que layout, pas paint => showiew = null
            layout_view->updateLayout((has_size? &size :null)); 
          else {
            layout_view->updateLayout((has_size? &size :null), layout_view!=showview);
            hardwin_view->updatePaint(showview);
          }
        }
        
        else if (upd.isHiddenObjectsMode()) {
          // toujours faire layout (mais paint inutile) si always
          bool can_show = emodes.IS_SHOWABLE;
          emodes.IS_SHOWABLE = true;

          // hiddenObjects() est typiquement appele qunad l'objet n'a jamais
          // ete affiche => taille pas encore calculee => size = null
          layout_view->updateLayout(null/*size*/);
          emodes.IS_SHOWABLE = can_show;
        }
      }
    }
  }
}


}
