/*
 *  finderImpl.cpp: ufinder implementation (see ufinder.hpp)
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
#include <unistd.h>
#include <cstdio>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ubit/udefs.hpp>
#include <ubit/ufile.hpp>
#include <ubit/uon.hpp>
#include <ubit/upix.hpp>
#include <ubit/usymbol.hpp>
#include <ubit/uborder.hpp>
#include <ubit/ubox.hpp>
#include <ubit/uboxgeom.hpp>
#include <ubit/utimer.hpp>
#include <ubit/ulistbox.hpp>
#include <ubit/uscrollpane.hpp>
#include <ubit/uappli.hpp>
#include <ubit/umservice.hpp>
#include <ubit/umsproto.hpp>
#include <ubit/umessage.hpp>
#include <ubit/udialogs.hpp>
#include <ubit/ufinder.hpp>
#include <ubit/ui/ufinderImpl.hpp>
#include <ubit/ucall.hpp>
using namespace std;
namespace ubit {

 
static void _openPMenuCB(MouseEvent& e, PopupMenu* m) {
  if (e.getButton() == e.RightButton) m->open(e);
}
  

UFinderDir::UFinderDir(Finder* fd, const String& _fpath, int _path_type) {
  fpath = _fpath;
  fname = _fpath.basename();
  // CP du /  : basename est null mais il faut mettre qq chose
  if (fname.empty() && fpath[0]=='/') fname = "/";
  iconbox = null;
  keep_open = false;
  path_type = _path_type;
  
  popmenu.add
    (ualpha(0.5) + Background::black
     + Color::white + Font::bold + Border::none
     + ubutton("     ReOpen " + Color::yellow + fpath // read again if already opened
               + ucall(fd, this, &Finder::openDir))
     + ubutton("     Close Folder"
               + ucall(fd, this, true, &Finder::removeIconbox))
     //+ uhsepar()
     //+ ubutton(UPix::cross + " Remove Folder" 
     //      + ucall(fd, this, &Finder::removeFromHistory)) // plante
     );
  popmenu.setSoftwin(true);

  add(//font + 
      fname
      + popmenu  
      // do not read again if already opened
      + UOn::action / ucall(fd, this, &Finder::openDir)
      // show info in the status bar
      + UOn::enter  / ucall(fd, this, &Finder::showDirInfo)
      + UOn::leave  / ucall(fd, (UFinderDir*)0, &Finder::showDirInfo)
      );
  
  // right mouse button opens the menu
  this->add(UOn::mpress / ucall(&popmenu, _openPMenuCB));
}
 
UFinderDir::~UFinderDir() {}

const String& UFinderDir::getDir() const {return fpath;}

class IconBox* UFinderDir::getIconbox() const {return iconbox;}

void UFinderDir::setDir(const String& _fpath) {fpath = _fpath;}

void UFinderDir::setIconbox(class IconBox* _ibox) {
  if (iconbox != _ibox) {
    emph(false);
    iconbox = _ibox;
  }
}

void UFinderDir::emph(bool state) {
  //if (state) font.set(Font::bold);
  //else font.set(Font::plain);
}

/* ==================================================== [Elc] ======= */
// HISTORY

UFinderDir* Finder::findDir(const String& name) {
  UFinderDir* found = null;

  for (ChildIter c = folderlist.cbegin(); c != folderlist.cend(); c++) {
    UFinderDir* de = dynamic_cast<UFinderDir*>(*c);
    if (de && de->fpath.equals(name)) found = de;
  }
  
  return found;
}

void Finder::removeIconbox(UFinderDir* de, bool upd) {
  if (!de) return;
  if (de->iconbox) {
    if (de->iconbox == piconbox) piconbox = null;
    //iconbox_panel->setAutoUpdate(upd);
    mainbox.remove(*de->iconbox);
    //iconbox_panel->setAutoUpdate(true);
    de->iconbox = null;    // unique_ptr : automatic deletion
    de->emph(false);
  }
}

void Finder::addToHistory(UFinderDir* de) {
  if (de) {          // if not already in list
    folderlist.remove(*de);
    folderlist.add(*de, folderlist.cbegin());
  }
  last_direntry = de;
}

