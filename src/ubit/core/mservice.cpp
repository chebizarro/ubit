/*
*
*  umservice.cpp: UMS (Ubit Mouse/Message Server) services 
*  Ubit GUI Toolkit - Version 6.0
*  (C) 2008 EricLecolinet / ENST Paris / www.enst.fr/~elc/ubit
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
#include <cstdlib>
#include <unistd.h>
#include <ubit/ucall.hpp>
#include <ubit/uappli.hpp>
#include <ubit/core/uappliImpl.hpp>
#include <ubit/usource.hpp>
#include <ubit/usocket.hpp>
#include <ubit/umessage.hpp>
#include <ubit/umservice.hpp>
#include <ubit/umsproto.hpp>
#include <ubit/core/event.h>
#if UBIT_WITH_X11
#  include <X11/X.h>
#else
#  define KeyPress                2
#  define KeyRelease              3
#  define ButtonPress             4
#  define ButtonRelease           5
#  define MotionNotify            6
#endif
using namespace std;
namespace ubit {

MessageService::MessageService(const String& _host, int _port, const char* _client_name)
: Socket(_host, (_port != 0 ? _port : UMS_PORT_NUMBER)) {
}

MessageService::MessageService(const String& _host, int _port)
: Socket(_host, (_port != 0 ? _port : UMS_PORT_NUMBER)) {
  onInput(ucall(this, &MessageService::inputCallback));
}

MessageService::~MessageService() {
  MessagePort& mp1 = Application::getMessagePort("_umsBrowse");
  if (browse_call) mp1.remove(*browse_call);

  MessagePort& mp2 = Application::getMessagePort("_umsNhb");
   if (neighbor_call) mp2.remove(*neighbor_call);
}


void MessageService::inputCallback() {
  InBuffer ib;
  if (receiveBlock(ib) && Application::impl.messmap) {
    //ib.data()[ib.size()-1] = 0; // faux
    Application::impl.messmap->fireMessagePort(ib.data());
  }
  /*
   if (input) {
     Application* app = Application::getApp();
     if (app) {
       // le champ 'display' doit etre celui de l'appli et non de l'UMS
       xev.xany.display = app->getNatDisp()->getXDisplay();
       app->getNatDisp()->dispatchEvent(&xev);
     }
   }
   */
}


bool MessageService::browseUMServers(UCall& c) {
  MessagePort& mp = Application::getMessagePort("_umsBrowse");
  //mp.removeAll();
  if (browse_call) mp.remove(*browse_call);
  browse_call = c;
  mp.add(c);
  return sendRequest(UMSrequest::BROWSE_SERVERS);
}

bool MessageService::browseUMSNeighbors(UCall& c) {
  MessagePort& mp = Application::getMessagePort("_umsNeighbor");
  //mp.removeAll();
  if (neighbor_call) mp.remove(*neighbor_call);
  neighbor_call = c;
  mp.add(c);
  return sendRequest(UMSrequest::BROWSE_NEIGHBORS);
}


MessageService::BrowseReply::BrowseReply(MessageEvent& e) {
  const String* msg = e.getMessage();
  flags = 0;
  errorCode = 0;
  interfaceIndex = 0;
  serviceName = "";
  serviceType = "";
  replyDomain = "";
  status = -1;
  if (!msg || !msg->c_str()) return;

  const char* s = msg->c_str();
  const char* p = strchr(s, '\t');
  if (p) serviceName.append(s, p-s);
  else {serviceName = s; return;}

  s = p+1;
  p = strchr(s, '\t');
  if (p) serviceType.append(s, p-s);
  else {serviceType = s; return;}

  s = p+1;
  p = strchr(s, '\t');
  status = atoi(s);

  if (p) {
    s = p+1;
    flags = atoi(s);
  }
}


struct _UMSResolveData : public UCall {
  unique_ptr<UCall>pc;

  _UMSResolveData(UCall& _c) : pc(_c) {}

  void operator()(Event& e) {
    (*pc)(e);
    pc = null;      // destruction de pc si pas reference' ailleurs
    delete this;    // destruction de _UMSResolveData
  };
};

