/*
 *  doc.cpp: Document Element
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
#include <ubit/ufile.hpp>
#include <ubit/udoc.hpp>
#include <ubit/udom.hpp>
using namespace std;
namespace ubit {


DocumentFactory& DocumentFactory::getDefaultFactory() {
  static DocumentFactory* fact = new DocumentFactory();
  return *fact;
}

DocumentFactory& Document::getDefaultFactory() {
  return DocumentFactory::getDefaultFactory();
}

Document::Document() : ppath(new String("")) {}
Document::Document(const String& _path) : ppath(new String(_path)) {}
Document::~Document() {}


bool Document::isEditable() const {return false;}
void Document::setEditable(bool state) {}

void Document::setPath(const String& _path) {
  if (!ppath) ppath = ustr();
  *ppath = _path;
}

void Document::makePath(String& path, const String& name) {
  path = name;
  path.trim();
  int pos;

  // http url:  not a local pathname
  if (path.find("http:") == 0) return;

  pos = path.find("https:");
  if (pos == 0) return;

  pos = path.find("ftp:");
  if (pos == 0) return;
  
  pos = path.find("ssh:");
  if (pos == 0) return;

  pos = path.find("file:");
  if (pos == 0) path.remove(0, 5); // remove "file:"

  if (path.at(0) != '/')  {
    String dirname = getPath().dirname();
    path.insert(0, dirname);
  }
}


void Document::addLinkCallback(const Child& cond_slash_callback) {
  if (!call_list) call_list = new Element();
  call_list->add(cond_slash_callback);
}

void Document::linkEventCB(UInputEvent& e, const String* path) {
  if (call_list) {
    //const UOn* on = dynamic_cast<const UOn*>(&e.getCond());  // ???
    //if (on) {
    //e.setTarget((Node*)path);   ??? sert a qq chose ??? @@@!!!
    //e.setCond(*on);
    call_list->fire(e);
    //}
  }
}


FlatDocument::FlatDocument(const String& _pathname) 
: Document(_pathname), edit(null) {
}

bool FlatDocument::isEditable() const {
  return edit ? edit->isEditable() : false;
}

void FlatDocument::setEditable(bool state) {
  if (edit) edit->setEditable(state);
}


class TextDocCreator : public DocumentCreator {
public:
  virtual Document* create(DocumentSource&);
};


Document* TextDocCreator::create(DocumentSource& so) {
  String* str = null;

  if (so.from == DocumentSource::PATHNAME) {
    str = new String();
    if ((so.stat = str->read(*so.fullpath)) <= 0) {
      delete str;
      return null;
    }
  }
  else if (so.from == DocumentSource::BUFFER) {
    str = new String(so.buffer ? *so.buffer : "");
    so.stat = 1;
  }
  else if (so.from == DocumentSource::SCRATCH) {
    str = new String();
    so.stat = 1;
  }
  else {
    so.stat = 0;
    return null;
  }
  
  Box& fbox = uflowbox(upadding(4,6) + *str);
  TextEdit& ed = uedit();
  fbox.addAttr(ed);      // editable

  FlatDocument* doc = new FlatDocument(*so.path);
  doc->add(fbox);
  doc->edit = &ed;
  return doc;
}

/* ==================================================== [Elc] ======= */

class ImaDocCreator : public DocumentCreator {
public:
  virtual Document* create(DocumentSource&);
};


Document* ImaDocCreator::create(DocumentSource& so) {
  Image* ima = null;

  if (so.from == DocumentSource::PATHNAME) {
    ima = new Image();
    if ((so.stat = ima->read(*so.fullpath)) <= 0) {
      delete ima; 
      return null;
    }
  }
  else if (so.from == DocumentSource::BUFFER) {
    so.stat = 0;
    return null;  // not implemented!
  }
  else if (so.from == DocumentSource::BUFFER) {
    ima = new Image();
    so.stat = 1;
  }
  else {
    so.stat = 0;
    return null;
  }
  
  Document* doc = new FlatDocument(*so.path);
  doc->addAttr(uleft());  // do not center images!
  doc->add(ima);
  return doc;
}

/* ==================================================== [Elc] ======= */

PluginDocument::PluginDocument(const String& _pathname) 
: Document(_pathname) {
}

class UPluginDocCreator : public DocumentCreator {
public:
  UPluginDocCreator(const String& doctype, const String& command);

