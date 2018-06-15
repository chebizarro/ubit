#include <iostream>
#include "ubit/ubit.hpp"
using namespace std;
using namespace ubit;


void foo(const char* msg, UStr* str) {
  cout <<"Event: " << msg << " / chars=[" << str->c_str() << "]" << endl;
}


int main(int argc, char *argv[]) {
  UAppli appli(argc, argv);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

  // simple editable text field with callbacks

  UStr& str = ustr();

  // will be called when the string content is changed
  str.onChange( ucall("onChange", &str, foo) );

  UBox& part1 = 
  uvbox(UBorder::etchedIn + upadding(4,4)
        + ulabel("Simple editable text field with Callbacks")
        + utextfield( // will be called when the string content is changed
                     UOn::strChange / ucall("UOn::strChange", &str, foo)
                     // will be called when the Enter/Return key is pressed
                     + UOn::action / ucall("UOn::action", &str, foo)
                     + UFont::large + UColor::navy + str)
        );

  // multi-colored multi-font editable text
  UBox& part2 = 
  uvbox(UBorder::etchedIn + upadding(4,4)
        + ulabel("Multi-colored multi-font editable text")
        + utextfield( UFont::large +"xyz" + UColor::orange 
                     + uelem(UFont::bold + "abcd")
                     + UColor::blue + "1234")
        );

  // multi-line warped text
  UBox& part3 = 
  uvbox(UBorder::etchedIn + upadding(4,4) 
        + ulabel("Multi-line warped text containing various objects")
        + utextarea(// we must specify the size of the text area
                    usize().setHeight(130)
                    + UFont::large 
                    + "xyz"
                    + UColor::orange + uelem(UFont::bold + "abcd")
                    + UColor::blue + "1234\n"
                    + UColor::navy
                    + UPix::diskette 
                    + uelem(UFont::italic + "Multi-line warped text")
                    + UPix::question + "\n"
                    + UColor::black
                    + " " + ulinkbutton("pseudo-link")
                    + " " + utextfield( UFont::small + "internal textfield" ) 
                    + ucheckbox(UFont::medium + "checkbox") + "\n\n"
                    + UColor::orange
                    + uelem(UFont::overline + "Overline")
                    + UColor::green
                    + uelem(UFont::strikethrough + "strikethrough")
                    + UColor::black
                    + uelem(UFont::underline + "underline") 
                    )
        );
  
  // Multi-line warped text in a Scrollpane
  UBox& part4 = 
  uvbox(UBorder::etchedIn + upadding(4,4) 
        + ulabel("Multi-line warped text in a Scrollpane")
        + uvflex()
        + uscrollpane( // we must specify the size of the scroll pane
                      usize().setHeight(130) 
                      + utextarea(UFont::large 
                                 + "xyz"
                                 + UColor::orange 
                                 + uelem(UFont::bold + "abcd")
                                 + UColor::blue + "1234"
                                 )
                      )
        );
  
  
  UFrame& frame = uframe
    (usize().setWidth(400) 
     + upadding(10,10) + uvspacing(10)
     + utop() 
     + part1
     + part2
     + uvflex() // following objects flexible in the vertical direction
     + part3
     + part4
     );
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

  appli.add(frame);
  frame.show(true);
  return appli.start();
}