// c est auto-detruit si pas reference
bool MessageService::resolveUMServer(const String& name, UCall& c) {
  _UMSResolveData* rd = new _UMSResolveData(c);
  
  Application::onMessage("_umsResolve", *rd);
  return sendRequest(UMSrequest::RESOLVE_SERVER, name);
}

MessageService::ResolveReply::ResolveReply(MessageEvent& e) {
  const String* msg = e.getMessage();
  flags = 0;
  errorCode = 0;
  interfaceIndex = 0;
  fullname = "";
  hosttarget = "";
  port = 0;
  if (!msg || !msg->c_str()) return;

  const char* s = msg->c_str();
  const char* p = strchr(s, '\t');
  if (p) fullname.append(s, p-s);
  else {fullname = s; return;}

  s = p+1;
  p = strchr(s, '\t');
  if (p) hosttarget.append(s, p-s);
  else {hosttarget = s; return;}

  s = p+1;
  p = strchr(s, '\t');
  port = atoi(s);

  if (p) {
    s = p+1;
    flags = atoi(s);
  }
}


bool MessageService::moveMouse(int event_flow, int x, int y, bool abs_coords) {
  UMSrequest req(UMSrequest::KEY_MOUSE_CTRL);
  req.writeEvent(MotionNotify, event_flow, x, y, abs_coords);
  return sendBlock(req);
}

bool MessageService::pressMouse(int event_flow, int button_mask) {
  UMSrequest req(UMSrequest::KEY_MOUSE_CTRL);
  req.writeEvent(ButtonPress, event_flow, 0, 0, button_mask);
  return sendBlock(req);
}

bool MessageService::releaseMouse(int event_flow, int button_mask) {
  UMSrequest req(UMSrequest::KEY_MOUSE_CTRL);
  req.writeEvent(ButtonRelease, event_flow, 0, 0, button_mask);
  return sendBlock(req);
}

bool MessageService::pressKey(int event_flow, int keycode) {
  UMSrequest req(UMSrequest::KEY_MOUSE_CTRL);
  req.writeEvent(KeyPress, event_flow, 0, 0/*ex:mods*/, keycode);
  return sendBlock(req);
}

bool MessageService::releaseKey(int event_flow, int keycode) {
  UMSrequest req(UMSrequest::KEY_MOUSE_CTRL);
  req.writeEvent(KeyRelease, event_flow, 0, 0/*ex:mods*/, keycode);
  return sendBlock(req);
}

/*
bool MessageService::tactos(const char* data) {
  UMSrequest req(UMSrequest::TACTOS);
  req.writeString(data);
  return sendBlock(req);
}
*/


bool MessageService::sendMousePress(const char* target,
			       int x, int y, int mouse_btn, int mods) {
  UMSrequest req(UMSrequest::SEND_EVENT);
  req.writeEvent(ButtonPress, /*flow*/0, x, y, mouse_btn | mods);
  req.writeString(target);
  return sendBlock(req);
}

bool MessageService::sendMouseRelease(const char* target,  
				 int x, int y, int mouse_btn, int mods) {
  UMSrequest req(UMSrequest::SEND_EVENT);
  req.writeEvent(ButtonRelease, /*flow*/0, x, y, mouse_btn | mods);
  req.writeString(target);
  return sendBlock(req);
}

bool MessageService::sendMouseClick(const char* target, 
			       int x, int y, int mouse_btn, int mods) {
  return 
    sendMousePress(target, x, y, mouse_btn, mods)
    && sendMouseRelease(target, x, y, mouse_btn, mods);
}


bool MessageService::sendRequest(int request, const String& data) {
  return sendRequest(request, data.c_str());
}

bool MessageService::sendRequest(int request, const char* data) {
  if (!data) data = "";    // data can be empty in this case
  UMSrequest req(request);
  req.writeString(data);
  return sendBlock(req);
}


bool MessageService::sendMessage(const char* target, const String& message) {
  return sendMessage(target, message.c_str());
}

bool MessageService::sendMessage(const char* target, const char* message) {
  // on ne doit retrourner false que si la comm deconne !
  //if (!message || !*message || !target || !*target) {
  //  UError::error("warning@UMSclient::sendMessage","empty target or message (not sent)");
  //  return false;
  //}
  UMSrequest req(UMSrequest::SEND_MESSAGE);
  req.writeString(target);
  req.writeString(message);
  return sendBlock(req);
}

}