  virtual Document* create(DocumentSource&);
  virtual void exec(UInputEvent&, Document*);
  virtual void open(UInputEvent&, Document*);
  virtual void openWith(UInputEvent&, Document*);
  virtual void openAsText(UInputEvent&, Document*);
protected:
  String doctype, command, alt_command, status;
};

UPluginDocCreator::UPluginDocCreator(const String& _doctype, 
                                     const String& _command) :
  doctype(_doctype),
  command(_command) {
}


Document* UPluginDocCreator::create(DocumentSource& so) {
  so.stat = 0;
  if (so.from == DocumentSource::BUFFER) {
    return null;
  }

  String* fname = new String(so.path->basename());
  Document* doc = new PluginDocument(*so.path);

  doc->addAttr(utop() + upadding(13,13) + uvspacing(8) + Font::large);
  doc->add(uhbox(uelem(Font::bold + "File: ") + fname));

  UFileMode fm(*so.path);
  if (fm.isExec()) {
    doc->add(uhbox(uleft() + USymbol::right + " "
                   + ulinkbutton(Font::bold + "Execute"
                                 + ucall(this, doc, &UPluginDocCreator::exec))));
  }

  if (!command.empty()) {
    String& help_s = ustr();
    if (command !="open") help_s = " (using " & command & " )";
    
    doc->add(uhbox(uleft() + USymbol::right + " "
                   + ulinkbutton(Font::bold + "Open"
                                 + ucall(this, doc, &UPluginDocCreator::open))
                   + help_s));
  }

  UCall& call = ucall(this, doc, &UPluginDocCreator::openWith);
  doc->add(uhbox(uleft() + USymbol::right + " "
                 + ulinkbutton(Font::bold + "Open With" + call)
                 + "  "
                 + utextfield(usize(200) + alt_command + call)));

  doc->add(uhbox(uleft() + USymbol::right + " "
                 + ulinkbutton(Font::bold + "Open As Text"
                               + ucall(this, doc, &UPluginDocCreator::openAsText))));
  status = "";
  doc->add(" " + uhbox(Color::red + Font::bold + status));
  return doc;
}


void UPluginDocCreator::open(UInputEvent& e, Document* doc) {
  status = "";
  if (command.empty()) return;
  if (e.getDisp() != Application::getDisp()) {
    status = "Operation not allowed on clones";
    return;
  }
  String s = command & " \"" & doc->getPath() & "\"&";
  // OK sur clones si le filename n'est pas trop bizarre
  //if (s.find('`') >= 0 || s.find('|') >= 0 || s.find('*') >= 0 || s.find('?') >= 0) {
  //  status = "Invalid file name";
  //  return;
  //}
  int stat = system(s.c_str());              // !! system = DANGEREUX
  if (stat != 0) status = "Status: ", status &= stat; 
}


void UPluginDocCreator::openWith(UInputEvent& e, Document* doc) {
  status = "";
  if (alt_command.empty()) return;
  if (e.getDisp() != Application::getDisp()) {
    status = "Operation not allowed on clones";
    return;
  }
  String s = alt_command & " \"" & doc->getPath() & "\"&";
  int stat = system(s.c_str());
  if (stat != 0) status = "Status: ", status &= stat;
}


void UPluginDocCreator::exec(UInputEvent& e, Document* doc) {
  status = "";
  if (e.getDisp() != Application::getDisp()) {
    status = "Operation not allowed on clones";
    return;
  }

  String dir  = doc->getPath().dirname();
  String name = doc->getPath().basename();

  // faire cd avant l'exec pour mettre dans le bon contexte si lecture
  // d'arguments
  String s = "(cd \"" & dir & "\"; " & " \"" & name & "\")&";  
  int stat = system(s.c_str());
  if (stat != 0) status = "Status: ", status &= stat;
}


void UPluginDocCreator::openAsText(UInputEvent& e, Document* doc) {
  status = "";
  doc->removeAll();

  TextDocCreator creator;
  DocumentSource so(DocumentSource::PATHNAME, &doc->getPath(), null);
  
  Document* doc2 = creator.create(so);
  if (doc2) {
    doc->removeAll();
    doc->add(doc2);
  }
}

/* ==================================================== [Elc] ======= */

