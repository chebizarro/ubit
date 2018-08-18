/*
 *  table.cpp
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
#include <cmath>
#include <ubit/uon.hpp>
#include <ubit/uboxgeom.hpp>
#include <ubit/ui/uviewImpl.hpp>
#include <ubit/ui/updatecontext.h>
#include <ubit/core/application.h>
#include <ubit/ui/table.h>
using namespace std;
namespace ubit {



Style* Table::createStyle() {
  Style* style = new Style();
  style->viewStyle = &UTableView::style;
  style->textSeparator = &ustr("\n");
  style->orient   = Orientation::HORIZONTAL;
  style->halign   = Halign::LEFT;
  style->valign   = Valign::TOP;
  style->hspacing = 0; //EX 1;
  style->vspacing = 1;
  style->local.padding.set(1, 1);
  return style;
}


Style* UTrow::createStyle() {
  Style* style = new Style();
  style->textSeparator  = &ustr("\t");
  style->orient = Orientation::HORIZONTAL;

  // il faut left car hflex a un resultat imprevu (bug?)
  style->halign = Halign::LEFT;

  // il faut vflex pour etre conforme a HTML (une utcell
  // devra occuper toute la hauteur d'un utrow (nb: bug corrige
  // dans uview.cc qui permet d'utilser cette option)
  style->valign = Valign::FLEX;
  
  style->hspacing = 1;
  style->vspacing = 0;  //EX 1
  style->local.padding.set(0, 0);
  return style;
}


Style* UTcell::createStyle() {
  Style* style = new Style();
  style->viewStyle = &FlowView::style;
  style->orient = Orientation::HORIZONTAL;
  style->halign = Halign::FLEX;
  style->valign = Valign::FLEX;
  style->hspacing = 0;
  style->vspacing = 0;
  style->local.padding.set(0,0);
  return style;
}


UViewCell::UViewCell() {
  d = min_d = max_d = spec_d =0;
  percent = -1;  // negative means: not a percent
  rowspan = colspan = 0;
}

static void augmentCells(vector<UViewCell>& tab, unsigned int new_size) {
  if (new_size> tab.size()) tab.resize(new_size);
}


UTcell::UTcell(Args a): UFlowbox(a) {
  colspan = 1;
  rowspan = 1;
}

UTcell::UTcell(short _colspan, Args a) : UFlowbox(a) {
  colspan = _colspan;
  rowspan = 1;
}

UTcell::UTcell(short _colspan, short _rowspan, Args a) : UFlowbox(a) {
  colspan = _colspan;
  rowspan = _rowspan;
}

void UTcell::setColspan(short _colspan) {
  colspan = _colspan;
  update();
}

void UTcell::setRowspan(short _rowspan) {
  rowspan = _rowspan;
  update();
}


ViewStyle UTableView::style(&UTableView::createView, UCONST);

View* UTableView::createView(Box* box, View* parview, UHardwinImpl* w) {
  return new UTableView(box, parview, w);
}

UTableView::~UTableView() {
}

UTableView::UTableView(Box* box, View* parview, UHardwinImpl* w) 
: View(box, parview, w) {
  lcur = ccur = ccount = lcount = 0;
}

class UTableLayoutImpl : public UViewLayoutImpl {
public:
  UTableView& t;
  UTableLayoutImpl(UTableView *v) : UViewLayoutImpl(v), t(*v) {
    t.lcur = t.ccur = t.ccount = t.lcount = 0;
  }
};


bool UTableView::doLayout(UpdateContext&parp, ViewLayout&vl) {
  Box* box = getBox();
  UTableLayoutImpl vd(this);
  
  vl.spec_w = vl.spec_h = -1;
  vl.dim.width  = vl.min_w = vl.max_w = 0;
  vl.dim.height = vl.min_h = vl.max_h = 0;
  
  // !!PREMIER ROUND!!
  vl.strategy = ViewLayout::GET_HINTS;
  UpdateContext ctx(parp, box, this, null);
  tableDoLayout(vd, ctx, *box, vl);
  
  if (vl.spec_w < 0) {        // "table uses as many space as needed"
    for (int c = 0; c < ccount; c++)  {
      cols[c].spec_d = cols[c].max_d;     // ?????
    }
  }
  
  else {  // "table width was explicitely specified"
    
    // REGLES: cell.uwidth est un 'hint':
    // - cell.w est tjrs >= cell.wmin qqsoit specif. cell.uwidth
    // - cell.w est < cell.uwidth si table.uwidth pas assez grande
    
    // spec. table.uwidth > max computed witdh ==> agrandir les cells
    if (vl.spec_w >= vl.max_w) {
      // 'fill_prop' = coefficient pour fill proportionnel =
      // (fill_w = used - reste_) / reste

      if (vl.max_w <= 0) vl.max_w = 1; //securite
      float fill_prop = (vl.spec_w - vl.max_w) / (double)vl.max_w;

      // favorite doit etre le MAX de la taille desiree et de la taille
      // requise par les enfants (sinon le scroll ne marchera pas dans
      // le 1e cas et le 'flex' ne marchera pas dans le second)
      // favoriteWidth = std::max(vl.spec_w, vl.max_w);
      // width = vl.spec_w;
      
      for (int c = 0; c < ccount; c++)  {
        // l'agrandissement est proportionnel au contenu de la cell
        cols[c].spec_d = cols[c].max_d + cols[c].max_d * fill_prop;
      }
    }
    
    // spec. table.uwidth > min computed witdh ==> reduire les cells
    else if (vl.spec_w >= vl.dim.width) {
      // 'fill_prop' = coefficient pour fill proportionnel =
      // (fill_w = used - reste_) / reste
      if (vl.max_w <= 0) vl.max_w = 1; //securite
      
      // !!! A REVOIR: FAIRE PROPORTION PAR RAPPORT AU MAX PAS AU MIN !!!
      // et tenir compte des SPECS
      float fill_prop = (vl.spec_w - vl.dim.width) / (double)vl.dim.width;
      
      for (int c = 0; c < ccount; c++)  {
        // taille min + proportion liee a la taille du max
        cols[c].spec_d = cols[c].min_d + cols[c].min_d * fill_prop;
      }
    }
    
    // spec. table.uwidth < min computed witdh ==> imposer cell.wmin
    else /*(vl.spec_w < vl.wmin)*/ {

      // !ATTENTION: diffs importantes avec hbox/vbox:
      // -- table.w est toujours >= somme{cells.wmin} qqsoit table.uwidth
      //    (==> les enfants ne sont jamais caches par les tables)
      // -- cell.finalw est tjrs >= cell.wmin qqsoit cell.width
      // -- cell.finalw est < cell.uwidth si table.uwidth trop petite
      //
      for (int c = 0; c < ccount; c++) cols[c].spec_d = cols[c].min_d;
    }
    
    // ENSUITE faudra voir les pourcentages des cells......!!!!
  }
  
  //else ... la table a une width proportionnelle....!!!
  
  vd.mustLayoutAgain = false;
  
  for (int k = 0; k < ccount; k++)
    cols[k].d = cols[k].min_d = cols[k].max_d = 0;  // !PAS tab[k].spec_d = 0;
  
  for (int k = 0; k < lcount; k++)
    lines[k].d = lines[k].min_d = lines[k].max_d = 0; // !PAS tab[k].spec_d = 0;
  
  ccur = ccount = lcount = 0; //APRES!
  
  vl.spec_w = vl.spec_h = -1;
  vl.dim.width  = vl.min_w = vl.max_w = 0;
  vl.dim.height = vl.min_h = vl.max_h = 0;
  
  // !!DEUXIEME ROUND!!
  vl.strategy = ViewLayout::IMPOSE_WIDTH;
  UpdateContext ctx2(parp, box, this, null);
  tableDoLayout(vd, ctx2, *box, vl);
  /*
  cerr << "Table: " << box << endl;
  for (int k = 0; k < ccount; k++)
    cerr << "- col" << k << " : "
    << cols[k].d <<" "<< cols[k].min_d <<" "<<  cols[k].max_d <<" "<<  cols[k].spec_d 
    << endl;
  cerr << endl;
  */
  return false;
};


