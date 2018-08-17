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
#include <ubit/ustyle.hpp>
#include <ubit/uboxgeom.hpp>
#include <ubit/uboxes.hpp>
#include <ubit/uinteractors.hpp>
#include <ubit/upix.hpp>
#include <ubit/ucolor.hpp>
using namespace std;
namespace ubit {



UStyle* UFrame::createStyle() {
  return Window::createStyle();
}

UFrame::UFrame(Args a) : UDialog(a) {
  wmodes.IS_HARDWIN = true;   // a frame is always a HARDWIN!
  wmodes.IS_FRAME  = true;
  wmodes.IS_DIALOG = false;
}

// Dialogs are centered but not Frames
void UFrame::show(bool state, Display* disp) {
  if (isShown(disp) == state) return;
  Window::show(state, disp);
}

bool UFrame::realize() {
  if (! wmodes.IS_HARDWIN) {
    error("UFrame::realize","can't realize UFrame object %p",this);
    return false;
  }
  if (wmodes.IS_MAINFRAME) return realizeHardwin(UWinImpl::MAINFRAME);
  else return realizeHardwin(UWinImpl::FRAME);
}

// ------------------------------------------------------------ [ELC] ----------

UStyle* UDialog::createStyle() {
  UStyle* s = Window::createStyle();
  static Border* b = new URoundBorder(Border::LINE,Color::black,Color::white,2,2,15,15);
  s->setBorder(b);
  s->setPadding(4, 6);
  return s;
}

UDialog::UDialog(Args a) : Window(a), open_call(null) {
  wmodes.IS_DIALOG = true;
}

bool UDialog::realize() {
  if (wmodes.IS_HARDWIN) return realizeHardwin(UWinImpl::DIALOG);
  else {
    error("UDialog::realize","can't realize UDialog object %p",this);
    return false;
  }
}

void UDialog::addingTo(Child& c, Element& parent) {
  Window::addingTo(c, parent);
  // ajouter handler pour ouvrir auto le Dialog si le parent est un UButton
  if (wmodes.IS_AUTO_OPENED && parent.isArmable()) {
    if (!open_call) open_call = &ucall(this, true, &UDialog::show);
    parent.addAttr(*open_call);
  }
}

//NB: removingFrom() requires a destructor to be defined
void UDialog::removingFrom(Child& c, Element& parent) {
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

void UDialog::show(bool state, Display* disp) {
  if (isShown(disp) == state) return;
  View* v = getView(0);
  bool already_shown = (v && v->hasVMode(View::INITIALIZED));
  Window::show(state, disp);
  
  // deconne si avant show, ne recentrer qu'a la la apparition
  if (state && !already_shown) centerOnScreen(disp);
}

// ------------------------------------------------------------ [ELC] ----------
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
 int UOptionDialog::showDialog(const String& title, Args message, Args icon, Args ok) {
 setDialog(title, message, icon, ok);
 show(true);
 showModal( );  !!!!!&&&&
 }
 */

UOptionDialog::UOptionDialog(Args message) {
  constructs(message);
}

UOptionDialog::UOptionDialog(const String& title, Args message, Args icon, Args buttons) {
  setTitle(title);
  constructs(message);
  setIcon(icon);
  setButtons(buttons);
}

void UOptionDialog::constructs(Args message) {
  pmessage = uhbox(message);
  pmessage->addAttr(uhflex());

  pbuttons = uhbox(ubutton("    OK    " + ucloseWin()));
  pbuttons->addAttr(uhcenter() + UFont::bold + UFont::large);
    
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

UOptionDialog& UOptionDialog::setTitle(const String& title) {
  Window::setTitle(title);
  return *this;
}

UOptionDialog& UOptionDialog::setMessage(Args nodelist) {
  pmessage->removeAll();
  pmessage->add(nodelist);
  adjustSize();
  return *this;
}

UOptionDialog& UOptionDialog::setMessage(const String& msg) {
  return setMessage(Args(ustr(msg)));
}

UOptionDialog& UOptionDialog::setIcon(Args icon) {
  picon->removeAll();
  if (!icon) picon->show(false);
  else {
    picon->add(icon);
    picon->show(true);
  }
  adjustSize();
  return *this;
}

UOptionDialog& UOptionDialog::setButtons(Args buttons) {
  pbuttons->removeAll();
  if (!buttons) pbuttons->add(ubutton(" OK " + ucloseWin()));
  else {
    pbuttons->add(buttons);
    for (ChildIter i = pbuttons->cbegin(); i != pbuttons->cend(); i++) {
      UButton* b = dynamic_cast<UButton*>(*i);
      if (b) b->addAttr(ucloseWin());
    }
  }
  adjustSize();
  return *this;
}
// ------------------------------------------------------------ [ELC] ----------
/*
 ERROR_MESSAGE
 INFORMATION_MESSAGE
 WARNING_MESSAGE
 QUESTION_MESSAGE
 PLAIN_MESSAGE
 */

UOptionDialog* UDialog::messdialog = null;

void UDialog::showMessageDialog(const String& title, Args message_nodes, Args icon) {
  if (!messdialog) messdialog = new UOptionDialog();
  messdialog->setMessage(message_nodes);
  messdialog->setIcon(icon);
  messdialog->show(true);
  messdialog->setTitle(title);
}

void UDialog::showMessageDialog(const String& title, const String& msg_string, Args icon) {
  showMessageDialog(title, Args(ustr(msg_string)), icon);
}

void UDialog::showAlertDialog(Args message_nodes) {
  showMessageDialog("Alert", 
                    UFont::large + upadding(5,5) + message_nodes, 
                    uscale(1.5) + UPix::ray);      // !!! CHOIX ICON !!!!
}

void UDialog::showAlertDialog(const String& message_string) {
  showAlertDialog(Args(ustr(message_string)));
}

void UDialog::showErrorDialog(Args message_nodes) {
  showMessageDialog("Error", 
                    UFont::large + upadding(5,10) + message_nodes, 
                    uscale(1.5) + UPix::ray);      // !!! CHOIX ICON !!!!
}

void UDialog::showErrorDialog(const String& message_string) {
  showErrorDialog(Args(ustr(message_string)));
}

void UDialog::showWarningDialog(Args message_nodes) {
  showMessageDialog("Warning", 
                    UFont::large + upadding(5,10) + message_nodes, 
                    uscale(1.5) + UPix::ray);      // !!! CHOIX ICON !!!!
}

void UDialog::showWarningDialog(const String& message_string) {
  showWarningDialog(Args(ustr(message_string)));
}

}
