#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <ubit/ubit.hpp>
using namespace std;
using namespace ubit;


class Events : public UFrame {
public:
  Events();
  
protected:
  UScrollpane msg_pane;
  UTextarea msg_area;
  
  UBox& createTimersBox();
  static void setTimerCB(UTextfield*, UTimer*);  
  void printCB(UEvent&, const char* message);
  void printTimerCB(UTimerEvent&, const char* message);
  void printSourceCB(UEvent&, int fd);  
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int main(int argc, char *argv[]) {
  UAppli appli(argc, argv);
  Events demo;
  demo.show();
  return appli.start();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Events::Events() {
  UBox& button = 
  ubutton("UButton"
          + UOn::action / ucall(this, "UOn::action", &Events::printCB)
          + UOn::click  / ucall(this, "UOn::click", &Events::printCB)
          + UOn::doubleClick / ucall(this, "UOn::doubleClick", &Events::printCB)
          + UOn::mpress  / ucall(this, "UOn::mpress", &Events::printCB)
          + UOn::mrelease/ ucall(this, "UOn::mrelease", &Events::printCB)
          );
  
  UBox& checkbox = 
  ucheckbox("UCheckbox"
            + UOn::action   / ucall(this, "UOn::action", &Events::printCB)
            + UOn::change   / ucall(this, "UOn::change", &Events::printCB)
            + UOn::select   / ucall(this, "UOn::select", &Events::printCB)
            + UOn::deselect / ucall(this, "UOn::deselect", &Events::printCB)
            );
  
  UBox& textfield = 
  utextfield(30, "UTextfield"
             + UOn::kpress   / ucall(this, "UOn::kpress", &Events::printCB)
             + UOn::krelease / ucall(this, "UOn::krelease", &Events::printCB)
             + UOn::ktype    / ucall(this, "UOn::ktype", &Events::printCB)
             + UOn::action   / ucall(this, "UOn::action", &Events::printCB)
             + UOn::change   / ucall(this, "UOn::change", &Events::printCB)
             + UOn::strChange/ ucall(this, "UOn::strChange", &Events::printCB)
             );
  
  UBox& widgets_box = 
  uhbox(uhspacing(6) + button + checkbox + uhflex() + textfield);

  UBox& canvas = 
  ubox(usize(400,200) + UBackground::white + UBorder::line
       + uhcenter() + uvcenter() + UFont::italic
       + UColor::navy + "Move, Press, Drag the mouse in this box"
       + UOn::mpress  / ucall(this, "UOn::mpress", &Events::printCB)
       + UOn::mrelease/ ucall(this, "UOn::mrelease", &Events::printCB) 
       + UOn::mdrag   / ucall(this, "UOn::mdrag", &Events::printCB)
       + UOn::mmove   / ucall(this, "UOn::mmove", &Events::printCB)    
       );
    
  UBox& timers_box = createTimersBox(); 

  msg_pane.add(usize(UIGNORE,150) + msg_area);

  add(UOrient::vertical + upadding(6,6) + uvspacing(6)
      + utop() + widgets_box + canvas + timers_box
      + uvflex() + msg_pane
      );
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UBox& Events::createTimersBox() {
  // create a timer: delay=600, ntimes=-1 (-1 means infinite), auto_delete=false
  // which means that the timer is not destroyed on completion
  UTimer* timer1 = new UTimer(600, -1, false);
  
  // printTimerCB will print a message when timer1 is fired
  timer1->onAction(ucall(this, "Timer *1*", &Events::printTimerCB));
  
  // this textfield controls the delay of timer1
  UTextfield* tf1 = &utextfield(10, "600");
  tf1->addAttr(UOn::strChange / ucall(tf1, timer1, &Events::setTimerCB));
  
  // another timer
  UTimer* timer2 = new UTimer(2000, -1, false);
  timer2->onAction(ucall(this, "Timer #2#", &Events::printTimerCB));
  
  UTextfield* tf2 =  &utextfield(10, "2000");
  tf2->addAttr(UOn::strChange / ucall(tf2, timer2, &Events::setTimerCB));
  
  // create an input source that fires a callback
  USource* source = new USource();
  int fd = 0;   // 0 = the standard input
  
  // printSourceCB will print a message when source is fired
  source->onAction(ucall(this, fd, &Events::printSourceCB));
  source->open(fd);
  
  return
  uvbox(upadding(4,4)
        + uhbox("UTimer 1: " + *tf1 + "mS " 
                + ucheckbox(" Run " 
                            + UOn::select / ucall(timer1, &UTimer::start)
                            + UOn::deselect / ucall(timer1, &UTimer::stop)))
        + uhbox("UTimer 2: " + *tf2 + "mS " 
                + ucheckbox(" Run " 
                            + UOn::select / ucall(timer2, &UTimer::start)
                            + UOn::deselect / ucall(timer2, &UTimer::stop)))
        + uhbox("USource: " 
                + UFont::italic + " Type text in the shell terminal then press Return")
        );
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Events::setTimerCB(UTextfield* tf, UTimer* timer) {
  UStr value = tf->retrieveText();
  timer->setDelay(value.toInt());
}

void Events::printCB(UEvent& e, const char* msg) {
  UStr& s = ustr(msg); s &= "\n";
  msg_area.add(s);
  msg_pane.setYScroll(100.);
  cout << s << endl;
}

void Events::printTimerCB(UTimerEvent& e, const char* msg) {
  UStr& s = ustr(msg);
  s &= " time: ";
  s &= double(e.getWhen());
  s &= "\n";
  msg_area.add(s);
  msg_pane.setYScroll(100.);
  cout << s << endl;
}

void Events::printSourceCB(UEvent& e, int fd) {
  char buffer[200];
  sprintf(buffer, "source from %d: ", fd);
  int l = strlen(buffer);
  read(fd, buffer+l, sizeof(buffer)-l-1);
  
  msg_area.add(buffer);
  msg_pane.setYScroll(100.);
  cout << buffer << endl;
}