void UTableView::tableDoLayout(UTableLayoutImpl& vd, UpdateContext& ctx, 
                               Element& grp, ViewLayout& vl) {
  UMultiList mlist(ctx, grp);
  ctx.rescale();
  vd.view->setScale(ctx.xyscale);
  
  if (ctx.local.content) {
    Element* content = ctx.local.content;
    ctx.local.content = null;	// avoid infinite recursion
    tableDoLayout(vd, ctx, *content, vl);   // pas de ctx, meme vd
  }
  
  // pour chaque ligne
  for (ChildIter ch = mlist.begin(); ch != mlist.end(); mlist.next(ch))
    if (!ch.getCond() || ch.getCond()->verifies(ctx, grp)) {

      Node* b = (*ch);
      Element* chgrp = null;  // att reinit!
      View* chboxview = null;
      ViewLayout chvl;      // att: reset by constr.
      
      if (b->toAttr()) {
        b->toAttr()->putProp(&ctx, grp);
      }
      
      else if (b->toData()) {
        Application::warning("Table::doLayout",
        "wrong child (%s %p) in Table (%p): a Table can only contain UTRows",
        &b->getClassName(), b, &grp);
      }
      
      else if ((chgrp = b->toElem()) && chgrp->isShowable()) {
        Box* chbox = chgrp->toBox();
        if (chbox) {  // QUE les Box
          
          if ((chbox->getDisplayType() == Element::BLOCK   // Box or pluggin
               && (chboxview = chbox->getViewInImpl(vd.view /*,&ch.child()*/)))
              ||
              (mlist.in_softwin_list && chbox->getDisplayType() == Element::SOFTWIN
               && (chboxview = chbox->getViewInImpl(vd.view /*,null*/))) //pas de ch!
              ) {
            
            //if (chboxview->hasVMode(View::FORCE_POS)) {
            if (chbox->isFloating()) {
              chvl.strategy = vl.strategy;
              vd.mustLayoutAgain |= chboxview->doLayout(ctx, chvl);
            }
            
            else {  // cas normal
              Application::warning("Table::doLayout",
              "wrong child (%s %p) in Table (%p): a Table can only contain UTRows", 
              &b->getClassName(), b, &grp);
            } // end cas normal
            
          } // endif(isDef(Mode::BOX) ...)
        } //endif(boxCast)
        
        else {           // UTrow or Element
          if ((dynamic_cast<UTrow*>(b))) {
            // agrandir lines si necessaire
            //if (vd.t.lcount >= vd.t.lmax) 
            //  vd.t.lines = augmentCells(vd.t.lines, &vd.t.lmax, 1);
            augmentCells(vd.t.lines, vd.t.lcount+1);
            
            // fonction specifique pour TableRows
            chvl.strategy = vl.strategy;
            rowDoLayout(null, vd, ctx, *chgrp, chvl);
            vd.t.lcount++;
          }
          
          else {   // just an Element
            UpdateContext chctx(ctx, chgrp, vd.view, null);
            tableDoLayout(vd, chctx, *chgrp, vl);  //own ctx, same vd
          }
        }
      }
    }
      
  // la suite ne concerne pas les Element
  if (grp.toBox()) {
    // a la ligne final
    // Les Table COMMENCENT et se TERMINENT par un alaligne! A COMPTER!!
    // alaligne(vl, ctx);

    PaddingSpec pad(0, 0);
    if (ctx.local.border) {
      ctx.local.border->getSize(ctx, pad); // !! A ETENDRE !!
    }

    const SizeSpec& size = ctx.local.size;

    vl.spec_w = 0.;  // redefini ensuite dans computeWidth
    //if (size.width.unit!=UAUTO && size.width.unit!=UPERCENT && size.width.unit!=UPERCENT_CTR)
      if (size.width!=UAUTO && size.width.unit!=UPERCENT && size.width.unit!=UPERCENT_CTR)
        vl.spec_w = size.width.toPixels(ctx.getDisp(), ctx.fontdesc, -1, -1);
      
    // prendre en compte les spacings (mais pas les borders rajoutes ensuite)
    vl.max_w = vl.dim.width = ctx.hspacing * (vd.t.ccount - 1);
 
    // redefini ensuite dans computeHeight
    vl.spec_h = 0;
    
    //if (size.height.unit!=UAUTO && size.height.unit!=UPERCENT && size.height.unit!=UPERCENT_CTR)
      if (size.height!=UAUTO && size.height.unit!=UPERCENT && size.height.unit!=UPERCENT_CTR)
      vl.spec_h = size.height.toPixels(ctx.getDisp(), ctx.fontdesc, -1, -1);

    // prendre en compte les spacings (mais pas les borders rajoutes ensuite)
    vl.dim.height = vl.max_h = ctx.vspacing * (vd.t.lcount - 1);

    for (int c = 0; c < vd.t.ccount; c++)  {
      vl.dim.width += vd.t.cols[c].d;
      vl.min_w += vd.t.cols[c].min_d;
      vl.max_w += vd.t.cols[c].max_d;
    }
    
    for (int l = 0; l < vd.t.lcount; l++)  {
      vl.dim.height += vd.t.lines[l].d;
      vl.min_h += vd.t.lines[l].min_d;
      vl.max_h += vd.t.lines[l].max_d;
    }

    // completement faux: min_* et max_* etant des tailles min et max 
    // theoriques ca n'a pas de sens de dire que les chidren font max_*
    //vd.chwidth = vl.max_w;
    //vd.chheight = vl.max_h;

    // NB: vd.chheight ne doit pas prendre en compte les borders (ce qui
    // doit etre le cas ceux-ci etant ajoutes dans computeWidth/Height)
    vd.chwidth  = vl.dim.width;
    vd.chheight = vl.dim.height;

    // arg opt. true => les cmin/cmax sont deja definis 
    // (seulement leur rajouter les borders)
    vd.computeWidth(ctx, pad, vl, true);
    vd.computeHeight(ctx, pad, vl, true);
  }
} 

