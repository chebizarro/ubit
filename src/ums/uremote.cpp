/*************************************************************************
 *
 *  uremote.cpp : Remote Control for the UMS server
 *  the UMS server provides multiple event flows, pseudo pointers, two-handed
 *  interaction, RendezVous identification, remote control facilities, etc.
 *
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2003-2008 Eric Lecolinet / ENST Paris / www.enst.fr/~elc/ubit
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
#include <cstdio>
#include <unistd.h>
#include "uremote.hpp"
#include <ubit/ubit.hpp>
#include <ubit/uxpm.hpp>
#include <ubit/nat/udispX11.hpp>
#include <X11/cursorfont.h>
using namespace std;

// config
const char* DISCONNECTED = "*** DISCONNECTED ***";
const char* CANT_RESOLVE = "[CAN'T RESOLVE] ";
const char* CANT_REACH   = "[CAN'T REACH] ";
UFont& bold = UFont::bold;

const char* UnixPanel::commands[] = {
  //"ls [-a|-la]",  sans interet
  "xhost [ + | - ]",
  "hostname",
  "ifconfig [-a|en0|eth0|eth0 down]",
  "iwconfig [-a|eth0]",
  "cardctl [eject 0|insert 0]",
  "netstat [-ntuw]",
  "who",
  "ps [aux]",
  //"ifconfig eth0 | grep inet",
  null
};

UStr FinderPanel::commands = "browser, ufinder, Campus";
UStr VrengPanel::commands = "vreng, vreng-ogl";
UStr WebPanel::commands = "Firefox, Mozilla, Netscape";

static void copyBoxText(UEvent& e, UStr* target) {
  UBox* box = e.getSource()->toBox();
  if (box && target) box->retrieveText(*target);
}

/* ==================================================== [Elc:] ======= */

int main(int argc, char* argv[]) {
  UAppli appli(argc, argv, null, "~/.uremote");

  const char* hostname = null;
  if (argc == 2) hostname = argv[1];
  Remote remote(hostname);

  UFrame frame(utitle("UMS Remote Control") + usize(240, 320) + remote);
  
  appli.add(frame);
  frame.show();
  return appli.start();
}

/* ==================================================== [Elc:] ======= */

Remote::Remote(const char* hostname) :
  local_server(null), remote_server(null)
{
  // getHostbox returns host_boxes[0]
  UCombobox* cb = new UCombobox(ulistbox());
  host_boxes.push_back(cb);
  cb->addAttr(UOn::action / ucall(this, cb, &Remote::selectHost));
  cb->setEditable(true);
  cb->list().addItem("localhost");
  if (hostname) cb->list().addItem(hostname);

  host_boxes[0]->choice().setSelectedIndex(0);
  //startBrowsing(); deja fait implicitement par setSelectedIndex(0)
  //(ce qui est un BUG!)

  // ATTENTION: ces initialisations ne DOIVENT pas etre faites
  // dans la +list (car l'ordre d'evaluation depend des platforms)
  disp_panel = new DisplayPanel(this);
  macro_panel = new MacroPanel(this);
  macedit = new MacroEditor(this, macro_panel);

  cardbox.tablist().addAttr(UFont::small);
  cardbox.addTab("Disp",   *disp_panel);
  cardbox.addTab("Mouse",  *new MousePanel(this));
  cardbox.addTab("Finder", *new FinderPanel(this));
  cardbox.addTab("Web",    *new WebPanel(this));
  cardbox.addTab("Vreng",  *new VrengPanel(this));
  cardbox.addTab("Macro",  *macro_panel);
  cardbox.addTab("Unix",   *new UnixPanel());
  cardbox.addCard(*macedit);
  cardbox.add(alert);
  cardbox.setSelectedIndex(0);
               
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  add(ubackground(UPix::velin)
     + UOrient::vertical
     + uhflex()
     + uvflex()
     + uvbox(uhflex() + uvflex() + cardbox)
     );
}

void Remote::setMsg(UCombobox& cb, const UStr& combotext) {
  setMsg(&cb, combotext);
}
  
void Remote::setMsg(UCombobox* cb, const UStr& combotext) {
  if (cb) {
    cb->entry().removeAll();
    cb->entry().add(ustr(combotext));
   }
}

/* ==================================================== ====== ======= */
  
void Remote::sendMsg(UStr* target, const char* action) {
  if (!remote_server || !target  || !action) return;
  if (!remote_server->sendMessage(target->c_str(), action)) 
    hostDisconnected();
}

void Remote::sendBoxMsg(UEvent& e, UStr* target, const char* action) {
  if (remote_server && e.getSource() && target) { 
    // collates the text enclosed in the source and copies it to 's'
    UStr s = e.getSource()->toBox()->retrieveText();
    if (!remote_server->sendMessage(target->c_str(), action & s)) 
      hostDisconnected();
  }
}

void Remote::sendClick(UStr* target, WinPos* pos) {
  if (!remote_server || !target) return;
  if (!remote_server->sendMouseClick(target->c_str(), pos->x, pos->y, 
                                     UMouseEvent::LeftButton, 0))
    hostDisconnected();
}

void Remote::sendPress(UStr* target, WinPos* pos) {
  if (!remote_server || !target) return;
  if (!remote_server->sendMousePress(target->c_str(), pos->x, pos->y, 
                                     UMouseEvent::LeftButton, 0))
    hostDisconnected();
}

