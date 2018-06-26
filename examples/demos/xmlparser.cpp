/* ==================================================== ======== ======= *
 *
 *  xmlparser.cpp
 *  Ubit GUI Toolkit - Version 6
 *  (C) 2009 / Eric Lecolinet / Telecom ParisTech / http://www.enst.fr/~elc/ubit
 *
 * ***********************************************************************
 * COPYRIGHT NOTICE :
 * THIS PROGRAM IS DISTRIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE
 * IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION;
 * EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 * SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
 * ************************************************************************/

#include <fstream>
#include <iostream>
#include <ubit/ubit.hpp>
#include <ubit/uhtml.hpp>
using namespace std;
using namespace ubit;


struct ParseGUI : public UBox {
  static const int WIN_WIDTH = 500, WIN_HEIGHT = 500;
  
  ParseGUI(const UStr& filename);
  void readXmlFile();
  
private:
  class UXmlParser& parser;   // for parsing XML files 
  UStr source_path, source_text, errors_text;
  UFilebox openbox;
  UDialog source_dialog;
  UScrollpane docspane;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int main(int argc, char** argv) {
  UAppli appli(argc, argv);
  
  appli.getDisp()->setPixelPerInch(100);
  
  ParseGUI gui((argc>1 ? argv[1] : ""));
  UFrame frame(usize(ParseGUI::WIN_WIDTH, ParseGUI::WIN_HEIGHT) + gui);
  frame.show();
  
  return appli.start();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct hbox : public UBox {
  UCLASS(hbox)
  hbox() {
    addAttr(UOrient::horizontal + UBorder::line);
  }
};

struct vbox : public UBox {
  UCLASS(vbox)
  vbox() {
    addAttr(UOrient::vertical + UBorder::line);
  }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

ParseGUI::ParseGUI(const UStr& filename) 
// UHtmlParser uses a predefined HMTL grammar and parses code in Permissive
// and CollapseSpaces modes.
: parser(*new UHtmlParser)
{
  // MyGrammar defines my own tags
  UXmlGrammar& g = *new UXmlGrammar;
  g.addElementClass(hbox::Class());
  g.addElementClass(vbox::Class());
  //g.addElementClass(ZMore::Class());
  //g.addElementClass(ZLess::Class());
  //g.addElementClass(ZLeaf::Class());
  parser.addGrammar(g);
  
  UErrorHandler* error_handler = new UErrorHandler("Parser");
  error_handler->setOutputStream(null);          // dont write errors on console
  error_handler->setOutputBuffer(&errors_text);  // write errors in this buffer
  parser.setErrorHandler(error_handler);
  
  openbox.addAttr(ucall(this, &ParseGUI::readXmlFile));
  
  source_dialog.addAttr(usize(WIN_WIDTH, WIN_HEIGHT) 
                        + utitle("Source Code & Errors"));
  source_dialog.add
  (utop() 
   + ulabel("File Path: " + source_path)
   + uvflex()
   + uvbox(uscrollpane(usize(UIGNORE,48|UPERCENT) + UBackground::white 
                       + utextarea(source_text))
           + uscrollpane(usize(UIGNORE,48|UPERCENT) + UBackground::white 
                         + utextarea(errors_text))
           )
   );
  
  UBox& menubar = umenubar
  (ubutton("Open" + udialog(utitle("Open XML File") + openbox))
   + ubutton("Reload" + ucall(this, &ParseGUI::readXmlFile))
   + ubutton("Source & Errors" + source_dialog)
   );
  
  docspane.addAttr(UBackground::white);
  
  addAttr(UOrient::vertical);
  add(utop() 
      + menubar 
      + ulabel("File Path: " + source_path)
      + uvflex() 
      + docspane
      );
  
  if (!filename.empty()) {
    openbox.setName(filename);
    readXmlFile();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ParseGUI::readXmlFile() {
  source_path = openbox.getPath();
  source_text.clear();
  errors_text.clear();
  docspane.removeAll();
  
  UXmlDocument* doc = parser.read(source_path);
  if (!doc) {
    UDialog::showAlertDialog("Can't parse file: " & source_path);
    return;
  }
  
  UElem* e = doc->getDocumentElement();
  if (!e) {
    UDialog::showAlertDialog("Empty document in file: " & source_path);
    return;
  }
  
  //doc->print(cout);       // prints the XML tree on this stream
  docspane.add(doc);
  doc->print(source_text);     // prints the XML tree in this buffer
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