void Finder::removeFromHistory(UFinderDir* de) {
  if (last_direntry == de) last_direntry = null;
  removeIconbox(de, true);
  if (de) folderlist.remove(*de);
}

void Finder::showDirInfo(UFinderDir* de) {
  //....
}

//void Finder::iconifyIconbox(UFinderDir* de) {
//  if (de->iconbox) de->iconbox->iconify(!de->iconbox->isIconified());
//}

/* ==================================================== [Elc] ======= */

void Finder::showFile(const String& pathname, Document* doc) {
  pdocbox->scrollpane().setScroll(0,0);
  if (listener) listener->docShown(pathname, doc);

  if (mode != DocMode) {
    mode = DocMode;
    mainbox.removeAll();
    mainbox.add(*pdocbox);
    showDocBox();
  }
  else {
    showSelectedIcon();
    // necessaire dans cette version quand on reaffiche une page deja affichee
    pdocbox->update();
  }

  //Application::setTitle(Application::getName() & " - " & pathname.basename());
}

void Finder::showDir(const String& pathname) {
  if (listener) listener->dirShown(pathname);
  
  if (mode != DirMode) {
    mode = DirMode;
    mainbox.removeAll();
    if (piconbox) {
      piconbox->scrollpane().setScroll(0,0);
      mainbox.add(*piconbox);
    }
    showDirBox();
  }
}

// =============================================================================
  
void Finder::linkActionCB(InputEvent& e, Document* doc) {
  LinkButton * link = null;
  String url;
  
  if (!doc || !e.getSource() 
      || !(link = dynamic_cast<LinkButton*>(e.getSource()))
      //|| !(url = link->getHRef()) || url->empty()
      || (url = link->getHRef()).empty()
      )
    return;
  
  String path;
  doc->makePath(path, url);
  
  if (!path.empty()) {
    if (piconbox) piconbox->selectIcon(url);
    open(path);
  }
}

// =============================================================================

int Finder::openFile(const String& pathname, int path_type) {
  hideAlert();
  if (listener) listener->fileRequest(pathname);

  DocumentFactory& fact = Document::getDefaultFactory();    
  Document* doc = fact.read(pathname);
  if (doc) {
    //pdocbox->content().setAutoUpdate(false);
    if (listener) listener->docLoaded(pathname, doc);
    if (pdocument) pdocbox->content().remove(*pdocument, false);
    
    doc->addLinkCallback(UOn::action / ucall(this, doc, &Finder::linkActionCB));
    pdocument = doc;
    pdocbox->content().add(*pdocument, pdocbox->content().cbegin());

    //pdocbox->content().setAutoUpdate(true);
    showFile(pathname, doc);
  }

  return fact.getStatus();
}

/* ==================================================== [Elc] ======= */

int Finder::openDir(const String& pathname, int path_type) {
  int stat = false;
  String fpath = pathname;
  UFileDir::expandDirPath(fpath);  // pour transformer . en sa valeur

  if (fpath.empty()) return false;
  hideAlert();

  // remove the final slash (except for /)
  if (fpath != "/" && fpath.at(-1) == '/') fpath.remove(-1, 1);

  // check if dentry already in the history
  UFinderDir* dentry = findDir(fpath);

  if (!dentry) dentry = new UFinderDir(this, fpath, path_type);
  stat = openDir(dentry);
  
  if (dentry->iconbox) addToHistory(dentry);
  else delete dentry;
  return stat;
}


