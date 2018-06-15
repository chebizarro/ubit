/* ***********************************************************************
 *
 *  urem.cpp: universal remote control for the WiiMote
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
#include <vector>
#include <cstdio>
#include <cstdio>
#include <ubit/ubit.hpp>
#include "urem.h"
using namespace std;
using namespace ubit;

const char* WIIMOTE_APP = "/Users/elc/ubit/darwin-remote/DarwinRemote/build/Debug/WiiRemote.app/Contents/MacOS/WiiRemote";

UPoint REMOTE_LOCATION(40, 150);
static const unsigned long LONG_PRESS_DELAY = 800; // msec
const unsigned long MODE_SWITCH_DELAY = 330; // msec

extern const char *radio_xpm[], *tv_xpm[], *itunes_xpm[], *dvd_xpm[];

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class Component {
public:
  class URem& md;
  UStr altLeftName, altRightName, altUpName, altDownName;
  
  Component(URem* _md) : md(*_md) {}
  virtual ~Component() {}
  
  virtual void show(bool state) {}
  virtual void play(bool state) {}
  virtual void playTrack(int no) {}

  virtual void left() {}
  virtual void right() {}
  virtual void up() {}
  virtual void down() {}
  
  virtual void altLeft() {}
  virtual void altRight() {}
  virtual void altUp() {}
  virtual void altDown() {}
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class URem : public UFrame {
public:  
  URem();
  ~URem();
  
  void setAllOff();
  void play(bool state) {if (player) player->play(state);}
  void playTrack(int n) {if (player) player->playTrack(n);}
  
  void left()     {if (player) player->left();}
  void right()    {if (player) player->right();}
  void up()       {if (player) player->up();}
  void down()     {if (player) player->down();}
  void altLeft()  {if (player) player->altLeft();}
  void altRight() {if (player) player->altRight();}
  void altUp()    {if (player) player->altUp();}
  void altDown()  {if (player) player->altDown();}
  
  void screenOnOff();
  void screenOn();
  void screenOff();
  void screenZoom();
  void screenSourceSwitch();
  void screenSourcePC();
  void screenSourceCable();
  
  void ampOff();
  void ampVolIncr();
  void ampVolDecr();
  void ampSourceRadio();
  void ampSourceITunes();
  void ampSourceDVD();
  void ampSourceCable();
  
  void createGUI();
  void showMsg(const char*);
  void showWiimoteState(bool is_connected);
  void fromWiimote();
  int  getModeSwitch() const {return mode_switch;}
  void setModeSwitch(int level) {mode_switch = level;}
  void setPlayerCB(class Component*);

  void tellAppli(const char* appli, const char* command);
  void tellAppli(const char* appli, const char* command, int no);
  void tellAppli(const char* appli, const char* command, bool state);
  void callScript(const char* command);
  void sendIRCode(const char* command);
  void sendIRCode(const char* command, int no);
  void say(const char* sentence);
  void say(const char* sentence, int no);
  void beep(int = 1);
  void systemSleep();
  virtual void close(int status) {}  // redefinition (defined in UFrame) 
  
private:
  UBox players_box, navig_box, alt_box, pad_box;
  URadioSelect players_choice;
  UFlowbox alert_box;
  UStr alert_msg_str;
  Component *radio, *itunes, *dvd, *cable, *player;
  uptr<UBox> leftPlayerItem, rightPlayerItem, upPlayerItem, downPlayerItem;
  UStr altLeftName, altRightName, altUpName, altDownName;
  bool screen_source_is_tv;
  FILE* pipe;
  USource psource;
  bool is_wiimote_connected;
  int mode_switch;
  int last_button;
  unsigned long last_press_time;
  vector<bool> pressed_buttons;
  //UTimer timer;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class ITunes : public Component {
  bool show_state;
public:
  void show(bool state) {
    show_state = state;
    md.tellAppli("ITunes","set visible of window 1 to ", state);
  }
  void play(bool state) {md.tellAppli("ITunes", state? "play" :"pause");} // stop
  void playTrack(int n) {md.tellAppli("ITunes","play track ", n);}

  void left()     {md.tellAppli("ITunes","back track");}
  void right()    {md.tellAppli("ITunes","next track");}
  void up()       {showHide();}
  void down()     {playPause();}
  
  void altLeft()  {}
  void altRight() {md.tellAppli("ITunes","eject");}
  void altUp()    {showHide();}
  void altDown()  {playPause();}

  void showHide() {show_state = !show_state; show(show_state);}
  void playPause(){md.tellAppli("ITunes","playpause");}
  
  // md.tellAppli("ITunes","set visible of EQ windows to true");}
  
  ITunes(URem* _md) : Component(_md), show_state(false) {
    // show_state may not reflect actual visibility, it just serves to exchange states
    altLeftName  = "";
    altRightName = "Eject";
    altUpName    = "Show";
    altDownName  = "Play";
  }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class Dvd : public Component {
  bool show_state, play_state;
public:
  void show(bool state) {
    show_state = state;
    if (state) md.callScript("activate application \"DVD Player\"");
    md.tellAppli("DVD Player","set viewer visibility to ", state);
  }
  void play(bool state) {
    play_state = state;
    md.tellAppli("DVD Player", (state? "play dvd": "pause dvd"));
  }
  void playTrack(int n) {}

  void left()     {md.tellAppli("DVD Player","play previous chapter");}
  void right()    {md.tellAppli("DVD Player","play next chapter");}  
  void up()       {showHide();}
  void down()     {playPause();}
  
  void altLeft()  {md.tellAppli("DVD Player","set viewer full screen to true");}
  void altRight() {md.tellAppli("DVD Player","eject dvd");}
  void altUp()    {showHide();}
  void altDown()  {playPause();}

  void showHide() {show_state = !show_state; show(show_state);}
  void playPause(){play_state = !play_state; play(play_state);}

  Dvd(URem* _md) : Component(_md), show_state(false), play_state(false) {
    // show_state and play_state may not reflect actual visibility, they just
    // serve to exchange states
    altLeftName  = "Full";
    altRightName = "Eject";
    altUpName    = "Show"; 
    altDownName  = "Play";
  }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class Radio : public Component {
  int channel;
public:
  void playTrack(int n) {md.sendIRCode("Amp ", n);}
  void left();
  void right();
  void up()   {playTrack(1);}
  void down() {playTrack(9);}
  
  Radio(URem* _md) : Component(_md), channel(0) {}
};

void Radio::left() {
  // md.sendIRCode("Amp previous"); ce code marche mal!
  // NB: 0 means: not init yet; channel 1 is set in this case
  if (channel <= 1) channel = 1; else channel--;
  
  // evidemment ca marche qu'entre 1 et 9 !!!
  md.sendIRCode("Amp ", channel);
  //md.say("Radio ", channel);
}

void Radio::right() {
  // md.sendIRCode("Amp next"); ce code marche mal!
  // NB: 0 means: not init yet; channel 1 is set in this case
  if (channel <= 0) channel = 1; else channel++;
  
  // evidemment ca marche qu'entre 1 et 9 !!!
  md.sendIRCode("Amp ", channel);
  //md.say("Radio ", channel);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class Cable : public Component {
  bool is_on;
public:
  void show(bool state) {}
  void play(bool state);
  void playTrack(int n) {md.sendIRCode("Cable ", n);}

  void left()     {md.sendIRCode("Cable arrow-left" );}
  void right()    {md.sendIRCode("Cable arrow-right");}
  void up()       {md.sendIRCode("Cable previous");}
  void down()     {md.sendIRCode("Cable next");}
  
  void altLeft()  {md.sendIRCode("Cable info");} 
  void altRight() {md.sendIRCode("Cable info");}  // guide 
  void altUp()    {md.sendIRCode("Cable more");}  // show selction box
  void altDown()  {md.sendIRCode("Cable on-off");}
  
  Cable(URem* _md) : Component(_md), is_on(false) {
    altLeftName = "Info", 
    altRightName = "Guide", 
    altUpName    = "Show"; 
    altDownName  = "OnOff";
  }
};

void Cable::play(bool state) {
  if (is_on == state) return;
  is_on = state;
  md.sendIRCode("Cable on-off");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int main(int argc, char* argv[]) {
  try {
    //UAppli::conf.setDefaultBackground(UBackground::velin);
    UAppli appli(argc, argv);
    URem remote;
    remote.show(true);
    remote.setScreenPos(REMOTE_LOCATION);
    appli.start();
    return 0;
  }
  catch(UError&) {cerr << "Ubit fatal error" << endl;}
  catch(...) {cerr << "Misc. fatal error" << endl;}
  return 1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

URem::URem() :
radio(new Radio(this)), 
itunes(new ITunes(this)), 
dvd(new Dvd(this)), 
cable(new Cable(this)),
player(null),
screen_source_is_tv(false),
pipe(null),
is_wiimote_connected(false),
mode_switch(0),
last_button(-1),  // -1 pas 0 !
last_press_time(0),
pressed_buttons(WiiRemoteButtonCount, false)
{
  createGUI();
 
  showMsg("Trying to Connect the WiiMote...");
  pipe = popen(WIIMOTE_APP,"r");
  if (!pipe) {
    showMsg("Can't open pipe");
    return;
  }

  // !!!BUG: USource ne marche pas avec GLUT !!!
  // cf aussi: psource.onClose(UCall&); psource.close();  
  psource.open(fileno(pipe));
  if (!psource.isOpened()) {
    showMsg("Can't open source");
    return;
  }  
  psource.onInput(ucall(this, &URem::fromWiimote));
}

URem::~URem() {
  if (pipe) pclose(pipe);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void URem::sendIRCode(const char* code) {
  char buf[100];
  sprintf(buf, "ired -send %s", code);
  cerr << "send: " << buf << endl;
  system(buf);
}

void URem::sendIRCode(const char* code, int no) {
  char buf[100];
  sprintf(buf, "ired -send %s%d", code, no);
  cerr << "send: " << buf << endl;
  system(buf);
}

void URem::say(const char* sentence) {
  char buf[500];
  sprintf(buf, "say '%s'", sentence);
  cerr << "say: " << buf << endl;
  system(buf);
}

void URem::say(const char* sentence, int no) {
  char buf[500];
  sprintf(buf, "say '%s%d'", sentence, no);
  cerr << "say: " << buf << endl;
  system(buf);
}

void URem::beep(int duration) {
  char buf[50];
  sprintf(buf, "osascript -e 'beep %d'", duration);
  system(buf);
}

void URem::callScript(const char* command) {
  char buf[500];
  sprintf(buf, "osascript -e '%s'", command);
  cerr << "call: " << buf << endl;
  system(buf);
}

void URem::tellAppli(const char* appli, const char* command) {
  char buf[500];
  sprintf(buf, "osascript -e 'tell application \"%s\" to %s'", appli, command);
  cerr << "call: " << buf << endl;
  system(buf);
}

void URem::tellAppli(const char* appli, const char* command, bool state) {
  char buf[500];
  sprintf(buf, "osascript -e 'tell application \"%s\" to %s%s'", 
          appli, command, (state ? "true" : "false"));
  cerr << "call: " << buf << endl;
  system(buf);
}

void URem::tellAppli(const char* appli, const char* command, int no) {
  char buf[500];
  sprintf(buf, "osascript -e 'tell application \"%s\" to %s%d'", appli, command, no);
  cerr << "call: " << buf << endl;
  system(buf);
}

void URem::systemSleep() {
  cerr << "systemSleep " << endl;
  system("osascript -e 'tell application \"System Events\" to sleep'");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void URem::createGUI() {
  {
    UArgs a = uhcenter();
    players_box.add
    (*new URoundBorder(UBorder::LINE, UColor::darkgrey, UColor::white,3,3,15,15)
     + UBackground::white
     + uhbox(uhcenter()
             + (upPlayerItem = 
                uitem(a + players_choice 
                      + UOn::select / ucall(this, itunes, &URem::setPlayerCB)
                      + upix(itunes_xpm) + utip("iTunes"))
                )
             )
     + uhbox(uhcenter()
             + (leftPlayerItem =
                uitem(a + players_choice
                     + UOn::select / ucall(this, radio, &URem::setPlayerCB)
                     + upix(radio_xpm) + utip("Radio"))
                )
             + UColor::blue + UFont::bold + " (A) "
             + (rightPlayerItem = 
                uitem(a + players_choice
                      + UOn::select / ucall(this, cable, &URem::setPlayerCB)
                      + upix(tv_xpm) + utip("TV"))
                )
             )
     + uhbox(uhcenter()
             + (downPlayerItem =
                uitem(a + players_choice
                     + UOn::select / ucall(this, dvd, &URem::setPlayerCB)
                      + upix(dvd_xpm) + utip("DVD"))
                )
             )
     );
  }
  {
    UArgs a = usize(30, 20) + uhcenter();  
    navig_box.add
    (uhcenter() + uvcenter()
     + *new URoundBorder(UBorder::ETCHED, UColor::darkgrey, UColor::white,1,4,15,15)
     + uhbox(uhcenter()
             + uitem(a + USymbol::up + ucall(this, &URem::up))
             )
     + uhbox(uhcenter()
             + uitem(a + USymbol::left + ucall(this, &URem::left))
             + "   "
             + uitem(a + USymbol::right + ucall(this, &URem::right))
             )
     + uhbox(uhcenter()
             + uitem(a + USymbol::down + ucall(this, &URem::down))
             )
     );
  }
  {
    UArgs a = usize(40, 25) + uhcenter();  
    alt_box.add
    (uhcenter() + uvcenter()
     + *new URoundBorder(UBorder::ETCHED, UColor::darkgrey, UColor::white,3,3,15,15)
     + uhbox(uhcenter()
             + uitem(a + altUpName + ucall(this, &URem::altUp))
             )
     + uhbox(uhcenter()
             + uitem(a + altLeftName + ucall(this, &URem::altLeft))
             + ulabel(UColor::blue + UFont::bold + "(h)")
             + uitem(a + altRightName + ucall(this, &URem::altRight))
             )
     + uhbox(uhcenter()
             + uitem(a + altDownName + ucall(this, &URem::altDown))
             )
     );
  }
  {
    UArgs a = uhcenter() + usize(25, 20);
    pad_box.add
    (uhcenter() + uvcenter()
     + uhbox(uhflex()
             + uitem(a + UFont::bold + UFont::large
                     + "-"+ ucall(this, &URem::ampVolDecr))
             + uitem(a + "0"+ ucall(this, 0, &URem::playTrack))
             + uitem(a + UFont::bold + UFont::large
                     + "+"+ ucall(this, &URem::ampVolIncr))
             )
     + uhbox(uhflex()
             + uitem(a + "1" + ucall(this, 1, &URem::playTrack))
             + uitem(a + "2" + ucall(this, 2, &URem::playTrack))
             + uitem(a + "3" + ucall(this, 3, &URem::playTrack))
             )
     + uhbox(uhflex()
             + uitem(a + "4" + ucall(this, 4, &URem::playTrack))
             + uitem(a + "5" + ucall(this, 5, &URem::playTrack))
             + uitem(a + "6" + ucall(this, 6, &URem::playTrack))
             )
     + uhbox(uhflex()
             + uitem(a + "7" + ucall(this, 7, &URem::playTrack))
             + uitem(a + "8" + ucall(this, 8, &URem::playTrack))
             + uitem(a + "9" + ucall(this, 9, &URem::playTrack))
             )
     );
  }
    
  //alert_box.addAttr(upos(0,0) + usize(100|UPERCENT, 100|UPERCENT)
  //                  + UBackground::black + ualpha(0.5) + UColor::white);
  alert_box.add(upadding(3,2) + uhcenter() + UColor::navy + alert_msg_str);
  
  addAttr(upadding(4,4) + uvspacing(6));
  add(UOrient::vertical 
      + uhcenter() + uvcenter() 
      + players_box + navig_box + alt_box + pad_box
      + uhflex() 
      + alert_box
      );
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void URem::showMsg(const char* s) {
  cerr << "showMsg: " << s << endl;
  if (!s) alert_box.show(false);
  else {
    alert_msg_str = s;
    alert_box.show(true);
  }
}

void URem::showWiimoteState(bool is_connected) {
  is_wiimote_connected = is_connected;
  if (is_connected) {
    showMsg("Connected to the WiiMote");
  }
  else {
    showMsg("Disconnected from the WiiMote");
  }
}

void URem::fromWiimote() {
  char buf[3] ={0, 0, 0};
  
  if (!fread(buf, 1, 3, pipe)) {
    cerr << "!!! fromWiiMote: no data; source state: " << psource.isOpened()<< endl;
    //cerr << "feof/ferror " <<feof(pipe) << " " << ferror(pipe)<< endl;
    uerror("URem::fromWiimote", "No data, disconnecting the WiiRemote...");
    psource.close();
    showWiimoteState(false); // disconnected
    return;
  } 
  
  cerr <<"*** fromWii: " << buf[0] << ' ' << int(buf[1]) <<' '<< int(buf[2]) << endl;

  if (buf[0]=='^') {
    switch (buf[1]) {
      case WiiRemoteInit:
        cerr << "init" << endl;
        break;
      case WiiRemoteDoDiscovery:
        cerr << "doDiscovery:" << endl;
        showMsg("Press Buttons 1 and 2 Simultaneously");
        break;
      case WiiRemoteDiscovered:
        cerr << "WiiRemoteDiscovered" << endl;
        showMsg("WiiMote Discovered");
        showWiimoteState(true); // connected
        break;
      case WiiRemoteDiscoveryError:
        cerr << "WiiRemoteDiscoveryError" << endl;
        showMsg("WiiMote Discovery Error");
        showWiimoteState(false); // disconnected
        break;
      case WiiRemoteDisconnected:
        cerr << "wiiRemoteDisconnected" << endl;
        showMsg("WiiMote Disconnected");
        showWiimoteState(false); // disconnected
        setAllOff();   // clicking on the POWER buttton will set all off
        break;
    }
  }

  else if (buf[0]=='#') {
    int button = buf[1];
    bool pressed = buf[2];
    
    unsigned long time = UAppli::getTime();
    //if (time-last_press_time > MODE_SWITCH_DELAY) setModeSwitch(0);
    //cerr << "##### mode_switch " << mode_switch << endl;
    
    switch(button) {
      case WiiRemoteAButton:     // shows panel + CTRL  / ALL OFF on long press
        if (pressed) {           // on press
          if (last_button==WiiRemoteAButton && time-last_press_time < MODE_SWITCH_DELAY)
            setModeSwitch(getModeSwitch()+1);
          else {
            setModeSwitch(1);
            setScreenPos(REMOTE_LOCATION);
            show(true);
            toFront();
          }
        }
        else {                    // on release and long press setAllOff
          if (last_button==WiiRemoteAButton && time-last_press_time > LONG_PRESS_DELAY) 
            show(false);
        }
        break;
        
      case WiiRemoteBButton:
        setModeSwitch(2);
        break;

      case WiiRemoteHomeButton:
        setModeSwitch(2);
        break;
    
        // - - - - - - -
    
      case WiiRemoteLeftButton:    // previous channel / RADIO if A before
        if (!pressed) setModeSwitch(0);
        else {
          if (mode_switch == 0) left();
          //else if (mode_switch == 1) players_choice.setSelectedIndex(0);
          else if (mode_switch == 1) players_choice.setSelectedItem(*leftPlayerItem);
          else if (mode_switch == 2) altLeft();
        }
        break;
        
        
      case WiiRemoteRightButton:    // next channel / TV if A before
        if (!pressed) setModeSwitch(0);
        else {
          if (mode_switch == 0) right();
          //else if (mode_switch == 1) players_choice.setSelectedIndex(3);
          else if (mode_switch == 1) players_choice.setSelectedItem(*rightPlayerItem);
          else if (mode_switch == 2) altRight();
        }
        break;
        
      case WiiRemoteUpButton:    // previous list / ITUNES if A before
        if (!pressed) setModeSwitch(0);
        else {
          if (mode_switch == 0) up();
          //else if (mode_switch == 1) players_choice.setSelectedIndex(1); 
          else if (mode_switch == 1) players_choice.setSelectedItem(*upPlayerItem);
          else if (mode_switch == 2) altUp();
        }
        break;
        
      case WiiRemoteDownButton:    // next list / DVD if A before
        if (!pressed) setModeSwitch(0);
        else {
          if (mode_switch == 0) down();
          //else if (mode_switch == 1) players_choice.setSelectedIndex(2);
          else if (mode_switch == 1) players_choice.setSelectedItem(*downPlayerItem);
          else if (mode_switch == 2) altDown();
        }
        break;
        
        // - - - - - - -

      case WiiRemotePlusButton:      // volume +
        if (pressed) ampVolIncr();
        break;
        
      case WiiRemoteMinusButton:
        if (pressed) ampVolDecr();   // volume -
        break;
        
      case WiiRemoteOneButton:       // screenSourceSwitch / screenOnOff on long press
        if (!pressed) {              // on release
          if (last_button!=WiiRemoteOneButton || pressed_buttons[WiiRemoteTwoButton])
            ;            // nop: 1 and 2 are both triggered for initializing the remote
          else if (time-last_press_time > LONG_PRESS_DELAY) 
            screenOnOff();           // long press
          else 
            screenSourceSwitch();    // normal case
        }
        break;
        
      case WiiRemoteTwoButton:       // screenZoom
        if (pressed) screenZoom();
        break;
        
        break;
        
      default:
        break;
    }
    
    last_button = button;
    pressed_buttons[button] = pressed;
    if (pressed) {
      last_press_time = time; 
      //cerr << "last_press_time " <<  last_press_time<< endl;
    }
  }
  
  else {
    uerror("URem::fromWiimote", "Invalid data from WiiRemote");
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void URem::setAllOff() {
  ampOff();
  radio->play(false);
  itunes->play(false);
  dvd->play(false);
  //cable->play(false);
  screenOff();
  players_choice.clearSelection();
  player = null;
  sleep(2);
  systemSleep();   // !!!
}

void URem::setPlayerCB(Component* p) {
  player = p;
  //cerr << endl <<  endl <<  endl <<  endl <<"setPlayerCB "<< player <<endl;
  
  altLeftName  = p->altLeftName;
  altRightName = p->altRightName;
  altUpName    = p->altUpName;
  altDownName  = p->altDownName;
  doUpdate(UUpdate::layoutAndPaint);  // doUpdate() updates now
 
  // !! pas satisfaisant : faudrait verifier si les etats sont OK
  
  if (player == radio) {
    ampSourceRadio();
    radio->play(true);
    //if (itunes->play_state) 
    itunes->play(false);
    //if (dvd->play_state) 
    dvd->play(false);
    //cable->play(false);
    screenSourcePC();
  }
  else if (player == itunes) {
    ampSourceITunes();
    //if (dvd->play_state)
    dvd->play(false); 
    //if (dvd->show_state) dvd->show(false);
    //itunes->show(true);
    itunes->play(true);
    radio->play(false);
    //cable->play(false);
    screenSourcePC();
  }
  else if (player == dvd) {
    ampSourceDVD();
    //if (itunes->play_state) 
    itunes->play(false); 
    //if (itunes->show_state) itunes->show(false);
    //dvd->show(true); 
    dvd->play(true);
    radio->play(false);
    //cable->play(false);
    screenSourcePC();
  }
  else if (player == cable) {
    ampSourceCable();
    radio->play(false);
    //if (itunes->play_state) 
      itunes->play(false); 
    //if (itunes->show_state) itunes->show(false);
    //if (dvd->play_state) 
      dvd->play(false); 
    //if (dvd->show_state) dvd->show(false);
    //cable->play(true);
    screenSourceCable();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void URem::screenOnOff() {
  URem::sendIRCode("TV on-off");
}

void URem::screenOn() {
  URem::sendIRCode("TV on");
}

void URem::screenOff() {
  URem::sendIRCode("TV off");
}

void URem::screenZoom() {
  URem::sendIRCode("TV zoom");
}

void URem::screenSourceSwitch() {
  if (screen_source_is_tv) {
    screen_source_is_tv = false;
    URem::sendIRCode("TV source");  // shows PC
  }
  else {
    screen_source_is_tv = true;
    URem::sendIRCode("TV av4");  // shows TV
  }
}

void URem::screenSourceCable() {
  if (screen_source_is_tv) return;
  screen_source_is_tv = true;
  URem::sendIRCode("TV av4");  // shows TV
}

void URem::screenSourcePC() {
  if (!screen_source_is_tv) return;
  screen_source_is_tv = false;
  //if (screen_source_is_tv == UNKNOWN) {
  //  URem::sendIRCode("TV av4");  // shows TV
  //  sleep(2);
  //}
  URem::sendIRCode("TV source");  // shows PC
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void URem::ampOff() {
  URem::sendIRCode("Amp off"); 
}

void URem::ampVolIncr() {
  URem::sendIRCode("Amp vol+");
}

void URem::ampVolDecr() {
  URem::sendIRCode("Amp vol-");
}

void URem::ampSourceRadio() {
  URem::sendIRCode("Amp radio");
}

void URem::ampSourceITunes() {
  URem::sendIRCode("Amp aux");
}

void URem::ampSourceDVD() {
  URem::sendIRCode("Amp aux");
}

void URem::ampSourceCable() {
  URem::sendIRCode("Amp tape2");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/*
 void changePlayer() {
 int current = URem::CTRL;
 
 UNode* node = players_pane.tablist().getChild(current);
 UBox* tab = node ? node->toBox() : null;
 if (tab) {
 UColor& col = tab->obtainAttr<UColor>();
 col = UColor::inherit;  // evidemment ca marche pas
 }      
 // !!! CF aussi cas des const avec obtain !!!
 if (tab) {
 tab->obtainAttr<UFont>() = UFont::plain;
 tab->obtainAttr<UBorder>().setColor(UColor::green);
 }
 */
