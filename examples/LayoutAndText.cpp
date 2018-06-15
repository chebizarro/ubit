#include <iostream>
#include <ubit/ubit.hpp>
using namespace std;
using namespace ubit;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

class Demo : public UFrame {
public:
  Demo(const UArgs& a = UArgs::none);

private:
  UStr str;               // string entered in the text field
  UStr text;              // text body in the text area
  UScrollpane scrollpane;

  void enter();		        // callback function
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

int main(int argc, char* argv[]) {
  UAppli appli(argc, argv);

  Demo frame(usize(300,300));
  appli.add(frame);
  frame.show(true);

  return appli.start();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

Demo::Demo(const UArgs& a) : UFrame(a) {
	
  UBox& toolbar = ubar
    (uleft()    + ulabel( UFont::bold + " Enter: " )
     + uhflex() + utextfield( str + ucall(this, &Demo::enter) )
     + uright() + ubutton( UFont::bold + "  Ok  " + ucall(this, &Demo::enter) )
     );

  UFlowbox& textarea = uflowbox
  (UBackground::white + UColor::navy
   + uedit()           // makes the text editable
   + text
   );

  scrollpane.add(textarea);

  UBox& statusbar = uhbox(" Ready");
		
  add(utop()      + toolbar
      + uvflex()  + scrollpane
      + ubottom() + statusbar
      );
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

void Demo::enter() {
  text.append(str);
  text.append("\n");
  str = "";
  scrollpane.setYScroll(99.);
}