int Finder::openDir(UFinderDir* de) {
  if (!de) return false;
  int stat = false;
  // conserver scale et disp_mode courants
  double scale = 1.;
  //int disp_mode = 0;

  hideAlert();
  if (listener) listener->fileRequest(de->fpath);

  if (pdocument) {
    pdocbox->content().setAutoUpdate(false);
    pdocbox->content().remove(*pdocument, false);
    pdocbox->content().setAutoUpdate(true);
  }
  pdocument = null;

  if (last_direntry && last_direntry->iconbox) {
    scale = last_direntry->iconbox->scale();
    //disp_mode = last_direntry->iconbox->getDisplayMode();
  }

  // on ne recharge le directory que s'il n'a jamais ete charge ou s'il a ete
  // modifie
  if (de->iconbox) {
    struct stat statbuf;
    ::lstat(de->fpath.c_str(), &statbuf);  // faudrait tester return val = 0
    if ((unsigned long)statbuf.st_mtime <= de->iconbox->filetime) {
      stat = true;
      if (listener) listener->dirLoaded(de->fpath);
      goto end;
    }
  }
  
  // fermer le precedent (sauf si KeepOpen)
  if (last_direntry && !last_direntry->isKeepOpen())  {
    removeIconbox(last_direntry, false);
    last_direntry = null;
  }

  // detruire l'ancien iconbox s'il existe
  if (de->iconbox) removeIconbox(de, false);
  if (listener) listener->dirLoaded(de->fpath);

  {      // create a new iconbox in the navigator
    IconBox* iconbox = new IconBox();
    de->iconbox = iconbox;
    openContextMenuIn(*iconbox);
    if (is_tracking) iconbox->scrollpane().setTracking(true);
    // tout de suite sinon flicke ensuite
    iconbox->scale() = scale;
  }
  
  de->iconbox->add
    (UOn::change / ucall(this, de->iconbox(), &Finder::iconSelectCB)
     + UOn::action / ucall(this, de->iconbox(), &Finder::iconActionCB)
     // called when the iconbox is closed (the user clicked the "x" btn)
     // this will delete the iconbox
     + UOn::close / ucall(this, de, true, &Finder::removeIconbox)
     );
  
  stat = de->iconbox->readDir(de->fpath, de->path_type);

  if (stat == UFilestat::Opened) {
    piconbox = de->iconbox;
    mainbox.add(*piconbox);
    de->emph(true);
    last_direntry = de;
    if (opts.show_icon_images) {      // charger les imagettes
      previews_current = piconbox->icons().cbegin();
      previews_end = piconbox->icons().cend();
      preview_timer->start(10, -1);   // -1 : loops until stop()
    }
  }
  else {
    if (stat == UFilestat::CannotOpen) {
      showAlert("Can't Read Folder: " & de->fpath);
    } 
  }

 end:
    if (stat > 0)  {
      de->iconbox->scale() = scale;
      showDir(de->fpath);
    }
  return stat;
}

/* ==================================================== [Elc] ======= */

Icon* Finder::getSelectedIcon() {
  return piconbox ? piconbox->getSelectedIcon() : null;
}

void Finder::previousEntry() {
  if (!piconbox) return;

  Icon* current_icon = piconbox->getSelectedIcon();
  Icon* prev_icon = piconbox->getPreviousIcon();

  if (mode == DocMode) {             // sauter les dirs
    //while (prev_icon && prev_icon->getMode().isDir()) {
    while (prev_icon && prev_icon->isDir()) {
      // ATT: ce code est FAUX: getPreviousIcon(0 renvoie toujours le meme!
      // ca marche uniquement parce que tous les dirs sont regroupes
      // en DEBUT de liste !
      Icon* prev_icon2 = piconbox->getPreviousIcon();
      if (prev_icon == prev_icon2) {
        prev_icon = current_icon;         // cas que des dirs
        break;
      }
      else prev_icon = prev_icon2;
    }
  }

  if (prev_icon) {
    piconbox->selectIcon(*prev_icon);
    if (mode == DocMode) {
      String pathname = piconbox->pathname() & "/" & prev_icon->getName();
      open(pathname);
    }
  }
}

void Finder::nextEntry() {
  if (!piconbox) return;

  Icon* current_icon = piconbox->getSelectedIcon();
  Icon* next_icon = piconbox->getNextIcon();

  if (mode == DocMode) {
    while (next_icon && next_icon->isDir()) {
      Icon* next_icon2 = piconbox->getNextIcon();
      if (next_icon == next_icon2) {
        next_icon = current_icon;         // cas que des dirs
        break;
      }
      else next_icon = next_icon2;
    }
  }

  if (next_icon) {
    piconbox->selectIcon(*next_icon);
    if (mode == DocMode) {
      String pathname = piconbox->pathname() & "/" & next_icon->getName();
      open(pathname);
    }  
  }
}