void Remote::sendRelease(UStr* target, WinPos* pos) {
  if (!remote_server || !target) return;
  if (!remote_server->sendMouseRelease(target->c_str(), pos->x, pos->y,
                                       UMouseEvent::LeftButton, 0))
    hostDisconnected();
}

void Remote::editPos(UMouseEvent& e, WinPos* pos) {
  //if (e.getButton() != UEvent::RightButton) return;
  //cerr << "editPos" << endl;
}

void Remote::press(UMouseEvent& e, int* pointer, int btn) {
  if (!remote_server) return;
  if (!remote_server->pressMouse(*pointer, (btn!=0 ? btn : e.getButtons())))
    hostDisconnected();
}

void Remote::release(UMouseEvent& e, int* pointer, int btn) {
  if (!remote_server) return;
  if (!remote_server->releaseMouse(*pointer, (btn!=0 ? btn : e.getButtons())))
    hostDisconnected();
}

void Remote::moveMouse(UMouseEvent& e, int* pointer, int* mag) {
  if (!remote_server) return;
  int x = int(e.getX() * *mag);
  int y = int(e.getY() * *mag);
  if (!remote_server->moveMouse(*pointer, x, y, true))
    hostDisconnected();
}

void Remote::pressKey(UKeyEvent& e, int* pointer) {
  if (!remote_server) return;
  if (!remote_server->pressKey(*pointer, e.getKeyCode()))
    hostDisconnected();
}

void Remote::releaseKey(UKeyEvent& e, int* pointer) {
  if (!remote_server) return;
  if (!remote_server->releaseKey(*pointer, e.getKeyCode()))
    hostDisconnected();
}

void Remote::serverRequest(UEvent& e, int request) {
  if (!remote_server) return;
  if (!remote_server->sendRequest(request))
    hostDisconnected();
}

/* ==================================================== ====== ======= */

bool Remote::isLocalConnected() {
  return (local_server && local_server->isConnected());
}

bool Remote::isHostConnected() {
  return (remote_server && remote_server->isConnected());
}

void Remote::hostDisconnected() {
  setMsg(getHostbox(), DISCONNECTED);
  delete remote_server;
  remote_server = null;
}

void Remote::startBrowsing() {
  if (local_server) delete local_server;
  local_server = new UMService("localhost");
  local_server->browseUMServers(ucall(this, &Remote::browseCB));
  local_server->browseUMSNeighbors(ucall(this, &Remote::neighborCB));
}

void Remote::browseCB(UMessageEvent& e) {
  UMService::BrowseReply r(e);
  if (r.serviceName.empty()) return;
  
  for (unsigned int k = 0; k < host_boxes.size(); k++) {
    UCombobox* cb = host_boxes[k];
    UBox* item = cb->choice().getSelectableItem(r.serviceName, false);
    
    if (r.isAdded()) {
      if (!item) cb->list().addItem(r.serviceName);
    }
    else if (r.isRemoved()) {
      if (item) {
        cb->list().remove(*item);
        if (cb->retrieveText() == r.serviceName)
          setMsg(cb, "DISCONNECTED: " & r.serviceName);
        if (cb == &getHostbox() && remote_server) {
          delete remote_server;
          remote_server = null;
        }
      }
    }
  }
}

void Remote::neighborCB(UMessageEvent& e) {
  UMService::ResolveReply r(e);
  
  if (r.fullname.empty()) r.fullname = "none";

  if (r.flags == 't') {
    disp_panel->top->entry().removeAll();
    disp_panel->top->entry().add(ustr(r.fullname));
    disp_panel->top->update();
  }
  else if (r.flags == 'b') {
    disp_panel->bottom->entry().removeAll();
    disp_panel->bottom->entry().add(ustr(r.fullname));
    disp_panel->bottom->update();
   }
  else if (r.flags == 'l') {
    disp_panel->left->entry().removeAll();
    disp_panel->left->entry().add(ustr(r.fullname));
    disp_panel->left->update();
  }
  else if (r.flags == 'r') {
    disp_panel->right->entry().removeAll();
    disp_panel->right->entry().add(ustr(r.fullname));
    disp_panel->right->update();
  }
}

/* ==================================================== ====== ======= */

static void cleanName(UStr& name) {
  // enlever ce qu'il y a avant la ] s'il y en a une
  int ix = name.find(']');
  if (ix >= 0) name.remove(0, ix+1);

  // enlever ce qu'il y a apres la ( s'il y en a une
  ix = name.find('(');
  if (ix >= 0) name.remove(ix, UStr::npos);

  // enlever les blancs autour
  name.trim();
}

/* ==================================================== ======== ======= */

void Remote::selectHostAction(UCombobox* cb, const UStr& name,
                              const UStr& address, int port) {
  delete remote_server;
  remote_server = null;

  if (address.empty()) {
    setMsg(cb, CANT_RESOLVE & name);
    return;
  }

  setMsg(cb, "Contacting " & name & " UMS server...");
  remote_server = new UMService(address, port);

  if (isHostConnected()) setMsg(cb, name & " (" & address & ")");
  else setMsg(cb, CANT_REACH & name & " (" & address & ")");
}

/* ==================================================== ======== ======= */

void Remote::selectHostCB(UMessageEvent& e, UCombobox* cb) {
  UMService::ResolveReply r(e);
  selectHostAction(cb, r.fullname, r.hosttarget, r.port);
}

