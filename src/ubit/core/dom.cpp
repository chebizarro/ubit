/*
 *  dom.cpp: XML DOM Nodes
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

#include <typeinfo>
#include <iostream>
#include <cstdio>
#include <ubit/ubit_features.h>
#include <ubit/ubit.hpp>
#include <ubit/core/dom.h>
#include <ubit/core/xmlgrammar.h>
#include <ubit/core/xmlparser.h>
#include <ubit/uhtml.hpp>
#include <ubit/ucss.hpp>   // pour CssAttachment
using namespace std;
namespace ubit {


const String& Comment::getNodeName() const 
{static String nn("#comment"); return nn;}

const String& CDATASection::getNodeName() const 
{static String nn("#cdata-section"); return nn;}

const String& XmlDocument::getNodeName() const 
{static String nn("#document"); return nn;}

// NB: theoriquement ca devrait pas etre la mais ce n'est pas tres grave
bool ImageAttachment::isLoaded() const {
  return ima->isLoaded();
}

int ImageAttachment::load(Document*) {
  return ima->loadNow();  // always reloads the file
}

int CssAttachment::load(Document*) {
  if (!doc || url.empty()) return 0;
  String path; 
  doc->makePath(path, url);
  String style;
  stat = style.read(path);
  //cerr << "link stylesheet: " << stat << " pathname " << " " << path << endl;
  if (stat > 0) {
    CssParser css;
    css.parse(style, doc);
  }
  return stat;
}


//XmlDocType::XmlDocType()
//: name(null), public_id(null), system_id(null) {}

XmlDocType::XmlDocType(const String& _name, const String& _public_id,
                         const String& _system_id)
: name(ustr(_name)), public_id(ustr(_public_id)), system_id(ustr(_system_id)) {
}

XmlDocType::~XmlDocType() {}


XmlDocument::XmlDocument() : Document() {
  constructs();
}

XmlDocument::XmlDocument(const String& _pathname) : Document(_pathname) {
  constructs();
}

void XmlDocument::constructs() {
  doc_type = null;
  grammars = new XmlGrammars;
  // c'est la grammaire par defaut pour les elements inconnus
  // elle est partagee, ce qui peut etre genant si sa taille devient grande
  // mais ceci permet que les UXmlElements pointent toujours sur des
  // classes qui continuent d'exister meme si les XmlDox correspondants
  // on ete detruits
  // ce mecanisme sera peut-etre a revoir ulterieurement....
  addGrammar(XmlGrammar::getSharedUndefGrammar());
  
  // doit etre fait apres addGrammar() sinon ca plantera
  doc_elem = createElement("DocumentElement");  
  add(doc_elem);
  xml_version = new String();
  xml_encoding = new String("1.0");
  xml_standalone = true;
}

XmlDocument::~XmlDocument() {
  delete doc_elem;  // faudrait un unique_ptr
  
  // NB: detruit le handle, pas les grammaires (pour des raisons de perfs
  // et car sinon les XmlNodes qui pointent dessus pourrainet planter
  delete grammars;

  //link_listener: externe: ne pas detruire
}



void XmlDocument::addGrammar(const XmlGrammar& g) {
  grammars->addGrammar(g);
}


XmlCreator::XmlCreator() {
  load_objects = true;
  keep_comments = false;
  permissive = false;
  collapse_spaces = false;
}

HtmlCreator::HtmlCreator() {
  permissive = true;
  collapse_spaces = true;
}

Document* XmlCreator::create(DocumentSource& so) {
  XmlDocument* doc = null;

  //XmlParser parser;   // ATT !!!
  HtmlParser parser; 
  parser.setPermissive(permissive);
  parser.setCollapseSpaces(collapse_spaces);
  
  if (so.from == DocumentSource::PATHNAME) {
    doc = parser.read(*so.fullpath/*, so.errors*/);
    so.stat = parser.getStatus();
  }
  else if (so.from == DocumentSource::BUFFER) {
    doc = parser.parse(*so.path, so.buffer ? *so.buffer : ""/*, so.errors*/);
    so.stat = parser.getStatus();
  }
  else if (so.from == DocumentSource::SCRATCH) {
    doc = new XmlDocument(*so.path);
    so.stat = 1;
  }
  else {
    so.stat = 0;
    return null;
  }
  
  return doc;
}


String* XmlDocument::createTextNode(const String& _data) {
  return new String(_data);
}

Comment* XmlDocument::createComment(const String& _data) {
  return new Comment(_data); 
}

Comment::Comment(const String& _data) 
: data(new String(_data)) {}


CDATASection* XmlDocument::createCDATASection(const String& _data) {
  return new CDATASection(_data); 
}

CDATASection::CDATASection(const String& _data) 
: data(new String(_data)) {}

// ==================================================== ======== 

ProcessingInstruction*
XmlDocument::createProcessingInstruction(const String& _target, const String& _data) {
  return new ProcessingInstruction(_target, _data); 
}

//ProcessingInstruction::ProcessingInstruction()
//: target(null), data(null) {}

ProcessingInstruction::ProcessingInstruction(const String& _target, 
                                               const String& _data)
: target(ustr(_target)), data(ustr(_data)) {}

// ==================================================== ======== 

Attribute* XmlDocument::createAttribute(const String& name) {
  // search if UAttributeClass in grammar
  const Class* c = grammars->getAttrClass(name);
  
  // create+add default AttrClass otherwise:
  if (!c) c = XmlGrammar::addUndefAttrClass(name); 
  
  // @@@ cast necessaire a cause pbm de refs croisees dans uclass.hpp
  return c->newInstance()->toAttr();
}

// ==================================================== ======== 

