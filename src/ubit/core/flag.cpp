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
#include <ubit/uflag.hpp>
#include <ubit/uupdatecontext.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

const UFlag UFlag::none;

bool UFlag::verifies(const UpdateContext& ctx, const Element&) const {
  return (ctx.getFlagdef(this) != null);
}

bool UNotFlag::verifies(const UpdateContext& ctx, const Element& par) const {
  return !cond.verifies(ctx, par);
}


UFlagdef::UFlagdef() : flag(null) {}
UFlagdef::UFlagdef(const UFlag& f) : flag(&f) {}

UFlagdef& UFlagdef::set(const UFlag& f) {
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
  if (flag && flag != &UFlag::none) ctx->addFlagdef(this);
}


UPropdef::UPropdef() : prop(null) {}
UPropdef::UPropdef(const UFlag& _f) : UFlagdef(_f), prop(null) {}
UPropdef::UPropdef(const UFlag& _f, Attribute& _p) : UFlagdef(_f), prop(&_p) {}

UPropdef& upropdef() {return *new UPropdef();}
UPropdef& upropdef(const class UFlag& _f, Attribute& _p) {return *new UPropdef(_f, _p);}

UPropdef& UPropdef::set(const UFlag& _f) {
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

UPropdef& UPropdef::set(const UFlag& _f, Attribute& _p) {
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
  if (prop && flag && flag != &UFlag::none)
    ctx->addFlagdef(this);
}


UPropval::UPropval() : flag(null) {}
UPropval::UPropval(const UFlag& _f) : flag(&_f) {}

UPropval& upropval(const class UFlag& _f) {
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

