/* ==================================================== ======== ======= *
*
*  uremote.hpp : Remote Control for the UMS server
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

#ifndef _uremote_hpp_
#define _uremote_hpp_

#include <ubit/ubit.hpp>
#include <ubit/umessage.hpp>
#include <ubit/umsproto.hpp>
#include <ubit/umservice.hpp>
using namespace ubit;

struct WinPos {
  int x, y;
};

/* ==================================================== ======== ======= */

class Remote : public UBox {
public:
  Remote(const char* hostname = null);
  void sendMsg(UStr* target, const char* action);
  void sendBoxMsg(UEvent&, UStr* target, const char* action);
  void sendClick(UStr* target, WinPos* pos);
  void sendPress(UStr* target, WinPos* pos);
  void sendRelease(UStr* target, WinPos* pos);
  void press(UMouseEvent&, int* pointer, int btn);
  void release(UMouseEvent&, int*  pointer, int btn);
  void moveMouse(UMouseEvent&, int* pointer, int* mag);
  void pressKey(UKeyEvent&, int*  pointer);
  void releaseKey(UKeyEvent&, int*  pointer);
  void editPos(UMouseEvent&, WinPos*);
  void serverRequest(UEvent&, int request);

  void startBrowsing();
  bool isLocalConnected();
  bool isHostConnected();
  void hostDisconnected();
  
  UCombobox& getHostbox() {return *host_boxes[0];}
  void selectHost(UCombobox*);
  void selectNeighbor(UCombobox* cb, const char* dir);
  void setMsg(UCombobox&, const UStr& combotext);
  void setMsg(UCombobox*, const UStr& combotext);
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  UMService *local_server, *remote_server;
  UCardbox cardbox;
  UAlertbox alert;
  std::vector<UCombobox*> host_boxes;
  class DisplayPanel* disp_panel;
  class MacroPanel* macro_panel;
  class MacroEditor* macedit;

protected:
  void browseCB(UMessageEvent&);
  void neighborCB(UMessageEvent&);
  void selectHostCB(UMessageEvent&, UCombobox*);
  void selectNeighborCB(UMessageEvent&, UCombobox*, const char* dir);
  void selectHostAction(UCombobox*, const UStr& name, const UStr& address, int port);
  void selectNeighborAction(UCombobox*, const char* dir, const UStr& name,
                            const UStr& address, int port);
};

/* ==================================================== ======== ======= */

struct DisplayPanel : public UBox {
  DisplayPanel(Remote*);
  UCombobox *top, *left, *right, *bottom;
};

/* ==================================================== ======== ======= */

struct MousePanel : public UBox {
  MousePanel(Remote*);
private:
  Remote& remote;
  int ptr, mag;
  UBox* no_drag_btn;
  void pressPad(UMouseEvent& e, int* pointer);
  void releasePad(UMouseEvent& e, int* pointer);
  ///< do nothing if no_drag_btn is selected.
};

/* ==================================================== ======== ======= */

class UnixPanel : public UBox {
public:
  static const char* commands[];
  UnixPanel();
private:
  UFont font;
  UCardbox cardbox;     // box that contains the buttons
  UScrollpane spane;    // scrollpane that contains resbox
  UStr results;          // string that contains the result
  void createComButtons(const char* comspec);
  void execCom(UStr* com);
};

/* ==================================================== ======== ======= */

struct FinderPanel : public UBox {
  static UStr commands;
  FinderPanel(Remote*);
private:
  UStr target;
};

/* ==================================================== ======== ======= */

struct WebPanel : public UBox {
  struct Buttons {
    WinPos back, forward, reload, stop, home,
      previous_slide, next_slide, top_slide;
  };
  static UStr commands;
  WebPanel(Remote*);
private:
  UStr target;
  static Buttons btn;
};

/* ==================================================== ======== ======= */

struct VrengPanel : public UBox {
  static UStr commands;
  VrengPanel(Remote*);
protected:
  UStr target;
};

/* ==================================================== ======== ======= */

class MacroPanel : public UBox {
public:
  MacroPanel(Remote*);
  void createMacro();
  void editMacro();
  void deleteMacro();
  void showStatus(const UStr&);
  Remote& rem;
  UListbox macro_list;
  UBox statusbox;
};

enum MacroType {Click, Message};

struct Macro : public UItem {
  Macro(class MacroEditor*);
  MacroType mtype;
  UStr name, target, mesg;
  WinPos pos;
};

class MacroEditor : public UBox {
public:
  MacroEditor(Remote*, MacroPanel*);
  void open(Macro*);
  void save();
  void cancel();
  void selectTarget();
  void execMacro(Macro*);
  void syncData();

protected:
  Remote& rem;
  MacroPanel& macpanel;
  int count;
  bool already_in_list;
  uptr<Macro> macro;
  MacroType mtype;
  UStr title, name, target, mesg;
  URadioSelect mtypesel;
  UTextfield namebox, targetbox, mesgbox;
  USpinbox xbox, ybox;
};

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */
#endif
