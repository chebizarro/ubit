/*
 *  css.cpp: HTML Style Sheets
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

#include <iostream>
#include <ubit/ubit_features.h>
#include <ubit/ubit.hpp>
#include <ubit/ui/updatecontext.h>
#include <ubit/ucss.hpp>
#include <ubit/core/dom.h>
using namespace std;

namespace ubit {



struct UCssMaker: public StyleParser::StyleMaker {
  XmlDocument* doc;
  AttributeList* attributes;

  UCssMaker(XmlDocument* d, AttributeList* alist) : doc(d), attributes(alist) {}

  virtual void begin() {
    attributes = null;
  }

  virtual void end(bool ok) {
    attributes = null;
    // delete ? ....;
  }

  virtual void create() {
    for (unsigned int k = 0; k < count; k++) {
      const Class* c = doc->getStyleSheet().obtainClass(*selectors[k]);
      //cerr << " *** " << *selectors[k] << endl;
      if (k == 0) {
        attributes = c->getAttributes();      //attributes partagee !!!
        if (!attributes) attributes = new AttributeList;
      }
      c->setAttributes(attributes);
    }
  }
   
  virtual void addProp(const String& name, const String& val) {
    static CssProperties css_props;   // !!!
    
    if (!attributes) 
      Application::error("UCssMaker","this UCssMaker object (%p) has no prop list", this);

    CssProperties::AddPropFunc func = css_props.findAddPropFunc(name);
    if (func) func(doc, attributes, val);
    /*
    UAttribute* p = c ? c(doc, val) : null;
    if (p) {
      //cerr << name << " " << val << endl;
      props->addAttr(*p);
    }
     */
 }
};


int CssParser::parse(const String& buf, XmlDocument* doc) {
  UCssMaker* smaker = new UCssMaker(doc, null);
  parseImpl(*smaker, buf);
  delete smaker;
  return stat;
}

int CssParser::parseAttr(const String& buf, XmlDocument* doc, AttributeList* props) {
  p = text_buffer = buf.c_str();
  if (!p || !*p) {
    p = text_buffer = null;
    return 0;
  }

  UCssMaker* smaker = new UCssMaker(doc, props);
  readStyleProps(*smaker);

  delete smaker;
  return stat;
}


void StyleProperties::defProp(const char* propname, StyleProperties::AddPropFunc func) {
  prop_map[new String(propname)] = func;
}

void StyleProperties::defProp(const String& propname, StyleProperties::AddPropFunc func) {
  prop_map[new String(propname)] = func;
}

StyleProperties::AddPropFunc StyleProperties::findAddPropFunc(const String& propname)  {
   PropMap::const_iterator k = prop_map.find(&propname);
   if (k == prop_map.end()) return 0;
   else return (k->second);
 }

StyleProperties::~StyleProperties() {
  for (PropMap::iterator k = prop_map.begin(); k != prop_map.end(); k++)
    delete k->first;
}

/*
 //static bool isEq(const String& s1, const char* s2);
 //static bool isEq(const String* s1, const char* s2);
 
bool StyleProperties::isEq(const String& s1, const char* s2) {
  return s1.equals(s2, true);      
}

bool StyleProperties::isEq(const String* s1, const char* s2) {
  return s1 ? s1->equals(s2, true) : false;
}

// QUE POSITIF !!!
bool StyleProperties::parseNum(const String& s, float& val, String& unit) {

bool StyleProperties::parseNum(const String* s, float& val, String& unit) {
  return s ? parseNum(*s, val, unit) : false;
}
*/

bool StyleProperties::parseUrl(const String& s, String& url, String& remain) {
  url.clear(); remain.clear();
  int begin = s.find("url(");

  if (begin >= 0) {
    int end = s.find(")");
    if (end > begin) {
      url.insert(0, s, begin+4, end-(begin+4));
      url.trim();
      if (url[0] == '"' || url[0] == '\'') url.remove(0,1);
      if (url[-1] == '"' || url[-1] == '\'') url.remove(-1,1);

      remain.insert(0, s, end+1);
      remain.trim();
      //cerr << "getURL '" << url  <<"' remain: " << remain << endl;
      return true;
    }
  }
  remain = s;
  return false;
}