/*
 current++;
 if (current >= PLAYER_COUNT) current = CTRL;
 //cerr << "current " << current << endl;
 players_pane.tablist().select(current);
 remote.show();
 remote.toFront();
 */
/*
 node = players_pane.tablist().getChild(current);
 tab = node ? node->toBox() : null;
 if (tab) {
 tab->obtainAttr<UFont>() = UFont::bold;
 tab->obtainAttr<UBorder>().setColor(UColor::red);
 }
 }
 */


const char *tv_xpm[] = {
/* width height ncolors chars_per_pixel */
"36 36 133 2",
/* colors */
//"AA c #FFFFFFFFFFFF",
"AA c None s None",
"BA c #FFFFF7F7B5B5",
"CA c #FFFFEFEFFFFF",
"DA c #FFFFEFEF9494",
"EA c #FFFFDEDE5252",
"FA c #F7F7EFEFB5B5",
"GA c #EFEFEFEFEFEF",
"HA c #EFEFE7E7B5B5",
"IA c #EFEFDEDEA5A5",
"JA c #EFEFCECE9494",
"KA c #EFEFBDBD7373",
"LA c #EFEF94946363",
"MA c #EFEF84848C8C",
"NA c #E7E7DEDED6D6",
"OA c #E7E7C6C6FFFF",
"PA c #E7E7ADAD7373",
"AB c #E7E739392929",
"BB c #DEDEDEDEDEDE",
"CB c #DEDEC6C6ADAD",
"DB c #DEDE9494FFFF",
"EB c #DEDE6B6B8C8C",
"FB c #D6D6CECEEFEF",
"GB c #D6D6BDBD8484",
"HB c #D6D6ADADEFEF",
"IB c #D6D6A5A56B6B",
"JB c #CECEA5A57B7B",
"KB c #CECE52522929",
"LB c #C6C6CECEC6C6",
"MB c #C6C6B5B5BDBD",
"NB c #C6C6B5B58C8C",
"OB c #C6C66B6B3939",
"PB c #BDBDEFEF5252",
"AC c #BDBDCECE7B7B",
"BC c #BDBDBDBDCECE",
"CC c #BDBD63636B6B",
"DC c #BDBD5A5A2121",
"EC c #B5B5ADAD6B6B",
"FC c #B5B5A5A58484",
"GC c #B5B552528C8C",
"HC c #B5B54A4A5252",
"IC c #ADADBDBDADAD",
"JC c #ADADBDBD7373",
"KC c #ADADADADC6C6",
"LC c #ADADADADADAD",
"MC c #ADAD94949494",
"NC c #ADAD39390808",
"OC c #A5A5D6D68484",
"PC c #A5A594946B6B",
"AD c #9C9CA5A5A5A5",
"BD c #9C9C73734A4A",
"CD c #9C9C63635252",
"DD c #94949C9C6B6B",
"ED c #949494948484",
"FD c #94948C8C7B7B",
"GD c #949473735A5A",
"HD c #949463632929",
"ID c #8C8CDEDE8C8C",
"JD c #8C8CDEDE7373",
"KD c #8C8CA5A53939",
"LD c #8C8C8C8C8C8C",
"MD c #8C8C7B7B5A5A",
"ND c #84848C8C6363",
"OD c #84844A4A2121",
"PD c #7B7BC6C66363",
"AE c #7B7BB5B58C8C",
"BE c #7B7B94949494",
"CE c #7B7B7B7B7B7B",
"DE c #7B7B6B6B5A5A",
"EE c #7B7B29294A4A",
"FE c #7373E7E79C9C",
"GE c #737394943131",
"HE c #73738C8C8484",
"IE c #73734A4A7B7B",
"JE c #6B6B42422929",
"KE c #6363CECE5252",
"LE c #636373735A5A",
"ME c #636352523939",
"NE c #63634A4A7B7B",
"OE c #636321211818",
"PE c #5A5A8C8C5A5A",
"AF c #5A5A7B7B6B6B",
"BF c #5A5A6B6B6B6B",
"CF c #5A5A5A5A5A5A",
"DF c #5A5A52524A4A",
"EF c #52529C9C6B6B",
"FF c #52527B7B1818",
"GF c #52526B6B5A5A",
"HF c #52524A4A5252",
"IF c #525242422929",
"JF c #525239391818",
"KF c #4A4A94948C8C",
"LF c #4A4A84846B6B",
"MF c #4A4A5A5A5252",
"NF c #4A4A4A4A4A4A",
"OF c #4242D6D69494",
"PF c #4242BDBD7B7B",
"AG c #42429C9C7373",
"BG c #42429C9C2929",
"CG c #42426B6B5252",
"DG c #424263636B6B",
"EG c #424242425252",
"FG c #424242423939",
"GG c #424231311818",
"HG c #3939EFEF5A5A",
"IG c #39398C8C4242",
"JG c #393939393939",
"KG c #313184845252",
"LG c #313118181010",
"MG c #292952524A4A",
"NG c #292921212929",
"OG c #2121B5B54A4A",
"PG c #18189C9C2929",
"AH c #18184A4A4242",
"BH c #18184A4A3939",
"CH c #181842425A5A",
"DH c #181831312121",
"EH c #181818182121",
"FH c #181810100808",
"GH c #10108C8C2121",
"HH c #101063631818",
"IH c #10105A5A3131",
"JH c #101039391818",
"KH c #101029292121",
"LH c #101010101010",
"MH c #080873733131",
"NH c #080852522121",
"OH c #080839394A4A",
"PH c #080829291818",
"AI c #080818180808",
"BI c #000031313131",
"CI c #000031311818",
"DI c #000021210000",
"EI c #000000000000",
/* pixels */
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAALDNFNFJGJGJGJGJGJGJGJGJGJGJGJGJGJGJGJGJGJGJGJGJGJGJGNFCEAAAAAAAA",
"AAAAAACENFLDLDMCADMCLCLCLCLCLCMBICMBMBICMBLCLCLCLCMCICMCADMCLDNFCEAAAAAA",
"AAAAAANFCECELDLDLDADHEMCBEMCADMCADMCBELDLDLDLDLDCELDCEBECECECECENFAAAAAA",
"AAAAAANGLDCELDLDLDLDLDBEMCADHEADMCADLDMBNBLBLBBCLBLBBBMBLBLBKCCENGAAAAAA",
"AAAAAAFGBFCECEBFCECECECECECELDLDBEMCHEPELFPELFEFEFPEMFMFNFNFDFBFJGAAAAAA",
"AAAAAANGCEMCCBLBLBLBBBLBLBNACELDLDCECEEFAGKFEFCGEGNFMFNFDFMFCFCENGAAAAAA",
"AAAAAALGBFDDIAIAIAIAIAFAIAJAEDCECEPELFPFLFCGNFGDDDLEMDNFEGEGCGBFNGAAAAAA",
"AAAAAANGDEFDJAIAIAFAIAIAIAJAJBCECEBFLFAGEGMEDEECJCACDDNFDFMFGFDENGAAAAAA",
"AAAAAALHNFECBABAHAIAJAJAGBPAJACEBFAFAFOFOFOCIBCDNDDDPCNFNFNFCGNFAIAAAAAA",
"AAAAAAEILHHDEABADAJAJAPAKALAJBFDCFGFCGAEOFDAJDAFNFNFIBJBPHLHPHLHEIAAAAAA",
"AAAAAAEILHOENCOBKBDCNCODOEOEBDCDFGMGKGFEIDEAPBHGOGOGKEPDKGNHNHLGEIAAAAAA",
"AAAAAAEILHJFODBDHDODJEJEGGLGJEIFEICIIGPGGHPGBGMHKHPHPHCINHPGBHLHEIAAAAAA",
"AAAAAAEILHNGEIJGIFIFIFOEIFJFLGEIAILHJFCICILHDHJGDICIBHIHBHNGLHLHEIAAAAAA",
"AAAAAAEINGFHNGFHLHLHLHKHLGLHKHNGCIJHNGNGNGNGEILHPHLGAILHFHLHNGLHEIAAAAAA",
"AAAAAAEILHLHLHJHLHEHLHKHDHMHHHNHNFHFIEIEIEEGJHEILHLHLHLGLHEHLHDIEIAAAAAA",
"AAAAAAEIEILHEILGEIHFFCAELBBCICLBMBMBMBMBBBCFLHEHEILHLHKHEILHEINGEIAAAAAA",
"AAAAAAEIEIEIEHEILHEIBHAHKHDHCIPHCIBHNHCIAIAHLHEIKHEILHEINGEILHEIEIAAAAAA",
"AAAAAAEIEILHEIEHEILHCGAHBHAFADHEBFKCBCEDAHIHEILGEIKHEIEILHEIEIEIEIAAAAAA",
"AAAAAAEIEIEIEIEIEIEIIGFFGEDGOACABCOABCOACACGLHEIEIEIEIEIEINGEIEIEIAAAAAA",
"AAAAAAEIEIEIEIEIEINGCBFFKDPENEDBHBFBCACAMFMFEIEIEIEIEIEIEIEIEIEIEIAAAAAA",
"AAAAAAEIEIEIEIEIEIPHEFPEDGMAKBEBGCGCGCEEMEEGEIEILHEIEILHEIEIEIEIEIAAAAAA",
"AAAAAAEIEILHNGEILHLHDHBIFGJGHCCCDCABHCEEBENGAIEHEILHNGEILHEILHEIEIAAAAAA",
"AAAAAAEIEILGEIJHLHPHAHLHKHKHOHAHCHCHCHDGBELHJHLHNGNGLHLHNGLHNGEIEICAAAAA",
"AAAAAAEILHKHNGLGEHEHLHDHEHKHNGLGKHNGNGPHKHJGNGNGNGNGJHNGLHJHLHLGLHAAAAAA",
"AAAAAACEEILGKHNGNGJGJGJGJGDFEGEGMFHFNFNFNFJGNFFGFGNGJGNGNGJGLHEICFAAAAAA",
"AAAAAABBCFEIEIEIEIEIEIEIEIEIEIEIEIEIEIEIEIEIEIEIEIEIEIEIEIEIEHDFCAAAAAAA",
"AAAAAAAAGABBCABBBBBBBBBBBBBBBBGABBBBBBBBBBNABBBBBBGABBNABBBBBBGAAAAAAAAA",
"AAAAAAAAAAAAGAAAAAAAAACAGAAAAAAAGAAAAAAACAAAAACAAAAAGAAAAAGAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"};


