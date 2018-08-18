/*
 *  html.cpp: HTML parser and renderer
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2008 | Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
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

#include <iostream>
#include <ubit/ubit_features.h>
#include <ubit/ubit.hpp>
#include <ubit/core/dom.h>
#include <ubit/core/html.h>
#include <ubit/ucss.hpp>
#include <ubit/core/appimpl.h>
using namespace std;
namespace ubit {



HtmlParser::HtmlParser() {
  static const XmlGrammar& html_grammar = *new HtmlGrammar();
  addGrammar(html_grammar);
  setPermissive(true);
  setCollapseSpaces(true);
}

void HtmlAttribute::setValueImpl(unique_ptr<String>& pvalue, const String& s) {
  if (pvalue) *pvalue = s; else pvalue = new String(s);
  pvalue->trim();  // !! attention: ce  n'est pas la norme XML !!
}

bool HtmlAttribute::getValueImpl(const unique_ptr<String>& pvalue, String& s) {
  if (pvalue) {s = *pvalue; return true;}
  else {s.clear(); return false;}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// ELEMENTS

#define UBIT_CLASS_DEF(CLASSNAME, CLASS, NEWCLASS, NEWSTYLE, MODES) \
struct Subclass : public Class { \
Subclass() : Class(CLASSNAME) {} \
virtual int getParseModes() const {return MODES;} \
virtual bool isInstance(UObject& obj) const {return dynamic_cast<CLASS*>(&obj);} \
virtual CLASS* newInstance() const {return NEWCLASS;} \
virtual Style* newStyle() const {return NEWSTYLE;} \
};\
static  const Class& Class() {static Subclass& c = *new Subclass; return c;} \
virtual const Class& getClass() const {return Class();}


#define HEAD_ELEMENT_CLASS(CL, SUPER, MODES) \
struct UHtml_##CL : public SUPER { \
  UBIT_CLASS_DEF(#CL, UHtml_##CL, new UHtml_##CL, UHtml_##CL::createStyle(), MODES) \
  UHtml_##CL() {show(false);} \
  virtual void initNode(Document*); \
};

#define ELEMENT_CLASS(CL, SUPER, MODES) \
struct UHtml_##CL : public SUPER { \
  UBIT_CLASS_DEF(#CL, UHtml_##CL, new UHtml_##CL, CssStyles::create_##CL##_style(), MODES) \
};

//NB: UHtml_html::createStyle, pas null sinon le layout sera incorrect
struct UHtml_html : public Element {
  UCLASSDEF("html", UHtml_html, new UHtml_html)
};

//NB: styles desfinis dans CssStyles
HEAD_ELEMENT_CLASS(head, Element, 0)
HEAD_ELEMENT_CLASS(title, Element, 0)
HEAD_ELEMENT_CLASS(meta, Element, Class::EMPTY_ELEMENT)
HEAD_ELEMENT_CLASS(link, Element, Class::EMPTY_ELEMENT)
HEAD_ELEMENT_CLASS(script, Element, Class::DONT_PARSE_CONTENT)
HEAD_ELEMENT_CLASS(style, Element, Class::DONT_PARSE_CONTENT)

ELEMENT_CLASS(body, UFlowbox, 0)
ELEMENT_CLASS(div, UFlowbox, 0)
ELEMENT_CLASS(p, UFlowbox, 0)
ELEMENT_CLASS(ul, UFlowbox, 0)
ELEMENT_CLASS(ol, UFlowbox, 0)
ELEMENT_CLASS(li, UFlowbox, 0)
ELEMENT_CLASS(pre, UFlowbox, Class::PRESERVE_SPACES)
ELEMENT_CLASS(blockquote, UFlowbox, 0)
ELEMENT_CLASS(center, UFlowbox, 0)
ELEMENT_CLASS(h1,UFlowbox, 0)
ELEMENT_CLASS(h2,UFlowbox, 0)
ELEMENT_CLASS(h3,UFlowbox, 0)
ELEMENT_CLASS(h4,UFlowbox, 0)
ELEMENT_CLASS(h5,UFlowbox, 0)
ELEMENT_CLASS(h6,UFlowbox, 0)
ELEMENT_CLASS(table, Table, 0)
ELEMENT_CLASS(tr, UTrow, 0)
ELEMENT_CLASS(td, UTcell, 0)
ELEMENT_CLASS(th, UTcell, 0)
ELEMENT_CLASS(span, Element, 0)
ELEMENT_CLASS(font, Element, 0)
ELEMENT_CLASS(b, Element, 0)
ELEMENT_CLASS(i, Element, 0)
ELEMENT_CLASS(em, Element, 0)
ELEMENT_CLASS(u, Element, 0)


#define ELEMENT_INIT_CLASS(CL, SUPER, MODES) \
struct UHtml_##CL : public SUPER { \
UBIT_CLASS_DEF(#CL, UHtml_##CL, new UHtml_##CL, CssStyles::create_##CL##_style(), MODES) \
virtual void initNode(Document*); \
};
  
ELEMENT_INIT_CLASS(img, Box, Class::EMPTY_ELEMENT)
ELEMENT_INIT_CLASS(br, Element, Class::EMPTY_ELEMENT)

struct UHtml_a : public LinkButton {
  UBIT_CLASS_DEF("a", UHtml_a, new UHtml_a, CssStyles::create_a_style(), 0) 
  virtual void initNode(Document*);
  //virtual const String* getHRef() const; ihnerited from LinkButton.
};
    

void UHtml_head::initNode(Document* doc) {
  Element::initNode(doc);
}

void UHtml_title::initNode(Document* doc) {   // set title....
  Element::initNode(doc);
}

void UHtml_meta::initNode(Document* doc) {    // set meta....
  Element::initNode(doc);
}

void UHtml_script::initNode(Document* doc) {    // load script....
  Element::initNode(doc);
}

void UHtml_br::initNode(Document* doc) {
  Element::initNode(doc);
  add(ustr("\n"));
}

// reload reste a faire (complique: il faut mettre a jour l'arbre)

void UHtml_link::initNode(Document* doc) {
  Element::initNode(doc);
  XmlDocument* xmldoc = dynamic_cast<XmlDocument*>(doc);
  if (xmldoc) {
    String href, rel, type;
    if (getAttrValue(href,"href") && !href.empty()
        && ((getAttrValue(rel,"rel") && rel.equals("stylesheet",true))
            || (getAttrValue(type,"type") && type.equals("text/css",true)))
        ) {
      String path; doc->makePath(path, href);
      CssAttachment* a = new CssAttachment(href, getNodeName(), xmldoc);
      xmldoc->addAttachment(a);
      a->load(xmldoc);
    }
  }
}


void UHtml_style::initNode(Document* doc) {
  Element::initNode(doc);
  XmlDocument* xmldoc = dynamic_cast<XmlDocument*>(doc);
  if (xmldoc) {
    ChildIter i = cbegin();
    if (i != cend()) {
      CssParser css;
      String* text = dynamic_cast<String*>(*i);    // UXmlText
      const String* val = text ? &text->getData() : null;
      if (val) css.parse(*val, xmldoc);
    }
  }
}


void UHtml_a::initNode(Document* doc) {
  Element::initNode(doc);
  
  /*
   const String* hname = getAttr("hname");
   if (hname && !hname->empty()) {
   // rajouter dans la table des links .......
   }
   */
  
  String href;
  if (getAttrValue(href,"href") && !href.empty()) {
    UCall& c = ucall(doc, const_cast<const String*>(new String(href)), &Document::linkEventCB);
    addAttr(UOn::enter / c);
    addAttr(UOn::leave / c);
    addAttr(UOn::mpress / c);
    addAttr(UOn::mrelease / c);
    addAttr(UOn::action / c);
  }
}