DocumentFactory::DocumentFactory() {
  stat = 0;
  errors = null;

#if (defined(__MACH__) && defined(__APPLE__))        //__POWERPC__
  default_creator = new UPluginDocCreator("","open");
#else
  default_creator = new UPluginDocCreator("","");  //empty = unknown command
#endif
  
  DocumentCreator& image_creator = *new ImaDocCreator();
  addCreator("xpm", image_creator);
  addCreator("jpg", image_creator);
  addCreator("jpeg",image_creator);
  addCreator("gif", image_creator);

  DocumentCreator& html_creator = *new HtmlCreator();
  addCreator("html", html_creator);
  addCreator("htm",  html_creator);

  DocumentCreator& xml_creator = *new XmlCreator();
  addCreator("xhtml",xml_creator);
  addCreator("xml",  xml_creator);
  
  DocumentCreator& text_creator = *new TextDocCreator();
  addCreator("txt", text_creator);
  addCreator("java",text_creator);
  addCreator("c",   text_creator);
  addCreator("h",   text_creator);
  addCreator("cpp", text_creator);
  addCreator("hpp", text_creator);
  addCreator("cc",  text_creator);
  addCreator("hh",  text_creator);
  addCreator("am", text_creator);
  addCreator("in", text_creator);

  DocumentCreator& pdf_creator =
#if (defined(__MACH__) && defined(__APPLE__))        //__POWERPC__
    *new UPluginDocCreator("PDF Document", "open");
#else
    *new UPluginDocCreator("PDF Document", "acroread");
#endif
  addCreator("pdf", pdf_creator);

  DocumentCreator& office_creator =
#if (defined(__MACH__) && defined(__APPLE__))        //__POWERPC__
    *new UPluginDocCreator("Office Document", "open");
#else
    *new UPluginDocCreator("Office Document", "soffice");
#endif
  addCreator("doc", office_creator);
  addCreator("ppt", office_creator);
  addCreator("xls", office_creator);
  addCreator("rtf", office_creator);
}

DocumentFactory::~DocumentFactory() {
  // detruire les Creators
}

int DocumentFactory::getStatus() const {
  return stat;
}

String* DocumentFactory::getErrors() const {
  return errors;
}

void DocumentFactory::saveErrors(bool mode) {
  if (mode) errors = new String();
  else errors = null;
}


DocumentSource::DocumentSource(int _from, const String* _path, String* _errors,
                       const String* _buffer)
: from(_from), stat(0), path(_path), buffer(_buffer), errors(_errors)
{
  // path is not duplicated
  // fullpath may be shared with path
  fullpath = UFileCache::getOrCreateFullPath(*_path);
}


Document* DocumentFactory::read(const String& pathname) {
  stat = 0;
  if (errors) errors->clear();

  String fext = pathname.suffix();
  DocumentCreator* cr = getCreator(fext);
  if (!cr) cr = getDefaultCreator();
  if (!cr) return null;

  DocumentSource so(DocumentSource::PATHNAME, &pathname, errors);
  Document* doc = cr->create(so);
  stat = so.stat;
  return doc;
}

Document* DocumentFactory::load(const String& name, const String& buffer) {
  stat = 0;
  if (errors) errors->clear();

  String fext = name.suffix();
  DocumentCreator* cr = getCreator(fext);
  if (!cr) cr = getDefaultCreator();
  if (!cr) return null;

  DocumentSource so(DocumentSource::BUFFER, &name, errors, &buffer);
  Document* doc = cr->create(so);
  stat = so.stat;
  return doc;
}

Document* DocumentFactory::create(const String& name) {
  stat = 0;
  if (errors) errors->clear();

  String fext = name.suffix();
  DocumentCreator* cr = getCreator(fext);
  if (!cr) cr = getDefaultCreator();
  if (!cr) return null;

  DocumentSource so(DocumentSource::SCRATCH, &name, errors);
  Document* doc = cr->create(so);
  stat = so.stat;
  return doc;
}


bool DocumentFactory::Comp::operator()(const String* s1, const String* s2) const {
  return s1->compare(*s2, true) < 0;   // ignore case
}

void DocumentFactory::addCreator(const String& suffix, DocumentCreator& creator) {
  // si existe deja ?
  rmap[&ustr(suffix)] = &creator;
}

void DocumentFactory::setDefaultCreator(DocumentCreator& creator) {
  default_creator = &creator;
}

DocumentCreator* DocumentFactory::getCreator(const String& fext) {
  if (fext.empty()) return null;

  Map::iterator k = rmap.find(&fext);
  if (k != rmap.end() && k->second) return k->second;

  return null;  // not found
}

DocumentCreator* DocumentFactory::getDefaultCreator() {
  return default_creator;
}

}

