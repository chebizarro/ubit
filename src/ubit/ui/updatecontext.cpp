/*
 *  pdatecontext.cpp: [implementation] stack context
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
#include <ubit/draw/color.h>
#include <ubit/core/boxgeom.h>
#include <ubit/ui/box.h>
#include <ubit/ui/updatecontext.h>
#include <ubit/core/application.h>
#include <ubit/core/flag.h>
using namespace std;
namespace ubit {



UpdateContext::UpdateContext(const UpdateContext& parctx, 
                               Element* elm, View* v, UViewUpdateImpl* vi) :
parent_ctx(&parctx),
win_ctx(parctx.win_ctx),
flag_count(parctx.flag_count),
obj(elm),
view(v),
view_impl(vi),
obj_style(&obj->getStyle(this)),    // att: le style depend de l'orient 
local(obj_style->local),            // local props that can't be inherited
pos(null),
graph(parctx.graph)
{
  obj->emodes.IS_WIDTH_UNRESIZABLE =(local.size.width.modes.val & Size::UNRESIZABLE.val);  
  obj->emodes.IS_HEIGHT_UNRESIZABLE=(local.size.height.modes.val & Size::UNRESIZABLE.val);
  
  xyscale = parctx.xyscale;
  boxIsVFlex = (parctx.valign == Valign::FLEX);
  boxIsHFlex = (parctx.halign == Halign::FLEX);

  // si pas de spec de l'orient prendre celle du style par defaut
  /*
  if (!obj->hasBMode(UMode::HAS_ORIENT)) {
    if (style.orient == Orientation::INHERIT)
      obj->setBmodes(UMode::IS_VERTICAL, parp.obj->hasBMode(UMode::IS_VERTICAL));
    else
      obj->setBmodes(UMode::IS_VERTICAL, style.orient == Orientation::VERTICAL);
  }
   */
  const Style& style = *obj_style;
  valign = (style.valign == Valign::INHERIT ? parctx.valign : style.valign);
  halign = (style.halign == Halign::INHERIT ? parctx.halign : style.halign);

  // diviser par /xyscale car sera multiplie par UpdateContext::rescale() 
  vspacing = (style.vspacing == VSpacing::INHERIT ? parctx.vspacing/xyscale : style.vspacing);
  hspacing = (style.hspacing == HSpacing::INHERIT ? parctx.hspacing/xyscale : style.hspacing);
  cursor = (style.cursor ? style.cursor : parctx.cursor);
  //edit = (style.edit ? style.edit : parp.edit);
  edit = parctx.edit;  // !!!ATT un button dans un textarea va etre editable A REVOIR !!!

  color = style.getColor(*obj);
  if (!color || color == &Color::inherit) color = parctx.color;

  bgcolor = style.getBgcolor(*obj);
  if (!bgcolor || bgcolor == &Color::inherit) bgcolor = parctx.bgcolor;
  
  // les fontes s'AJOUTENT et c'est pourquoi il faut toujours recopier parp->fontdesc
  // (les autres champs pouvant etre modifies ensuite)
  fontdesc = parctx.fontdesc;

  // c'est un ajout de caracteristiques, pas forcement une recopie totale
  // => utiliser merge()

  if (style.font) {
    fontdesc.merge(*style.font);
    // pas besoin de faire setScale si font heritee telle quelle
    fontdesc.setScale(xyscale);
  }
}

// creates the first layer of the context stack
// !Warning: 'win_view' must be a valid (not null) window view!

WindowUpdateContext::WindowUpdateContext(View* win_view, Graph* g) :
UpdateContext(win_view) {
  win_ctx = this;
  graph = g;
}


