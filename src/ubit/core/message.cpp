/*
 *  message.cpp: Ubit Inter-Client Messages: see umservice.hpp 
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
#include <ubit/ubit_features.h>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <ubit/core/on.h>
#include <ubit/core/call.h>
#include <ubit/ui/box.h>
#include <ubit/core/event.h>
#include <ubit/core/application.h>
#include <ubit/core/appimpl.h>
#include <ubit/core/message.h>
#include <ubit/umservice.hpp>
#include <ubit/umsproto.hpp>
#include <ubit/udispX11.hpp>
using namespace std;
namespace ubit {


MessagePort::MessagePort(const String& _name) : name(_name) {}

MessagePort* MessagePortMap::findMessagePort(const String& name) {
  MessMap::iterator k = mess_map.find(&name);
  if (k != mess_map.end()) return k->second;
  else return null;
}

MessagePort& MessagePortMap::getMessagePort(const String& name) {
  MessagePort* msg = findMessagePort(name);
  if (!msg) {
    msg = new MessagePort(name);
    mess_map[&msg->getName()] = msg;
  }
  return *msg;
}


void MessagePortMap::fireMessagePort(const char* buf) {
  if (buf) {
    String name;
    const char* p = strchr(buf,' ');
    //if (p) {*p = 0; p++;}
    //name.append(buf);
    if (!p) name = buf;
    else {
      name.append(buf, p-buf);
      p++;
    }

    MessagePort* port = findMessagePort(name);
    if (port) {
      if (p) port->value = p;  // A REVOIR: marche pas si multistrings ou binaires
      else port->value = "";

      //MessageEvent e(UOn::message, null, null, null);  // a quoi sert message ?
      //e.setCond(UOn::action);
      //e.setSource(port);
      MessageEvent e(UOn::action, port);
      port->fire(e);
    }
  }
}
  
#if !UBIT_WITH_X11

void Message::send(UHardwinImpl&, const char* message) {
  cerr << "Message::send not implemented " << endl;
}

#else  // - - - UBIT_WITH_X11 - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//ifdef UBIT_WITH_GDK
//typedef GdkEventClient ClientEvent;
//else
typedef XClientMessageEvent ClientEvent;
//endif

static void sendShortMessage(UDispX11* nd, UHardwinX11* nw, const char* message) {
  Atom UMS_MESSAGE = nd->getAtoms().UMS_MESSAGE;
  ClientEvent e;
  e.type = ClientMessage;
  e.serial = 0;
  e.send_event = false;	//true if this came from a SendEvent request

  // ici il y un pbm: ca doit etre le display de l'appli pas celui du mouse
  // driver. c'est OK pour XSendEvent (qui corrige) mais pas pour write
  // direct (il faut alors corriger a la reception)
  e.display = nd->getSysDisp();   // Display the event was read from
  e.window = nw->getSysWin();      //``event'' window it is reported relative to
  e.message_type = UMS_MESSAGE,   // property
  e.format = 8;                   // table of chars
  strcpy(e.data.b, message);

  long ev_mask = 0;
  XSendEvent(nd->getSysDisp(), nw->getSysWin(), False, ev_mask, (XEvent*)&e);
  XFlush(nd->getSysDisp());
}

 
static void sendLongMessage(UDispX11* nd, UHardwinX11* nw,
                            const char* message, int message_len) {
  Atom UMS_MESSAGE = nd->getAtoms().UMS_MESSAGE;

  // ATT: If there is insufficient space, a BadAlloc error results.
  XChangeProperty(nd->getSysDisp(), nw->getSysWin(),
                  UMS_MESSAGE,     // property
                  XA_STRING,       // type
                  8,               // format
                  // PropModeAppend: ajoute a la fin (NB: PropModeReplace remplacerait
                  // la valeur courante ce qui aurait pour effet de perdre des donnees
                  // si le callback du client est appele apres un 2e appel de XChangeProperty)
                  PropModeAppend,
                  (unsigned char*)message, message_len);
  
  XFlush(nd->getSysDisp()); // necessaire sinon notification differee  
  // a revoir: le cas ou les XChangeProperty sont trop proches et s'accumulent
}


void Message::send(UHardwinImpl& nw, const char* message) {
  if (!message) return;
  unsigned int len = strlen(message);
  Display* nd = Application::impl.disp;
  
  ClientEvent e;
  if (len < sizeof(e.data.b)) 
    sendShortMessage((UDispX11*)nd, (UHardwinX11*)&nw, message);
  else sendLongMessage((UDispX11*)nd,(UHardwinX11*)&nw, message, len);
}

void Message::send(UHardwinImpl& nw, const String& message) {
  if (message.empty()) return;
  unsigned int len = message.length();
  Display* nd = Application::impl.disp;

  ClientEvent e;
  if (len < sizeof(e.data.b)) 
    sendShortMessage((UDispX11*)nd, (UHardwinX11*)&nw, message.c_str());
  else sendLongMessage((UDispX11*)nd,(UHardwinX11*)&nw, message.c_str(), len);
}

#endif  // UBIT_WITH_X11


}

