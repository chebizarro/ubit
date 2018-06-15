
#include <iostream>
#include <ubit/ubit.hpp>
#include <ubit/uxpm.hpp>
using namespace std;
using namespace ubit;

// Example extends UFrame, a subclass of UWin (Ubit windows)

class Example : public UFrame {
public:
  Example();
  void setFont(UFont*);
  void setColor(UColor*);
  void setBgcolor(UColor*);
  void openFile();

private:
  UTextarea   textbox;
  UStr        content;    // the text content of the textbox
  UBackground bgcolor;    // background of the textbox
  UColor      color;      // color of the textbox
  UFont       font;       // font of the textbox
  UBox        controls;   // UHbox = horizontal box
  UFilebox    filebox;    // to select a file interactively
};

// =================================================================

int main(int argc, char* argv[]) {
  UAppli appli(argc, argv);  // must be first
 
  Example ex;           // Example extends UFrame (and UWin)

  appli.add(ex);
  ex.show(true);        // makes the 'exemple' UFrame visible
 
  return appli.start();   // starts the event loop
}

// =================================================================

Example::Example() {	

  // content is a UStr (Ubit string). It is empty by default.
  content = "Editable text";

  textbox.add
    (usize(400, 250)              // size of textbox (in pixels)
     + font + bgcolor + color     // the font and colors of textbox
     + content                    // text contained in textbox
     );
  
  controls.add                    // this is a UHbox (horizontal box)
    (UOrient::horizontal
     + uleft()
     + upix(UXpm::edit) + "Font: "
     // setFont() is a callback method. It is fired when the button is clicked
     + ubutton("Normal" + ucall(this, &UFont::plain, &Example::setFont))
     + ubutton("Bold"   + ucall(this, &UFont::bold,   &Example::setFont))
     + ubutton("Italic" + ucall(this, &UFont::italic, &Example::setFont))
     + "        "
     // the dialog is auto-opened when the button is clicked
     + ubutton(upix(UXpm::folder) + "Open file" + udialog(filebox))
     );

  // openFile() is a callback method. it is fired when the OK button
  // of the firebox is clicked 
  filebox.add( ucall(this, &Example::openFile) );
  
  this->add      // 'this' is a UFrame (vertical layout by default)
    (uvflex()     // the following children are flexible vertically
     + textbox
     + ubottom()  // the following children are NOT flexible vertically
     + controls
     );
}

// =================================================================
// callback methods (= member functions) can have 0 to 2 parameters
// callback functions (= non member functions) can have 1 to 2 parameters

void Example::setFont(UFont* f) {
  // font is changed to the value given as an argument.
  // this will auto-update the textbox and its content.
  font = *f;
}

void Example::setColor(UColor* c) {
  color = *c;
}

void Example::setBgcolor(UColor *c) {
  bgcolor = *c;
}

void Example::openFile() {
  // this method reads the file given as an argument (which is
  // the file name entered in the filebox). The text data is
  // stored in 'content'. This will auto-update the textbox.
  content.read(filebox.getPath());
}

 
