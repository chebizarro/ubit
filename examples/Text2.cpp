#include <iostream>
#include <ubit/ubit.hpp>
using namespace std;
using namespace ubit;


int main(int argc, char *argv[]) {
  UAppli appli(argc, argv);
  appli.setImaPath("../images");   // location of the images

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - 

  UBox& h1 = uflowbox
    (UFont::bold + UFont::xx_large + UColor::blue + upadding().setHeight(6)
     + "Title Title Title Title Title Title Title Title Title Title Title"
     );

  UBox& h2 = uflowbox
    (UFont::bold + UFont::large + UColor::orange + upadding().setHeight(3)
     + "SubtitleSubtitle Subtitle Subtitle Subtitle Subtitle Subtitle Subtitle Subtitle Subtitle ");

  UBox& p_1 = uflowbox
    ("para 1 para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para ");

  UBox& p_2 = uflowbox
    ("para 2 para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para para ");

  UBox& li_1 = uflowbox
    ("List item 1 list item 1 list item 1 list item 1 list item 1 list item 1 list item 1 list item 1 list item 1 list item 1 list item 1 list item 1 list item 1 list item 1 list item 1 list item 1 list item 1 ");

  UBox& li_2 = uflowbox
    ("List item 2 list item 2 list item 2 list item 2 list item 2 list item 2 list item 2 list item 2 list item 2 list item 2 ");

  UBox& li_3 = uflowbox
    ("List item 3 list item 3 list item 3 list item 3 list item 3 list item 3 list item 3 list item 3 list item 3 list item 3 list item 3 list item 3 list item 3 list item 3 list item 3");


  UBox& li_4 = uflowbox
    ("sublist item A sublist item A sublist item A sublist item A sublist item A sublist item A sublist item A sublist item A sublist item A sublist item A");

  UBox& li_5 = uflowbox
    (UBackground::wheat
     + "sublist item B sublist item B sublist item B sublist item B sublist item B sublist item B sublist item B sublist item B sublist item B sublist item B sublist item B sublist item B sublist item B sublist item B sublist item B sublist item B sublist item B ");

  UBox& li_6 = uflowbox
    ("sublist item C sublist item C sublist item C sublist item C sublist item C sublist item C sublist item C sublist item C sublist item C sublist item C sublist item C sublist item C ");


  UBox& ul_2 = uflowbox
    (upadding(15,10) + uvspacing(8) + UColor::green
     + li_4 + li_5 + li_6
     );


  UBox& ul_1 = uflowbox
    (upadding(15,10) + uvspacing(8)  + UColor::blue
     + "begin ul" 
     + li_1 + li_2 + ul_2 + li_3 
     + "end ul"
     );


  UBox& page = 
  uflowbox (UBackground::white + upadding(5,5)
            + " "
            + h1
            + h2
            + p_1
            + ul_1
            + p_2
            + " "
            );

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - 

  UFrame& frame = 
  uframe(usize().setWidth(500) + uhflex() 
         + uscrollpane(page)
         );

  appli.add(frame);
  frame.show(true);
  return appli.start();
}


