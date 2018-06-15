/* ***********************************************************************
 *
 *  viewer.hpp (part of udemo)
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
 * ***********************************************************************/

#ifndef _viewer_hpp_
#define _viewer_hpp_  1
#include <ubit/ubit.hpp>
#include <list>
using namespace ubit;


class ViewerItem : public UPalette {
public:
  ViewerItem(const UStr& name, int no, UArgs args = UArgs::none);

  const UStr& getName() const {return *pname;}
  ViewerItem& setName(const UStr& name) {*pname = name; return *this;}

private:
  uptr<UStr> pname;
public:
  int no;
};


typedef std::list<ViewerItem*> ViewerItems;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class Fisheye : public UBox {
public:
  Fisheye(UArgs args = UArgs::none);
  
  const UStr& getDir() const {return openbox.getDir();}
  void readOpenDir();
  void readDir(const UStr& dir_path);
  void clear();
  void display(int no);
  void displayItem(ViewerItem*);
  int  loadItem(ViewerItem*);

private:
  ViewerItems items;
  UBox itembox;
  UFilebox openbox;
  int current_index;
  float item_width, item_height, fisheye_mag;
};

#endif