Element* XmlDocument::createElement(const String& name) {
  // search if UElemClass in grammar
  const Class* c = grammars->getElementClass(name);
  
  // create+add default ElemClass otherwise:
  if (!c) c = XmlGrammar::addUndefElementClass(name);
  
  // @@@ cast necessaire a cause pbm de refs croisees dans uclass.hpp
  return c->newInstance()->toElem();
}

void XmlDocument::initElement(Element* e) {
  const String& nname = e->getNodeName();
  //if (nname) {
    const Class* c = getStyleSheet().findClass(nname);
    //il faut le mettre en premier!!!
    // cette fonctionnalite n'est pas standard !!!
    if (c) e->addImpl1(c->getAttributes(), e->abegin(), e->attributes());  // reafficher ????
  //}
  e->initNode(this);
}

// probleme: faire en sorte que les proplists soient dans le bon ordre et
// qu'elles soient avant les proplist des styles attributes qui doivent 
// etre plus loin dans la liste pour l'emporter

// NB: ca ne serait pas mal de concatener pour accelerer le rendu

static void _addProp(Element* e, Attribute* prop) {
  ChildIter i = e->abegin();
  for ( ; i != e->aend(); ++i) {
    // !! vraiment un hack !!
    //if (dynamic_cast<UProps*>(*i) && !dynamic_cast<UXmlAttr*>(*i)) k++;
    if (dynamic_cast<AttributeList*>(*i) && (*i)->getNodeName().compare("style")!=0)
      ;
    else break;
  }
  
  // cette fonctionalite n'est pas standard !!!
  //e->addImpl(e->attributes(), prop, i, true, null);
  e->addImpl1(prop, i, e->attributes());   // reafficher ????
}

// ==================================================== ======== 

void XmlDocument::setClassStyle(Element* e, const String& name, const String& value) {
  setClassIdStyle(e, name, value);
}

void XmlDocument::setIdStyle(Element* e, const String& name, const String& value) {
  setClassIdStyle(e, name, value);
}

void XmlDocument::setClassIdStyle(Element* e, const String& att_name, const String& att_value) 
{
  if (att_name.empty() /*|| !att_value*/) return;
  char fullname[1000];  // ATT MAX LEN !!
  const Class* id = null;
  
  // NB: generalisable a tous les attributs
  
  if (att_value.empty())
    sprintf(fullname, "[%s]", att_name.c_str());
  else
    sprintf(fullname, "[%s=%s]", att_name.c_str(), att_value.c_str());
  
  if ((id = getStyleSheet().findClass(fullname)))
    _addProp(e, id->getAttributes());
  //cerr <<"setClassIdStyle1 "<<fullname << " " << id
  //  <<" : "<< *att_name <<"="<< *att_value <<endl;
  
  if (att_value.empty())
    sprintf(fullname, "%s[%s]", e->getNodeName().c_str(), att_name.c_str());
  else
    sprintf(fullname, "%s[%s=%s]", e->getNodeName().c_str(), 
            att_name.c_str(), att_value.c_str());
  
  if ((id = getStyleSheet().findClass(fullname)))
    _addProp(e, id->getAttributes());
  
  //cerr <<"setClassIdStyle2 "<<fullname << " " << id
  //  <<" : "<< *att_name <<"="<< *att_value <<endl;
}


const DocumentAttachments* XmlDocument::getAttachments() const {
  return &attachments;
}

void XmlDocument::addAttachment(DocumentAttachment* a) {
  if (a) attachments.push_back(a);
}

bool XmlDocument::loadAttachment(DocumentAttachment* a, bool reload) {
  if (reload || !a->isLoaded()) return (a->load(this) > 0);
  else return true;
}

int XmlDocument::loadAttachments(bool reload) {
  int count = 0;
  for (DocumentAttachments::iterator k = attachments.begin(); 
       k != attachments.end();
       k++) {
    if (*k && (reload || !(*k)->isLoaded())) {
      if ((*k)->load(this) >= 0) count++;
    }
  }
  return count;
}


static void _print(ostream* out, String* buf, Node* node) {
  if (!node) return;
  Element* e = null;
  String* text = null;

  if ((e = dynamic_cast<Element*>(node))) {
    // print begin tag (and node name)
    if (out) *out << "<" << e->getNodeName();
    if (buf) *buf &= "<" & e->getNodeName();

    // print attributes (if any)
    for (ChildIter ia = e->abegin(); ia != e->aend(); ++ia) {
      Attribute* a = dynamic_cast<Attribute*>(*ia);
      if (a /* && a->getValue()*/) {    // !! hack a revoir !!
        String val;
        a->getValue(val);
        if (out) *out << " " << a->getName() << "=\"" << val << "\"";
        if (buf) *buf &= " " & a->getName() & "=\"" & val & "\"";
      }
    }
    if (out) *out << ">" << endl;
    if (buf) *buf &= ">\n";

    // print child nodes
    for (ChildIter ic = e->cbegin(); ic != e->cend(); ++ic)
      _print(out, buf, *ic);

    // print end tag
    if (out) *out << "</" << e->getNodeName() << ">" << endl;
    if (buf) *buf &= "</" & e->getNodeName() & ">\n";
  }

  // c'est faux pour les subclasses de String !!!!!!!!
  else if ((text = dynamic_cast<String*>(node)) /*&& text->getData()*/) {
    if (out) *out << text->getData();
    if (buf) *buf &= text->getData();
  }
}


void XmlDocument::print(std::ostream& fout) {
  Element* e = getDocumentElement();
  if (e) {
    _print(&fout, null, e);
    fout << endl << endl;
  }
}

void XmlDocument::print(String& buf) {
  Element* e = getDocumentElement();
  if (e) {
    _print(null, &buf, e);
    buf &= "\n";
  }
}

}