/* ==================================================== ======== ======= */

void Remote::selectHost(UCombobox* cb) {
  if (!isLocalConnected())     // tenter une reconnection
    startBrowsing();

  if (!isLocalConnected()) {
    setMsg(getHostbox(), DISCONNECTED);
    alert.show("Can't connect to the local UMS server \n(start the 'umsd' program on this machine then try again)");
    return;
  }
  
  UStr name = cb->retrieveText();
  cleanName(name);
  
  if (name.empty()) name = "localhost";
  
  if (name == "localhost") {
    selectHostAction(cb, name, name, UMS_PORT_NUMBER);
  }
  else {
    setMsg(getHostbox(), "Resolving " & name & " ...");
    // NB: le ucall est detruit automatiquement
    local_server->resolveUMServer(name, ucall(this, cb, &Remote::selectHostCB));
  }
}

/* ==================================================== [Elc:] ======= */

void 
Remote::selectNeighborAction(UCombobox* cb, const char* dir, const UStr& name, 
                             const UStr& address, int port) {
  if (address.empty()) {
    // annule setting precedent
    local_server->sendRequest(UMSrequest::SET_NEIGHBOR, dir);
    setMsg(cb, CANT_RESOLVE & name);
    return;
  } 
  
  // ATT: envoyer a local_server pas remote_server!!
  // si address==empty la la topologie est annulee
  if (local_server->sendRequest(UMSrequest::SET_NEIGHBOR, dir & address))
    setMsg(cb, name & " (" & address & ")");
  else setMsg(cb, CANT_REACH & name & " (" & address & ")");
}

void Remote::selectNeighborCB(UMessageEvent& e, UCombobox* cb, const char* dir) {
  UMService::ResolveReply r(e);
  selectNeighborAction(cb, dir, r.fullname, r.hosttarget, r.port);
}

/* ==================================================== ======== ======= */

void Remote::selectNeighbor(UCombobox* cb, const char* dir) {
  if (!isLocalConnected())     // tenter une reconnection
    startBrowsing();

  if (!isLocalConnected()) {
    setMsg(getHostbox(), DISCONNECTED);
    alert.show("Can't connect to the local UMS server \n(start the 'umsd' program on this machine then try again)");
    return;
  }
  
  UStr name = cb->retrieveText();
  cleanName(name); 

  if (name.empty() || name == "none" || name == "localhost") {
    // address==empty la la topologie est annulee
    local_server->sendRequest(UMSrequest::SET_NEIGHBOR, dir);
  }
  else if (name == "localhost") {
    selectNeighborAction(cb, dir, name, name, UMS_PORT_NUMBER);
  }
  else {
    setMsg(cb, "Resolving " & name & " ...");
    // NB: le ucall est detruit automatiquement
    local_server->resolveUMServer(name, ucall(this,cb,dir,&Remote::selectNeighborCB));
  }
}

/* ==================================================== [Elc:] ======= */
/* ==================================================== ====== ======= */

DisplayPanel::DisplayPanel(Remote* rem) {
  top = new UCombobox(ulistbox());
  top->list().addItem("none");
  top->addAttr(UOn::action / ucall(rem, top,"top ",&Remote::selectNeighbor));
  rem->host_boxes.push_back(top);

  left = new UCombobox(ulistbox());
  left->list().addItem("none");
  left->addAttr(UOn::action / ucall(rem, left,"left ",&Remote::selectNeighbor));
  rem->host_boxes.push_back(left);

  right = new UCombobox(ulistbox());
  right->list().addItem("none");
  right->addAttr(UOn::action / ucall(rem, right,"right ",&Remote::selectNeighbor));
  rem->host_boxes.push_back(right);

  bottom = new UCombobox(ulistbox());
  bottom->list().addItem("none");
  bottom->addAttr(UOn::action / ucall(rem, bottom,"bottom ",&Remote::selectNeighbor));
  rem->host_boxes.push_back(bottom);

  USize& ww = usize(10|UEX);

  UBox& part1 = uvbox
    (uvspacing(3)
    + uhbox(UBorder::etchedOut 
	    + ulabel(ww + bold+ " " + USymbol::up + " Top:")
	    + uhflex() + top
	    )
     + uhbox(UBorder::etchedOut
	     + ulabel(ww + bold+ " " + USymbol::left + " Left:")
	     + uhflex() + left
	     )
     + uhbox(UBorder::etchedOut
	     + ulabel(ww + bold+ " "+ USymbol::right + " Right:") 
	     + uhflex() + right
	     )
     + uhbox(UBorder::etchedOut
	     + ulabel(ww + bold + " " +  USymbol::down + " Bottom:") 
	     + uhflex() + bottom
	     )
     );

  add(UOrient::vertical + upadding(5,2) + uvspacing(3)
     + uhbox(bold + UFont::large + uhcenter() + upadding(1,5) +" Displays ")
     + part1
     + " "
     + ubutton(uhcenter()  + UOrient::vertical
	       + UFont::bold + "Move Back Main Pointer"
               + ucall(rem, (int)UMSrequest::MOVE_BACK_POINTER, &Remote::serverRequest))

     + ulabel(uhcenter()  + UOrient::vertical + UFont::small
	      + "you can also:"
              + "click simultaneously on left and right buttons"
              + "click several times on the orange window")
     );
}

