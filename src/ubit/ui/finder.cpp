/*
 *  finder.cpp: Finder element
 *  Ubit GUI Toolkit - Version 8
 *  (C) 2018 Chris Daley
 *  (C) 2009 | Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */

#include <ubit/ubit_features.h>
#include <cmath>
#include <unistd.h>
#include <ubit/udefs.hpp>
#include <ubit/ufile.hpp>
#include <ubit/umservice.hpp>
#include <ubit/umsproto.hpp>
#include <ubit/ui/control-menu.h>
#include <ubit/uon.hpp>
#include <ubit/uboxgeom.hpp>
#include <ubit/ui/interactors.h>
#include <ubit/udialogs.hpp>
#include <ubit/core/doc.h>
#include <ubit/upix.hpp>
#include <ubit/usymbol.hpp>
#include <ubit/uscrollpane.hpp>
#include <ubit/uicon.hpp>
#include <ubit/core/application.h>
#include <ubit/utimer.hpp>
#include <ubit/ufilebox.hpp>
#include <ubit/ufinder.hpp>
#include <ubit/ui/ufinderImpl.hpp>
using namespace std;
namespace ubit {



static const char *doc_xpm[] = {       // moins large que UPix::doc
  "13 16 4 1",
  "       c None s None",
  ".      c black",
  "X      c white",
  "o      c #808080",
  "             ",
  " .......     ",
  " .XXXXX..    ",
  " .XoooX.X.   ",
  " .XXXXX....  ",
  " .XooooXoo.o ",
  " .XXXXXXXX.o ",
  " .XooooooX.o ",
  " .XXXXXXXX.o ",
  " .XooooooX.o ",
  " .XXXXXXXX.o ",
  " .XooooooX.o ",
  " .XXXXXXXX.o ",
  " ..........o ",
  "  oooooooooo ",
  "             "
};
UPix& Finder::doc_pix = *new UPix(doc_xpm, UObject::UCONST);


Finder::Options::Options() :
  clone_frame_size(600,450),
  default_background(Color::white),
  default_color(Color::black),
  //controls_width(100),
  //controls_folder_height(125),
  show_icon_images(true) {
}

/* ==================================================== (c)[Elc] ======= */

void Finder::setTracking(bool state) {
  setTracking(state, state);
}

void Finder::setTracking(bool doc_track, bool icon_track) {
  is_tracking = icon_track;
  pdocbox->scrollpane().setTracking(doc_track);
  if (piconbox) piconbox->scrollpane().setTracking(icon_track);
}

//void Finder::showDocGlass(bool state) {docglass->show(state);}

void Finder::showAlert(const String& msg) {alertbox->showMsg(msg);}

void Finder::hideAlert() {alertbox->show(false);}

void Finder::setListener(FinderListener* ll) {
  listener = ll;
}

FinderListener* Finder::getListener() {
  return listener;
}

/* ==================================================== (c)[Elc] ======= */

Finder::~Finder() {
delete local_ums;
}

Finder::Finder(const String& _pathname) :
mode(NoMode),
is_tracking(false), 
open_in_fullwin(false),
//iconbox_panel(new UVbox()),
listener(new FinderListener()),
alertbox(new AlertBox()),
ctlmenu(&createContextMenu()),
local_ums(null),
remote_ums(null),
pdocument(null),
pdocbox(new DocumentBox()),
piconbox(null),
//docglass(new Box()),
last_direntry(null),
last_preview_request(null),
last_preview(null),
preview_timer(new Timer(false)) 
{
  preview_timer->onAction(ucall(this, &Finder::showIconPreviews));
  Application::onMessage("next",    ucall(this, &Finder::nextEntry));
  Application::onMessage("previous",ucall(this, &Finder::previousEntry));
  Application::onMessage("parent",  ucall(this, &Finder::openParent));
  Application::onMessage("file",    ucall(this, &Finder::openMsg));
  Application::onMessage("open",    ucall(this, &Finder::openMsg));
  Application::onMessage("clone",   ucall(this, &Finder::createCloneRequest));
  //Application::onMessage("put",     ucall(this, &Finder::sendFileMessage)); 
  /*
  docglass->addAttr(Background::none);
  docglass->addAttr(upos(0,0));
  docglass->addAttr(usize(50000,100000)); // a revoir
  docpane->content().add(*docglass);
  docglass->show(false);
   */
  openContextMenuIn(*pdocbox);
  
  alertbox->addAttr(upos(0,0) + usize(100|UPERCENT,100|UPERCENT));
  alertbox->show(false);

  ask_dialog_msg = new String(); 
  ask_dialog = new Dialog
  (uhcenter() + uvflex()
   + upadding(15,20) + uvspacing(20)
   + Font::bold + Font::large
   + uhbox(Color::orange + "Remote Request")
   + uhbox(Color::navy + ask_dialog_msg)
   + uhcenter()
   + uhbox(ubutton("  OK " + ucall(1, &Element::closeWin))
           + ubutton(" Cancel " + ucall(0, &Element::closeWin)))
   );

  // doit toujours rester une hardwin, meme en mode -group et
  // etre afficher sur le display initial (= celui du maitre)
  ask_dialog->setSoftwin(false);

  //===========================================

  pdocbox->titlebar().show(false);

  initOptbox();
  optbox.show(false);

  toolbar.addAttr(Background::metal + Font::xx_large + Font::bold);
  toolbar
  .add(*optbox_btn
       + ubutton(uhcenter() + UPix::bigUp + ucall(this, &Finder::openParent))
       + ubutton(uhcenter() + UPix::bigLeft + ucall(this, &Finder::previousEntry))
       + ubutton(UPix::bigRight + ucall(this, &Finder::nextEntry))
       + " " 
       + ubutton(Font::bold + "  Z  " + ucall(this, &Finder::zoomIn))
       + ubutton(Font::bold + "  z  " + ucall(this, &Finder::zoomOut))
       + ubutton(Font::bold + "  =  " + ucall(this, 1.0f, &Finder::zoom))
       );
  
  mainbox.addAttr(Orientation::vertical + uvflex() + uvflex());
  
  addAttr(Background::white); 
  add(Orientation::vertical 
      + utop() + toolbar
      + uvflex()
      + uhbox(uvflex() + uleft() + optbox + uhflex() + mainbox)
      + ctlmenu
      + ask_dialog
      + alertbox
     );

  // rajouter HOME dans l'historique
  const char* pwd = ::getenv("HOME");  // pas de free() sur un getenv!
  if (pwd) open(pwd);

  // ouvrir le dir donne en arg (celui ou on est par defaut)
  open(_pathname);
  
  addHost("localhost");
}


void Finder::showSideBar(bool state) {
  optbox_btn->setSelected(state);
}

void Finder::initOptbox() {
  optbox_btn = 
  ubutton(Symbol::square 
          + UOn::select   / ushow(optbox, true)
          + UOn::deselect / ushow(optbox, false)
          );
  optbox.addAttr(Orientation::vertical + Background::velin
                 + usize(120,UAUTO) + upadding(6,9).setRight(2) + utop());

  // - - - HISTORY - - - -
  
  folderlist.addAttr(Orientation::vertical + utop() + uchoice());
  Box& folders = uscrollpane(true, false, folderlist);
  folders.addAttr(usize(UAUTO,100) + upadding().setLeft(15)); 
  folders.show(false);

  // opens/closes history
  optbox.add(uhbox(UOn::select / ushow(folders, true)
                   + UOn::deselect / ushow(folders, false)
                   + UOn::select / Symbol::down
                   + UOn::deselect / Symbol::right
                   + Font::bold + "History")
             + folders
             + ubox(usize(UAUTO,6))   // separator
             );
  
  // - - - HOSTS - - - -
 
  String* host_str = new String;
  Menu& new_host_menu = 
  umenu(Orientation::horizontal
        + "Host Name: "
        + utextfield(usize(150) + host_str
                     + ucall(this, (const String*)host_str, &Finder::addHostCB)
                     + ucall(0, &Element::closeWin))
        + ubutton(" Add "
                  + ucall(this, (const String*)host_str, &Finder::addHostCB)
                  + ucall(0, &Element::closeWin))
        );
  
  hostlist.addAttr(Orientation::vertical + utop());
  hostlist.add(uitem(Font::italic + " Add Host" + new_host_menu));

  Box& hosts = uscrollpane(true, false, hostlist);
  hosts.addAttr(usize(UAUTO,100) + upadding().setLeft(15));
  hosts.show(false);

  // opens/closes hosts
  optbox.add(uhbox(UOn::select / ushow(hosts, true)
                   + UOn::deselect / ushow(hosts, false)
                   + UOn::select / Symbol::down
                   + UOn::deselect / Symbol::right
                   + Font::bold + "Remote Hosts")
             + hosts
             + ubox(usize(UAUTO,6))   // separator
             );
  
  // - - - FILES - - - -
  
  filelist.addAttr(uhcenter());
  filelist.addAttr(uvflex());
  filelist_btn = uhbox(Symbol::down + Font::bold + "Folder Files");
  optbox.add(*filelist_btn + uvflex() + filelist);
}


DocumentBox* Finder::getCurrentBox() {
  if (mode == DirMode) return getIconbox();
  else if (mode == DocMode) return getDocbox();
  else return null;
};

void Finder::zoom(float z) {
  if (getCurrentBox()) getCurrentBox()->zoom(z);
}

void Finder::zoomIn() {
  if (getCurrentBox()) getCurrentBox()->zoomIn();
}

void Finder::zoomOut() {
  if (getCurrentBox()) getCurrentBox()->zoomOut();
}

void Finder::setZoomQuantum(float z) {
  if (getCurrentBox()) getCurrentBox()->setZoomQuantum(z);
}

void Finder::showSelectedIcon() {
  IconBox* ibox = getIconbox();
  if (ibox) {
    Icon* icon = ibox->getSelectedIcon();
    if (icon) ibox->scrollpane().makeVisible(*icon);
  }
}

void Finder::showDocBox() {
  IconBox* ibox = getIconbox();
  if (ibox) {
    ibox->title() = ibox->pathname().basename();
    filelist.add(*ibox);
  }
  filelist.show(true);
  filelist_btn->show(true);
  showSelectedIcon();
}


void Finder::showDirBox() {
  filelist.removeAll();
  IconBox* ibox = getIconbox();
  if (ibox) ibox->title() = ibox->pathname();
  filelist.show(false);
  filelist_btn->show(false);
}


void Finder::open(const String& path) {
  if (path.empty()) return;

  int path_type = FileInfo::parsePrefix(path);

  if (path_type == FileInfo::LOCAL)
    openImpl(path, 0/*mode unknow at this stage*/, FileInfo::LOCAL);
  else
    new UFinderCom(this, path, path_type);
}


void Finder::openImpl(const String& path_name, int path_mode, int path_type) {
  if (path_name.empty()) return;
  String path = path_name;

  // local file or dir  
  if (path[0] == '~' && (path[1] == '/' || path[1] == 0)) {
    //NB: ne PAS faire de free() sur un getenv!
    static char* home = ::getenv("HOME");
    if (home) path.replace(0, 1, home);   // virer ~ mais pas /
  }

  // cas xxxx/. virer le . (mais pas /..)
  if (path.length() > 1
      && path[path.length()-1] == '.'
      && path[path.length()-2] == '/'
      ) {
    path.remove(-1, 1);  // remove last char
  }

  if (path_type == FileInfo::LOCAL) {
    FileMode fmode(path);
    if (!fmode.isValid()) {
      showAlert("Can't load: " & path);
      return;
    }
    if (fmode.isDir()) openDir(path, path_type);
    else if (fmode.isFile()) openFile(path, path_type);
    else showAlert("Invalid file: " & path);
  }

  else {
    if (path_mode == FileInfo::DIR) openDir(path, path_type);
    else openFile(path, path_type);
  }
}


void Finder::openParent() {
  String name;
  if (piconbox) name = piconbox->pathname();
  else name = ".";

  if (mode == DocMode) {
    // c'est necessaire pour les URL pour indiquer un directory
    if (name[-1] != '/') name.append('/');
    open(name);
  }
  else {
    //c'est pas bon: si basename est protege en lecture on peut plus remonter!
    //name &= "/.."; name = name.dirname(); openDir(name);
    open(name.dirname(true));
  }
}

void Finder::openFBox(FileChooser& openbox) {
  String path = openbox.getPath();
  open(path);
}

void Finder::openMsg(MessageEvent& e) {
  const String* file = e.getMessage();
  if (!file || file->empty()) openEntry();
  else {
    // d'abord ouvrir le directory pour que next/prev fonctionne
    open(file->dirname());
    if (piconbox) piconbox->selectIcon(file->basename());
    open(*file);
  }
}

/* ==================================================== (c)[Elc] ======= */
namespace impl {