const char *dvd_xpm[] = {
/* width height ncolors chars_per_pixel */
"36 36 32 2",
/* colors */
//"AA c #FFFFFFFFFFFF",
"AA c None s None",
"BA c #FFFFFFFFEFEF",
"CA c #F7F7F7F7DEDE",
"DA c #EFEFF7F7DEDE",
"EA c #EFEFEFEFEFEF",
"FA c #EFEFEFEFDEDE",
"GA c #E7E7EFEFCECE",
"HA c #DEDEDEDEDEDE",
"IA c #DEDEDEDEC6C6",
"JA c #CECED6D6BDBD",
"KA c #CECECECECECE",
"LA c #CECECECEBDBD",
"MA c #BDBDC6C6ADAD",
"NA c #BDBDBDBDBDBD",
"OA c #B5B5B5B5ADAD",
"PA c #ADADB5B59C9C",
"AB c #ADADADADB5B5",
"BB c #ADADADADADAD",
"CB c #ADADADAD9C9C",
"DB c #9C9C9C9CA5A5",
"EB c #9C9C9C9C9C9C",
"FB c #9C9C9C9C8C8C",
"GB c #8C8C94948484",
"HB c #8C8C8C8C8C8C",
"IB c #8C8C8C8C7B7B",
"JB c #7B7B7B7B7B7B",
"KB c #7B7B7B7B6B6B",
"LB c #6B6B6B6B6B6B",
"MB c #636363635A5A",
"NB c #5A5A5A5A5A5A",
"OB c #4A4A4A4A4A4A",
"PB c #393939393939",
/* pixels */
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAHANAKAKANAKAKANAKAKANAKAKANAKAKANAKAKAKAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAKABBKAEAEAEAEAEAEAEAEAEAEAEAEAEAEAEAEAKANAKAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAANAKAHAKAKAKAKAKAKAKAKAKAKAKAKAKAKAKAKAHANAKAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAANAKAKAJBNBNBNBMBNBNBMBNBNBMBNBNBMBNBIBNAHABBAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAANAKANAIBCABACABACABACABACABACABACABAKBNAKANAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAABBKANAIBBACACAKBKBKBLAFBKBMBGBCABACAFBNAKABBAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAANANANAFBCACAJAMBMBOAKBCBFBMBIBFACACAHBNANANAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAABBKAOAHBDACAIACBOAIAPALAOAOALAGACADAFBOAKABBAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAABBNANAIBGAGAMAKBKBMBMBNBMBKBCBIAGAGAIBNANABBAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAABBKABBFBFAFAFADAFAFAFACACAFAFAFAFAFAIBNAKABBAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAABBNANAOAIBFBHBFBFBGBFBFBFBGBFBGBFBGBBBNANAEBAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAEBNANAABKALAKANAKAKANAKAKAKAKANAKAKANABBKAEBAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAABBNABBNAOAABBBNABBBBNABBBBBBBBNAABBBBBBBNABBAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAEBNABBBBABOAABEBJBLBNBMBLBHBEBBBBBNABBNANAEBAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAEBNANABBBBBBJBHBNAEAAAAAEANAHBJBBBBBBBBBNAEBAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAEBNABBBBBBLBNAEAAAEAKANAAAAAEABBJBEBBBBBNAEBAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAHBNABBBBJBBBBBEAEAHANBOBKAEAEABBBBIBBBBBNAEBAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAEBNABBHBHBHAEABBEAEAEAAAEAEABBEAHAHBHBBBNAHBAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAHBNADBHBOAEAEAEAEBNAEBEBNAEBHAEAEAABHBBBABEBAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAHBNABBLBKAHAKAHAKAEBHAHAEBKAHAKAHAKALBBBNAHBAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAHBBBBBNBHABBOBHAHBHAEAEAHAHBHAOBBBHANBBBBBHBAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAHBBBBBNBKABBOBHAHBKAHAHAKAHBHAPBBBKANBBBBBHBAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAEBEBBBJBBBKANAKABBJBNANAJBBBKANAKABBJBBBDBHBAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAEBHBBBJBHBNAKANAHBNAJBJBNAJBKANAKAHBHBBBHBBBAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAANAJBEBEBJBBBNAJBHAKAKAHAKAHAJBNABBLBEBEBJBBBAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAKAJBEBBBHBJBJBKABBEBPBPBBBBBKAJBJBHBBBEBJBHAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAADBLBEBBBHBJBEBBBNAHBHBBBNAEBJBHBBBEBLBEBEAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAKAJBJBEBBBHBLBHBHBBBBBHBJBJBHBBBEBJBJBKAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAANALBJBEBBBBBHBJBNBNBKBHBBBBBEBJBLBNAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAANANBLBHBDBBBNAKANANABBDBHBLBLBNAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAHAJBNBMBLBEBHBEBHBJBNBNBJBKAAAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAAEAKAHBLBOBOBPBOBLBHBKAEAAAAAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"};


const char *itunes_xpm[] = {
/* width height ncolors chars_per_pixel */
"36 36 120 2",
/* colors */
//"AA c #FFFFFFFFFFFF",
"AA c None s None",
"BA c #F7F7F7F7CECE",
"CA c #EFEFF7F7F7F7",
"DA c #EFEFE7E7DEDE",
"EA c #EFEFDEDEC6C6",
"FA c #E7E7FFFFC6C6",
"GA c #E7E7F7F7A5A5",
"HA c #E7E7EFEFEFEF",
"IA c #E7E7DEDEBDBD",
"JA c #E7E7DEDEA5A5",
"KA c #E7E7CECE9494",
"LA c #DEDEEFEFA5A5",
"MA c #DEDEE7E7E7E7",
"NA c #DEDEDEDEEFEF",
"OA c #DEDEDEDEDEDE",
"PA c #DEDEDEDE9494",
"AB c #DEDED6D6CECE",
"BB c #DEDECECECECE",
"CB c #DEDECECEADAD",
"DB c #DEDECECE9C9C",
"EB c #D6D6E7E7F7F7",
"FB c #D6D6CECEE7E7",
"GB c #CECEF7F7DEDE",
"HB c #CECEEFEFC6C6",
"IB c #CECEDEDEE7E7",
"JB c #CECEC6C6CECE",
"KB c #C6C6FFFFA5A5",
"LB c #C6C6E7E79494",
"MB c #C6C6B5B5A5A5",
"NB c #BDBDFFFFB5B5",
"OB c #BDBDEFEF9494",
"PB c #BDBDD6D6CECE",
"AC c #BDBDCECEDEDE",
"BC c #BDBDC6C6BDBD",
"CC c #BDBDBDBDEFEF",
"DC c #BDBDBDBDD6D6",
"EC c #B5B5D6D6EFEF",
"FC c #B5B5BDBDC6C6",
"GC c #B5B5ADADB5B5",
"HC c #B5B5A5A58C8C",
"IC c #ADADBDBDEFEF",
"JC c #ADADB5B5DEDE",
"KC c #ADADB5B5BDBD",
"LC c #A5A5F7F79C9C",
"MC c #A5A5ADADEFEF",
"NC c #A5A5A5A5ADAD",
"OC c #A5A5A5A57373",
"PC c #9C9CFFFFB5B5",
"AD c #9C9CF7F7A5A5",
"BD c #9C9CE7E7FFFF",
"CD c #9C9CBDBDDEDE",
"DD c #9C9CA5A5DEDE",
"ED c #9C9CA5A5B5B5",
"FD c #9C9C9C9C9C9C",
"GD c #9494CECE7B7B",
"HD c #9494BDBDF7F7",
"ID c #9494B5B5EFEF",
"JD c #8C8CEFEFC6C6",
"KD c #8C8CEFEFBDBD",
"LD c #8C8CE7E7A5A5",
"MD c #8C8CDEDEEFEF",
"ND c #8C8CC6C69494",
"OD c #8C8C9C9CCECE",
"PD c #8C8C8C8C8C8C",
"AE c #8C8C8484BDBD",
"BE c #8484FFFFE7E7",
"CE c #8484EFEFD6D6",
"DE c #8484C6C6E7E7",
"EE c #8484ADADEFEF",
"FE c #8484ADAD8C8C",
"GE c #7B7BE7E79C9C",
"HE c #7B7BDEDEE7E7",
"IE c #7B7BCECEF7F7",
"JE c #7B7BB5B5EFEF",
"KE c #7B7BA5A5D6D6",
"LE c #7B7BA5A5ADAD",
"ME c #7B7B9C9CCECE",
"NE c #7373E7E7C6C6",
"OE c #7373CECED6D6",
"PE c #7373A5A5EFEF",
"AF c #6B6BCECEA5A5",
"BF c #6B6BADADD6D6",
"CF c #6B6B9494D6D6",
"DF c #6B6B8C8CC6C6",
"EF c #6B6B8484B5B5",
"FF c #6363CECEEFEF",
"GF c #5A5AADADF7F7",
"HF c #5A5AA5A5DEDE",
"IF c #5A5A8C8CD6D6",
"JF c #5252B5B5E7E7",
"KF c #52529C9CCECE",
"LF c #52527B7BC6C6",
"MF c #52527373ADAD",
"NF c #4A4AC6C6C6C6",
"OF c #4A4A9C9CE7E7",
"PF c #4A4A9494DEDE",
"AG c #4A4A8484CECE",
"BG c #42429C9C7B7B",
"CG c #42427B7BD6D6",
"DG c #42427B7BC6C6",
"EG c #39398484D6D6",
"FG c #39397373CECE",
"GG c #31315A5AB5B5",
"HG c #29299494F7F7",
"IG c #29297B7BCECE",
"JG c #29297B7BA5A5",
"KG c #29296B6BD6D6",
"LG c #21216363CECE",
"MG c #21215252A5A5",
"NG c #18187373DEDE",
"OG c #18185A5ABDBD",
"PG c #181852529C9C",
"AH c #10106B6BB5B5",
"BH c #10106363CECE",
"CH c #08084A4AA5A5",
"DH c #0000A5A5F7F7",
"EH c #00008C8CEFEF",
"FH c #00007373E7E7",
"GH c #00006B6BD6D6",
"HH c #00005252C6C6",
/* pixels */
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAANAIBJCCCAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAAOAJBJBBCJBOAABOAMAAAAACAFBICODKEDFIFFGAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAOABCJBFBNANACAEBJBACMCODDFLFDGCGDGIGCGCGEGGGAAAAAAAAAA",
"AAAAAAAAAAAAAAHABCACICICACICNAFBODDGFGFGFGFGCGKGCGCGIFEGOFCGCHAAAAAAAAAA",
"AAAAAAAAAAAAJBOAMDPEHDCCICCCJBNADGKGLGIGCGCGOFPFEGOFGFOFKGHHCHAAAAAAAAAA",
"AAAAAAAAAAJBGBBEHEJEBFHDICACFBNADGIFIFOFEGOFOFGFGFIFGHGHOGFHCHAAAAAAAAAA",
"AAAAAAAAOAHBADNECECEDEIDMCICJBCCCGPFOFOFGFOFEGNGNGFHGHDHFHBHMGAAAAAAAAAA",
"AAAAAAAABCLCLDJDKDCEHEJEJEMCICJBCGGFEGDHGHFHFHEHGHFHIGCGKEGHMGAAAAAAAAAA",
"AAAAAABCFAADPCLDLDKDJDHEDEEEMCACFGOFHHGHKGCGCFIDFBEBAAAAAAIFMGAAAAAAAAAA",
"AAAAAAJBGAKBKBLCLCKDKDNEOEJEEEMCGGEGEFJBCACAAAAAAAAACACACAIFCHHAAAAAAAAA",
"AAAANAJAGALBGAGBOBOBLDGEKDOEBFKECGMGJBFCACCAAAAACACACAEBNAIFMGGCAAAAAAAA",
"AAAAMBBAKAJAJAPALAKBOBOBLDAFLEFCDGCHMANAJBECCAEBEBMAIBMAMAFGJGFBDAAAAAAA",
"AAAAGCFAEAIAJAJAJADBPAPALCNDJBOACGPGJBOAOAOANCOAOAOAIBOAOAAGPGHAGCAAAAAA",
"AAAABCBADADADAJAIAIAJADBDBGDACMAFGMGAAMBECDAEDKCOAJBBBBBBBDGMGEAJBAAAAAA",
"AAAAGCDADAEAJAABIAIAJBIAMBMBNADAFGPGAAAAJBOAJBHCCBCBBBBBIACGMGIAMAOAAAAA",
"AAAABCDAEADAOABBBBBBBBBBBBNCOAOADGPGAAJBABIBBCOCDBDBDBJAIADGMGIADAOAAAAA",
"AAAAGCHAOAOAOAOANAJBPBOAICOAGCOAFGMGBCJBOAOAFEGDLBPAPAPADBDGMGPADAJBAAAA",
"AAAAOANAOANAOAOAECGBIBOAMAIBEBACFGPGOAOAACLEBGGELCLCGALALADGMGDBJAJBAAAA",
"AAAAAAFCMAMAOAIBOANANAEBCAAACAJBFGMGFDAEMFJGNFNEKDPCNBGBKBDGPGLBGADAAAAA",
"AAAAAANCEBMAMAMAMANACACAAACAEBMAFGMGDDAEDFOFHFHEHEKFJEBFGFIGPGKBHBCAAAAA",
"AAAAAABAJBCAEBEBCACACACACAEBMAMAFGMGFCGCDDPEPEFFIGBDIEIEJFHHCHLCOAAAAAAA",
"AAAAAACAGCMACACACACAAAEBCFIEIEIFIGCHJBDDDDODEEOFIGGFHGHGFHEHCHJDMAAAAAAA",
"AAAAAAEBMAGCCAAAAAAACAAGBDMDOFJFBHCHDDJBODJCEEOFHHFHEHEHDHEHAHOAAAAAAAAA",
"AAAAAACADAJBFCAAAAAAIDCGGFOFHGEHFHCHFCJCJCODMCEELGFHDHDHDHOGOEJBAAAAAAAA",
"AAAABAMAOAABNCKCCACADDGHFHEHEHEHEHOGCDDDJCJCMCMCEEIFHHKGLGHEIAEBAAAAAAAA",
"AAAAAAAAOAJBBCFDNCEBMAGGFHDHDHDHHHKECDJCODJCDDMCIDIDMDGFMDBCFCCAAAAAAAAA",
"AAAAAAAAMAMABBNCPDPDJBFBMEKGLGDGDDCDJCJCJBJCDDDDIDHDPEPBKCGCOAHAAAAAAAAA",
"AAAAAAAAAAMAMAABBCFDPDFDACHAOAJBICBCACFCJCFCDDDDDDICKCFDBCOADACAAAAAAAAA",
"AAAAAAAAAAAAAACAOAOABCFDPDPDFDACBCGBACACJBDCDCGCNCPDNCFCDAMACAAAAAAAAAAA",
"AAAAAAAAAAAAAAAACAHAMANAJBJBFCFDPDPDFDPDFDPDFDFDBCABDACAEBAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAAAACAHAHAMAOAOAABNAOADAHAMACACAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"};


const char *radio_xpm[] = {
/* width height ncolors chars_per_pixel */
"36 36 41 2",
/* colors */
//"AA c #FFFFFFFFFFFF",
"AA c None s None",
"BA c #EFEFEFEFF7F7",
"CA c #EFEFEFEFEFEF",
"DA c #E7E7E7E7DEDE",
"EA c #DEDEE7E7EFEF",
"FA c #DEDEDEDEDEDE",
"GA c #D6D6EFEFF7F7",
"HA c #CECEDEDEE7E7",
"IA c #CECECECECECE",
"JA c #BDBDDEDEEFEF",
"KA c #BDBDBDBDBDBD",
"LA c #B5B5CECEE7E7",
"MA c #ADADD6D6EFEF",
"NA c #ADADCECEE7E7",
"OA c #ADADADADADAD",
"PA c #A5A5BDBDDEDE",
"AB c #9C9CD6D6EFEF",
"BB c #9C9CBDBDE7E7",
"CB c #9C9C9C9C9C9C",
"DB c #9494CECEEFEF",
"EB c #9494B5B5DEDE",
"FB c #8C8CBDBDEFEF",
"GB c #8484ADADDEDE",
"HB c #7B7BC6C6EFEF",
"IB c #7B7BBDBDEFEF",
"JB c #7B7BA5A5EFEF",
"KB c #7373ADADDEDE",
"LB c #6B6BC6C6EFEF",
"MB c #6B6BBDBDDEDE",
"NB c #6B6BADADEFEF",
"OB c #6B6B9C9CDEDE",
"PB c #5A5AA5A5DEDE",
"AC c #5A5A9C9CE7E7",
"BC c #52529494DEDE",
"CC c #4A4AADADE7E7",
"DC c #4A4A9494E7E7",
"EC c #4A4A8C8CDEDE",
"FC c #29299C9CE7E7",
"GC c #29297373E7E7",
"HC c #18188C8CE7E7",
"IC c #10107B7BDEDE",
/* pixels */
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAAAACAFAFAIAFAIAFAFAFACAAAAAAAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAACAIACAAAAAAAAAAAAAAAAACAIACAAAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAFACAAAAAAAAAAAAAAAAAAAAAAAAACAFACAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAACAFACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAFACAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAACAFAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAFACAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAFAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAAAAAAAFAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAFACAAAAACAAAAAAAAACABAEACAGAAAAAAAAAAAAACAAACAFAAAAAAAAAAAAA",
"AAAAAAAAAACAFAAAAACAAAAAEAHANADBDBHBIBIBHBEBNAEAAAAAAAAAAAFAAAAAAAAAAAAA",
"AAAAAAAAAACACAAACAAAEAJADBDBDBDBDBABDBLBIBMBCCCCIBFAAACAAACAFAAAAAAAAAAA",
"AAAAAAAAAAFAAACAAAHANADBABJAGAGAAAEAAAAAEAJALBPBFCFCEBAAAAAAFAAAAAAAAAAA",
"AAAAAAAACAFABACANADBMAJABAAACAAACACAEAEAAAAAAAEADBFCHCECEAAAFACAAAAAAAAA",
"AAAAAAAAFACAAAJADBNAGAAACACALADBGBIBIBIBEBDBDAAACAFADCICKBCAAAFAAAAAAAAA",
"AAAAAAAACACACACAHABACACANAFBFBBBDBFBFBJBMBBCDCKBCACAEAKBEACACACAAAAAAAAA",
"AAAAAAAAFAAACACABACAEAFBFBBBLAHAEACACAEAHAFBDCGCBCIAAACACADAAAFAAAAAAAAA",
"AAAAAAAAFACACACACACAEBBBNAEACACACAFAFACABACAFANBGCBCCACACACACAFAAAAAAAAA",
"AAAAAAAAFACACAFACAFACALACACAFAPAGBOBECOBGBFACADAGBCADACAFACACAFAAAAAAAAA",
"AAAAAAAAIACAFACAFACADACAFAPAJBNBJBACNBECECECGBCACADACAFACAFACAIAAAAAAAAA",
"AAAAAAAAFAFACAFACADAFAFACAEBJBEBFACAFAEAKBECOBDADAFAFACAFACAFAFAAAAAAAAA",
"AAAAAAAACAIACAFAFAEACAFAFAFAFADAFALAFAFADALAFADADADAEAFAFACAIACAAAAAAAAA",
"AAAAAAAAAAKAFACAFAFAFAFAFAFAFAFAKBMBCCKBFADAFAFAEAFAFAFACADAKAAAAAAAAAAA",
"AAAAAAAAAAFAIACAFAFAFAFAFAFAFAJAHBLBHBIBFAFAFAFAFAFAFAFACAIAFAAAAAAAAAAA",
"AAAAAAAAAACAOAFACAFAFAFAFAFAFAFAFAABABFAFAFAFAFAFAFAFACAFAOACAAAAAAAAAAA",
"AAAAAAAAAAAAEAOAFACAIAFAIAFAIAFAIAFAJAIAIAFAIAFAIAFACAFAOACAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAFACBFAAAIAFAIAFAIAFAIAFAIAFAIAFAIAFACAFAOAIAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAACADACBKACACAIAIAFAIAFAIAFAIAIAIACACAIACBIAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAACAFAOAOAIACAAAFADAIAFAFACACACAFACBOAIAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAACAKACBCBKAIAFADAFAFAIAKACBCBIADAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAACAIAKACBCBCBCBCBOAOAIAFAAAAAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAAAAAACACAFAFAFAFACACAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"};