void Finder::openEntry() {
  if (!getSelectedIcon()) return;

  //if (open_in_fullwin) showFullWin();  // et si pas un dir
  //else
  {
    //c'est pas bon: si basename est protege en lecture on peut plus remonter!
    //name &= "/..";
    String pathname;
    if (getSelectedIcon()->getName() == "..")
      pathname = piconbox->pathname().dirname();
    else
      pathname = piconbox->pathname() & "/" & getSelectedIcon()->getName();
    open(pathname);
  }
}

/* ==================================================== [Elc] ======= */
// Callbacks

void Finder::iconActionCB(IconBox* ibox) {
  if (piconbox && piconbox != ibox) {
    piconbox->selectIcon(null);
    // must be reloaded
    //selected_ima = null;  // unique_ptr : automatic deletion
  }
  openEntry();
}


void Finder::iconSelectCB(IconBox* ibox) {
  if (piconbox && piconbox != ibox) {
    piconbox->selectIcon(null);
    // must be reloaded
    //selected_ima = null;  // unique_ptr : automatic deletion
  }

  piconbox = ibox;  
  showPreviewRequest(getSelectedIcon());
  //if (open_in_fullwin && fullwin->isShown()) showFullWin();
}


void Finder::showPreviewRequest(Icon* icon) {
  if (!icon) return;
  
  // afficher preview sauf si deja selectionne 
  if (icon != last_preview_request) {
    last_preview_request = icon;
    Timer* t = new Timer(10, 1, true);  // true => auto_delete
    t->onAction(ucall(this, icon, &Finder::showPreview));
    t->start();
  }
}


void Finder::showPreview(Icon* icon) {
  if (!icon) return; 
  if (icon != last_preview) {
    last_preview = icon;
    String pathname = piconbox->pathname() & "/" & icon->getName();
    //ctrls->preview.read(icon->name(), pathname);
  }
}


void Finder::showIconPreviews() {
  if (previews_current == previews_end) {
    preview_timer->stop();
    return;
  }

  while (previews_current != previews_end) {
    Icon* icon = dynamic_cast<Icon*>(*previews_current);
    previews_current++;
    if (icon) {
      String pathname = piconbox->pathname() & "/" & icon->getName();
      //icon->readIconImage(icon->getName(), pathname);
      icon->loadImage(pathname);
      return;  // sortir du callback qui sera rappele par le timer
    }
    // sinon ce n'est pas un icon: passer au suivant
  }
}

/* ==================================================== (c)[Elc] ======= */
// FinderCOM

extern "C" {
  typedef void* (*START_ROUTINE)(void*);
}

#ifndef NO_REMOTE_DOC

UFinderCom::UFinderCom(Finder* _fd, const String& _path, int _type)
: fd(_fd), path(_path), type(_type), mode(0), stat(0) {
  pthread_create(&thread_id, NULL, (START_ROUTINE)loadDoc, this);
}