  struct CMenu : public ControlMenu {
    Element * mopener;
    CMenu() {}
  };
  

  struct ZoomAction : public UCall {
    CMenu& cmenu;
    Finder& browser;
    double base;
    int incr, last_incr;
    unique_ptr<Scale> scale;
    
    static const int QUANTUM;
    static const float ZOOM_QUANTUM;
    
    ZoomAction(CMenu& _c, Finder& _b) :  cmenu(_c), browser(_b) {}
    
    virtual void operator()(Event& event) {
      MouseEvent& e = (MouseEvent&)event;
      
      if (e.isFirstDrag()) {
        Element* source = cmenu.mopener;
        scale = null;
        
        if (source == browser.getIconbox())
          scale = &browser.getIconbox()->scale();
        else if (source == browser.getDocbox())
          scale = &browser.getDocbox()->scale();
        
        if (scale) {
          base = *scale;
          incr = 0;
          last_incr = 0;
        }
      }
      
      else {  // !isFirstDrag
        if (!scale) return;
        int val = int(cmenu.getMouseMovement().x / QUANTUM);
        if (val == incr) return;
        
        if (val > 10) val = 10;  // eviter les valeurs extermes
        else if (val < -10) val = -10;
        incr = val;
        
        if (scale && incr != last_incr) {
          last_incr = incr;
          *scale = base * pow(ZOOM_QUANTUM, incr);
        }
      }
    }
  };
  