void UHtml_img::initNode(Document* doc) {
  Element::initNode(doc);
  
  // doit ignorer les events sinon les liens contenant des images
  // marcheront pas
  ignoreEvents();
  
  XmlDocument* xmldoc = dynamic_cast<XmlDocument*>(doc);
  if (xmldoc) {
    String src;
    if (getAttrValue(src,"src") && !src.empty()) {
      String path; xmldoc->makePath(path, src);
      Image* ima = new Image(path);   // att pas compat avec reload ima redef!
      add(ima);
      //                      CF AUSSI: tailles predefinies et rescale !!!
      
      xmldoc->addAttachment(new ImageAttachment(src, getNodeName(), ima));
    }
  }
}

// ATTRIBUTES

void HtmlStyle::initNode(Document* doc, Element*) {
  XmlDocument* xmldoc = dynamic_cast<XmlDocument*>(doc);
  if (xmldoc && pvalue && !pvalue->empty()) {
    CssParser css;
    css.parseAttr(*pvalue, xmldoc, this);
  }
}

void HtmlClass::initNode(Document* doc, Element* parent) {
  //pourrait tester s'il y a au moins un attribut de ce nom dans la base
  //pourrait etre generalise a tous les attributs
  XmlDocument* xmldoc = dynamic_cast<XmlDocument*>(doc);
  String val;
  if (xmldoc && getValue(val)) xmldoc->setClassStyle(parent, getName(), val);
}