CssProperties::CssProperties() {  
  defProp("font-family",     create_font_family);
  defProp("font-size",       create_font_size);
  defProp("font-weight",     create_font_weight);
  defProp("font-style",      create_font_style);
  //defProp("font",          create_font);  etc...

  defProp("color",           create_color);
  defProp("background-color",create_background_color);
  defProp("background-image",create_background_image);
  defProp("background",      create_background);

  defProp("width",           create_width);
  defProp("height",          create_height);
  defProp("text-align",      create_text_align);       // ~~ align
  defProp("vertical-align",  create_vertical_align);   // ~~ valign

  // text-indent
  //'text-decoration'
  // direction
  //  'position'static | relative | absolute | fixed | inherit
  //  'top'  'right'  'bottom' 'left'

  /*
   defProp("margin-top",     create_margin_top);   A REVOIR
   defProp("margin-bottom",  create_margin_bottom);
   defProp("margin-left",    create_margin_left);
   defProp("margin-right",   create_margin_right);
   //defProp("margin",       create_margin);
   */

  defProp("padding-top",     create_padding_top);
  defProp("padding-bottom",  create_padding_bottom);
  defProp("padding-left",    create_padding_left);
  defProp("padding-right",   create_padding_right);
  //defProp("padding",       create_padding);

  defProp("border",          create_border);
  // border-*....
  // table_* ...
}


void CssProperties::create_font_family(XmlDocument*, AttributeList* props, const String& v) {
  //Font* p = new Font;
  // NB: obtainAttr() add attribute to props
  if (!v.empty()) props->obtainAttr<Font>().setFamily(v);  //  trim ??
}

void CssProperties::create_font_size(XmlDocument*, AttributeList* props, const String& v) {
  //Font* p = new Font;
  if (!v.empty()) props->obtainAttr<Font>().setSize(v);  //  trim ??
}

void CssProperties::create_font_weight(XmlDocument*, AttributeList* props, const String& v) {
  //Font* p = new Font;
  if (v.empty()) return;
  Font& f = props->obtainAttr<Font>();

  if (v.equals("bold",true)) f.setBold(true);
  else /* if (isEq(v,"medium")) */ f.setBold(false);
}

void CssProperties::create_font_style(XmlDocument*, AttributeList* props, const String& v) {
  //Font* p = new Font();
  if (v.empty()) return;
  Font& f = props->obtainAttr<Font>();

  if (v.equals("italic",true)) f.setItalic(true);
  else /* if (isEq(v,"normal")) */ f.setItalic(false);
}

//UAttribute* CssProperties::create_font(const String& v) {
//return p;
//}

// etc...


void CssProperties::create_color(XmlDocument*, AttributeList* props, const String& v) {
  Color* p = new Color();
  //trim ??
  //p->set(v);
  p->setNamedColor(v);
  props->addAttr(*p);
}

void CssProperties::create_background_color(XmlDocument*, AttributeList* props, 
                                        const String& v) {
  Background* p = new Background();
  //trim ??
  if (v.equals("transparent",true)) p->setColor(Color::none);
  else if (!v.empty()) p->setNamedColor(v);
  props->addAttr(*p);
}