void UFinderCom::loadDoc(UFinderCom* d) {        // A REVOIR!!!!!!!!! 
#if 0 
  //cerr << "BEGIN thread " << pthread_self() << endl;
  //cerr << "\n*** LoadDoc " << d->path << endl;  
  String com;

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  
  if (d->type == UFileInfo::SSH) {
    String server, spath;
    d->mode = UFileMode::FILE;
    if (d->path[-1]=='/') d->mode = UFileMode::DIR;

    // makeCachePath enleve le / final
    if (!UFileCache::getCachePath(d->path, UFileInfo::SSH,
                                  server, spath, d->cachepath)) {
      d->stat = INVALID_URL;
      goto END_THREAD;
    }
    
    if (d->mode == UFileInfo::DIR)
      com = "ssh '" & server & "' ls -F " & spath 
        & " > \"" & d->cachepath & "\"; echo $?";
    else
      com = "scp -p -q -B '" & server & ":" & spath 
        & "' '" & d->cachepath & "'; echo $?";
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

  else if (d->type == UFileInfo::HTTP) {
    String server, spath;
    if (!UFileCache::getCachePath(d->path, UFileInfo::HTTP,
                                  server, spath, d->cachepath)) {
      d->stat = INVALID_URL;
      goto END_THREAD;
    }
    
    // cf -r et -N et -P prefix
    com = "cd " & UFileCache::getCachePath() 
      & "http:/; wget -q -N -S -x " & spath & "; echo $?";
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

  else if (d->type == UFileInfo::FTP) {
    String server, spath;
    if (!UFileCache::getCachePath(d->path, UFileInfo::FTP,
                                  server, spath, d->cachepath)) {
      d->stat = INVALID_URL;
      goto END_THREAD;
    }
    com = "cd " & UFileCache::getCachePath() 
      & "; ftp " & spath & "; echo $?";
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  // exec command
  {
    //cerr << "cachepath: " << d->cachepath << " - pathtype " << d->type<< endl;
    //cerr << "....loading: " << com << endl;
    
    /*int stat =*/ system(com.c_str());
    /*
    FILE* p = popen(com.c_str(), "r");
    if (p) {
      String res;
      char buf[1000];
      while (fgets(buf, sizeof(buf), p)) res.append(buf);
      cout << "Result: " << res << endl;
      pclose(p);
      }
    */
  }
  
END_THREAD:
  // sert a mettre a jour le graphique, mais dans le thread principal
  Application::postpone(ucall(d, &UFinderCom::showDoc));

  //cerr << "END thread " << pthread_self() << endl;
  pthread_exit(NULL);
  
#endif
}


void UFinderCom::showDoc() {
  if (stat == INVALID_URL) {
    fd->showAlert("Invalid URL: " & path);
    return;
  }
  
  UFileMode fm(cachepath);

  if (!fm.isValid()) fd->showAlert("Can't find: " & path);
  else if (type == UFileInfo::SSH) fd->openImpl(path, mode, UFileInfo::SSH);

  else if (type == UFileInfo::HTTP) {
    if (fm.isFile()) {
      fd->openImpl(cachepath, UFileInfo::FILE, UFileInfo::HTTP);
    }
    else if (fm.isDir()) {
      cachepath &= "index.htm";   // faudrait gerer les majuscules !
      UFileMode fm2(cachepath);

      if (fm2.isValid())
        fd->openImpl(cachepath, UFileInfo::FILE, UFileInfo::HTTP);
      else {
        cachepath &= "l";  // index.html
        UFileMode fm3(cachepath);
        if (fm3.isValid())
          fd->openImpl(cachepath, UFileInfo::FILE, UFileInfo::HTTP);
      }
    }
  }

  else if (type == UFileInfo::FTP)
    fd->openImpl(cachepath, UFileInfo::FILE, UFileInfo::FTP);
  else
    fd->showAlert("Unknown protocol: " & path);

  delete this;
}

#endif

/* ==================================================== [Elc] ======= */

void UFinderHost::putFile() {
  Icon* icon = fd.getSelectedIcon();
  if (!icon) {
    fd.showAlert("PutFile: no file selected (select an icon!)");
    return;
  }
  resolve(&UFinderHost::putFileImpl);
}

void UFinderHost::putFileImpl() {
  Icon* icon = fd.getSelectedIcon();
  if (!icon) {
    fd.showAlert("PutFile: no file selected (select an icon!)"); // !!!ET SIL A CHANGE?
    return;
  }

  //if (remote_ums) delete remote_ums;         // !!!  A REVOIR
  MessageService* remote_ums = new MessageService(address);

  String path = fd.piconbox->pathname() & "/" & icon->getName();

  int f = ::open(path.c_str(), O_RDONLY, 0);
  if (f < 0) {
    fd.showAlert("PutFile: selected file does not exist ");
    return;
  }

  {// header
    String header = "!UFP1 RFB [header] ";
    header &= path;                                // !! pbm des blancs !!
    if (!remote_ums->sendBlock(header.c_str(), header.length()+1)) {
      fd.showAlert("PutFile: can't connect remote host ");
      ::close(f);
      return;
    }
  }

  // body
  static const int BUF_SIZE = 49152;  // valeur conseille dans doc Qt
  char buf[BUF_SIZE + 10] = "!UFP1 RFC ";
  int n = 0;

  while ((n = ::read(f, buf+10, BUF_SIZE))) {
    if (n < 0) {
      fd.showAlert("PutFile: read file error ");
      goto ERROR;
    }
    else if (n == 0) {
      // << "fini" << endl;
      break;
    }
    else {
      if (!remote_ums->sendBlock(buf, n+10)) {
        fd.showAlert("PutFile: remote connection aborted ");
        goto ERROR;
      }
    }
  }

  { // footer
    static char footer[] = "!UFP1 RFE";
    remote_ums->sendBlock(footer, sizeof(footer));
  }

  ::close(f);
  delete remote_ums;
  fd.showAlert("PutFile: file " & icon->getName() & " succesfully sent to target ");
  return;

ERROR:
  if (f >= 0) ::close(f);
  static char errmsg[] = "!UFP1 RFX";
  remote_ums->sendBlock(errmsg, sizeof(errmsg));
  delete remote_ums;
}

/* ==================================================== (c)[Elc] ======= */
// HOSTS

bool Finder::isBrowsing() const {
  return (local_ums && local_ums->isConnected());
}

bool Finder::browseHosts() {
  if (local_ums) delete local_ums;
  local_ums = new MessageService("localhost");
  return local_ums->browseUMServers(ucall(this, &Finder::browseCB));
}

void Finder::browseCB(MessageEvent& e) {
  MessageService::BrowseReply r(e);
  if (r.serviceName.empty()) return;
 
  UFinderHost* found = null;
  for (ChildIter i = hostlist.cbegin(); i != hostlist.cend(); i++) {
    UFinderHost* host = dynamic_cast<UFinderHost*>(*i);
    if (host) {
      if (host->hostname == r.serviceName) {
        found = host;
        break;
      }
    }
  }

  if (found && !r.isAdded())        // enlever
    removeHost(found);
  else if (!found && r.isAdded())   // ajouter (si pas deja present!)
    addHost(r.serviceName);
}

UFinderHost* Finder::addHost(const String& hostname) {
  UFinderHost* host = new UFinderHost(this, hostname);
  hostlist.add(host);
  return host;
}

UFinderHost* Finder::addHostCB(const String* hostname) {
  if (!hostname || hostname->empty()) return null;
  else return addHost(*hostname);
}

void Finder::removeHost(UFinderHost* host) {
  if (host) hostlist.remove(*host);
}

void Finder::addHosts(const vector<String*>& hostnames) {
  for (unsigned int k = 0; k < hostnames.size(); k++) 
    if (hostnames[k]) addHost(*hostnames[k]);
}
void Finder::addHosts(char const* hostnames[]) {
  for (unsigned int k = 0; hostnames[k] != null; k++)
    if (hostnames[k]) addHost(hostnames[k]);
}

/* ==================================================== (c)[Elc] ======= */

UFinderHost::UFinderHost(class Finder* _fd, const String& _hostname)
: fd(*_fd), hostname(_hostname), clone_win(null) //, remote_ums(null) 
{
  clone_btn = 
  &ucheckbox(//Background::none
             "Clone"
             + UOn::select / ucall(this, &UFinderHost::createClone)
             + UOn::deselect / ucall(this, &UFinderHost::deleteClone)
             );

  xhost_btn = 
  &ucheckbox(//Background::none
             + "Xhost"
             + UOn::select / ucall(this, &UFinderHost::addXhost)
             + UOn::deselect / ucall(this, &UFinderHost::removeXhost)
             );

  put_btn = null;
  
  Box& funcbox = uvbox(*clone_btn + *xhost_btn);
  funcbox.setAttr(upadding().setLeft(6));
  funcbox.show(false);

  if (hostname == "localhost")
    ;//box.add(ulinkbutton("Calibrate" + ucall(this, &UFinderHost::calibrate)));
  else {
    put_btn = new LinkButton("-- Put"+ ucall(this, &UFinderHost::putFile));
    funcbox.add(*put_btn /* + get_btn */);  // to be completed...
  }
  
  Item& name_box = uitem
    (Color::black + UOn::select / Font::bold
     + UOn::select / Background::none
     + UOn::select / UPix::minus
     + UOn::deselect / UPix::plus
     + " " + hostname
     + UOn::select / ushow(funcbox, true)
     + UOn::deselect / ushow(funcbox, false)
     // essayer de se reconnecter si !isBrowsing()
     //+ UOn::select / ucall(this, (HostMethod)0, &UFinderHost::resolve)
     );
  
  add(name_box + funcbox);
}

/* ==================================================== (c)[Elc] ======= */

void UFinderHost::resolve(HostMethod m) {
  if (!fd.isBrowsing()) fd.browseHosts();     // tenter une reconnection

  if (!fd.isBrowsing()) {
    fd.showAlert("Can't connect to the local UMS server \n(start the 'umsd' program on this machine then try again)");
    // initialiser localhost qunad meme
    if (hostname == "localhost") address = "localhost";
    return;
  }

  // NB: on remet a jour l'adresse a chaque appel
  if (hostname == "localhost" || hostname.find(".local")>0) {
    address = hostname; 
    if (m) (this->*m)();
  }
  else {           // NB: le ucall est detruit automatiquement
    fd.local_ums->resolveUMServer(hostname, ucall(this,m,&UFinderHost::resolveCB));
  }
}

void UFinderHost::resolveCB(MessageEvent& e, HostMethod m) {
  MessageService::ResolveReply r(e);
  address = r.hosttarget;
  if (m) (this->*m)();
}

/* ==================================================== (c)[Elc] ======= */

void UFinderHost::calibrate() {
  resolve(&UFinderHost::calibrateImpl);
}

void UFinderHost::calibrateImpl() {
  if (fd.local_ums) fd.local_ums->sendRequest(UMSrequest::CALIBRATE);
}


void UFinderHost::addXhost() {
  resolve(&UFinderHost::addXhostImpl);
}

void UFinderHost::addXhostImpl() {
  if (address.empty()) {
    fd.showAlert("can't xhost + display: " & hostname & " address is unknown");
    return;
  }
  String com = "xhost +"; com &= address;
  system(com.c_str());
}

void UFinderHost::removeXhost() {
  if (address.empty()) {
    fd.showAlert("can't xhost - display: " & hostname & " address is unknown");
    return;
  }
  String com = "xhost -"; com &= address;
  system(com.c_str());
}


void Finder::createClone(const String& hostname) {
  UFinderHost* host = addHost(hostname);
  host->createClone();
}

void UFinderHost::createClone() {
  resolve(&UFinderHost::createCloneImpl);
}

void UFinderHost::createCloneImpl() {
  if (address.empty()) {
    fd.showAlert("can't open display: " & hostname & " address is unknown");
    return;
  }

  Display* disp = Application::openDisp(address);
  if (!disp) {
    fd.showAlert("can't open display: " & hostname & " (address: " & address & ")");
    return;
  }

  delete clone_win;  // un seul clone !
  clone_win = fd.createCloneFrame("Clone from " & hostname);
  clone_win->add(UOn::close / ucall(clone_btn, false, true, &Element::setSelected));
  disp->add(*clone_win);
  clone_win->show();
}

void UFinderHost::deleteClone() {
  //host->select(false);
  if (clone_win) clone_win->show(false);
  delete clone_win;   // faudrait aussi fermer le display!
  clone_win = null;
}


void Finder::createCloneRequest(MessageEvent& e) {
  const String* hostname = e.getMessage();
  if (!hostname || hostname->empty()) return;

  *ask_dialog_msg = "Open Clone on " & *hostname & " ?";
  int resp = ask_dialog->showModal();
  if (resp > 0) {
    UFinderHost* h = addHost(*hostname);
    h->createClone();
  }
}

UFrame* Finder::createCloneFrame(const String& title) {
  return &uframe(opts.clone_frame_size + this).setTitle(title);
}

}
