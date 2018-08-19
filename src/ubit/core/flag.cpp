/*
 *  flag.cpp
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2009 | Eric Lecolinet | ENST Paris | http://www.enst.fr/~elc/ubit
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

#include <ubit/ubit_features.h>
#include <iostream>
#include <ubit/core/flag.h>
#include <ubit/ui/updatecontext.h>
using namespace std;

namespace ubit {


const Flag Flag::none;

bool Flag::verifies(const UpdateContext& ctx, const Element&) const {
  return (ctx.getFlagdef(this) != null);
}

bool NotFlag::verifies(const UpdateContext& ctx, const Element& par) const {
  return !cond.verifies(ctx, par);
}


UFlagdef::UFlagdef() : flag(null) {}
UFlagdef::UFlagdef(const Flag& f) : flag(&f) {}

UFlagdef& UFlagdef::set(const Flag& f) {
  if (checkConst()) return *this;
  // test equals ???
  flag = &f;
  update();  // pas de changed() ?
  return *this;
}

UFlagdef& UFlagdef::clear() {
  flag = null;
  update();  // pas de changed() ?
  return *this;
}

void UFlagdef::update() {
  updateAutoParents(Update::LAYOUT_PAINT);
}

void UFlagdef::putProp(UpdateContext* ctx, Element&) {
  if (flag && flag != &Flag::none) ctx->addFlagdef(this);
}


UPropdef::UPropdef() : prop(null) {}
UPropdef::UPropdef(const Flag& _f) : UFlagdef(_f), prop(null) {}
UPropdef::UPropdef(const Flag& _f, Attribute& _p) : UFlagdef(_f), prop(&_p) {}

UPropdef& upropdef() {return *new UPropdef();}
UPropdef& upropdef(const class Flag& _f, Attribute& _p) {return *new UPropdef(_f, _p);}

UPropdef& UPropdef::set(const Flag& _f) {
  if (checkConst()) return *this;
  // test equals ???
  flag = &_f;
  update();  // pas de changed() ?
  return *this;
}

UPropdef& UPropdef::set(Attribute& _p) {
  if (checkConst()) return *this;
  // test equals ???
  prop = _p;
  update();  // pas de changed() ?
  return *this;
}

UPropdef& UPropdef::set(const Flag& _f, Attribute& _p) {
  if (checkConst()) return *this;
  // test equals ???
  flag = &_f;
  prop = _p;
  update();  // pas de changed() ?
  return *this;
}

UPropdef& UPropdef::clear() {
  if (checkConst()) return *this;
  flag = null;
  prop = null;
  update();  // pas de changed() ?
  return *this;
}

void UPropdef::putProp(UpdateContext *ctx, Element&) {
  if (prop && flag && flag != &Flag::none)
    ctx->addFlagdef(this);
}


UPropval::UPropval() : flag(null) {}
UPropval::UPropval(const Flag& _f) : flag(&_f) {}

UPropval& upropval(const class Flag& _f) {
  return *new UPropval(_f);
}

void UPropval::update() {
  updateAutoParents(Update::LAYOUT_PAINT);
}

void UPropval::putProp(UpdateContext* ctx, Element& par) {
  if (flag) {
    const UPropdef* fd = ctx->getPropdef(flag);
    if (fd && fd->getProp()) fd->getProp()->putProp(ctx, par);
  }
}

}