// nb: la logique est la meme pour les lines

static void computeSizes(vector<UViewCell>& cols, int ccur, int span,
                         float d, float min_d, float max_d, float spec_d) {
  if (span == 1) {
    cols[ccur].d = std::max(cols[ccur].d, d);
    cols[ccur].min_d = std::max(cols[ccur].min_d, min_d);
    cols[ccur].max_d = std::max(cols[ccur].max_d, max_d);
    cols[ccur].spec_d= std::max(cols[ccur].spec_d, spec_d);
  }

  else {
    // compter l'espace total pris par les lignes precedentes sur
    // les 'colspan' colonnes concernees
    float total_d = 0, total_mind = 0, total_maxd = 0, total_specd = 0;  // ex int
    for (int c = ccur; c < ccur + span; c++) {
      total_d     += cols[c].d;
      total_mind  += cols[c].min_d;
      total_maxd  += cols[c].max_d;
      total_specd += cols[c].spec_d;
    }

    // si espace necessaire pour cet dataent superieur au total des
    // 'colspan' colonnes precedentes alors il faut les agrandir
    // (en repartissant sur chaque colonne)
    float add_d = 0, add_mind = 0, add_maxd = 0, add_specd = 0;  // ex int

    if (total_d < d)
      add_d = (double)(d - total_d) / span;

    if (total_mind < min_d)
      add_mind = (double)(min_d - total_mind) / span;

    if (total_maxd < max_d)
      add_maxd = (double)(max_d - total_maxd) / span;

    if (total_specd < spec_d)
      add_specd = (double)(spec_d - total_specd) / span;

    // rajouter le surplus (eventuellement nul) a chaque colonne
    for (int c = ccur; c < ccur + span; c++) {
      cols[c].d  += add_d;
      cols[c].min_d  += add_mind;
      cols[c].max_d  += add_maxd;
      cols[c].spec_d += add_specd;
    }
  }  
}


