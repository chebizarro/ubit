/*
 *  dialogs.cpp: dialog boxes
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
#include <ubit/ucall.hpp>
#include <ubit/udialogs.hpp>
#include <ubit/ui/uwinImpl.hpp>
#include <ubit/draw/style.h>
#include <ubit/uboxgeom.hpp>
#include <ubit/uboxes.hpp>
#include <ubit/ui/interactors.h>
#include <ubit/upix.hpp>
#include <ubit/draw/color.h>
using namespace std;
namespace ubit {



Style* Frame::createStyle() {
  return Window::createStyle();
}

Frame::Frame(Args a) : Dialog(a) {
  wmodes.IS_HARDWIN = true;   // a frame is always a HARDWIN!
  wmodes.IS_FRAME  = true;
  wmodes.IS_DIALOG = false;
}

// Dialogs are centered but not Frames
void Frame::show(bool state, Display* disp) {
  if (isShown(disp) == state) return;
  Window::show(state, disp);
}

bool Frame::realize() {
  if (! wmodes.IS_HARDWIN) {
    error("Frame::realize","can't realize Frame object %p",this);
    return false;
  }
  if (wmodes.IS_MAINFRAME) return realizeHardwin(UWinImpl::MAINFRAME);
  else return realizeHardwin(UWinImpl::FRAME);
}


Style* Dialog::createStyle() {
  Style* s = Window::createStyle();
  static Border* b = new RoundBorder(Border::LINE,Color::black,Color::white,2,2,15,15);
  s->setBorder(b);
  s->setPadding(4, 6);
  return s;
}

Dialog::Dialog(Args a) : Window(a), open_call(null) {
  wmodes.IS_DIALOG = true;
}

bool Dialog::realize() {
  if (wmodes.IS_HARDWIN) return realizeHardwin(UWinImpl::DIALOG);
  else {
    error("Dialog::realize","can't realize Dialog object %p",this);
    return false;
  }
}

void Dialog::addingTo(Child& c, Element& parent) {
  Window::addingTo(c, parent);
  // ajouter handler pour ouvrir auto le Dialog si le parent est un Button
  if (wmodes.IS_AUTO_OPENED && parent.isArmable()) {
    if (!open_call) open_call = &ucall(this, true, &Dialog::show);
    parent.addAttr(*open_call);
  }
}

//NB: removingFrom() requires a destructor to be defined
void Dialog::removingFrom(Child& c, Element& parent) {
  if (wmodes.IS_AUTO_OPENED && parent.isArmable()) {
    // don't delete the ucall as it is shared
    if (open_call) {
      bool auto_up = parent.isAutoUpdate();
      parent.setAutoUpdate(false);
      parent.removeAttr(*open_call, false);
      parent.setAutoUpdate(auto_up);
    }
  }
  Window::removingFrom(c, parent);
}

void Dialog::show(bool state, Display* disp) {
  if (isShown(disp) == state) return;
  View* v = getView(0);
  bool already_shown = (v && v->hasVMode(View::INITIALIZED));
  Window::show(state, disp);
  
  // deconne si avant show, ne recentrer qu'a la la apparition
  if (state && !already_shown) centerOnScreen(disp);
}

/* creates a new OptionDialog; see also creator shortcut uoptiondialog().
 * All parameters are optional:
 * - 'title' is the dialog title
 * - 'content' controls the main display area, typically for displaying a text message,
 *    but it can contain any combination of Node objects (separated by a + operator)
 * - 'icon' controls an area on the left hand side, normaly used for displaying an image.
 *    As for 'content' it can in fact contain any combination of objects.
 *    No icon is displayed if this argument is 'none'.
 * - 'buttons' controls the button area, located on the bottom. By default, a "OK"
 *    button is displayed if this argument is 'none'. Otherwise, whatever passed
 *    to this argument will be displayed in the button area.
 * @see Element::add(Args arguments) for more info on argument lists.
 */
