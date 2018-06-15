#include <iostream>
#include "ubit/ubit.hpp"
using namespace std;
using namespace ubit;


void foo(const char* s) {
  cout << s << " was activated \n";
}


int main(int argc, char *argv[]) {
  UAppli appli(argc, argv);
  appli.setImaPath("../images");   // location of the images

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - 
  // Table1

  const char* line0[] = {"", "Column 1", "Column 2", "Column 3"}; 
  const char* line1[] = {"Line 1", "Item 1:1", "Item 1:2", "Item 1:3"}; 
  const char* line2[] = {"Line 2", "Item 2:1", "Item 2:2", "Item 2:3"}; 
  const char* line3[] = {"Line 3", "Item 2:1", "Item 2:2", "Item 2:3"}; 
  const char** tabdata[] = {line0, line1, line2, line3};

  UTable& table1 = utable(UBackground::darkgrey + UBorder::shadowIn);
  UTrow* trow = null;

  // header
  table1.add(trow = &utrow());
  for (int c = 0; c < 4; c++) trow->add(ubutton(tabdata[0][c]));

  // body
  for (int l = 1; l < 4; l++) {
    table1.add(trow = &utrow());
    // left item
    trow->add(ubutton(tabdata[l][0]));
    for (int c = 1; c < 4; c++) 
      trow->add( uitem( UOn::idle/UBackground::white + tabdata[l][c]) );
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - 
  // Table2

  UElem& rowA = 
  utrow(ubutton(UFont::bold + UColor::navy + "button 1:1 ") 
        + utcell(1, 2,		// colspan, rowspan
                 UBackground::yellow + uedit()   // makes text editable
                 + "1:2 Texte editable en double ligne... ")
        + ubutton("button 1:3")
        + ubutton("button 1:4")
        + uscrollpane(true, false,  // scrollbars
                      ulistbox(uitem("aaaa ")
                               + uitem("bbbb ") 
                               + uitem("cccc ")
                               + uitem("1111 ")
                               + uitem("2222 ")
                               )
                      )
        );
  
  UElem& rowB = 
  utrow(ucheckbox("button 2:1")
        + utcell(2, 1, 		// colspan, rowspan
                 UBackground::white + UBorder::shadowIn
                 + uedit()
                 + "2:3 Texte editable en double colonne... ")
        + uspinbox(UBorder::etchedOut)
        );
    
  UTable& table2 = 
  utable(usize(300,UIGNORE)  // imposer une largeur (sinon prend tout l'espace disponible)
         + UBackground::darkgrey + uhspacing(5) + UBorder::etchedOut
         + rowA
         + rowB
         );
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - 
  // Table3

  UTrow& row1 = 
  utrow(utcell(UFont::bold + UColor::white + uedit() + "truc muchetruc ") 
        + utcell(2, 1,		// colspan and rowspan
                 "texte en double colonnne") 
        + utcell(UColor::white + UBackground::teal + UOn::arm / UBackground::red
                 + "Click here" + ucall("Click here", foo))
        + utcell(uedit()
                 + "this an editable cell, bla bla bla, bla bla, bla...")
        + utcell("encore une colonne\n")
        );
  
  UTrow& row2 = 
  utrow(UBackground::wheat
        + utcell(2, 1, 		// colspan and rowspan
                 UBackground::teal + UColor::yellow + uedit()
                 + "CE TEXTE EST EDITABLE\nune seconde fois\nle layout du parent\n(sauf dans le cas ou Width est fixe\n etc etc etc... "
                 + ucall("EDITABLE", foo)) 
        + utcell(uima("louvre.xpm") + ucall("LOUVRE", foo))
        + utcell(3, 1,  		// colspan and rowspan
                 UBackground::green
                 + ubutton(UFont::large + UFont::large + uedit()
                           + "Gros Bouton EDITABLE en Triple Colonne"
                           + ucall("Gros Bouton", foo))
                 )
        );
  
  UTrow& row3 = 
  utrow(utcell(UFont::italic + "italic italic italic italic italic ")
        + utcell(UFont::x_large + UFont::bold + "1234567890")     
        + utcell(uima("picasso.xpm") + ucall("PICASSO", foo))
        + utcell(UBackground::blue + ucheckbox("Check Me"+ ucall("Check Me", foo)))
        + utcell( uedit()
                 + "Width pour les UFlowView une seconde fois le layout du parent\n(sauf dans le cas ou Width est fixe a priori auquel cas...")
        );
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - 

  UTable& table3 = utable
  (usize(600,UIGNORE)   //imposer la meme largeur que celle d'une page standard
   + UBackground::orange
   + uhspacing(5) + uvspacing(5)
   + row1
   + row2
   + row3
   );

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - 

  UFrame& frame = uframe
    (upadding(10,5) + uvspacing(14)
     + utop()
     + ulabel(UFont::bold + UFont::xx_large + upadding(UIGNORE,10)
              + "This is an example with 3 Tables")
     + table1
     + table2
     + uvflex()
     + uscrollpane(table3)
     + ubottom()
     + utextfield("Status: Ready")
     );
  
  appli.add(frame);
  frame.show(true);
  return appli.start();
}
