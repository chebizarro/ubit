/* ==================================================== ======== ======= *
*
*  Example1.cpp: a simple example for editing text.
*  Ubit GUI Toolkit
*  (C) 2008 Eric Lecolinet / TELECOM ParisTech / http://www.enst.fr/~elc/ubit
*
* ==================================================== ======== ======= */

#include <ubit/ubit.hpp>   // includes headers of main Ubit classes
using namespace ubit;      // Ubit classes are part of the "ubit" namespace


// this simple application is a UFrame (main window) that contains a textarea
class Example : public UFrame {
public:
  Example();
private:
  UTextarea textearea;  // the text area displayed in the frame.
};


int main(int argc, char* argv[]) {
  UAppli appli(argc, argv);   // the application context, must be created first
  Example example;            // our example (which is a UFrame)
  example.show();             // shows this frame
  return appli.start();       // starts the event main loop
}


Example::Example() {
  // The add() function can add a combination of attributes (which must come first)
  // and chidren (text, images and child widgets) to any Ubit widget.
  
  // In this case, a font and a color attribute and textual data are added to the 
  // textarea widget. The font and the color are predefined attributes which are
  // static constants or their respective classes. We'll see in the next example how 
  // to add instance variables. The text will be editable because it is contained 
  // in a UTextarea.
  
  textearea.add(UFont::italic + UColor::blue + "Hello World");
  
  // Here, we add a size attribute and the textarea instance variable to 'this',
  // which is the "Example" widget being created.
   
  add(usize(400, 300) + textearea);
}

