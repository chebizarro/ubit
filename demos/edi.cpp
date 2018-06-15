/*************************************************************************
 *
 *  edi.cpp: a simple text editor
 *  Ubit Demos - Version 6
 *  (C) 2009 Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
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

#include <iostream>
#include <cmath>
#include <ubit/ubit.hpp>
#include "edi.hpp"
using namespace std;
using namespace ubit;


static void openMenuCB(UMouseEvent& e, UCtlmenu* m) {
  if (e.getButton() == e.RightButton 
      || (e.getButton() == e.LeftButton && e.isControlDown())) {
    m->open(e);
  }
}

static void setScale(UScale* scale, UInt* i) {
  double val = pow(1.15, i->intValue());
  *scale = val;
}                              

static void setFontFamily(UFont* f, UFontFamily* ff) {
  f->setFamily(*ff);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Edi::Edi() : spane(true,false) {
  openbox.add
    (utitle("Open File")    // openFileCB() is called when the OK button is clicked
     + UOn::action / ucall(this, &openbox.path(), &Edi::openFileCB)
     );
      
   UMenu &file_menu = umenu
    (ubutton(UPix::ofolder  + " Open" + umenu(openbox))
     + ubutton(UPix::diskette + " Save" + umenu("not_impl"))
     + usepar()
     + ubutton(UPix::stop + " Quit" + ucall(this, &Edi::quit))
     );
  
  UBox& toolbar = ubar
    (" " + uflatbutton(USymbol::down + file_menu) + " " + UFont::bold + path);

  // text and scale - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  
  textarea.setAttr(scale);
  textarea.setAttr(font);
  textarea.add(text);
  
  // openMenuCB() is called when the mouse is pressed. It opens the cmenu if 
  // Button3 or Ctrl+Button1 are pressed
  initCMenu();
  textarea.add(cmenu + UOn::mpress / ucall(&cmenu, openMenuCB));  
  spane.add(textarea);

  // the value of the spin box controls the text scale
  spinbox.add(UOn::action / ucall(&scale, &spinbox.value(), setScale));

  // controls - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  
  UBox& controls = uhbox
    (upadding(3,3).setTop(1) 
     + "Scale" + spinbox
     + ubutton("bold" 
               + UOn::select / ucall(&font, true, &UFont::setBold)
               + UOn::deselect / ucall(&font, false, &UFont::setBold)
               )
     + ubutton("italic" 
               + UOn::select / ucall(&font, true, &UFont::setItalic)
               + UOn::deselect / ucall(&font, false, &UFont::setItalic)
               )     
     + uhbox(uchoice()
             + ubutton("sans"
                       + UOn::select / ucall(&font, &UFontFamily::sans_serif, setFontFamily))
             + ubutton("serif"
                       + UOn::select / ucall(&font, &UFontFamily::serif, setFontFamily))
             + ubutton("mono" 
                       + UOn::select / ucall(&font, &UFontFamily::monospace, setFontFamily))
             + ubutton("more" + *fontDialog())
             )
     + uhflex() 
     + ulabel(UFont::italic + UColor::navy + uhcenter()
              + "Press right mouse button on text to open menu  ")
    );
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    
  mainbox.add
    (uhflex()
     + utop()
     + toolbar
     + uvflex() + spane
     + ubottom() + controls
     );
  
  // frame
  this->add(mainbox + UOn::close / ucall(this, &Edi::quit));
}
 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// quits the appli

class MessageDialog : public UDialog {
public:
  UCLASS(MessageDialog)
  
  MessageDialog(const UArgs& = UArgs::none);
  
  UBox& messageBar() {return *pmessage_bar;}
  UBox& iconBox()    {return *picon_box;}
  UBox& textBox()    {return *ptext_box;}

  UBox& buttonBar()        {return *pbutton_bar;}
  UButton& cancelButton()  {return *pcancel_button;}
  UButton& okButton()      {return *pok_button;}

protected:
  uptr<UBox> pmessage_bar, picon_box, ptext_box, pbutton_bar;
  uptr<UButton> pcancel_button, pok_button;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

MessageDialog::MessageDialog(const UArgs& args) {
  pmessage_bar = uhbox();
  picon_box = uhbox(uscale(2) + UPix::ray);
  ptext_box = uhbox(args);
  pbutton_bar = uhbox();
  pcancel_button = ubutton("  Cancel  ");
  pcancel_button->addAttr(ucall(0, &UButton::closeWin));
  pok_button = ubutton("    OK    ");
  pok_button->addAttr(ucall(1, &UButton::closeWin));

  pmessage_bar->add
    (UFont::bold + UFont::large 
     + uleft() + *picon_box + "  " + uhflex() + *ptext_box
     );
  
  pbutton_bar->add
    (UFont::bold + uhcenter() + *pcancel_button + *pok_button);

  add
    (UBackground::velin
     + usize(400,UIGNORE) + upadding(25,20).setBottom(15)  + uvspacing(15)
     + *pmessage_bar
     + ubottom()
     + uhbox(UBorder::etchedOut)
     + *pbutton_bar
   );
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Edi::quit() {
  static MessageDialog* quitdial;
  if (!quitdial) { 
    quitdial = new MessageDialog("Do you want to quit uedi?");
  };

  int stat = quitdial->showModal();
  if (stat > 0) UAppli::quit(stat);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// loads the file designed by 'fname'

void Edi::openFile(const UStr& fname) {
  UAppli::setTitle("uedi: " & fname);

  // reads 'fname' and puts its content in 'text'
  text.read(fname);
  
  // scrolls the scrollpane to pos 0,0
  spane.setScroll(0, 0);
  path = fname;
}

void Edi::openFileCB(UStr* fname) {
  if (fname) openFile(*fname);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void deleteObj(UDialog* d) {
  //if (d) remove(*d);   // alternate way
  delete d;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Edi::openCloneOnDisplay(const UStr& display_name) {
  UDisp* d = UAppli::openDisp(display_name);
  if (d) {
    UStr t = "uedi on " & display_name;
    UFrame& f2 = uframe( utitle(t) + mainbox);
    d->add(f2);
    f2.show();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct ZoomAction : public UCtlAction {
  static const int QUANTUM = 10;
  Edi& mainbox;
  int base, incr;
  
  ZoomAction(Edi* _mainbox) : UCtlAction(0.25), mainbox(*_mainbox) {}
  
  virtual void mdrag(UMouseEvent& e, UCtlmenu& m) {
    if (e.isFirstDrag()) {
      base = mainbox.spinbox.value().intValue();
      incr = 0;
    }
    else {
      int val = int(m.getMouseMovement().x / QUANTUM);
      if (val != incr) {
        if (base+val >= 30) val = 30 - base;
        else if (base+val <= -8) val = -8 - base;
        incr = val;
        mainbox.spinbox.setValue(base + incr);
        mainbox.update();
      }
    }
  }
};


void Edi::initCMenu() {
  ZoomAction* za = new ZoomAction(this);
  UScrollAction* sa = new UScrollAction(spane);
  cmenu.item(0).add(UFont::x_large + UFont::bold + UColor::red + "Z+" + *za);
  cmenu.item(4).add(UFont::x_large + UFont::bold + UColor::red + "Z-" + *za);    
  cmenu.item(2).add(UPix::bigUp + *sa);
  cmenu.item(6).add(UPix::bigDown + *sa);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void setFont(UFont* f, UStr* file) {
  static UFontFamily more_ff("more_ff");
  UStr path = "/usr/X11R6/lib/X11/fonts/Type1/" & *file;
  more_ff.setFreeType(path.c_str(), path.c_str(), path.c_str(), path.c_str());
  f->setFamily(more_ff);
}

UDialog* Edi::fontDialog() {
  static UDialog* fd = null;
  static UStr font_path;
  
  if (!fd) {
    fd = new UDialog
    (utitle("Font Name")
     + upadding(10,15) + uvspacing(20) + uvcenter()
     + uhbox("TrueType FileName: " + uhflex() 
             + utextfield(usize(350,UIGNORE) + font_path ))
     + uhbox(uhcenter() + upadding(10,UIGNORE)
             + ubutton("Cancel" + ucloseWin()) 
             + ubutton("  OK  " + ucall(&font, &font_path, setFont) + ucloseWin())
             )
    );
  }
  return fd;
}
