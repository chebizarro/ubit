/* ==================================================== ======== ======= *
 *
 *  Example3.cpp: a simple example for editing text.
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
  UFont font;
  UColor color;
  UBackground background;
};


int main(int argc, char* argv[]) {
  UAppli appli(argc, argv);
  Example example;
  example.show();
  return appli.start();
}


Example::Example() {
  textearea.add(font + color + background + "Hello World");

  // the difference with the previous example is that the "Font" text is contained
  // in a ulabel() that contains a UFont::bold attribute. This text will thus be
  // shown in bold characters (note that if UFont::bold was in the uhbox, the button
  // names would also be bold because fonts are inherited in the instance graph)
  
  UBox& font_box = 
  uhbox(ulabel(UFont::bold + "Font: ")
        + ubutton("Bold"   + uassign(font, UFont::bold))
        + ubutton("Italic" + uassign(font, UFont::italic))
        + ubutton("Normal" + uassign(font, UFont::plain))
        );

  UBox& color_box = 
  uhbox(ulabel(UFont::bold + "Colors: ")
        + ubutton("Red"   + uassign(color, UColor::red))
        + ubutton("Green" + uassign(color, UColor::green))
        + ubutton("Blue"  + uassign(color, UColor::blue))
        );
  
  UBox& background_box = 
  uhbox(ulabel(UFont::bold + "Background: ")
        + ubutton("None"  + uassign(background, UBackground::none))
        + ubutton("Wheat" + uassign(background, UBackground::wheat))
        + ubutton("White" + uassign(background, UBackground::white))
        );
  
  // In contrast with the previous example, layout constraints are specified to
  // make the GUI more visually attractive. Besides, the size of the widgets will 
  // be changed a reasonable way when the the frame is interactivelly resized.
 
  // The general principle is quite simple: uvflex() means that the following 
  // widgets are vertically flexible (i.e. resized when the frame height changes)
  // and ubottom() that the following widgets will keep their prefered size. 
  
  // Besides, font_box, color_box and background_box are contained in a nested vbox.
  // The layout would be the same without this hbox but it makes it possible to
  // specify a 'padding', a vertical 'spacing' and 'border' (by the ways, note that 
  // a padding and a vertical spacing was also added to the frame.
  
  // UPadding is an attribute that specifies the internal margins of a widget
  // (which is quite similar to XML CSS paddings). USpacing specifies the amount
  // ou space between the children of a widget. UBorder specifies the decoration
  // and the size of the border of a widget. These attributes can be added to all
  // widgets. This will override the default values of these attributes for a
  // given widget class (for instance, by default, UButtons have a border but 
  // UBoxes don't have one). In fact, each Ubit widget have a corresponding 
  // UStyle that defines these default value, and styles can be cascaded in the
  // same way as for XML Cascaded Style Sheets.
    
  add(usize(400, 300) + upadding(5,5) + uvspacing(5)
      + uvflex() 
      + textearea
      + ubottom()
      + uvbox(upadding(0,5) + uvspacing(10) + UBorder::etchedIn
              + font_box + color_box + background_box)
      );
}


