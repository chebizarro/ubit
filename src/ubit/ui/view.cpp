/*
 *  view.cpp
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
#include <ubit/ucond.hpp>
#include <ubit/ubox.hpp>
#include <ubit/uview.hpp>
#include <ubit/ui/uviewImpl.hpp>
#include <ubit/uwin.hpp>
#include <ubit/ui/uwinImpl.hpp>
#include <ubit/uappli.hpp>
#include <ubit/core/uappliImpl.hpp>
#include <ubit/ugraph.hpp>
#include <ubit/ufontmetrics.hpp>
#include <ubit/uon.hpp>
#include <ubit/core/event.h>
using namespace std;
namespace ubit {



UViewStyle::UViewStyle(View*(*make_view)(Box*, View*, UHardwinImpl*), UConst c) 
: Attribute(c), createView(make_view) {}

void UViewStyle::addingTo(Child& c, Element& parent) {
  Node::addingTo(c, parent);
  if (parent.emodes.HAS_LAYOUT) {
    Application::warning("UViewStyle::addingTo","This UViewStyle object (%p) has a parent (%s %p) that contains another UViewStyle object", this, &parent.getClassName(), &parent);
  }
  parent.emodes.HAS_LAYOUT = true;
}

void UViewStyle::removingFrom(Child& c, Element& parent) {
  parent.emodes.HAS_LAYOUT = false;
  Node::removingFrom(c, parent);
}

// ==================================================== [ELC] ==================

UViewStyle View::style(&View::createView, UViewStyle::UCONST);

View* View::createView(Box* box, View* parview, UHardwinImpl* w) {
  return new View(box, parview, w);
}

View::View(Box*_box, View*_parview, UHardwinImpl* w) :
vmodes(0),
scale(1.),
chwidth(0.), chheight(0.),
edit_shift(0.),
hflex_count(0), vflex_count(0), 
parview(_parview), box(_box), hardwin(w),
next(null) {
}

View::~View() {
  addVModes(DESTRUCTED);  // this view has been destructed
  for (UViewProps::iterator i = props.begin(); i != props.end(); ++i) delete (*i);  
  next = null;
  Application::deleteNotify(this); // notifies the Appli that this view has been destructed
}

void View::operator delete(void* p) {
  if (!p) return;
  if (Application::impl.isTerminated()) ::operator delete(p);
  else Application::impl.addDeleteRequest(static_cast<View*>(p));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool   View::isRealized() const {return hardwin && hardwin->isRealized();}
Display* View::getDisp()    const {return hardwin ? hardwin->disp : null;}
Window*  View::getWin()     const {return hardwin ? hardwin->win : null;}

View* View::getWinView() const {
  if (!hardwin || !hardwin->win) return null; 
  else return hardwin->win->views;      // OPTIMISATION: suppose une seule Hardwin par disp !!
}

void View::setParentView(View* pv) {
  parview = pv;
  hardwin = pv->hardwin;
}

Box* View::getBoxParent() const {
  return parview ? parview->getBox() : null;
}

bool View::isChildOf(const vector<View*>& parent_views) {
  for (unsigned int k = 0; k < parent_views.size(); k++) {
    if (parview == parent_views[k]) return true;
  }
  return false;
}

bool View::isShown() const {                 // devrait etre optimise !!!
  // ex: 4nov06: plantage dans destructeurs: getBox() peut alors etre nul
  // if (!getBox()->isShowable()) return false;
  Box* b = getBox();
  if (!b || !b->isShowable()) return false;
  if (!parview)              // cas hardwin, parent_view is null !!
    return b->isShown();
  else
    return parview->isShown();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/*
 //void setPos(const Point&);
 * changes the position of the view (see details).
 * Note that most box views are layed out automatically. Calling setPos()
 * on such views won't have the expected effect because their position is
 * controlled by the layout manager.
 
 //void setAutoPositioning(bool);
 // speficies update policy when the object is modified.
 
 Box* View::getBox() const {
 Box* b = null;
 if (!childnode || !(b = (**childnode)->toBox())) {
 //ne pas faire car peut arriver dans etape de destruction
 //Application::internalError("View::getBox","this View has no corresponding Box");
 return null;
 }
 else return b;
 }

void View::setPos(const Point& p) {
  if (hasVMode(FORCE_POS)) {
    UViewForcePosProp* sp = null;
    obtainProp(sp);
    sp->x = p.x; sp->y = p.y;
  }
  else { // AUTO_POS
    if (parview) {x = p.x + parview->x; y = p.y + parview->y;}
    else {x = p.x; y = p.y;}
  }
}

void View::setAutoPositioning(bool autopos) {
  if (autopos) removeVModes(FORCE_POS);
  else {
    addVModes(FORCE_POS);
    UViewForcePosProp* sp = null;
    obtainProp(sp);
  }
}
*/
/* EX:
Point View::getTopWinPos() const {
  return Point(x, y);
}

Point View::getWinPos() const {
  View* winview = getWinView();
  // must return (0,0) for a subwindow's view
  if (!winview || this == winview) return Point(0,0);
  else return Point(x, y);
}
*/
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Dimension View::getSize() const {return Dimension(width, height);}
void View::setSize(const Dimension& s) {width = s.width; height = s.height;}