/*
 int OptionDialog::showDialog(const String& title, Args message, Args icon, Args ok) {
 setDialog(title, message, icon, ok);
 show(true);
 showModal( );  !!!!!&&&&
 }
 */

OptionDialog::OptionDialog(Args message) {
  constructs(message);
}

OptionDialog::OptionDialog(const String& title, Args message, Args icon, Args buttons) {
  setTitle(title);
  constructs(message);
  setIcon(icon);
  setButtons(buttons);
}

void OptionDialog::constructs(Args message) {
  pmessage = uhbox(message);
  pmessage->addAttr(uhflex());

  pbuttons = uhbox(ubutton("    OK    " + ucloseWin()));
  pbuttons->addAttr(uhcenter() + Font::bold + Font::large);
    
  picon = new Box;
  picon->addAttr(uhcenter() + uvcenter());

  addAttr(upadding(12,12) + uvspacing(12));
  
  add(uhflex() + uvflex() 
      + uhbox(*picon + uhflex() + *pmessage)
      + ubottom()
      //+ usepar()
      + *pbuttons
      );
}

OptionDialog& OptionDialog::setTitle(const String& title) {
  Window::setTitle(title);
  return *this;
}

OptionDialog& OptionDialog::setMessage(Args nodelist) {
  pmessage->removeAll();
  pmessage->add(nodelist);
  adjustSize();
  return *this;
}

OptionDialog& OptionDialog::setMessage(const String& msg) {
  return setMessage(Args(ustr(msg)));
}

OptionDialog& OptionDialog::setIcon(Args icon) {
  picon->removeAll();
  if (!icon) picon->show(false);
  else {
    picon->add(icon);
    picon->show(true);
  }
  adjustSize();
  return *this;
}

OptionDialog& OptionDialog::setButtons(Args buttons) {
  pbuttons->removeAll();
  if (!buttons) pbuttons->add(ubutton(" OK " + ucloseWin()));
  else {
    pbuttons->add(buttons);
    for (ChildIter i = pbuttons->cbegin(); i != pbuttons->cend(); i++) {
      Button* b = dynamic_cast<Button*>(*i);
      if (b) b->addAttr(ucloseWin());
    }
  }
  adjustSize();
  return *this;
}
/*
 ERROR_MESSAGE
 INFORMATION_MESSAGE
 WARNING_MESSAGE
 QUESTION_MESSAGE
 PLAIN_MESSAGE
 */

OptionDialog* Dialog::messdialog = null;

void Dialog::showMessageDialog(const String& title, Args message_nodes, Args icon) {
  if (!messdialog) messdialog = new OptionDialog();
  messdialog->setMessage(message_nodes);
  messdialog->setIcon(icon);
  messdialog->show(true);
  messdialog->setTitle(title);
}

void Dialog::showMessageDialog(const String& title, const String& msg_string, Args icon) {
  showMessageDialog(title, Args(ustr(msg_string)), icon);
}

void Dialog::showAlertDialog(Args message_nodes) {
  showMessageDialog("Alert", 
                    Font::large + upadding(5,5) + message_nodes, 
                    uscale(1.5) + UPix::ray);      // !!! CHOIX ICON !!!!
}

void Dialog::showAlertDialog(const String& message_string) {
  showAlertDialog(Args(ustr(message_string)));
}

void Dialog::showErrorDialog(Args message_nodes) {
  showMessageDialog("Error", 
                    Font::large + upadding(5,10) + message_nodes, 
                    uscale(1.5) + UPix::ray);      // !!! CHOIX ICON !!!!
}

void Dialog::showErrorDialog(const String& message_string) {
  showErrorDialog(Args(ustr(message_string)));
}

void Dialog::showWarningDialog(Args message_nodes) {
  showMessageDialog("Warning", 
                    Font::large + upadding(5,10) + message_nodes, 
                    uscale(1.5) + UPix::ray);      // !!! CHOIX ICON !!!!
}

void Dialog::showWarningDialog(const String& message_string) {
  showWarningDialog(Args(ustr(message_string)));
}

}
