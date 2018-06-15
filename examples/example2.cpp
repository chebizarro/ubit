/* ==================================================== ======== ======= *
 *
 *  Example2.cpp: a simple example for editing text.
 *  Ubit GUI Toolkit
 *  (C) 2008 Eric Lecolinet / TELECOM ParisTech / http://www.enst.fr/~elc/ubit
 *
 * ==================================================== ======== ======= */

#include <ubit/ubit.hpp>
using namespace ubit;


class Example : public UFrame {
public:
  Example();
private:
  UTextarea textearea;
  UFont font;              // the current font
  UColor color;            // the current foreground color
  UBackground background;  // the current background
};


int main(int argc, char* argv[]) {
  UAppli appli(argc, argv);
  Example example;
  example.show();
  return appli.start();
}


Example::Example() {
  // Here, in contrast with the previous example, attributes are instance variables
  // whose values can be changed dynamically, typically by calling callback functions
  
  textearea.add(font + color + background + "Hello World");

  // hbox() creates a horizontal box widget, its arguments are the same as for the 
  // add() method seen before, i.e. a combination of attributes (which must come first)
  // and chidren (text, images and child widgets)  
  
  // In this case, this hbox contains buttons that will change the current font.
  // uassign() is a callback function that sets its left hand argument to its right
  // hand value. In this case, the "font" variable will be assigned to a prefined
  // UFont constant. uassign() just calls UFont::operator=
  
  // By default, callback functions (such as uassign()) are called when the widget
  // is activated (i.e. clicked, in most cases). We'll see later how to specify
  // other callback conditions.

  UBox& font_box = 
  uhbox("Font: "
        + ubutton("Bold"   + uassign(font, UFont::bold))
        + ubutton("Italic" + uassign(font, UFont::italic))
        + ubutton("Normal" + uassign(font, UFont::plain))
        );

  // This hbox contains buttons that changes the current color.
  UBox& color_box = 
  uhbox("Colors: "
        + ubutton("Red"   + uassign(color, UColor::red))
        + ubutton("Green" + uassign(color, UColor::green))
        + ubutton("Blue"  + uassign(color, UColor::blue))
        );
  
  // This hbox contains buttons that changes the current backgound.
  UBox& background_box = 
  uhbox("Background: "
        + ubutton("None"  + uassign(background, UBackground::none))
        + ubutton("Wheat" + uassign(background, UBackground::wheat))
        + ubutton("White" + uassign(background, UBackground::white))
        );
  
  // Here, we add a size attribute, the textarea and the hboxes that were previously
  // created to 'this', the "Example" widget. The resulting GUI won't be particularly
  // nice because we did not take care of the layout (which is the subject of the
  // next example...)
  
  add(usize(400, 300) + textearea + font_box + color_box + background_box);
}