UpdateContext::UpdateContext(View* win_view) :
parent_ctx(null),
win_ctx(null),   // init in WindowUpdateContext
flag_count(0),
obj(win_view->getBox()),
view(win_view),
view_impl(null),
obj_style(&obj->getStyle(this)),    // att: le style depend de l'orient 
local(obj_style->local),            // local props that can't be inherited
pos(null),
graph(null) 
{ 
  // DES CHOSES A FAIRE POUR BLOQUER LA FENETRE !!!!!!!!
  /*
   if (local.size.width.unit != UNONE && !local.size.width_mode.is_resizable)
   obj->emodes.IS_WIDTH_NOT_RESIZABLE = true;
   else obj->emodes.IS_WIDTH_NOT_RESIZABLE = false;
   
   if (local.size.height.unit != UNONE && !local.size.height_mode.is_resizable)
   obj->emodes.IS_HEIGHT_NOT_RESIZABLE = true;
   else obj->emodes.IS_HEIGHT_NOT_RESIZABLE = false;
   */
  obj->emodes.IS_WIDTH_UNRESIZABLE = (local.size.width.modes.val & Size::UNRESIZABLE.val);
  obj->emodes.IS_HEIGHT_UNRESIZABLE= (local.size.height.modes.val & Size::UNRESIZABLE.val);
  
  boxIsHFlex = boxIsVFlex = false;
  
  // si pas de prop qui definit l'orient prendre celle du style par defaut
  /*
   if (!obj->hasBMode(UMode::HAS_ORIENT)) {
   obj->setBmodes(UMode::IS_VERTICAL, style.orient == Orientation::VERTICAL);
   }
   */
  
  const Style& style = *obj_style;
  valign = (style.valign == Valign::INHERIT ? 0 : style.valign);
  halign = (style.halign == Halign::INHERIT ? 0 : style.halign);
  vspacing = (style.vspacing == VSpacing::INHERIT ? 0 : style.vspacing);
  hspacing = (style.hspacing == HSpacing::INHERIT ? 0 : style.hspacing);
  cursor = style.cursor;
  //edit = (style.edit ? style.edit : null);
  edit = null;
  
  color = style.getColor(*obj); 
  if (!color || color == &Color::inherit) color = &Color::black;
  
  bgcolor = style.getBgcolor(*obj);  
  if (!bgcolor || bgcolor == &Color::inherit) bgcolor = &Color::none;
  
  xyscale = Application::conf.scale;  // set by Display::Display  
  fontdesc.set(*Application::conf.default_font);   // cant be null!
  
  // c'est un ajout de caracteristiques: n'ecrase pas forcement tous les champs
  if (style.font) fontdesc.merge(*style.font);
  fontdesc.setScale(xyscale);
}


void UpdateContext::rescale() {  
  vspacing = vspacing * xyscale;
  hspacing = hspacing * xyscale;
  
  // !!! A COMPLETER !!! prendre en compte les Units si necessaire !!!!!!

  local.padding.left.val   = local.padding.left.val * xyscale;
  local.padding.right.val  = local.padding.right.val * xyscale;
  local.padding.top.val    = local.padding.top.val * xyscale;
  local.padding.bottom.val = local.padding.bottom.val * xyscale;
}


void UpdateContext::addFlagdef(const UFlagdef* flagdef) {
  win_ctx->flags.push_back(flagdef);
  flag_count++;
}

const UFlagdef* UpdateContext::getFlagdef(const Flag& f) const {
  return getFlagdef(&f);
}

const UFlagdef* UpdateContext::getFlagdef(const Flag* f) const {
  for (int k = 0; k < flag_count; k++) {
    if (win_ctx->flags[k]->getFlag() == f) return win_ctx->flags[k];
  }
  return null;  // not found
}


const UPropdef* UpdateContext::getPropdef(const Flag& f) const {
  return getPropdef(&f);
}
  
const UPropdef* UpdateContext::getPropdef(const Flag* f) const {
  // att: comme il peut y avoir plusieurs propdef empilees il faut
  // toujours prendre la derniere
  const UPropdef* last_pdef = null;
  
  for (int k = 0; k < flag_count; k++) {
    const UPropdef* pdef = null;
    if (win_ctx->flags[k]->getFlag() == f
        && (pdef = dynamic_cast<const UPropdef*>(win_ctx->flags[k]))) {
      last_pdef = pdef;
    }
  }
  return last_pdef;
}

}