/* ==================================================== ====== ======= */

void toInt(UEvent& e, int* val) {
  UStr s = e.getSource()->toBox()->retrieveText();
  if (!s.empty()) *val = atoi(s.c_str());
}

void toFloat(UEvent& e, float* val) {
  UStr s = e.getSource()->toBox()->retrieveText();
  if (!s.empty()) *val = atof(s.c_str());
}

void MousePanel::pressPad(UMouseEvent& e, int* pointer) {
  if (!no_drag_btn->isSelected()) remote.press(e, pointer, 0);
}

void MousePanel::releasePad(UMouseEvent& e, int* pointer) {
  if (!no_drag_btn->isSelected()) remote.release(e, pointer, 0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

MousePanel::MousePanel(Remote* rem) : remote(*rem) {
  // MousePad
  const char* ptr_values[] = {" 0 ", " 1 ", " 2 ", " 3 ", " 4 ", " 5 ", null};
  const char* mag_values[] = {" 1 ", " 2 ", " 3 ", " 4 ", " 5 ", " 6 ", null};
 
  UCombobox& ptr_list = ucombobox(ulistbox().addItems(ptr_values), 
                                  UOn::action / ucall(&ptr, toInt));
  ptr_list.choice().setSelectedIndex(1);

  UCombobox& mag_list = ucombobox(ulistbox().addItems(mag_values),
                                  UOn::action / ucall(&mag, toInt));
  mag_list.choice().setSelectedIndex(1);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  no_drag_btn = &ucheckbox("Move Only");
  
  UBox& misc_controls = uhbox
    (uhflex() + UFont::small
     + no_drag_btn
     + ubutton(" LEFT "
               + UOn::mpress  /ucall(rem,&ptr,int(UMouseEvent::LeftButton), &Remote::press)
               + UOn::mrelease/ucall(rem,&ptr,int(UMouseEvent::LeftButton),
               &Remote::release))
     + ubutton(" MIDDLE "
               + UOn::mpress  /ucall(rem,&ptr,int(UMouseEvent::MidButton), &Remote::press)
               + UOn::mrelease/ucall(rem,&ptr,int(UMouseEvent::MidButton), &Remote::release))
     + ubutton(" RIGHT "
               + UOn::mpress  /ucall(rem,&ptr,int(UMouseEvent::RightButton), &Remote::press)
               + UOn::mrelease/ucall(rem,&ptr,int(UMouseEvent::RightButton), &Remote::release))
     + ubutton("Calibrate"
               + ucall(rem, (int)UMSrequest::CALIBRATE, &Remote::serverRequest))
     + "  "
     );

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  add(UOrient::vertical 
      + utop()
      + uhbox(bold + UFont::large + uhcenter() + upadding(1,5) + " Mouse Pad ")
      
      + uhbox(ulabel(UFont::bold + "Host: ") + uhflex() + rem->getHostbox())
      + uhbox(uhcenter() + upadding(1,1) + UFont::small //+ UFont::italic
              + "Pointer: " + ptr_list + "  Mag: "   + mag_list)
      + uvflex()
      + ubox(UBackground::white
             + UOn::mmove    / ucall(rem,  &ptr,&mag, &Remote::moveMouse)
             // do nothing if no_drag_btn is selected.
             + UOn::mpress   / ucall(this, &ptr, &MousePanel::pressPad)
             + UOn::mrelease / ucall(this, &ptr, &MousePanel::releasePad)
             + UOn::kpress   / ucall(rem, &ptr, &Remote::pressKey)
             + UOn::krelease / ucall(rem, &ptr, &Remote::releaseKey)
             + uhcenter() + uvcenter()
             + uvbox(UFont::italic
                     + "You can move/drag the mouse"+"or type text in this area"
                     ).ignoreEvents()
             )
      + ubottom()
      + misc_controls
      );
}

/* ==================================================== [Elc:] ======= */

static const char *highlight_xpm[] = {
"    16    12        2            1",
". c None s None",
"# c orange",
  "................",
  "......#####.....",
  "....########....",
  "...##########...",
  "..############..",
  ".##############.",
  "#######...######",
  "#####......#####",
  "####........####",
  "###.........####",
  "##...........###",
  "................."
};

FinderPanel::FinderPanel(Remote* rem) {
  UStr* files = null;
  UAttrList* props = UAppli::conf.getAttributes("finder");
  if (props) files = props->getAttr("files");

  UCombobox& com_box =
  ucombobox(ulistbox().addItems(commands), UOn::action / ucall(&target, copyBoxText));
  target = com_box.retrieveText();

  UListbox& lb = ulistbox();
  
  if (files) lb.addItems(*files);
  UCombobox& doc_box = ucombobox(lb, ucall(rem, &target, "open ", &Remote::sendBoxMsg));

  UCombobox* host_box = new UCombobox(ulistbox());
  rem->host_boxes.push_back(host_box);
  host_box->addAttr(ucall(rem, &target, "clone ", &Remote::sendBoxMsg));

  USize& ww = usize(7|UEX);
  UBox& part1 = uvbox
    (uvspacing(3)
     + uhbox(uleft() + ulabel(bold + ww + "Host:") + uhflex() + rem->getHostbox())
     + uhbox(uleft() + ulabel(bold + ww + "Appli:") + uhflex() + com_box)
     + uhbox(uleft() + ulabel(bold + ww + "Open:")  + uhflex() + doc_box)
     + uhbox(uleft() + ulabel(bold + ww + "Clone:") + uhflex() + host_box)
     );

  // colors
  const char* colnames[] = {"yellow","orange","blue","red","green","navy"};
  int colcount = sizeof(colnames) / sizeof(colnames[0]);
 
  // modes
  UBox& part2 = uhbox
    (uhflex()
     + ubutton(upix(UXpm::select)
               + ucall(rem, &target, "mode select", &Remote::sendMsg))
     + ubutton(upix(UXpm::edit)
               + ucall(rem, &target, "mode write", &Remote::sendMsg))
     + ubutton(upix(highlight_xpm)
               + ucall(rem, &target, "mode highlight", &Remote::sendMsg))
     );
  
  for (int k = 0; k < colcount; k++) {
    UStr* com = new UStr("color ");
    com->append(colnames[k]);
    UBox& b = ubutton(ubackground().setNamedColor(colnames[k]) + "    "
                      + ucall(rem, &target, com->c_str(), &Remote::sendMsg));
    part2.add(b);
  };
  
  
  // navigation  
  UBox& part3 = uhbox
    (uhcenter() + uvcenter()
     + uitem(uhcenter() + UPix::bigLeft
             + ucall(rem, &target, "previous", &Remote::sendMsg))
     + "  "
     + uvbox(uhcenter() + uvcenter()
             + uitem(uhcenter() + UPix::bigUp
                     + ucall(rem, &target, "parent", &Remote::sendMsg))
             +  "  "
             + uitem(uhcenter() + UPix::bigDown
                     + ucall(rem, &target, "open", &Remote::sendMsg))
             )
     + "  "
     + uitem(uhcenter() + UPix::bigRight
             + ucall(rem, &target, "next", &Remote::sendMsg))
     );
  
  add(UOrient::vertical + upadding(5,2) + uvspacing(3)
      + utop()
      + uhbox(bold + UFont::large + uhcenter() + upadding(1,5) + " Finder ")
      + uvflex()
      + part1
      + part2
      + part3
      );
}

/* ==================================================== [(c)Elc] ======= */

// platform and appli dependant!
WebPanel::Buttons WebPanel::btn = {
  {17,37},  //back
  {57,41},  //forward
  {97,37},  //reload 
  {127,37}, //stop
  {157,37}, //home
  {24, 80}, //previous_slide
  {50, 80}, //next_slide
  {80, 80}  //top_slide
};

static void getButtonPos(UAttrList* props, WinPos& btnpos, const char* name) {
 UStr* val = null;
 if ((val = props->getAttr(name))) {
   int sep = val->find(',');
   if (sep > 0) {
     btnpos.x = val->toInt();
     val->remove(0,sep+1);
     btnpos.y = val->toInt();
   }
 }
}

WebPanel::WebPanel(Remote* rem) {

 UStr* urls = null;
  UAttrList* props = UAppli::conf.getAttributes("web");
  if (props) urls = props->getAttr("urls");

  props = UAppli::conf.getAttributes("firefox");
  if (props) {
    getButtonPos(props, btn.back, "back");
    getButtonPos(props, btn.forward, "forward");
    getButtonPos(props, btn.reload, "reload");
    getButtonPos(props, btn.stop, "stop");
    getButtonPos(props, btn.home, "home");
    getButtonPos(props, btn.previous_slide, "previous_slide");
    getButtonPos(props, btn.next_slide, "next_slide");
    getButtonPos(props, btn.top_slide, "top_slide");
  }
 
  UCombobox& com_box = ucombobox(ulistbox().addItems(commands),
                                  ucall(&target, copyBoxText));    
  target = com_box.retrieveText();

  UListbox& lb = ulistbox();
  if (urls) lb.addItems(*urls);
  UCombobox& doc_box = 
    ucombobox(lb, ucall(rem, &target, "open ", &Remote::sendBoxMsg));

  USize& ww = usize(7|UEX);
  UBox& part1 = uvbox
    (uvspacing(3)
     + uhbox(uleft() + ulabel(bold + ww + "Host:") + uhflex() + rem->getHostbox())
     + uhbox(uleft() + ulabel(bold + ww + "Appli:") + uhflex() + com_box)
     + uhbox(uleft() + ulabel(bold + ww + "Open:") + uhflex() + doc_box)
     );
  
  UBox& part2 = uhbox
    (uhcenter() + uvflex() + uhspacing(10)
     + ubutton(UBorder::etchedIn + uhcenter() + " Reload "
               + ucall(rem, &target, &btn.reload, &Remote::sendClick)
               + UOn::mpress / ucall(rem, &btn.reload, &Remote::editPos)
               )
     + ubutton(UBorder::etchedIn + uhcenter() + "  Stop  "
               + ucall(rem, &target, &btn.stop, &Remote::sendClick)
               + UOn::mpress / ucall(rem, &btn.stop, &Remote::editPos)
               )
     );
  
  UBox& part3 = uhbox
    (uhflex() + uvflex() + uhspacing(7)
     + ubutton(UBorder::etchedIn + uhcenter() + "  Back  "
               + ucall(rem, &target, &btn.back, &Remote::sendClick)
               + UOn::mpress / ucall(rem, &btn.back, &Remote::editPos)
               )
     + ubutton(UBorder::etchedIn +uhcenter() + "  Home  "
               + ucall(rem, &target, &btn.home, &Remote::sendClick)
               + UOn::mpress / ucall(rem, &btn.home, &Remote::editPos)
               )
     + ubutton(UBorder::etchedIn + uhcenter() + "Forward"
                + ucall(rem, &target, &btn.forward, &Remote::sendClick)
                + UOn::mpress / ucall(rem, &btn.forward, &Remote::editPos)
                )
     );
  
  UBox& part4 = uhbox
    (uhcenter() + uvcenter()
     + uitem(uhcenter() + UPix::bigLeft
             + ucall(rem, &target, &btn.previous_slide, &Remote::sendClick)
             + UOn::mpress / ucall(rem, &btn.previous_slide, &Remote::editPos)
             )
     + "   "
     + uitem(uhcenter() + UPix::bigUp
             + ucall(rem, &target, &btn.top_slide, &Remote::sendClick)
             + UOn::mpress / ucall(rem, &btn.top_slide, &Remote::editPos)
             )
     + "   "
     + uitem(uhcenter() + UPix::bigRight
             + ucall(rem, &target, &btn.next_slide, &Remote::sendClick)
             + UOn::mpress / ucall(rem, &btn.next_slide, &Remote::editPos)
             )
     );

  add(UOrient::vertical + upadding(5,2) + uvspacing(5)
      + uhbox(bold + UFont::large + uhcenter() + upadding(1,5) + " Web Browser ")
      + uvflex()
      + part1
      + part2
      + part3
      + " "
      + part4
      + " "
      );
}

/* ==================================================== ======== ======= */

VrengPanel::VrengPanel(Remote* rem) {
 UStr* worlds = null;
  UAttrList* props = UAppli::conf.getAttributes("vreng");
  if (props) worlds = props->getAttr("worlds");

  UCombobox& com_box = ucombobox(ulistbox().addItems(commands),
                                  ucall(&target, copyBoxText));
  target = com_box.retrieveText();

  UListbox& lb = ulistbox();
  if (worlds) lb.addItems(*worlds);
  UCombobox& world_box =
    ucombobox(lb, ucall(rem, &target, "file ", &Remote::sendBoxMsg));

 UCombobox& put_box =
   ucombobox(ulistbox(), ucall(rem, &target, "put ", &Remote::sendBoxMsg));

  USize& ww = usize(7|UEX);
  UBox& part1 = uvbox
    (uvspacing(3)
     + uhbox(uleft() + ulabel(bold + ww + "Host:") + uhflex() + rem->getHostbox())
     + uhbox(uleft() + ulabel(bold + ww + "Appli:") + uhflex() + com_box)
     + uhbox(uleft() + ulabel(bold + ww + "World:") + uhflex() + world_box)
     + uhbox(uleft() + ulabel(bold + ww + "Put:") + uhflex() + put_box)
     );

  UBox& part2 = uhbox
  (uhflex() + uvflex()
     + uhspacing(3)
     + ubutton(UBorder::etchedIn + uhcenter() + " Back  "
               + ucall(rem, &target, "back", &Remote::sendMsg))
     + ubutton(UBorder::etchedIn + uhcenter() + "Forward"
               + ucall(rem, &target, "forward", &Remote::sendMsg))
     + ubutton(UBorder::etchedIn + uhcenter() + " Home  "
               + ucall(rem, &target, "home", &Remote::sendMsg))
     + ubutton(UBorder::etchedIn + uhcenter() + " Enter  "
               + ucall(rem, &target, "enter", &Remote::sendMsg))
     );

  UBox& part3 = uhbox
    (uhcenter() + uvcenter()
     // left column
     + uvbox(uhcenter() + uvflex()
             + uitem(uscale(1.3) + uhcenter()+ uvcenter() + upix(UXpm::uparrow)
                     + UOn::mpress / ucall(rem,&target,
                                           "move up 1", &Remote::sendMsg)
                     + UOn::mrelease / ucall(rem,&target,
                                             "move up 0", &Remote::sendMsg)
                     )
             + uitem(uhcenter() + UPix::bigLeft
                     + UOn::mpress / ucall(rem,&target, 
                                           "move turn left 1", &Remote::sendMsg)
                     + UOn::mrelease / ucall(rem,&target, 
                                             "move turn left 0", &Remote::sendMsg)
                     )
             + uitem(uscale(1.3) + uhcenter()+ uvcenter() + upix(UXpm::leftarrow)
                     + UOn::mpress / ucall(rem, &target, 
                                           "move left 1", &Remote::sendMsg)
                     + UOn::mrelease / ucall(rem, &target,
                                             "move left 0", &Remote::sendMsg)
                     )
             )
     
     // middle column
     + uvbox(uhcenter() + uvflex()
             + uitem(uhcenter() + UPix::bigUp
                     + UOn::mpress / ucall(rem, &target,
					   "move forward 1",&Remote::sendMsg)
                     + UOn::mrelease/ucall(rem, &target,
				           "move forward 0",&Remote::sendMsg)
                     )     
             +  "  "
             + uitem(uhcenter() + UPix::bigDown
                     + UOn::mpress / ucall(rem, &target,
					   "move backward 1",&Remote::sendMsg)
                     + UOn::mrelease/ucall(rem, &target, 
				           "move backward 0",&Remote::sendMsg)
                     )
             )

     // right column
     + uvbox(uhcenter() + uvflex()
	     + uitem(uscale(1.3) + uhcenter() + uvcenter()+ upix(UXpm::downarrow)
		     + UOn::mpress / ucall(rem, &target, 
					   "get", &Remote::sendMsg)
		     )
             + uitem(uhcenter() + UPix::bigRight
		     + UOn::mpress/ucall(rem, &target, 
					 "move turn right 1", &Remote::sendMsg)
		     + UOn::mrelease/ucall(rem, &target,
					     "move turn right 0",&Remote::sendMsg)
		     )
	     + uitem(uscale(1.3) + uhcenter() + uvcenter()+ upix(UXpm::rightarrow)
		     + UOn::mpress / ucall(rem, &target,
					   "move right 1", &Remote::sendMsg)
		     + UOn::mrelease / ucall(rem, &target,
					     "move right 0", &Remote::sendMsg)
		     )
	     )
     );

  add(UOrient::vertical + upadding(5,2) + uvspacing(3)
      + uhbox(bold + UFont::large + uhcenter() + upadding(1,5) +" Vreng ")
      + uvflex()
      + part1
      + part2
      + part3
      );
}

/* ==================================================== ======== ======= */

MacroPanel::MacroPanel(Remote* _rem) : rem(*_rem) {
  add(UOrient::vertical + upadding(7,7) + uvspacing(10)
      + utop()
      + uhbox(uhcenter() + bold + UFont::large + "Macros")
      + uhbox(uleft() + ulabel(UFont::bold + "Host: ") + uhflex() + rem.getHostbox())
      + uvflex()
      + uscrollpane(macro_list)
      + ubottom()
      + uhbox(uhcenter() + bold
              + ubutton(UBorder::etchedIn + " New  "
                        + ucall(this, &MacroPanel::createMacro))
              + " " + ubutton(UBorder::etchedIn +" Edit "
                              + ucall(this, &MacroPanel::editMacro))
              + " " + ubutton(UBorder::etchedIn + "Delete"
                              + ucall(this, &MacroPanel::deleteMacro))
              )
      + statusbox);
  
  statusbox.addAttr(UColor::red);
  statusbox.addAttr(UFont::bold);
  statusbox.addAttr(uhcenter());
}

void MacroPanel::createMacro() {
  rem.macedit->open(null);
}

void MacroPanel::editMacro() {
  UElem* item = macro_list.choice().getSelectedItem();
  Macro* m = (item ? dynamic_cast<Macro*>(item) : null);
  if (m) rem.macedit->open(m);
}

void MacroPanel::deleteMacro() {
  UElem* item = macro_list.choice().getSelectedItem();
  Macro* m = (item ? dynamic_cast<Macro*>(item) : null);
  if (m) macro_list.remove(*m);
}

void MacroPanel::showStatus(const UStr& s) {
  statusbox.removeAll();
  if (s.empty())  statusbox.add(ustr(" "));
  else statusbox.add(ustr(s));
}

/* ==================================================== ======== ======= */

Macro::Macro(MacroEditor* e) {
  mtype = Click;
  pos.x = pos.y = 0;
  add(USymbol::right + " " + name + ucall(e, this, &MacroEditor::execMacro));
}

MacroEditor::MacroEditor(Remote* r, MacroPanel* mp) : 
  rem(*r), macpanel(*mp), count(0), already_in_list(false) 
{
  UBox& table = ubar
    (UOrient::vertical + uhflex() + uvspacing(5)
     + uhbox("Name:  " + uhflex() + namebox.add(name))
     + uhbox("Target: " + uhflex() + targetbox.add(target)
             + uright() + ubutton(UColor::red + " Select " 
                                  + ucall(this, &MacroEditor::selectTarget))
	     )
     + uhbox(ucheckbox(" MouseClick" + mtypesel + UOn::select / ucopy(mtype, Click)
                       ).setSelected()
             + uhflex() + " x:" + xbox + " y:" + ybox
             )
     + uhbox(ucheckbox(" Message" + mtypesel
                       + UOn::select / ucopy(mtype, Message))
             + uhflex() + mesgbox.add(mesg)
             )
     );

  // -------------------------------------------------------------------

  add(UOrient::vertical + upadding(1,7) + uvspacing(10)
      + uhbox( bold + UFont::large + uhcenter() + title)
      + uvflex() 
      + table
      + "  "
      + uhbox(uhcenter()
              + ubutton(UBorder::etchedIn + UFont::large + UFont::bold
                        + UColor::blue + " Test Macro " 
                        + ucall(this, (Macro*)0, &MacroEditor::execMacro))
              )
      + " "
      + macpanel.statusbox
      + ubottom()
      + uhbox(uhflex() + upadding(4,8)
              + UFont::bold
              + ubutton(UBorder::etchedIn +"Save " + ucall(this, &MacroEditor::save))
              + ubutton(UBorder::etchedIn+ "Cancel"+ ucall(this, &MacroEditor::cancel))
              )
      );
}

/* ==================================================== ======== ======= */

void MacroEditor::open(Macro* m) {
  //showCard(&rem.cardbox, this);
  rem.cardbox.setSelectedIndex(7);

  macpanel.showStatus("");

  if (m) {
    already_in_list = true;
    title = "Edit Macro";
    macro = m;
  }
  else {
    already_in_list = false;
    title = "New Macro";
    if (!macro) {
      macro = new Macro(this);
      macro->name = UStr("macro ") & count;
    }
  }
  
  mtype = macro->mtype;
  mtypesel.setSelectedIndex(macro->mtype);
  name = macro->name;
  target = macro->target;
  mesg = macro->mesg;
  xbox.value() = macro->pos.x;
  ybox.value() = macro->pos.y;
}

/* ==================================================== ======== ======= */

void MacroEditor::cancel() {
  //showCard(&rem.cardbox, &macpanel);
  rem.cardbox.setSelectedIndex(6);
  macro = null; // detruire la macro
}

void MacroEditor::save() {
  syncData();
  macpanel.showStatus("");

  //showCard(&rem.cardbox, &macpanel);
  rem.cardbox.setSelectedIndex(6);

  if (!already_in_list && macro) 
    macpanel.macro_list.add(*macro);
  macpanel.macro_list.choice().setSelectedItem(*macro);

  already_in_list = true;
  macro = null;
  count++;
}

/* ==================================================== ======== ======= */

void MacroEditor::syncData() {
  macro->mtype = mtype;
  macro->name = name;
  macro->target = target;
  macro->mesg = mesg;
  macro->pos.x = xbox.value();
  macro->pos.y = ybox.value();
}

/* ==================================================== ======== ======= */

void MacroEditor::execMacro(Macro* m) {
  macpanel.showStatus("");

  if (!m) {
    if (macro) {syncData(); m = macro;}
  }
  if (!m) return;
  
  if (m->target.empty()) 
    macpanel.showStatus("No window specified");
  else if (mtype == Click)
    rem.sendClick(&m->target, &m->pos);
  else if (mtype == Message) {
    if (m->mesg.empty()) 
      macpanel.showStatus("Empty message");
    else
      rem.sendMsg(&m->target, m->mesg.c_str());
  }
}

/* ==================================================== ======== ======= */

void MacroEditor::selectTarget() {
#if WITH_X11
  int x, y;
  static UHardWinX11* win = (UHardWinX11*)UAppli::getDisp()->createWinImpl(null);
  bool stat = UAppli::getDisp()->pickWindow(x, y, win);
  if (!stat) {
    macpanel.showStatus("Can't grab the mouse");
    cerr << "Can't grab the mouse"<< endl;
  }
  else if (win->getSysWin() != None) {
    char buf[20];
    sprintf(buf, "0x%lx", long(win->getSysWin()));
    target = buf;
    xbox.value() = x;
    ybox.value() = y;
  }
#else 
  macpanel.showStatus("Only available with X11");
  cerr << "Only available with X11" << endl;
#endif
}

/* ==================================================== [(c)Elc] ======= */

UnixPanel::UnixPanel() : spane(true, false) {
  
  for (int k = 0; commands[k] != null; k++) createComButtons(commands[k]);
  cardbox.setSelectedIndex(0);
  
  spane.add(utextarea(font + results));
  
  UBox& size_box = uhbox
    (uchoice()
     + ulabel("Font size:")
     + ubutton("normal" + uassign(font, UFont::medium))
     + ubutton("small"  + uassign(font, UFont::small)) 
     + ubutton("xsmall" + uassign(font, UFont::x_small))
     );
  
  add(UOrient::vertical 
      + utop() + cardbox 
      + uvflex() + spane
      + ubottom() + size_box
      );
}

/* ==================================================== ======== ======= */
// exple: compsec= "ls [-a|-la]"

void UnixPanel::createComButtons(const char* comspec) {
  if (!comspec) return;
  
  const char* p1 = strchr(comspec, '[');
  UStr* com = new UStr();
  
  if (!p1) *com = comspec;
  else {
    com->append(comspec, p1-comspec);  // exple: creates "ls"
    if (com->at(-1) == ' ') com->remove(-1, 1); // remove last wspace
  }
  
  // creates the option box of this command
  UBox& com_list = uhbox
    (uchoice()
     + uhspacing(6)
     + ulabel(UFont::bold + "Run: ")
     + ulinkbutton(com + ucall(this, com, &UnixPanel::execCom)) 
     );
   
  cardbox.addTab(com, com_list);
  
  // adds the options to the optbox
  while (p1) {
    p1++;
    
    const char* p2 = strpbrk(p1, "|]");
    if (p2 && p2 > p1) {  
      UStr* opt = new UStr();
      opt->append(p1, p2-p1);   // exple: creates "-a"
      
      UStr* optcom = new UStr(*com);
      optcom->append(" ");
      optcom->append(*opt);    // exple: creates "ls -a"
      
      // creates opt button and adds to opt_box
      com_list.add( ulinkbutton(opt + ucall(this, optcom, &UnixPanel::execCom)) );
    }
    p1 = p2;
  }
}

/* ==================================================== ======== ======= */

void UnixPanel::execCom(UStr* com) {
  spane.setScroll(0,0);
  results = "";
  
  UStr comline = *com & " 2>&1";
  FILE* p = popen(comline.c_str(),"r");
  UStr s;
  
  char buf[500];
  while (fgets(buf, sizeof(buf), p)) {
    if (buf[0] != '\t') s &= buf; else {s &= " - "; s &= (buf+1);}
  }
  pclose(p);
  results = s; //single update
}

/* ==================================================== [TheEnd] ======= */

