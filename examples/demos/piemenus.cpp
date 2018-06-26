/* ***********************************************************************
 *
 *  piemenus.cpp (part of udemo)
 *  A simple example that illustrates how to use Pie menus.
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

#include <vector>
#include <cmath>
#include <ubit/ubit.hpp>
#include "piemenus.hpp"
using namespace std;
using namespace ubit;

PiemenuDemo::PiemenuDemo() 
{  
  if (!UAppli::isUsingGL()) {    // GL is required for alpha blending!
    uwarning("PieMenus","This application require OpenGL for proper rendering");
  }
  initToolbar();
  initMenus();
  
  // canvas - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  UBox& canvas = 
  ubox(uhcenter() + uvcenter() + UBackground::white 
       + main_menu + UOn::mpress / uopen(main_menu)
       //+ uima("frisco.gif")
       );
  
  // main window - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - -

  add(UOrient::vertical + uhflex()
      + utop() + toolbar
      + uvflex() + canvas
      + ubottom() 
      + ulabel(uhcenter() + UColor::navy + UFont::large
               + "Press the mouse in the canvas to open the Pie menu")
      );
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PiemenuDemo::initToolbar() {
  UMenu& colors_menu = 
  umenu(uitem(UBackground::red + "    " 
              + ucall(this, &UColor::red, &PiemenuDemo::setColor))
        + uitem(UBackground::green + "    " 
                + ucall(this, &UColor::green, &PiemenuDemo::setColor))
        + uitem(UBackground::blue + "    " 
                + ucall(this, &UColor::blue, &PiemenuDemo::setColor))
        + uitem(UBackground::yellow + "    " 
                + ucall(this, &UColor::yellow, &PiemenuDemo::setColor))
        + uitem(UBackground::orange + "    " 
                + ucall(this, &UColor::orange, &PiemenuDemo::setColor))
        + uitem(UBackground::wheat + "    " 
                + ucall(this, &UColor::wheat, &PiemenuDemo::setColor))
        + uitem(UBackground::teal + "    " 
                + ucall(this, &UColor::teal, &PiemenuDemo::setColor))
        + uitem(UBackground::black + "    " 
                + ucall(this, &UColor::black, &PiemenuDemo::setColor))
        );
  color.add("   " + colors_menu);
  
  /*
   const char* tab[] = {" 1 ", " 2 ", " 3 ", " 4 ", " 5 ", " 6 ", null};
  thicknesses.addItems(tab);
  thickness = ucombobox(thicknesses, uwidth(100));
  */
  toolbar.add("Shape:" + shape + "Color:" + color + "Thickness:" + thickness);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PiemenuDemo::initMenus() {
  UPiemenu& shape_menu = upiemenu();
  shape_menu.item(0).add("Line"   + ucall(this, "Line", &PiemenuDemo::setShape));
  shape_menu.item(2).add("Rect"   + ucall(this, "Rect", &PiemenuDemo::setShape));
  shape_menu.item(6).add("Ellipse"+ ucall(this, "Ellipse", &PiemenuDemo::setShape));
  
  UPiemenu& color_menu = upiemenu();
  color_menu.item(7).add(//UBackground::red 
                         "Red" + ucall(this, &UColor::red, &PiemenuDemo::setColor));
  color_menu.item(0).add(//UBackground::green 
                        "Green" + ucall(this, &UColor::green, &PiemenuDemo::setColor));
  color_menu.item(1).add("Blue"  + ucall(this, &UColor::blue, &PiemenuDemo::setColor));
  color_menu.item(2).add("Yellow"+ ucall(this, &UColor::yellow, &PiemenuDemo::setColor));
  color_menu.item(3).add("Orange"+ ucall(this, &UColor::orange, &PiemenuDemo::setColor));
  color_menu.item(4).add("Wheat" + ucall(this, &UColor::wheat, &PiemenuDemo::setColor));
  color_menu.item(5).add("Teal"  + ucall(this, &UColor::teal, &PiemenuDemo::setColor));
  color_menu.item(6).add("Black" + ucall(this, &UColor::black, &PiemenuDemo::setColor));
  
  UPiemenu& width_menu = upiemenu();
  width_menu.item(2).add(" 1 "+ ucall(this, 1, &PiemenuDemo::setThickness));
  width_menu.item(3).add(" 2 "+ ucall(this, 2, &PiemenuDemo::setThickness));
  width_menu.item(4).add(" 3 "+ ucall(this, 3, &PiemenuDemo::setThickness));
  width_menu.item(5).add(" 4 "+ ucall(this, 4, &PiemenuDemo::setThickness));
  width_menu.item(6).add(" 5 "+ ucall(this, 5, &PiemenuDemo::setThickness));
  
  UPiemenu& func_menu = upiemenu();
  func_menu.item(0).add("Rem All" + ucall(this, "remove all", &PiemenuDemo::doFunction));
  func_menu.item(6).add("Remove" + ucall(this, "remove", &PiemenuDemo::doFunction));
  
  // top menu - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  main_menu.item(0).add("Shape");
  main_menu.addSubMenu(0, &shape_menu);
  
  main_menu.item(2).add("Color");
  main_menu.addSubMenu(2, &color_menu);
  
  main_menu.item(4).add("Width");
  main_menu.addSubMenu(4, &width_menu);
  
  main_menu.item(6).add("Functions");
  main_menu.addSubMenu(6, &func_menu);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PiemenuDemo::setShape(const char* com) {
  cerr << "setShape: " << com <<endl;
}

void PiemenuDemo::setColor(UColor* col) {
  //cerr << "setColor: " << col <<endl;
  color.setAttr(ubackground(*col));
}

void PiemenuDemo::setThickness(int val) {
  //thickness.setText(val);
  //thickness.setAttr(ustr().setInt(val));
  UStr& s = ustr().setInt(val);
  cerr << "setThickness: " << val <<" " << s<<endl;
  thickness.setAttr(s);
}

void PiemenuDemo::doFunction(const char* com) {
  cerr << "doFunction: " << com <<endl;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0 
/*
 UColor   pie_bg;
 UColor   center_bg;
 UColor   slice_bg;
 UColor   item_bg;
 UColor   item_fg;
 UFont    item_font;
 UScale   main_scale, sub_scale;
 */
PieMenus::foo()
{
  // setMenuItems(main_menu, mainlabels);  
  //main_menu.addAttr(main_scale); 

  //pie_bg = ucolor(UColor::blue, 0.5);
  pie_bg = ucolor(UColor::blue, 124);  // value is an int !!!
  center_bg = UColor::orange;
  slice_bg = UColor::lightblue;
  item_bg = UColor::none;
  item_fg = UColor::white;
  main_scale = 1;
  sub_scale = 1;
  
  struct Stamp : public ULabel {
    Stamp(UColor& c) {add(UBorder::line + ubackground(c) + "    "); ignoreEvents();}
    Stamp(UFont& f) {add(UBorder::line + f + " fff "); ignoreEvents();}
  };
  
  UBox& bar = ubar
  (ubutton("Pie Bg"      + *new Stamp(pie_bg) + pie_bg_menu)
   + ubutton("Center Bg" + *new Stamp(center_bg) + center_bg_menu)   
   + ubutton("Slice Bg"  + *new Stamp(slice_bg) + slice_bg_menu)
   + ubutton("Item Bg"   + *new Stamp(item_bg) + item_bg_menu)
   + ubutton("Item Fg"   + *new Stamp(item_fg) + item_fg_menu)
   + ubutton("Item Font "+ *new Stamp(item_font) +item_font_menu)
   );
  UMenu& pie_bg_menu = umenu
  (uvspacing(0)
   + ubutton(UBackground::none + "no" + uassign(pie_bg, UColor::none))
   + ubutton(UBackground::white + "    " + uassign(pie_bg, UColor::white))
   + ubutton(UBackground::yellow+ "    " + uassign(pie_bg, UColor::yellow))
   + ubutton(UBackground::wheat + "    " + uassign(pie_bg, UColor::wheat))
   + ubutton(UBackground::orange + "    " + uassign(pie_bg, UColor::orange))
   + ubutton(UBackground::grey + "    " + uassign(pie_bg, UColor::grey))
   + ubutton(UBackground::darkgrey + "    " + uassign(pie_bg, UColor::darkgrey))
   + ubutton(UBackground::blue + "    " + uassign(pie_bg, UColor::blue))
   + ubutton(UBackground::lightblue + "    " + uassign(pie_bg, UColor::lightblue))
   );
  
  UMenu& center_bg_menu = umenu
  (uvspacing(0)
   + ubutton(UBackground::none + "no" + uassign(center_bg, UColor::none))
   + ubutton(UBackground::white + "    " + uassign(center_bg, UColor::white))
   + ubutton(UBackground::yellow+ "    " + uassign(center_bg, UColor::yellow))
   + ubutton(UBackground::wheat + "    " + uassign(center_bg, UColor::wheat))
   + ubutton(UBackground::orange + "    " + uassign(center_bg, UColor::orange))
   + ubutton(UBackground::grey + "    " + uassign(center_bg, UColor::grey))
   + ubutton(UBackground::darkgrey + "    " + uassign(center_bg, UColor::darkgrey))
   + ubutton(UBackground::blue + "    " + uassign(center_bg, UColor::blue))
   + ubutton(UBackground::lightblue + "    " + uassign(center_bg, UColor::lightblue))
   );
  
  UMenu& slice_bg_menu = umenu
  (uvspacing(0)
   + ubutton(UBackground::none + "no" + uassign(slice_bg, UColor::none))
   + ubutton(UBackground::white + "    " + uassign(slice_bg, UColor::white))
   + ubutton(UBackground::yellow+ "    " + uassign(slice_bg, UColor::yellow))
   + ubutton(UBackground::wheat + "    " + uassign(slice_bg, UColor::wheat))
   + ubutton(UBackground::orange + "    " + uassign(slice_bg, UColor::orange))
   + ubutton(UBackground::grey + "    " + uassign(slice_bg, UColor::grey))
   + ubutton(UBackground::darkgrey + "    " + uassign(slice_bg, UColor::darkgrey))
   + ubutton(UBackground::blue + "    " + uassign(slice_bg, UColor::blue))
   + ubutton(UBackground::lightblue + "    " + uassign(slice_bg, UColor::lightblue))
   );
  
  UMenu& item_bg_menu = umenu
  (uvspacing(0)
   + ubutton(UBackground::none + "no" + uassign(item_bg, UColor::none))
   + ubutton(UBackground::white + "    " + uassign(item_bg, UColor::white))
   + ubutton(UBackground::yellow+ "    " + uassign(item_bg, UColor::yellow))
   + ubutton(UBackground::wheat + "    " + uassign(item_bg, UColor::wheat))
   + ubutton(UBackground::orange + "    " + uassign(item_bg, UColor::orange))
   + ubutton(UBackground::grey + "    " + uassign(item_bg, UColor::grey))
   + ubutton(UBackground::darkgrey   + "    " + uassign(item_bg, UColor::darkgrey))
   + ubutton(UBackground::blue + "    " + uassign(item_bg, UColor::blue))
   + ubutton(UBackground::lightblue + "    " + uassign(item_bg, UColor::lightblue))
   );
  
  UMenu& item_fg_menu = umenu
  (uvspacing(0)
   + ubutton(UBackground::black + "    " + uassign(item_fg, UColor::black))
   + ubutton(UBackground::white + "    " + uassign(item_fg, UColor::white))
   + ubutton(UBackground::yellow+ "    " + uassign(item_fg, UColor::yellow))
   + ubutton(UBackground::orange+ "    " + uassign(item_fg, UColor::orange))
   + ubutton(UBackground::red   + "    " + uassign(item_fg, UColor::red))
   + ubutton(UBackground::blue  + "    " + uassign(item_fg, UColor::blue))
   + ubutton(UBackground::navy  + "    " + uassign(item_fg, UColor::navy))
   );
  
  UMenu& item_font_menu = umenu
  (uvspacing(0)
   + ubutton(UFont::plain + "plain" + uassign(item_font, UFont::plain))
   + ubutton(UFont::bold  + "bold"  + uassign(item_font, UFont::bold))
   + ubutton(UFont::italic+ "italic" + uassign(item_font, UFont::italic))
   );
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                         
UPiemenu* PieMenus::createSubMenu(UPiemenu& supermenu, int N, const char* labels[]) {
  UPiemenu* submenu = new UPiemenu;
  supermenu.addSubMenu(N, submenu);
  setMenuItems(*submenu, labels);
  submenu->addAttr(sub_scale);
  return submenu;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void foo(const char* msg) {
  cerr << msg << endl;
}

void PieMenus::setMenuItems(UPiemenu& menu, const char* labels[]) {
  //menu.setShowDelay(0);  // not in expert mode!
  
  menu.addAttr(item_font);
  menu.addAttr(UOn::idle/item_fg);
  /*
   menu.setPieColor(pie_bg);
   menu.setCenterColor(center_bg);
   menu.setSliceColor(slice_bg);
   */
  for (int k = 0; k < 8; ++k) 
    if (labels[k]) {
      menu.item(k)->add(UOn::idle / ubackground(item_bg) 
                        //+ UBorder::line
                        + labels[k]
                        + UOn::action / ucall(this, "action", labels[k], &PieMenus::printMessage)
                        /*
       + UOn::action / ucall("action", foo)
       + UOn::arm / ucall("arm", foo)
       + UOn::disarm / ucall("disarm", foo)
       + UOn::mpress / ucall("mpress", foo)
       + UOn::mrelease / ucall("mrelease", foo)
       + UOn::mdrag / ucall("mdrag", foo)
       + UOn::mmove / ucall("mmove", foo)
       */
 );
    }
}

#endif