float View::getX() const {if (parview) return x - parview->x; else return x;}
float View::getY() const {if (parview) return y - parview->y; else return y;}

Point View::getPos() const {
  if (parview) return Point(x - parview->x, y - parview->y);
  else return Point(x, y);
}

Point View::getHardwinPos() const {
  return Point(x, y);
}

Point View::getGLPos() const {
  View* winview = getWinView();
  // must return (0,0) for a subwindow's view
  if (!winview || this == winview) return Point(0,0);
  else return Point(x, winview->height - height - y);
}

Point View::getScreenPos() const {
  Window* w = getWin();
  if (!w) return Point(0,0);
  else {
    Point p = w->getScreenPos(getDisp());
    // dans le cas des hardwin et softwin il n'y a rien a ajouter
    // (et dans le cas des softwins on ajouterait 2 fois x et y)
    Box* b = getBox();
    if (!b || !b->toWin()) {
      p.x += x;
      p.y += y;
    }
    return p;
  }
}

Point View::getPosIn(View& ref_view) const {
  return convertPosTo(ref_view, *this, Point(0,0));
}

Point View::convertPosTo(const View& to, const View& from, const Point& from_pos) {
  // marche que si both view in same win
  if (to.hardwin == from.hardwin) {
    return Point(from_pos.x + from.x - to.x, from_pos.y + from.y - to.y);
  }

  // on devrait plutot comparer les Screens !!!
  if (to.getDisp() == from.getDisp()) {
    Point frompos = from.getScreenPos();
    Point topos = to.getScreenPos();
    return Point(from_pos.x + frompos.x - topos.x, from_pos.y + frompos.y - topos.y);
  }
  
  return Point(0,0);
}

bool View::caretPosToXY(long _pos, int& _x, int& _y) const {
  _y = 0;
  _x = _pos;
  return true;
}

bool View::xyToCaretPos(int _x, int _y, long& _pos) const {
  _pos = _x;
  return (_y == 0);
}

// =========================================================== [Elc] ===========

