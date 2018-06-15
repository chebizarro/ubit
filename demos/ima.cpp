/* ==================================================== ======== ======= *
*
*  uima.cpp: a simple image viewer
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
#include <ubit/ubit.hpp>
#include <ubit/uxpm.hpp>
#include "ima.hpp"
using namespace std;
using namespace ubit;

static const float ZOOM_MAG = 1.33;
extern const char *FILE_xpm[], *SETTINGS_xpm[], *TOOLS_xpm[];

void scale2str(UScale* scale, UStr* str) {
  str->setFloat(*scale);
}

void str2scale(UStr* str, UScale* scale) {
  *scale = str->toFloat();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Ima::Ima() {
  File     = new UIma(FILE_xpm);
  Tools    = new UIma(TOOLS_xpm);
  Settings = new UIma(SETTINGS_xpm);
  font     = UFont::bold;
  foreground = UColor::white;
  background = UColor::black;
  scale_str = "1";
  
  // imascale controls the scale of the image. When its value is changed, the box
  // that contains the image is automatically updated.
  // - imascale is modified by several GUI widgets. These widgets change its 
  //   value directly by firing UScale::mult, UScale::div, UScale::set
  // - scale_str displays the value of the current imascale. It must be updated
  //   when imascale id changed. The scale2str callback does this job:
  imascale.onChange( ucall(&imascale, &scale_str, scale2str) );

  //================================================================

  UCall& not_impl = ucall(&alertbox, "Not implemented", 
                          (void(UAlertbox::*)(const char*))&UAlertbox::showMsg);

  openbox.add(utitle("Open Image") + UOn::action / ucall(this, &Ima::openFile));
  
  UMenu &file_menu = umenu
    (ubutton(UPix::ofolder  + " Open" + umenu(openbox))
     + ubutton(UPix::diskette + " Save" + not_impl)
     + usepar()
     + ubutton(UPix::stop     + " Quit" + ucall(0, &UAppli::quit))
     );
  
  UMenu &tools_menu = umenu
    (ubutton(UPix::plus  + " Zoom -" 
             + ucall(&imascale, ZOOM_MAG, &UScale::div))
     + ubutton(UPix::minus + " Zoom +" 
               + ucall(&imascale, ZOOM_MAG, &UScale::mult))
     + uhbox("... Zoom = "
             + utextfield(usize(50,UIGNORE) + scale_str
                          + ucall(&scale_str, &imascale, str2scale))
             + uflatbutton("Ok" + ucall(&scale_str, &imascale, str2scale)))
     );
  
  UMenu &settings_menu = umenu
    (ubutton(UPix::undo   + "Undo..."   + not_impl)
     //+ ubutton(UPix::colors + "Colors..." + not_impl)
     //+ ubutton(UPix::edit   + "Edit..."   + not_impl)
     );
  
  //================================================================
  // menubar, toolbar and contextual menu
  
  UArgs bprops = upadding(2,2);

  UMenubar& menubar = 
  umenubar(ubutton(bprops + File + file_menu)
           + ubutton(bprops + Tools + tools_menu)
           + ubutton(bprops + Settings + settings_menu)
           + uhflex() + ulabel()
           );
  
  UBox& toolbar = 
  uhbox(uleft()
        + ubutton(UOrient::horizontal + " Image: " + umenu(openbox))
        + uhflex()
        + utextfield(UBackground::none + UColor::black
                     + ucall(this, (const UStr*)&current_file, &Ima::openFile)
                     + current_file) 
        );
  
  // control menu  
  UCtlmenu& cmenu = *new UCtlmenu;
  UScrollAction* sa = new UScrollAction(scrollpane);
  cmenu.item(0).add(UPix::bigRight + *sa);
  cmenu.item(4).add(UPix::bigLeft + *sa);
  cmenu.item(2).add(UPix::bigUp + *sa);
  cmenu.item(6).add(UPix::bigDown + *sa);
    
  UBox& scalebox = uhbox
    (upadding(3,5)
     + uleft() + "  Zoom: " 
     + uflatbutton(bprops + " - " 
                   + ucall(&imascale, ZOOM_MAG, &UScale::div))
     + uflatbutton(bprops + " + " 
                   + ucall(&imascale, ZOOM_MAG, &UScale::mult))
     + utextfield(UBackground::black + UColor::white
                  + usize(50,UIGNORE) + UBorder::line
                  + scale_str + ucall(&scale_str, &imascale, str2scale))
     + uhflex() 
     + ulabel(UFont::plain + UFont::italic + uhcenter()
              +"Press any mouse button to open the Menu")
     );
  
  UBox& imabox = ubox
    (imascale + upadding(2,2) 
     + UBackground::none
     + cmenu
     // cmenu.show() is called when the mouse is pressed
     + UOn::mpress / uopen(cmenu)
     + UOn::kpress / ucall(this, &Ima::keyPressCB)
     + utop() + uleft() + image
     );
  
  //================================================================
  
  scrollpane.add(imabox);
  customizeScrollButtons();

  UBox& mainbox = uvbox
    (background + foreground + font 
     + utop() 
     + menubar 
     + toolbar
     + uvflex()
     + scrollpane
     + ubottom()
     + scalebox
     );
     
  alertbox.show(false);
  this->add(mainbox + alertbox);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Ima::keyPressCB(UKeyEvent& e) {
  switch (e.getKeyChar()) {
    case '-':
      imascale.div(ZOOM_MAG);
      break;
    case '+':
      imascale.mult(ZOOM_MAG);
      break;
    case '=':
      imascale = 1;
      break;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Ima::openFile(const UStr* fname) {
  if (!fname) return;

  scrollpane.setScroll(0,0);
  if (image.read(*fname) > 0) {  // if the file could be read
    current_file = *fname;
    alertbox.show(false);
  }
  else alertbox.showMsg("Can't open file: " & *fname);
}

void Ima::openFile() {
  openFile(&openbox.getPath());
}

void Ima::customizeScrollButtons() {
  UScrollbar* sbar = scrollpane.getHScrollbar();
  UBox *b;

  if (sbar && (b = sbar->getLessButton())) {
    b->removeAll(); b->add(upix(UXpm::leftarrow));
  }
  if (sbar && (b  = sbar->getMoreButton())) {
     b->removeAll(); b->add(upix(UXpm::rightarrow));
  }

  sbar = scrollpane.getVScrollbar();
  if (sbar && (b = sbar->getLessButton())) {
   b->removeAll(); b->add(upix(UXpm::uparrow));
  }
  if (sbar && (b  = sbar->getMoreButton())) {
     b->removeAll(); b->add(upix(UXpm::downarrow));
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

const char *FILE_xpm[] = {
    "39 23 16 1",
    "A c #BDBDD6D6D6D6",
    "B c #9C9CC6C6CECE",
    "C c #9494BDBDC6C6",
    "D c #8C8CADADBDBD",
    "E c #7B7BB5B5C6C6",
    "F c #7B7BADADB5B5",
    "G c #6B6B9C9CADAD",
    "H c #4A4A8C8C9494",
    "I c #39396B6B7373",
    "J c #313173738C8C",
    "K c #313152525A5A",
    "L c #21215A5A6B6B",
    "M c #18184A4A5252",
    "N c #101031314242",
    "O c #080821212121",
    "P c None s None",
    "PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP",
    "PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP",
    "PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP",
    "PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP",
    "PPPPPOOOOOOOOPOOPOPOOPPPPPPOOOOOOOOPPPP",
    "PPPPPMGJHHJGMOLGJPOMGHOPPPPNGJHHHHIOPPP",
    "PPPOOGGJJHJCJNGFCOOGGBOOPOOHFIJHJHCNPPP",
    "PPPPOJIFACCAKOGJDOOJJCNPPPOJIEACCCEOPPP",
    "PPPPOHHCMMONNOHHCONHHDOOPPOHJCMMNNOPOPP",
    "PPPPOHJDMPOOPNHHCPOHJCNPPPPHJCMOPOPOPPP",
    "PPPOPHJALILNNOHHDNOHHDOPPOPJHCHLIMNPPPP",
    "PPPPOGJGCDCCMNGJCPNHICNOPPOHJGDDFAGOPPP",
    "PPPPOJHIHJGEMOHHDNOHHDOPPPOIHLHHIGGPPPP",
    "PPPOPGJBHJLLOMHHCPNHICNPPOPHJCHIIINOPPP",
    "PPPPNJHCNOPPOOHHDNOHJCOOPPNIHCMMPNPPPPP",
    "PPPPPHHDNPOPPMHHDPNHHCMPNPNHJDMPMPPOPPP",
    "PPPOOHICNPPPPNHHDNPHIAJHIIMJHCHJIJIPOPP",
    "PPPPPHJCNPPPOPGICPMHILHJHEMHJLHHJGEMPPP",
    "PPPOOGAAPOPPPMGACPNHACCCCCMHACCCCCCNPPP",
    "PPPPPPMNOPPPPPOLONPNMLLKLLPNMLLLKMNPPPP",
    "PPPPPOPPPPPPPPPPPPPPPPPPPPOPPPPPPPPPPPP",
    "PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP",
    "PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP",
};  


const char *SETTINGS_xpm[] = {
    "89 24 17 1",
    "A c #BDBDDEDEE7E7",
    "B c #A5A5CECED6D6",
    "C c #9494BDBDC6C6",
    "D c #7373B5B5C6C6",
    "E c #7373A5A5ADAD",
    "F c #5252A5A5ADAD",
    "G c #52528C8CA5A5",
    "H c #525284849494",
    "I c #525273737B7B",
    "J c #39396B6B7B7B",
    "K c #29296B6B8484",
    "L c #212142424A4A",
    "M c #18185A5A6363",
    "N c #101063637B7B",
    "O c #080842424A4A",
    "P c #080821212929",
    "Q c None s None",
"QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ",
    "QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ",
    "QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ",
    "QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ",
    "QQQQQQQPPOPPQQQQPPPPPPPPQPPPPPPPPPPPPPPQPQPPQQPQPPQQQQQQPPQQQQQQPPOPOPQQQQQQQPPPOPQQQQQQQ",
    "QQQQQQPLECEIOPQPJHHJHJHLOHHHKHHIMHJHJHJFPMHJPPPHHIPPQQQLHIOQQQQOJGEEEHJPPQQPQOHCEHMPQQQQQ",
    "QQQQQPJADFFBCOPOCHHHHHFFHEKHHKHDFDHKHHKBMHBDMQMAKAHQPQPJDBLQPPOCBDFHFDADLPQQOBAFFCAMPQQQQ",
    "QQQQQOBBCDCDBEPLFKCDDDBIKBBENCBCFBCENDCALIFHMQJDPJBMQPQHKCLPPLACBCEFDBDCBOPPEBCDDCCBPQQQQ",
    "QQQQPJBFDOJCDGPMGFELLLLPPOMFHBOLPLJEHEMLPEGFLPLCONBEPQPKHCLPPBBBDLLOLLABABPLBFDJODDELQQQQ",
    "QQQQQIDDGOPOLOPLGHEPPPQPQQOFHEPQPQOFKFPPQIEGLPJEHEDAJPPKHCMPJADCOPQQQPLGEHPLCFCOPOLOPQQQQ",
    "QQQQQLBFAEKOQPQMGEELLLOQPQOHHCPQQPPEKFPQPHEHMQJCKFCADOPJHEJPCFBMQPQQPQPQPQPODCDCHOPQQPQQQ",
    "QQQQQPHADDCCJPPLFJBCCCBMQPPGHEPQQQLFKGPPQIEGMQJEFKGAAJOHHCLOCGCOPQPOQOPOPOPPLBCDCCHOPQQQQ",
    "QQQQQPQHCBDDAHPOGMJHKHCMQQOGICQPQQLFKEPQQEGHMPLCJGOCAAOHKCJOFGEQPLGGFGGHGFJPOMCCDDAEPQQQQ",
    "QQQQQQPQOJCBDBOMFHAKHHJPQPPGGEPQQPPEKGPQPIEGMQJDHJOMAAFHHCLOCECOQJDGJHHJJGCOQQPJDAFAJPQQQ",
    "QQQQPPPOQPOCDCJOGFHPPPPQQQOHHCQPQQLGKFPQQHEHMPJEHJPOGADFHDJQCDCKOLCCCDCBMGEPOOQQOHCDHQPQQ",
    "QQQPQJDGOOQHDFJMGGEPPQPQPQPFHEPQQQPFHHPPQHGFLPLEHKPQLAFGMCLPJACBMOPPOOMBFBJJGDMMQMCGGPQQQ",
    "QQQQPECAEMJBCAOMGECMJNMLQPOHHCQPQQLFKEPQPHEHLPJCHJQPPGAMOCLPPEABBGJLMGBBBDPJACBMKDBAIQQQQ",
    "QQQQQLACCCCCAGPMGMGGHGEIPQPGHEPQQQLGKEPQQHGHJQJEKKPQPPCFQEJQQPDADCCCCCDADLQOCCCCCCBCPPQQQ",
    "QQQQPPJCBGDAGLQMCDEEEECGQPPEACQPQQOCBFPQQHACLPLABJQQQPLACALQQPOJBCDGGCBGOQPQMCBDGBCMQQQQQ",
    "QQQQQQQOHGHJOQPQLIIIJIJLQQPLJLQQQPQLJLQPQPIJPQQIJPPQQQPJJJPQQQQQLJHHHHLOQQQQQQJGGJMQQQQQQ",
    "QQQQQQQQQPQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQPQQQQQQQQQQQQQQQQQQQQPQPQQQQQQQOQQQPQQQQQQQQ",
    "QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQPQQQQQQQQQ",
    "QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQPQQQQ",
    "QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ",
  };
  
  
const char *TOOLS_xpm[] = {
  "66 24 19 1",
  "A c #FFFFFFFFFFFF",
  "B c #BDBDDEDEEFEF",
  "C c #A5A5CECED6D6",
  "D c #9494C6C6D6D6",
  "E c #8C8CB5B5BDBD",
  "F c #6B6BB5B5BDBD",
  "G c #6B6B9494A5A5",
  "H c #525284849494",
  "I c #525273737B7B",
  "J c #4A4A9C9CA5A5",
  "K c #42426B6B7373",
  "L c #31317B7B9494",
  "M c #313163637373",
  "N c #21214A4A5A5A",
  "O c #181863636B6B",
  "P c #080842425252",
  "Q c #080829293939",
  "R c #000018181818",
  "S c None s None",
  "SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS",
  "SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS",
  "SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS",
  "SSSSSSSRSSRSSSSSSSSRSSRSSSSSSSSSSSSRSSRSSSSSSSSSSSSSSSSSSRSSSSSSSS",
  "SSSSSRRRRRRRRSSSSRRRPQRRSRSSSSSRSRRQQQRRSRSSSRRRSSSSSRSRRQQRSSSSSS",
  "SSSSQNJILHLHMRSRQNHEGEGKPSRSSSSRQNHEGEGNQRSSSPIKNRSSSSQMEEGMQRSSSS",
  "SSSSQEHLHILGFQQQHCDGJLEDDNQSSRRPGBDJLJEBEPRRSKEFMSRSRRLBEJEBGQRSSS",
  "SSSSQGCDLHCDGQQGBEDDJFDBEANRRRPDBEDFFFDDCDNRRMLJISSSRNBDBJDDBMRSSS",
  "SSSSSQNIJEHNQQLBDDHPQQNFBDBPRREDDCMNRNMEBCEQRIJLKRSSRGEDLPLDDISRSS",
  "SSSSSSRMLEISRPDDCNRRSRRQGDDIQMCDDPRSRSRPEDBNQMJHLSSSRJGDHPQNPQRSSS",
  "SSSSSRRNJGIRRMDEGQRSSSSRPDEDPGEDMRSSSSSQODEGQILJKSSRSKBFCGOQRSSSSS",
  "SSSSSSRMLEISQHGFMRSSSSSSRFJEOEJGNRSSSSSSQFJEPILJKRRSSQGCFDCENQRSSS",
  "SSSSSSSMHFKRRGJJNSSSSSSRRHJJLGJJQSSSSSSRNHJJQLJKKSSSRRQHDDEDCNRSSS",
  "SSSSSSRMLEISRHFEMRRSSSSSQEJFMFJENRRSSSSRQDJGPKLJKQSSSSRQPIDEDEQSSS",
  "SSSRSSRNGGIRRNDEEQRSSRSQNDEDQHDDIRRSSSRRLDDGRLHJKSRSRQQRQRPDJCNSSS",
  "SSSSSSRMLEISRQDDBHQRRRRPDDBMQNDDBMQRRRQNBECNRIJLKQRRQGGHQQPJJBQRSS",
  "SSSSSSRMHFKSSRMCFBEMNNIDEDEQRQHBFBGMNNHBEBHQRKJHGMMMOCDBHPHDDCQSSS",
  "SSSSSSSOLGIRSRRIBDDEDEDEDBPRSSQHBEDEDEDFBEQRSMLOHJLEHLBDDDDCBMRSSS",
  "SSSSSSRMDBKSSSRRNEDEHJDBGQRSSRRQMDDFLEDDHQRSRKDDGEGDHPICDJEBHRRSSS",
  "SSSSSSSRKIQSSSSSRQMHJHIPRRSSSSSSRPKHHHKNRRSSSQKKIKKKNRQNIHHNRRSSSS",
  "SSSSSSSSSSSSSSSSSRSSRQSRSSSSSSSSSSRSRRSSSSSSSSSSSSSSSSSRSQSRSSSSSS",
  "SSSSRSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSRSSSSSSSSS",
  "SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSRSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS",
  "SSSSSSSSSSSSSSSSSSSSSSSSSSSRSSSSSSSSSSSSSSSSSSSSSSRSSSSSSSSSSSSSSS",
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

