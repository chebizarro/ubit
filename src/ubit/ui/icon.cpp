/*
 *  icon.cpp: Icon widget
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2008 | Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
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

#include <ubit/ubit_features.h>
#include <iostream>
#include <sys/stat.h>
#include <ubit/ubit.hpp>
#include <ubit/ufile.hpp>
#include <ubit/uicon.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT


UStyle* UIconbox::createStyle() {
  //UStyle* style = UVbox::createStyle();
  UStyle* style = Box::createStyle();
  return style;
}

UIconbox::UIconbox(Args args) : UDocbox(), show_parent_dir(true) {
  filetime = 0;
  icon_hspacing = new UHspacing(5);
  icon_vspacing = new UVspacing(5);
  ppathname = new String;
  ptitle = new String;
  titlebar().addAttr(upadding(3,2) + uhspacing(3) + UFont::italic);
  titlebar().add(*ptitle);

/* beaucoup trop long ! bloque l'interaction              A REVOIR!!!
  UButton& alt_btn = uflatbutton(" # ");
  alt_btn.addAttr(UFont::small);
  alt_btn.addAttr(UOn::select / ucall(this,true, &UIconbox::showSmallIcons));
  alt_btn.addAttr(UOn::deselect / ucall(this,false, &UIconbox::showSmallIcons));
  //titlebar().insert(0, alt_btn);
  titlebar().add(alt_btn, titlebar().cbegin());
 */
  
  picons = new UListbox;
  // NB: icons ne doit contenir que des UIcon sinon getIcon ne marchera pas
  picons->addAttr(UFlowView::style + upadding(0,0) 
                  + *icon_hspacing + *icon_vspacing + UFont::small);                     
                     //+ show_dirs + show_docs + show_icon_names + show_icon_contents);
  picons->add(args);
  content().add(*picons);
}

UIconbox::~UIconbox() {}

UChoice& UIconbox::choice() {return picons->choice();}
const UChoice& UIconbox::choice() const {return picons->choice();}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UIconbox::addIcon(UIcon& icon) {
  picons->add(icon);
}

void UIconbox::removeIcon(UIcon& icon, bool auto_delete) {
  picons->remove(icon, auto_delete);
}

void UIconbox::removeAllIcons(bool auto_delete) {
  picons->removeAll(auto_delete);
}

void UIconbox::okBehavior(UInputEvent& _e) {  // A REVOIR
  Event e2(UOn::action, this, _e.getSource());  //UElemEvent
  fire(e2);
}

UIcon* UIconbox::getSelectedIcon() {
  Element* i = choice().getSelectedItem();
  return i ? dynamic_cast<UIcon*>(i) : null;
}

void UIconbox::selectIcon(UIcon& i) {
  choice().setSelectedItem(i);
}

void UIconbox::selectIcon(const String& name) {
  for (ChildIter i = picons->cbegin(); i != picons->cend(); ++i) {
    UIcon* icon = dynamic_cast<UIcon*>(*i);
    if (icon && icon->getName() == name) {
      choice().setSelectedItem(*icon);
      return;
    }
  }
}

void UIconbox::selectPreviousIcon() {
  UItem* item = getPreviousIcon();
  if (item) choice().setSelectedItem(*item);
}

void UIconbox::selectNextIcon() {
  UItem* item = getNextIcon();
  if (item) choice().setSelectedItem(*item);
}

UIcon* UIconbox::getPreviousIcon() {
  int index = choice().getSelectedIndex();
  if (index > 0) return getIcon(index-1);
  else return null;
}

UIcon* UIconbox::getNextIcon() {
  int index = choice().getSelectedIndex();
  UIcon* item = getIcon(index+1);
  // si pas de suivant prendre le dernier
  if (!item) item = getIcon(-1);
  return item;
}

UIcon* UIconbox::getIcon(int index) const {
  Box* item = choice().getSelectableItem(index);
  return item ? dynamic_cast<UIcon*>(item) : null;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int UIconbox::readDir(const String& _pathname, bool remote_dir) {
  String prefix, filter;
  bool want_dotfiles = false;
  pathname() = _pathname;
  title() = _pathname;

  UFileDir dir;
  if (remote_dir) dir.readRemote(_pathname, prefix, filter, want_dotfiles);
  else dir.read(_pathname, prefix, filter, want_dotfiles);

  filetime = dir.dir_info.getModTime();
  picons->setAutoUpdate(false);

  if (show_parent_dir) {
    UIcon& icon = *new UIcon("..", UPix::bigUp);
    icon.setDir(true);
    icon.addAttr(usize(75,75) + ucall(this, &UIconbox::okBehavior));
    picons->add(icon);
  }
  
  const UFileInfos& entries = dir.getFileInfos(); 

  for (UFileInfos::const_iterator pe = entries.begin(); pe != entries.end(); ++pe) {
    const UFileInfo& e = *(*pe);

    UIcon& icon = *new UIcon(*e.getFileName(), e.getIconImage());
    if (e.isDir()) icon.setDir(true);
    icon.addAttr(usize(75,75) + ucall(this, &UIconbox::okBehavior));
    picons->add(icon);
  }

  picons->setAutoUpdate(true);
  return UFilestat::Opened;
}


static const int CONTENT_WIDTH = 55, CONTENT_HEIGHT = 55;

UStyle* UIcon::createStyle() {
  return UItem::createStyle();
}
  
UIcon::~UIcon() {}

UIcon::UIcon(const String& name, Args content) {
  is_dir = false;  
  pname = new String(name);

  text_box = &uhbox(*pname);
  text_box->ignoreEvents();
  text_box->addAttr(uhcenter());
  //name_edit->setCaretStr(null, 0);
  //name_edit->setEditable(false);
  
  ima_box = &uhbox(content);
  ima_box->addAttr(uhcenter()+uvcenter());
  ima_box->ignoreEvents();
  
  addAttr(UOrient::vertical + uvspacing(2) + upadding(5, 5));
  add(uhflex() 
      + uvflex() + *ima_box
      + ubottom() + *text_box
      + new UTip(*pname)  // tant pis on copie nme: a revoir..
      );
}

/*
void UIcon::set(const String& _name, Data&  _content) {
  *pname = _name;
  //ima_box->setAutoUpdate(false);
  ima_box->removeAll();
  ima_box->add(_content);
  //ima_box->setAutoUpdate(true);
  //ima_box->update();  // pour layout
  //update(Update::paint);  // pour affichage
}
*/

int UIcon::loadImage(const String& ima_path) { 
  String fext = ima_path.suffix();
  int stat = false;
  
  if (fext.equals("gif",true/*ignore case*/)    // !!! A REVOIR (Image devrait faire ca) !!!@@@
      || fext.equals("jpg",true)
      || fext.equals("jpeg",true)
      || fext.equals("xpm",true)
      ) {
    uptr<Image> ima = new Image;
    stat = ima->read(ima_path, CONTENT_WIDTH, CONTENT_HEIGHT);

    //delete full_ima; automatic deletion
    if (stat <= 0) return stat;
    else {
      //set(*pname, *ima);
      ima_box->removeAll();
      ima_box->add(*ima);
      return UFilestat::Opened;
    }
  }
  return UFilestat::UnknownType;
}

}