  const int ZoomAction::QUANTUM = 15;
  // eventuellement (mais pas forcement) le meme ZOOM_QUANTUM que zZ
  const float ZoomAction::ZOOM_QUANTUM = 1.10;
  
    
  struct ScrollAction : public ScrollAction {
    CMenu& cmenu;
    Finder& f;
    
    ScrollAction(CMenu& _c, Finder& _f, float _gain = 0.3)
      : ScrollAction(_gain), cmenu(_c), f(_f) {}
    
    virtual void mdrag(MouseEvent& e, ControlMenu&) {
      if (e.isFirstDrag()) {
        Element* source = cmenu.mopener;
        if (source == f.getIconbox()) setPane(f.getIconbox()->scrollpane());
        else if (source == f.getDocbox()) setPane(f.getDocbox()->scrollpane());
      }
      ScrollAction::mdrag(e, cmenu);
    }  
  };
  

  void mpressCB(MouseEvent& e, CMenu* m) {
    // open menu if RightButton ou CTRL-LeftButton
    if (e.getButton() == e.RightButton
        || (e.getButton() == e.LeftButton && e.isControlDown())) {
      m->mopener = e.getSource();
      m->open(e);
    }
    // propagate event to children otherwise
    else e.propagate();
  }
  
}  // endof nested namespace

  
void Finder::openContextMenuIn(Box& box) {  
  // mpressCB() opens menu if RightButton or CTRL-LeftButton and propagates events
  // to children otherwise
  box.catchEvents(UOn::mpress / ucall((impl::CMenu*)ctlmenu(), impl::mpressCB));
}

ControlMenu& Finder::createContextMenu() {
  impl::CMenu* cmenu = new impl::CMenu();
  
  impl::ZoomAction* za = new impl::ZoomAction(*cmenu, *this);
  cmenu->item(4).add(Font::xx_large + Font::bold + Color::red + " Z- " 
                      + UOn::mdrag/za);
  cmenu->item(0).add(Font::xx_large + Font::bold + Color::red + " Z+ "
                      + UOn::mdrag/za);
  
  impl::ScrollAction* sa = new impl::ScrollAction(*cmenu, *this);
  cmenu->item(2).add(UPix::bigUp + UOn::mdrag/sa);
  cmenu->item(6).add(UPix::bigDown + UOn::mdrag/sa);
  
  return *cmenu;
}

}