void View::updatePaintData(const Rectangle* winrect) {     // window coordinates
  View* winview = getWinView();
  if (!winview) return;
  if (!winrect) winrect = this;
  
  UWinUpdateContext winctx(winview, null);
  UViewUpdate vup(UViewUpdate::UPDATE_DATA);
  addVModes(View::DAMAGED);
  winview->doUpdate(winctx, *winview, *winrect, vup);
  //inutile: setVmodes(View::DAMAGED, false);
  //return vup.above_damaged_count;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void View::updatePaint(const Rectangle* winrect) {     // window coordinates
  View* winview = getWinView();
  if (!winview) return;
  if (!winrect) winrect = this;

  // !ATT: paint impossible sur winrect vide: peut poser probleme pour maj des donnees
  if (winrect->width == 0 || winrect->height == 0) return;
  
  //cerr << endl <<">>> View::paintImpl: PAINT win: " << winview->getWin() << endl; 
  
  if (Application::impl.isProcessingLayoutUpdateRequests()) {
    // UAppliImpl::processUpdateRequests() is processing the layout requests
    // so that nothing should be drawn at this stage (normally the next line
    // should never be executed except in exotic cases)
    updatePaintData(winrect);
  }
  else {
    Graph g(winview);
    UViewUpdate vup(UViewUpdate::PAINT_ALL);
    UWinUpdateContext winctx(winview, &g);
    
    // ctlmeneu laisse des bords for some reason
    //winview->doUpdate(winctx, *winview, *winrect, vup);    

    winview->doUpdate(winctx, *winview, *winview, vup);
  }
  
  //cerr << "<<< View::paintImpl: PAINT win: " << winview->getWin() << endl; 
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void View::updateLayout(const Dimension* size, bool upd_paint_data) {
  View* winview = getWinView();
  if (!winview) return;
  
  UViewLayout vl;
  UWinUpdateContext winctx1(winview, null);
  winctx1.xyscale = parview ? parview->scale : 1.;
  //cerr << this << " " << getClassName() << " " << parview->scale << endl; 
  
  bool mustLayoutAgain = this->doLayout(winctx1, vl);
  
AGAIN:
  // forcer valeurs de w et h, indep de ce que veut layout()
  if (size) this->setSize(*size);
  
  // dans ce cas (typiquement pour show() d'un floating) il faut recalculer
  // le layout a partir de layoutview (= par ex. le parent du floating)
  if (upd_paint_data) {
    updatePaintData();
    if (size) this->setSize(*size);
  }
  
  if (mustLayoutAgain) {
    updatePaintData();
    if (size) this->setSize(*size);
    
    UWinUpdateContext winctx2(winview, null);     // nb: not the same WinContext!
    winctx2.xyscale = parview ? parview->scale : 1.;
    this->doLayout(winctx2, vl);
    
    mustLayoutAgain = false;
    goto AGAIN;
  }
}

// ==================================================== [ELC] ==================

View* View::findInChildren(Element* grp, const Point& winpos,
                             const UpdateContext& ctx, UViewFind& vf) 
{
  if (!grp->isShowable() || grp->isIgnoringEvents()) return null;
  bool in_softwin_list = grp->getDisplayType() == Element::WINLIST;

  for (UChildReverseIter ch = grp->crbegin(); ch != grp->crend(); ++ch) {
    if (!ch.getCond() || ch.getCond()->verifies(ctx, *grp)) {
      
      Element* chgrp = (*ch)->toElem();
      View* chview = null;
        
      if (!chgrp || !chgrp->isShowable() || chgrp->isIgnoringEvents())
        continue;
      
      if (!chgrp->toBox()) {   // group but not box
        View* v = findInGroup(chgrp, winpos, ctx, vf);
        if (v) return v; 
      }
      
      else if (chgrp->getDisplayType() == Element::BLOCK   // elimine les Window
               && (chview = ((Box*)chgrp)->getViewInImpl(this /*,&ch.child()*/))) {
        // !!! faudrait tester chview->isShown() !!!
        View* v = chview->findInBox((Box*)chgrp, winpos, ctx, vf);
        if (v) return v; 
      }
      
      else if (in_softwin_list && chgrp->getDisplayType() == Element::SOFTWIN
               && (chview = ((Box*)chgrp)->getViewInImpl(this /*, null*/))) {//pas de ch
        // !!! faudrait tester chview->isShown() !!!
        View* v = chview->findInBox((Box*)chgrp, winpos, ctx, vf);
        if (v) return v; 
      }
    }
  }
  
  return null;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

View* View::findInGroup(Element* grp, const Point& winpos, 
                          const UpdateContext& par_ctx, UViewFind& vf)
{
  UpdateContext ctx(par_ctx, grp, this, null);
  UMultiList mlist(ctx, *grp);   // necessaire pour parser ctx
  return findInChildren(grp, winpos, ctx, vf);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// wpos = pos in hard window or in 3D window

bool View::containsWC(const Point& pos_in_win) {     // CF SHAPE & HIDE   !!!@@@!!!!
  return (pos_in_win.x >= x && pos_in_win.y >= y 
          && pos_in_win.x < x + width && pos_in_win.y < y + height);
}

View* View::findInBox(Box* box, const Point& wpos, 
                        const UpdateContext& par_ctx, UViewFind& vf) 
{
  if (!box->isShowable() || box->isIgnoringEvents() || !containsWC(wpos))
    return null;

  UpdateContext ctx(par_ctx, box, this, null);
  UMultiList mlist(ctx, *box);   // necessaire pour parser ctx
  vf.updateProps(this, box, ctx);  // !ATT evprops n'est plus recopie
  
  // si c'est une hardwin, chercher si l'event est dans ses softwins
  //BUG: if (box->getDisplayType() == Element::HARDWIN) {
  Window* w = box->toWin();
  if (w && w->isHardwin()) {   // all hardwins, including subwins
    UHardwinImpl* hw = w->hardImpl(/*ctx.getDisp()*/);
    UWinList* softwins = null;
    if (hw && (softwins = hw->getSoftwins())) {
      View* v = findInGroup(softwins, wpos, ctx, vf);
      if (v) return v;
    }
  }
  
  // chercher si l'element catches ce type d'event 
  if (box != vf.uncatchable && (box->emodes.CATCH_EVENTS & vf.CATCH_MASK)) {
    vf.catched = box;
    goto FOUND;
  }

  if (box->emodes.OBSERVE_EVENTS) {
    vf.bp.event_observer = box;
  }
  
  { // chercher si l'event est dans les borders
    Element* chgrp = null;
    UViewBorderProp* vb = null;
    if (getProp(vb) && ctx.local.border && (chgrp = ctx.local.border->getSubGroup())) {
      View* v = findInGroup(chgrp, wpos, ctx, vf);
      if (v) return v;
      else {
        PaddingSpec padding(0, 0);
        ctx.local.border->getSize(ctx, padding);
        padding.add(*vb);
        //clip.set(clip.x + padding.left, clip.y + padding.top,
        //         clip.width - padding.left - padding.right,
        //         clip.height - padding.top - padding.bottom);
      }
    }
  }
  
  {// sinon chercher si l'event est dans les enfants
    View* v = findInChildren(box, wpos, ctx, vf);
    if (v) return v;
  }
  
  // else: si l'objet n'a pas d'enfant ou si la souris n'est contenue dans aucun 
  // de ses enfants alors retourner l'objet lui-meme
FOUND:
  vf.found_ctx = par_ctx;     // !ATT: par_ctx PAS ctx !
  return this;
} 

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

View* View::findSource(UViewFind& vf, Point& source_pos) {
  // vf.ref_pos = pos in refview
  View* source_view = findInBox(getBox(), vf.ref_pos, vf.win_ctx, vf);
  if (source_view) {
    // refview is either the window view or the 3Dwidget view
    // pos in source = pos in refview - pos of sourceview in refview
    source_pos.x = vf.ref_pos.x - source_view->x;
    source_pos.y = vf.ref_pos.y - source_view->y;
  }
  return source_view;
}

View* View::findSourceNext(UViewFind& vf, Point& source_pos) {
  Box* box = getBox();
  vf.catched = null;
  vf.uncatchable = box;  // this box cannot be catched by findInBox()
  
  View* source_view = findInBox(box, vf.ref_pos, vf.found_ctx, vf);
  if (source_view) {
    source_pos.x = vf.ref_pos.x - source_view->x;
    source_pos.y = vf.ref_pos.y - source_view->y;
  }
  return source_view;
}
  
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
UViewFind::UViewFind(View* win_view, const Point win_pos, 
                     UBehavior::InputType intype, unsigned char catch_mask) :
  ref_view(win_view),   // window or 3Dwidget view (may be changed later)
  ref_pos(win_pos),     // pos in refview
  canvas_view(null),
  refpos_in_canvas(null),
  win_ctx(win_view, null),
  CATCH_MASK(catch_mask),
  catched(null),
  uncatchable(null),
  bp(intype) {
}

void UViewFind::updateProps(View* v, Element* grp, const UpdateContext& ctx) {  
  // cursor herite mais ecrase si defini au niveau local
  if (ctx.cursor) bp.cursor = ctx.cursor;
  
  if (grp->toMenu()) bp.SOURCE_IN_MENU = true;
  else if (grp->toWin()) bp.SOURCE_IN_MENU = false;
  // EX: louche, SUBWIN oublies
  //else if (grp->getDisplayType()==Element::HARDWIN || grp->getDisplayType()==Element::SOFTWIN) bp.SOURCE_IN_MENU = false;
    
  if (grp->isMenuClosingDisabled()) bp.DONT_CLOSE_MENU = true;
  
  // browsing_group herité mais dominant : ecrase le niveau local
  if (!bp.browsing_group && grp->isBrowsingGroup())  bp.browsing_group = grp;
}  

// ==================================================== [ELC] ==================

UViewContext::UViewContext() {
  upd_context = null;
}

UViewContext::~UViewContext() {
  delete upd_context;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
namespace impl {

struct UViewLink {
  View* view;
  UViewLink* prev;
  UViewLink(View* v, UViewLink* l) : view(v), prev(l) {}
};


static bool setCtx(UViewLink* l, const UpdateContext& parctx, Rectangle clip, UViewContext& vc) 
{
  if (!clip.doIntersection(*l->view)) return false;

  Box* box = l->view->getBox();
  if (!box) {Application::internalError("uview::setCtx", "Null box!"); return false;}
  
  UpdateContext ctx(parctx, box, l->view, null);
  UMultiList mlist(ctx, *box);   // necessaire pour parser ctx
  
  //if (!box.isEnabled()) vc.IS_SOURCE_ENABLED = false;
  if (box->isFloating()
      || (l->view->hasVMode(View::FIXED_WIDTH) && l->view->hasVMode(View::FIXED_HEIGHT)))
    vc.layout_view = l->view;
  
  Element* chgrp = null;
  UViewBorderProp* vb = null;

  if (l->view->getProp(vb) && ctx.local.border 
      && (chgrp = ctx.local.border->getSubGroup())) 
  {
    //View* v = findInGroup(chgrp, ctx, clip, e, searchedView, ep);
    //if (v) return v;
    //else {
      PaddingSpec padding(0, 0);
      ctx.local.border->getSize(ctx, padding);
      padding.add(*vb);
    
    // !!!!!!  A COMPLETER !!! prendre en compte les Units du padding !!!!!!!
    
      clip.setRect(clip.x + padding.left.val, clip.y + padding.top.val,
                   clip.width - padding.left.val - padding.right.val,
                   clip.height - padding.top.val - padding.bottom.val);
    //}
  }
  
  if (l->prev) return setCtx(l->prev, ctx, clip, vc);
  else {
    vc.is_clip_set = true;
    vc.clip = clip;
    if (vc.find_mode == View::FIND_VIEW_CONTEXT) 
      vc.upd_context = new UpdateContext(ctx);
    else if (vc.find_mode == View::FIND_PARENT_CONTEXT) 
      vc.upd_context = new UpdateContext(parctx);
    return true;
  }
}


static bool findView(UViewLink* l, UViewContext& vc) {
  View* parv = l->view->getParentView();
  if (parv) {
    UViewLink parl(parv, l);
    return findView(&parl, vc);
  }
  else {
    UWinUpdateContext winctx(l->view, null);
    vc.clip = *(l->view);
    return setCtx(l, winctx, vc.clip, vc);
  }
}

}  //endof namespace impl
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool View::findContext(UViewContext& vc, FindMode fmode) {
  delete vc.upd_context; vc.upd_context = null; // faudrait un uptr<> !!!
  vc.find_mode = fmode;
  vc.is_clip_set = false;
  vc.layout_view = null;
  
  impl::UViewLink l(this, null);
  return impl::findView(&l, vc);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Data* View::findData(UDataContext& dc, const Point& pos,
                       const Data* searched_data, int strpos1, int strpos2) {
  UViewContext vc;
  bool stat = findContext(vc, FIND_PARENT_CONTEXT);
  
  if (!stat || !vc.upd_context) return null;

  UViewUpdate vup(searched_data? UViewUpdate::FIND_DATA_PTR: UViewUpdate::FIND_DATA_POS);  
  vup.datactx = &dc;
  dc.it = dc.it2 = ChildIter();
  dc.data = (Data*)searched_data;
  dc.strpos = strpos1;
  dc.strpos2 = strpos2;
  dc.exact_match = false;  // !! 
  dc.source_view = this;
  dc.win_eventpos.set(pos.x + this->x, pos.y + this->y);
  //delete dp.dataContext; dp.dataContext = null;
  
  Rectangle clip = *this;
  doUpdate(*vc.upd_context, clip, clip, vup);
  
  if (dc.it != dc.it2) return dc.data; else return null;
}

// ==================================================== [ELC] ==================

static void saveDataProps(UpdateContext& ctx, ChildIter it, ChildIter end,
                          const Rectangle& r, UViewUpdate& vup, bool exact_match) {
  int strpos = -1;
  Data* data = (*it)->toData();
  String* str;
  if (data && (str = data->toStr())) {   // search the strpos
    strpos = UFontMetrics(ctx).
    getCharPos(str->c_str(), str->length(), vup.datactx->win_eventpos.x - r.x);
  }
  vup.datactx->set(ctx, data, it, end, r, strpos, exact_match);
}

// Horizontal Search. 
// retrieves data and info from x,y position
// !!ATT: 
// - suppose que data_props!= null !
// - return==true signifie: ne pas chercher davantage car:
//                trouve' OU pas trouve' mais plus la peine de chercher
// - return==false signifie: continuer a chercher car:
//                pas trouve' mais on peut encore trouver 

bool View::findDataH(UpdateContext& ctx, ChildIter data_iter, ChildIter end_iter, 
                      const Rectangle& r, UViewUpdate& vup) {
  if (!vup.datactx) {
    Application::internalError("View::findDataH","null event or wrong type");
    return false;
  }
  const Point& evpos = vup.datactx->win_eventpos;
  
  if (r.x > evpos.x) return true;  // plus rien a chercher (not found)

  if (r.y <= evpos.y && evpos.y < r.y + r.height) {
    if (evpos.x < r.x + r.width) {
      // data exactly found
      saveDataProps(ctx, data_iter, end_iter, r, vup, true);
      return true;  // trouve' => finir la recherche
    }
    // data approximatively found
    else saveDataProps(ctx, data_iter, end_iter, r, vup, false);
  }

  return false;	// continuer a chercher (dans les 2cas)
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool View::findDataV(UpdateContext& ctx, ChildIter data_iter, ChildIter end_iter, 
                      const Rectangle& r, UViewUpdate& vup) {
  if (!vup.datactx) {
    Application::internalError("View::findDataV","null event or wrong type");
    return false;
  }
  const Point& evpos = vup.datactx->win_eventpos;

  if (r.y > evpos.y) return true;  // plus rien a chercher (not found)

  if (r.x <= evpos.x && evpos.x < r.x + r.width) {
    if (evpos.y < r.y + r.height) {
      saveDataProps(ctx, data_iter, end_iter, r, vup, true);
      return true;
    }
    else saveDataProps(ctx, data_iter, end_iter, r, vup, false);
    
  }
  return false;	// continuer a chercher (dans les 2cas)
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool View::findDataPtr(UpdateContext& ctx, ChildIter data_iter, ChildIter end_iter, 
                        const Rectangle& r, UViewUpdate& vup) {
  if (!vup.datactx) {
    Application::internalError("View::findDataPtr","null event or wrong type");
    return false;
  }
  
  Data* data = null;
  // retrieves info from Data link or Data ptr
  if ((vup.datactx->it == data_iter || vup.datactx->data==(*data_iter))
      && ((data = (*data_iter)->toData()))) {
    vup.datactx->set(ctx, data, data_iter, end_iter, r, vup.datactx->strpos, true);
    return true;    //trouve, inutile de continuer a chercher
  }
  
  return false;   //tous les cas pas trouve (PAS de else!)
}

// ==================================================== [ELC] ==================
// parses the AttrList and the ElemList, put properties and add
// elements (and groups) as a sublist
// also takes into account the softwin list when applicable

UMultiList::UMultiList(UpdateContext& curp, Element& grp) 
: card(0), current(0) {
  //for (int k = 0; k < MAXCOUNT; k++) clists[k] = null;
  sublists[0].begin = sublists[0].end = ChildIter(); //securite
  
  // put ALL the properties of the AttrList
  // does not take into account other elements
  Attribute* prop = null;
  for (ChildIter c = grp.abegin(); c != grp.aend(); ++c) {
    if ((prop = (*c)->toAttr())) {
      // NB: null cond means always
      if (!c.getCond() || c.getCond()->verifies(curp, grp))  
        prop->putProp(&curp, grp);
    }
  }
  
  for (ChildIter c = grp.cbegin(); c != grp.cend(); ++c) {
    // put the properties of the childlist that are BEFORE the first elem
    if ((prop = (*c)->toAttr())) {
      // NB: null cond means always
      if (!c.getCond() || c.getCond()->verifies(curp, grp))  
        prop->putProp(&curp, grp);
    }
    // adds the first elem (or group) and its followers as a sublist
    else if ((*c)->toData() || (*c)->toElem()) {
      addChildList(c, grp.cend());
      break;  		// passer a la suite
    }
  }
  
  //BUG: if (grp.getDisplayType() == Element::HARDWIN) {
  Window* w = grp.toWin();
  if (w && w->isHardwin()) {   // all hardwins, including subwins
    UHardwinImpl* hw = w->hardImpl(/*curp.getDisp()*/);
    Children* softwin_list = null;    
    if (hw && (softwin_list = hw->getSoftwinList())) {
      addChildList(softwin_list->begin(), softwin_list->end());
    }
    //cerr << "****** softwin " <<w <<" " << hw << " " <<  softwin_list << endl;
  }
  
  in_softwin_list = grp.getDisplayType() == Element::WINLIST;
}

void UMultiList::next(ChildIter& c) {
  ++c;
  if (c == sublists[current].end) {
    if (current+1 < card) c = sublists[++current].begin;
  }
}

void UMultiList::addChildList(ChildIter begin, ChildIter end) {
  if (begin == end) return;  // nop (nb: test important: sauter listes nulles)
  else if (card < MAXCOUNT) {
    sublists[card].begin = begin;
    sublists[card].end = end;
    card++;
  }
  else Application::internalError("MultiList::addChildList","Too many elements: %d", card);
}

}