void HtmlId::initNode(Document* doc, Element* parent) {
  XmlDocument* xmldoc = dynamic_cast<XmlDocument*>(doc);
  String val;
  if (xmldoc && getValue(val)) xmldoc->setIdStyle(parent, getName(), val);
}

void HtmlColor::setValue(const String& v) {
  HtmlAttribute::setValueImpl(pvalue, v);     // ! fait trim() !
  setNamedColor(*pvalue);
}

void HtmlBgcolor::setValue(const String& v) {
  HtmlAttribute::setValueImpl(pvalue, v);  // fait trim() !
  setNamedColor(*pvalue);
}

void HtmlBorder::setValue(const String& v) {
  HtmlAttribute::setValueImpl(pvalue, v);     // ! fait trim() !
  String unit;
  float n = 0;
  if (!pvalue->scanValue(n, unit)) return;
  if (n > 1) setDecoration(Border::LINE);   // A COMPLETER
  else setDecoration(Border::NONE);
}

void HtmlFontFace::setValue(const String& v) {
  HtmlAttribute::setValueImpl(pvalue, v);     // ! fait trim() !
  if (pvalue) setFamily(*pvalue);
}

void HtmlFontSize::setValue(const String& v) {
  HtmlAttribute::setValueImpl(pvalue, v);     // ! fait trim() !
  String unit; 
  float n = 0;
  if (!pvalue->scanValue(n, unit)) return;
  
  //cerr << "SIZE: " << n << " [" << unit << "]" << endl;
  // faudrait gerer taille relatives !!!
  switch(int(n)) {
    case 1 : setPixelSize(8); break;    // devrait etre parametrable
    case 2 : setPixelSize(10); break;
    case 3 : setPixelSize(12); break;
    case 4 : setPixelSize(14); break;
    case 5 : setPixelSize(16); break;
    case 6 : setPixelSize(20); break;
    case 7 : setPixelSize(24); break;
    case 8 : setPixelSize(28); break;
    case 9 : setPixelSize(34); break;
    default: setPixelSize(48); break;
  }
}

/*
void HtmlWidth::setValue(const String& s) {
  if (!pvalue) pvalue = new String();
  *pvalue = s;
  UWidth::set(s);
}
void HtmlHeight::setValue(const String& s) {
  if (!pvalue) pvalue = new String();
  *pvalue = s;
  UHeight::set(s);
}
*/

void HtmlWidth::initNode(Document*, Element* parent) {
  // NB: obtainAttr() add attribute to props
  if (pvalue && !pvalue->empty()) parent->obtainAttr<USize>().setWidth(*pvalue);
}

void HtmlHeight::initNode(Document*, Element* parent) {
  // NB: obtainAttr() add attribute to props
  if (pvalue && !pvalue->empty()) parent->obtainAttr<USize>().setHeight(*pvalue);
}

void HtmlAlign::setValue(const String& v) {
    HtmlAttribute::setValueImpl(pvalue, v);     // ! fait trim() !
    if (pvalue->equals("left",true)) *(Halign*)this = Halign::left;
    else if (pvalue->equals("center",true)) *(Halign*)this = Halign::center;
    else if (pvalue->equals("right",true)) *(Halign*)this = Halign::right;
  }

void HtmlValign::setValue(const String& v) {
  HtmlAttribute::setValueImpl(pvalue, v);     // ! fait trim() !
  if (pvalue->equals("top",true)) *(Valign*)this = Valign::top;
  else if (pvalue->equals("middle",true)) *(Valign*)this = Valign::center;
  else if (pvalue->equals("bottom",true)) *(Valign*)this = Valign::bottom;
}

void HtmlColspan::initNode(Document* doc, Element* parent) {
  UTcell* cell;
  if (parent && pvalue && (cell = dynamic_cast<UTcell*>(parent))) {
    int n = pvalue->toInt();
    if (n > 0) cell->setColspan(n);
  }
}
  
void HtmlColspan::setValue(const String& v) {
  HtmlAttribute::setValueImpl(pvalue, v);     // ! fait trim() !
  String unit;
  float n = 0;
  if (!pvalue->scanValue(n, unit)) return;
  //if (unit.empty()) colspan = int(n);
}

