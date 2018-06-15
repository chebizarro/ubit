/* ***********************************************************************
 *
 *  viewer.hpp
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
#include <cmath>
#include "viewer.hpp"
#include <ubit/ufile.hpp>
using namespace std;
using namespace ubit;


ViewerItem::ViewerItem(const UStr& _name, int _no, UArgs _content) : 
UPalette(_content),
pname(new UStr(_name)),
no(_no)
{
  pos().set(UAUTO,UAUTO);

  titleBar().addAttr(uhcenter());
  title().add(*pname);
    
  setAttr(UBorder::shadowIn);
  setAttr(upadding(2,2));
}

//============================================================================

Fisheye::Fisheye(UArgs args) : 
current_index(0),
item_width(150), 
item_height(150), 
fisheye_mag(1.3)
{  
  openbox.add(ucall(this, &Fisheye::readOpenDir));
  UDialog& open_dialog = udialog(utitle("Open Images") + openbox);
  open_dialog.setSoftWin(true);
  //UPalette& open_dialog = upalette(utitle("Open Images") + openbox);
  
  itembox.addAttr(UOrient::horizontal+ UBackground::black + uvcenter() + uhcenter());
  // removed once the photos are loaded
  itembox.add(ubutton(UFont::bold + UFont::large + UColor::orange
                      + "Click to select an image directory"
                      + open_dialog));
  
  addAttr(uhflex() + uvflex());
  add(args 
      + itembox 
      + ubutton(upos(5,5) + UColor::red + "Open Directory" + open_dialog)
      );
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Fisheye::clear() {
  for (ViewerItems::iterator p = items.begin(); p != items.end(); ++p) delete *p;
  items.clear();
  itembox.removeAll();
}

void Fisheye::readDir(const UStr& path) {
  if (path.empty()) return;
  
  clear();
  UFileDir dir(path);
  int no = 0;  
  
  for (UFileInfos::const_iterator f = dir.getFileInfos().begin();
       f != dir.getFileInfos().end();
       ++f) {
    const UStr* filename = (*f)->getFileName();
    if (!filename || filename->empty()) continue;
    UStr suffix = filename->suffix();
    
    if (suffix.equals("jpg",true)
        || suffix.equals("jpeg",true)
        || suffix.equals("gif",true)
        || suffix.equals("xpm",true)      
        ) {
      ViewerItem* i = new ViewerItem(*filename, no);
      //i->titlebar().show(false);
      items.push_back(i);
      no++;
    }
  }
  
  //cerr << "count "<< items.size() << endl;
  display(5);
}

void Fisheye::readOpenDir() {
  readDir(openbox.getDir());
}

void Fisheye::display(int index) {
  // false: remove items but dont detroy them as they are pointed by the items list
  itembox.removeAll(false);
  
  current_index = index;
  int from = index -5 ;
  int to = index +5;
  int no = 0;
  //cerr << endl<<endl;
  
  for (ViewerItems::iterator p = items.begin(); p!=items.end(); ++p, no++) {
    if (no >= from && no <= to) {
      float s = 1. / (1 + abs(no - index) * fisheye_mag);
      //cerr << "item: " << no << " sc "  <<s<< endl;
      //(*p)->scale() = s;
      (*p)->contentScale() = s;
      if (!(*p)->content().getChildCount() == 0) {
        loadItem(*p);;
        //cerr << "read" << no << endl; 
      }
      itembox.add(*p);
    }
  }
}

void Fisheye::displayItem(ViewerItem* i) {
  //cerr << "displayItem " << i->no << endl;
  display(i->no);
}

int Fisheye::loadItem(ViewerItem* i) {
  UStr path = getDir() & i->getName();
  UIma* ima = new UIma();
  int stat = ima->read(path, item_width, item_height);
  //i->pcontent = ima;
  i->content().add(ima);
  // i->add(utip(fname));
  //cerr << "ima: "<< path << " " << stat << endl;
  
  i->content().setArmable(true);
  i->content().addAttr(ucall(this, i, &Fisheye::displayItem));

  //i->addAttr(usize(item_width, item_height));
  /*     + scale
          + uhcenter() + uvcenter() + UBackground::none
          + ucall(this, &Item::clickCB)
          );
   */
  return stat;
}