static void setSizes(View *chboxview, UpdateContext &ctx,
                     int colspan, int rowspan,
                     UTableLayoutImpl& vd, ViewLayout& chvl,
                     ViewLayout::Strategy strategy) {
  int ccur = vd.t.ccur;
  
  if (strategy == ViewLayout::GET_HINTS) {
    chvl.strategy = ViewLayout::GET_HINTS;
  }
  else if (strategy == ViewLayout::IMPOSE_WIDTH) {
    chvl.strategy = ViewLayout::IMPOSE_WIDTH;
    chvl.spec_w = 0;
    for (int c = ccur; c < ccur + colspan; c++) {
      chvl.spec_w += vd.t.cols[c].spec_d;
    }    
  }
  else {
    Application::internalError("UTrow::doLayout","wrong ViewLayout mode");
    return;
  }
  
  chboxview->doLayout(ctx, chvl);  // init chvl
  vd.t.cols[ccur].colspan = colspan;
  vd.t.cols[ccur].rowspan = rowspan;
  
  // colonnes
  computeSizes(vd.t.cols, ccur, colspan,
               chvl.dim.width, chvl.min_w, chvl.max_w, chvl.spec_w);
  // lignes  
  computeSizes(vd.t.lines, vd.t.lcount, rowspan,
               chvl.dim.height, chvl.min_h, chvl.max_h, chvl.spec_h);
}