void HtmlRowspan::initNode(Document* doc, Element* parent) {
  UTcell* cell;
  if (parent && pvalue && (cell = dynamic_cast<UTcell*>(parent))) {
    int n = pvalue->toInt();
    if (n > 0) cell->setRowspan(n);
  }
}
  
void HtmlRowspan::setValue(const String& v) {
  HtmlAttribute::setValueImpl(pvalue, v);     // ! fait trim() !
  String unit;
  float n = 0;
  if (!pvalue->scanValue(n, unit)) return;
  //if (unit.empty()) rowspan = int(n);
}

void HtmlCellspacing::setValue(const String& v) {
  HtmlAttribute::setValueImpl(pvalue, v);     // ! fait trim() !
  String unit;
  float n = 0;
  if (!pvalue->scanValue(n, unit)) return;
  //if (unit.empty() || UCSS::isEq(unit,"px")) set(i);   // A FAIRE
}

void HtmlCellpadding::setValue(const String& v) {
  HtmlAttribute::setValueImpl(pvalue, v);     // ! fait trim() !
  String unit;
  float n = 0;
  if (!pvalue->scanValue(n, unit)) return;
  //if (unit.empty() || UCSS::isEq(unit,"px")) set(i);   // A FAIRE
}

void HtmlNowrap::setValue(const String& v) {
  HtmlAttribute::setValueImpl(pvalue, v);     // ! fait trim() !
  // pas de valeur ..
  // A FAIRE
}


HtmlGrammar::HtmlGrammar() 
{  
  addElementClass(UHtml_html::Class());
  addElementClass(UHtml_head::Class());
  addElementClass(UHtml_title::Class());
  addElementClass(UHtml_meta::Class());
  addElementClass(UHtml_link::Class());
  addElementClass(UHtml_script::Class());
  addElementClass(UHtml_style::Class());

  addElementClass(UHtml_body::Class());
  addElementClass(UHtml_div::Class());
  addElementClass(UHtml_p::Class());
  addElementClass(UHtml_ul::Class());
  addElementClass(UHtml_ol::Class());
  addElementClass(UHtml_li::Class());
  addElementClass(UHtml_pre::Class());  
  addElementClass(UHtml_blockquote::Class());
  addElementClass(UHtml_center::Class());
  addElementClass(UHtml_h1::Class());
  addElementClass(UHtml_h2::Class());
  addElementClass(UHtml_h3::Class());
  addElementClass(UHtml_h4::Class());
  addElementClass(UHtml_h5::Class());
  addElementClass(UHtml_h6::Class());
  addElementClass(UHtml_table::Class());
  addElementClass(UHtml_tr::Class());
  addElementClass(UHtml_td::Class());
  addElementClass(UHtml_th::Class());
  addElementClass(UHtml_span::Class());
  addElementClass(UHtml_font::Class());
  addElementClass(UHtml_em::Class());
  addElementClass(UHtml_b::Class());
  addElementClass(UHtml_i::Class());
  addElementClass(UHtml_u::Class());
  addElementClass(UHtml_a::Class());
  addElementClass(UHtml_img::Class()); 
  addElementClass(UHtml_br::Class()); 
 
  //  - - - - - - - - - -- - - - - - - - - - - - - - -
    
  addAttrClass(HtmlStyle::Class());
  addAttrClass(HtmlClass::Class());
  addAttrClass(HtmlId::Class());
  addAttrClass(HtmlRel::Class());
  addAttrClass(HtmlType::Class());
  addAttrClass(HtmlAlt::Class());
  addAttrClass(HtmlName::Class());
  addAttrClass(HtmlSrc::Class());
  addAttrClass(HtmlHref::Class());
  addAttrClass(HtmlColor::Class());
  addAttrClass(HtmlBgcolor::Class());
  addAttrClass(HtmlFontFace::Class());
  addAttrClass(HtmlFontSize::Class());
  addAttrClass(HtmlWidth::Class());
  addAttrClass(HtmlHeight::Class());
  addAttrClass(HtmlAlign::Class());
  addAttrClass(HtmlValign::Class());
  addAttrClass(HtmlBorder::Class());
  addAttrClass(HtmlColspan::Class());
  addAttrClass(HtmlRowspan::Class());
  addAttrClass(HtmlCellspacing::Class());
  addAttrClass(HtmlCellpadding::Class());
  addAttrClass(HtmlNowrap::Class());
}

}
