/*
 *  geom.cpp: Geometry
 *  Ubit GUI Toolkit - Version 6
 *  (C) 2009 | Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
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
#include <ubit/ubit_features.h>
#include <ubit/ulength.hpp>
#include <ubit/ustr.hpp>
#include <ubit/uappli.hpp>
#include <ubit/draw/ufontImpl.hpp>
using namespace std;
namespace ubit {


const Unit 
UPX(Unit::PX),
UEM(Unit::EM),
UEX(Unit::EX),
UPERCENT(Unit::PERCENT),
UPERCENT_CTR(Unit::PERCENT_CTR),
UIN(Unit::IN),
UCM(Unit::CM),
UMM(Unit::MM),
UPT(Unit::PT),
UPC(Unit::PC);

const Length
UAUTO(0., Unit(Unit::AUTO)),
UIGNORE(0., Unit(Unit::IGNORE));

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

ostream& operator<<(ostream& s, const Length& l) {
  return (s << l.toString());
}

bool Length::operator==(const Length& l) const {
  return val == l.val && unit.type == l.unit.type && modes.val == l.modes.val;
}

bool Length::operator!=(const Length& l) const {
  return val != l.val || unit.type != l.unit.type || modes.val != l.modes.val;
} 

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Length::Length(const String& spec) : val(0), unit(UPX), modes(0) {
  set(spec);
}

Length& Length::set(const String& spec) {
  val = 0.;
  unit = UPX;
  modes.val = 0;
  
  if (spec.empty()) {
    Application::error("Length:","empty textual specification");
    return *this;
  }
  
  const char* u = null;
  spec.scanValue(val, u);
  return setUnit(u);
}

Length& Length::setUnit(const char* spec) {
  if (!spec || !*spec) {
    unit.type = Unit::PX;
    return *this;
  }
  
  char u0 = tolower(spec[0]);
  char u1 = tolower(spec[1]);
  
  switch (u0) {
    case 'a':
      if (u1 == 'u' || u1 == 0) unit.type = Unit::AUTO;
      else goto ERROR;
      break;
    case 'p':
      if (u1 == 'x') unit.type = Unit::PX;
      else if (u1 == 't') unit.type = Unit::PT;
      else if (u1 == 'c') unit.type = Unit::PC;
      else goto ERROR;
      break;
    case '%':
      if (u1 == 0) unit.type = Unit::PERCENT;
      else goto ERROR;
      break;
    case 'e':
      if (u1 == 'x') unit.type = Unit::EX;
      else if (u1 == 'm') unit.type = Unit::EM;
      else goto ERROR;
      break;
    case 'i':
      if (u1 == 'n') unit.type = Unit::IN;
      else goto ERROR;
      break;
    case 'm':
      if (u1 == 'm') unit.type = Unit::MM;
      else goto ERROR;
      break;
    case 'c':
      if (u1 == 'm') unit.type = Unit::CM;
      else goto ERROR;
      break;
    default:
      goto ERROR;
      break;
  }
  return *this;
  
ERROR: 
  Application::error("Length::setUnit","invalid unit");
  return *this;
}


float Length::toPixels(Display* disp, const UFontDesc& fd, 
                        float view_len, float parview_len) const {
  float pixlen = 0;
  switch (unit.type) {
    //case Length::Unit::AUTO:
      //...;
      //break
    case Unit::PX:
      pixlen = val;
      break;
    case Unit::PERCENT:
      pixlen = val * parview_len / 100.; // percentages of the parent size 
      break;
    case Unit::PERCENT_CTR:
      // the size of this view is removed from the parent size
      pixlen = val * (parview_len - view_len) / 100.;
      break;
    case Unit::EX:
      pixlen = val * fd.actual_size * 0.5 * disp->PT_TO_PX;
      break;      
    case Unit::EM:      
      //pixlen = val * UFontMetrics(fd, disp).getHeight();
      pixlen = val * fd.actual_size * disp->PT_TO_PX;
      break;
    case Unit::IN:
      pixlen = val * disp->IN_TO_PX;
      break;
    case Unit::MM:
      pixlen = val * disp->MM_TO_PX;
      break;      
    case Unit::CM:
      pixlen = val * disp->CM_TO_PX;
      break;
    case Unit::PT:
      pixlen = val * disp->PT_TO_PX;
      break;
    case Unit::PC:
      pixlen = val * disp->PC_TO_PX;
      break;
    default:
      Application::error("Length::toPixels","invalid unit");
      return 0;
  }
  
  if (modes.val) return parview_len - view_len - pixlen;
  else return pixlen;
} 


String Length::toString() const {
  String s;
  s.setFloat(val);

  switch (unit.type) {
    case Unit::AUTO:
      s = "auto";
      break;
     case Unit::PX:
      s &= "px";
      break;
    case Unit::PERCENT:
      s &= "%";
      break;
    case Unit::PERCENT_CTR:
      s &= "%c";
      break;
    case Unit::EX:
      s &= "ex";
      break;      
    case Unit::EM:      
      s &= "em";
      break;
    case Unit::IN:
      s &= "in";
      break;
    case Unit::MM:
      s &= "mm";
      break;      
    case Unit::CM:
      s &= "cm";
      break;
    case Unit::PT:
      s &= "pt";
      break;
    case Unit::PC:
      s &= "pc";
      break;
    default: //case Unit::IGNORE:
      s = "none";
      break;
  }
  return s;
} 

// =========================================================== [Elc] ===========
/*
 void PaddingSpec::setH(float l_margin, float r_margin) {
 left = l_margin;
 right = r_margin;
 }
 
 void PaddingSpec::setV(float t_margin, float b_margin) {
 top = t_margin;
 bottom = b_margin;
 }
 */

void PaddingSpec::add(const PaddingSpec& p2) {
  // !!! A COMPLETER !!! prendre en compte les Units !!!!!!
  top.val    += p2.top.val; 
  bottom.val += p2.bottom.val;
  left.val   += p2.left.val; 
  right.val  += p2.right.val;
}

}