void CssProperties::create_background_image(XmlDocument* doc, AttributeList* props, 
                                        const String& v) {
  Background* p = new Background();
  String url, remain;
  //trim
  if (!parseUrl(v, url, remain)) {
    props->addAttr(*p);
    return;
  }
  
  if (v.equals("none",true)) ;   // rien a faire ?...
  else {
    String path; doc->makePath(path, url);
    Image* ima = new Image(path);
    p->setIma(*ima);
    static String name = "background_image";
    doc->addAttachment(new ImageAttachment(url, name, ima));
  }
  props->addAttr(*p);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void CssProperties::create_background(XmlDocument* doc, AttributeList* al, const String& v) {
  Background* p = new Background();
  //trim
  String url, remain;
  
  if (!parseUrl(v, url, remain)) {
    if (!v.empty()) p->setNamedColor(v);
  }
  else  {
    String path; doc->makePath(path, url);
    Image* ima = new Image(path);
    // EX: if (!remain.empty()) p->set(*ima, *new UBgcolor(remain));   // !! %%
    //else 
    *p = *ima;
    static String name = "background";
    doc->addAttachment(new ImageAttachment(url, name, ima));
  }  
  al->addAttr(*p);
}


void CssProperties::create_width(XmlDocument*, AttributeList* al, const String& v) {
  // EX: UWidth* p = new UWidth(-1);
  // NB: obtainAttr() add attribute to props
  if (!v.empty()) al->obtainAttr<Size>().setWidth(v);
}

void CssProperties::create_height(XmlDocument*, AttributeList* al, const String& v) {
  // EX: UHeight* p = new UHeight(-1);
  // NB: obtainAttr() add attribute to props
  if (!v.empty()) al->obtainAttr<Size>().setHeight(v);
}

// egalement: 'line-height'


void CssProperties::create_padding_top(XmlDocument*, AttributeList* al, const String& v) {
  // NB: obtainAttr() add attribute to props
  if (!v.empty()) al->obtainAttr<Padding>().setTop(v);
}

void CssProperties::create_padding_bottom(XmlDocument*, AttributeList* al, const String& v) {
  if (!v.empty()) al->obtainAttr<Padding>().setBottom(v);
 }

void CssProperties::create_padding_left(XmlDocument*, AttributeList* al, const String& v) {
  if (!v.empty()) al->obtainAttr<Padding>().setLeft(v);
}

void CssProperties::create_padding_right(XmlDocument*, AttributeList* al, const String& v) {
  if (!v.empty()) al->obtainAttr<Padding>().setRight(v);
}

//CssProperties::create_padding

/*  align n'existe pas, text-align joue son role ?
 void create_align(XmlDocument*, const String& v) {
 ?? trim()
 if (isEq(UXmlAttr::value,"left")) set(Halign::left);
 else if (isEq(UXmlAttr::value,"center")) set(Halign::center);
 else if (isEq(UXmlAttr::value,"right"))  set(Halign::right);
 props->addAttr(*p);
 }
 */

void CssProperties::create_text_align(XmlDocument*, AttributeList* al, const String& v) {
  Halign* p = new Halign();
  //trim ??
  if (v.equals("left",true)) *p = Halign::left;
  else if (v.equals("center",true)) *p = Halign::center;
  else if (v.equals("right",true))  *p = Halign::right;
  // A COMPLETR:   justify, etc...
  al->addAttr(*p);
}

// valign existe pas: vertical_align joue son role ?
//
void CssProperties::create_vertical_align(XmlDocument*, AttributeList* al, const String& v) {
  Valign* p = new Valign();
  //trim ??
  if (v.equals("top",true)) *p = Valign::top;
  else if (v.equals("middle",true)) *p = Valign::center;
  else if (v.equals("bottom",true)) *p = Valign::bottom;
  // baseline et bien d'autres.....
  al->addAttr(*p);
}

/*
 'text-indent'
 'text-decoration'
 'position'static | relative | absolute | fixed | inherit
 'top'  'right'  'bottom' 'left'
 direction
 */


void CssProperties::create_border(XmlDocument*, AttributeList* al, const String& v) {
  Border* p = new Border();
  String unit;
  float n = 0;
  if (!v.scanValue(n, unit)) {
    if (n > 1) p->setDecoration(Border::LINE);   // A COMPLETER
    else p->setDecoration(Border::NONE);
  }
  al->addAttr(*p);
}

/* TABLES:
'border-collapse' collapse | separate | inherit
'border-spacing' <length> <length>? | inherit
remplacent cellspacing et cellpadding

'border-style'
'caption-side' top | bottom | inherit
*/


Style* CssStyles::create_body_style() {
  //Style* style = UVbox::createStyle();
  Style* style = Box::createStyle();
  style->setVertPadding(8,8);
  Font* f = new Font(); f->setPointSize(12); style->setFont(f);
  return style;
}

Style* CssStyles::create_div_style() {
  Style* style = UFlowbox::createStyle();
  style->local.padding.set(6,1);
  return style;
}

Style* CssStyles::create_p_style() {
  Style* style = UFlowbox::createStyle();
  style->local.padding.set(6,1);
  return style;
}


Style* CssStyles::create_ul_style() {
  Style* style = UFlowbox::createStyle();
  style->setHorizPadding(30, 0);
  style->setVertPadding(1, 0);
  return style;
}

Style* CssStyles::create_ol_style() {
  Style* style = UFlowbox::createStyle();
  style->setHorizPadding(30, 1);
  style->setVertPadding(1, 0);
  return style;
}

Style* CssStyles::create_li_style() {
  Style* style = UFlowbox::createStyle();
  style->local.padding.set(1, 1);
  //style->local.content = new Element(UPix::rball);
  style->local.content = new Element(" - ");
  return style;
}


Style* CssStyles::create_pre_style() {
  Style* style = UFlowbox::createStyle();
  style->setHorizPadding(80,0);
  style->setVertPadding(5,5);
  style->setFont(&Font::monospace);
  return style;
}

Style* CssStyles::create_blockquote_style() {
  Style* style = UFlowbox::createStyle();
  style->local.padding.set(20, 8);
  return style;
}

Style* CssStyles::create_center_style() {
  Style* style = UFlowbox::createStyle();
  style->local.padding.set(1, 1);
  style->halign = Valign::CENTER;
  return style;
}


Style* CssStyles::create_h1_style() {
  Style* style = UFlowbox::createStyle();
  style->setVertPadding(14,14);
  Font* f = new Font(Font::bold); f->setPointSize(32); style->setFont(f);
  return style;
}

Style* CssStyles::create_h2_style() {
  Style* style = UFlowbox::createStyle();
  style->setVertPadding(12,12);
  Font* f = new Font(Font::bold); f->setPointSize(18); style->setFont(f);
  return style;
}

Style* CssStyles::create_h3_style() {
  Style* style = UFlowbox::createStyle();
  style->setVertPadding(10,10);
  Font* f = new Font(Font::bold); f->setPointSize(14); style->setFont(f);
  return style;
}

Style* CssStyles::create_h4_style() {
  Style* style = UFlowbox::createStyle();
  style->setVertPadding(8,8);
  Font* f = new Font(Font::bold); f->setPointSize(12); style->setFont(f);
  return style;
}

Style* CssStyles::create_h5_style() {
  Style* style = UFlowbox::createStyle();
  style->setVertPadding(7,7);
  Font* f = new Font(Font::bold); f->setPointSize(10); style->setFont(f);
  return style;
}

Style* CssStyles::create_h6_style() {
  Style* style = UFlowbox::createStyle();
  style->setVertPadding(6,6);
  Font* f = new Font(Font::bold); f->setPointSize(8); style->setFont(f);
  return style;
}


Style* CssStyles::create_table_style() {
  Style* style = Table::createStyle();
  style->valign = Valign::BOTTOM;
  style->halign = Halign::LEFT;
  return style;
}

Style* CssStyles::create_tr_style() {
  Style* style = UTrow::createStyle();
  style->hspacing = 0;
  style->valign = Valign::BOTTOM;
  style->halign = Halign::LEFT;
  return style;
}

Style* CssStyles::create_td_style() {
  Style* style = UTcell::createStyle();
  style->valign = Valign::BOTTOM;
  style->halign = Halign::LEFT;
  return style;
}

Style* CssStyles::create_th_style() {
  Style* style = UTcell::createStyle();
  return style;
}


Style* CssStyles::create_span_style() {
  Style* style = Element::createStyle();
  return style;
}

Style* CssStyles::create_b_style() {
  Style* style = Element::createStyle();
  style->setFont(&Font::bold);
  return style;
}

Style* CssStyles::create_i_style() {
  Style* style = Element::createStyle();
  style->setFont(&Font::italic);
  return style;
}

Style* CssStyles::create_em_style() {
  Style* style = Element::createStyle();
  //style->font = &Font::fill;
  style->setColors(Color::orange, Color::white);
  return style;
}

Style* CssStyles::create_u_style() {
  Style* style = Element::createStyle();
  style->setFont(&Font::underline);
  return style;
}

Style* CssStyles::create_font_style() {
  Style* style = Element::createStyle();
  return style;
}

Style* CssStyles::create_img_style() {
  Style* style = Box::createStyle();
  return style;
}

Style* CssStyles::create_br_style() {
  Style* style = Element::createStyle();
  return style;
}

Style* CssStyles::create_a_style() {                  // A REVOIR  !!!@@@@
  Style* href_style = LinkButton::createStyle();
  href_style->setBgcolors(Color::none);
  return href_style;
}

}