void UTableView::rowDoLayout(View* row_view, UTableLayoutImpl& vd,
                             UpdateContext& parp, Element& grp, ViewLayout& vl) {
  // soit le row est un Box et il a sa propre view soit c'est un Element
  // et il utilise celle de la table
  if (!row_view) row_view = vd.view;
  UpdateContext ctx(parp, &grp, row_view, null);
  UMultiList mlist(ctx, grp);
  ctx.rescale();
  vd.view->setScale(ctx.xyscale);

  vd.t.ccur = 0;
  
  for (ChildIter ch = mlist.begin(); ch != mlist.end(); mlist.next(ch))
    if (!ch.getCond() || ch.getCond()->verifies(ctx, grp)) {

      Node* b = (*ch);
      View *chboxview = null;
      Element *chgrp = null; //!att reinit!
      ViewLayout chvl; //att: reinit by constr.
      
      if (b->toAttr())  
        b->toAttr()->putProp(&ctx, grp);
      
      else if (b->toData()) {
        /* normalement interdits
        b->dataCast()->getSize(&ctx, &chw, &chh);
        vd.chwidth += chw;
        if (ch->next()) vd.chwidth += ctx.hspacing; // FAUX!
        if (chh > vd.chheight) vd.chheight = chh;  // =STD::MAX()
        */
        Application::warning("UTrow::doLayout",
        "wrong child (%s %p) in UTrow (%p): a UTrow can only contain objects that derive from Element",
        &b->getClassName(), b, &grp);
      }
      
      // Element, UBoxes, UWins
      else if ((chgrp = b->toElem()) && chgrp->isShowable()) {
        Box* chbox = chgrp->toBox();
        if (chbox) {  // QUE les Box
          
          if ((chbox->getDisplayType() == Element::BLOCK   // Box or pluggin
               && (chboxview = chbox->getViewInImpl(row_view /*,&ch.child()*/)))
              ||
              (mlist.in_softwin_list && chbox->getDisplayType() == Element::SOFTWIN
               && (chboxview = chbox->getViewInImpl(row_view /*,null*/))) //pas de ch!
              ) {
            
            //if (chboxview->hasVMode(View::FORCE_POS)) {
            if (chbox->isFloating()) {
              chvl.strategy = vl.strategy;
              vd.mustLayoutAgain |= chboxview->doLayout(ctx, chvl);
            }
            
            else {  // cas normal
              int colspan = 1;  //default
              int rowspan = 1;
              
              //cas des UTcell: peuvent avoir un colspan et un rowspan
              UTcell *cell = dynamic_cast<UTcell*>(b);
              if (cell) {
                colspan = cell->getColspan();
                rowspan = cell->getRowspan();
              }
              
              // agrandir cols si necessaire
              //if (vd.t.ccur + colspan > vd.t.cmax) 
              //  vd.t.cols = augmentCells(vd.t.cols, &vd.t.cmax, colspan);
              augmentCells(vd.t.cols, vd.t.ccur + colspan);

              // tenir compte des rowspan des lignes precedentes
              if (vd.t.cols[vd.t.ccur].rowspan > 1) {
                (vd.t.cols[vd.t.ccur].rowspan)--;
                vd.t.ccur += vd.t.cols[vd.t.ccur].colspan;
              }
              
              setSizes(chboxview, ctx, colspan, rowspan, vd, chvl,vl.strategy);
              
              // passer a la Nieme colonne suivante
              vd.t.ccur += colspan;
            } // end cas normal
          } // endif(isDef(Mode::BOX) ...)
        } //endif(boxCast)
        
        // just an Element
        else {
          UpdateContext chctx(ctx, chgrp, row_view, null);
          rowDoLayout(null, vd, chctx, *chgrp, vl);//own ctx, same vd
        }
      }
    }
      
  // ccount = MAX de c pour chaque ligne
  vd.t.ccount = std::max(vd.t.ccount, vd.t.ccur);
}

}

