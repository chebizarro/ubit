/*
 *  appli.cpp: Appplication Context (must be created first)
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
#include <algorithm>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <locale.h>
#include <unistd.h>       // darwin
#include <sys/stat.h>

#include <ubit/core/on.h>
#include <ubit/core/call.h>
#include <ubit/ui/dialog.h>
#include <ubit/ui/umenuImpl.hpp>
#include <ubit/ui/uwinImpl.hpp>
#include <ubit/core/appimpl.h>
#include <ubit/ui/eventflow.h>
#include <ubit/ui/update.h>
#include <ubit/ui/selection.h>
#include <ubit/core/config.h>
#include <ubit/core/string.h>
#include <ubit/net/source.h>
#include <ubit/ui/timer.h>
#include <ubit/draw/graph.h>

namespace ubit {


//UConf Application::conf is in uconf.cpp to avoid init problems

// the (unique) implementation of the application context
static AppImpl appli_impl;
AppImpl& Application::impl = appli_impl;


AppImpl::AppImpl() : 
application(null),
disp(null),
app_name(null),
is_terminated(false),
// should not be set to null because AppImpl constr may be called after the
// init. of this variable
//error_handler(null), cf Application::getErrorHandler()
main_frame(null),
sources(null),
modalwins(null),
messmap(null),
app_motion_lag(15),
nat_motion_lag(100),
main_status(0), modal_status(0),
mainloop_running(false), subloop_running(false),
request_mask(0),
is_processing_update_requests(false),
is_processing_layout_update_requests(false) {
}


String& Application::initConf(int& argc, char** argv, Option* opts, const char* cfile) {
  conf.readAttributes(cfile);
  // parseOptions(opts) first because they superseede Ubit options
  if (opts) conf.parseOptions(argc, argv, opts);
  conf.parseUbitOptions(argc, argv);
  conf.freeze_options = true;

  return conf.display;
}


Application::Application(int& argc, char** argv, Option* opts, const char* confile) {
  String s = argv[0];
  impl.app_name = new String(s.basename());
  impl.disp = Display::create(initConf(argc, argv, opts, confile));

  if (!impl.disp->isOpened()) {   // throws an exception
    Application::fatalError("Application","The Application display can't be opened (is the X11 Server running?)");
  }

  if (impl.appli) {
    Application::fatalError("Application","The Application is already created (an application must have only one Application)");
    return;
  }
  impl.appli = this;
  impl.sources = new Element();
  // UWinList elements are not counted has parents when auto deleting children
  impl.modalwins = new UWinList();
  
  // creates the default flow (action required by certain toolkits)
  impl.disp->obtainChannelFlow(0);
  
  impl.mainloop_running = impl.subloop_running = false; 
  impl.disp->startAppli(); 
}


Application::~Application() {
  impl.is_terminated = true;
  //pose probleme: des destructeurs peuvent etre appeles juste apres
  //-> ne pas faire d'exception dans un tel cas!  
  // impl.appli = null;
  impl.disp->quitAppli();
}

/*  APPELE OU ?
void Application::realize() {
  Display::realize();
  //if (conf.locale) setlocale(LC_CTYPE, conf.locale);
  if (!conf.locale.empty()) setlocale(LC_ALL, conf.locale.c_str());
  //impl.natappli->setProperties(conf);
  const String& name = getAppName();
  static const String res_class = "Ubit";
  natdisp->getMainFrame().setNames(res_class, name, name, name);
} 
*/
  //bool Application::isRealized() const {return natdisp->isRealized();}
  //const char* Application::getCommandName() const {return conf.app_name;}
  //const char* Application::getCommandPath() const {return (conf.app_argc>0 ? conf.app_argv[0] : null);}
  
Application& Application::application() {
  if (!impl.appli)    // throws an exception
    Application::fatalError("Application::application()","no Application instance (not created yet?, already destroyed?)");
  return *impl.appli;
}

Application* Application::getApplication() {return impl.appli;}

bool Application::isExiting() {return impl.is_terminated;}

const char* Application::getVersion() {return UBIT_VERSION;}

//const char* Application::getWindowingToolkit() {return conf.windowing_toolkit;}

bool Application::isRunning() {return impl.mainloop_running;}

Frame* Application::getMainFrame() {return impl.main_frame;}

const String& Application::getName() {
  if (!appli_impl.app_name) appli_impl.app_name = new String();
  return *appli_impl.app_name;
}

StyleSheet& Application::getStyleSheet() {
  return impl.stylesheet;
}

const Style& Application::getDefaultStyle() {
  static Style* default_style = new Style();
  return *default_style;
}


String Application::getTitle() {
  if (impl.main_frame) return impl.main_frame->getTitle();
  else return "";
}

void Application::setTitle(const String& title) {
  if (impl.main_frame) impl.main_frame->setTitle(title);
}

void Application::setFocus(View* v) {
  getFlow(0)->setFocus(v);
}


// En arg: n'importe quelle Window; mais seul le MainFrame (= le 1er Frame 
// ajoute a Application) est automatiquement affichee par defaut
void Application::add(Window& win) {
  impl.disp->add(win);
  Frame* frame = dynamic_cast<Frame*>(&win);
  if (frame && !impl.main_frame) {    // makes the first Frame be the 'Main Frame'
    impl.main_frame = frame; 
    impl.main_frame->wmodes.IS_MAINFRAME = true;
  }
}

void Application::add(Window* win) {
  if (!win) error("Application::add","null argument");
  else add(*win);
}

void Application::remove(Window& win, bool remove_mode) {
  if (&win == impl.main_frame) impl.main_frame = null;
  impl.disp->remove(win, remove_mode);
}

void Application::remove(Window* win, bool remove_mode) {
  if (!win) error("Application::remove","null argument");
  else remove(*win, remove_mode);
}


static void updateAll2(Element* grp, const Update& mode) {
  // NB: du fait des parents multiples, il est possible de remettre
  // a jour plusieurs fois la meme window
  // ==> il faudrait tenir compte du flag must-update
  if (grp->toWin()) grp->toWin()->update(mode);
  
  for (ChildIter c = grp->cbegin(); c != grp->cend(); ++c) {
    Element* g = (*c)->toElem();
    if (g) updateAll2(g, mode);    // in any case
  }
}

//  updates all visible windows recursively
void Application::updateAll(const Update& mode) {
  for (ChildIter c = impl.disp->winlist.cbegin(); c != impl.disp->winlist.cend(); ++c) {
    Element* g = (*c)->toElem();
    if (g) updateAll2(g, mode);
  }
}


int Application::start() {
  // updateAll() remet l'affichage a jour avant d'entrer dans la mainloop
  // pour eviter des pbms d'initialisation du layout, en part. avec le texte
  impl.appli->updateAll();

  //if (impl.app_title.empty()) setTitle(*impl.app_name);
  //else setTitle(impl.app_title);
  String t = getTitle();
  if (t.empty()) setTitle(*impl.app_name);

  impl.main_status = 0;
  impl.disp->startLoop(true);  // main loop

  // detruire ce qui doit l'etre mais ne pas appeler les requetes d'affichage
  impl.processDeleteRequests();
  return impl.main_status;
}


void Application::quit(int status) {
  impl.main_status = status;
  impl.is_terminated = true;
  impl.disp->quitLoop(true);  // main loop
}


int AppImpl::startModalWinLoop(Window& win) {
  modal_status = 0;
  disp->startLoop(false);  // sub loop
  processPendingRequests();
  return modal_status;
}

void AppImpl::setModalStatus(int status) {
  modal_status = status;
}

void AppImpl::addModalWin(Window& win) {
  modalwins->remove(win, false);   // remove if already in the list
  modalwins->add(win, 0);            // add at the beginning of the list
}

void AppImpl::removeModalWin(Window& win) {
  disp->quitLoop(false);     // subloop
  modalwins->remove(win, false);
}


void AppImpl::processPendingRequests() {
  is_processing_update_requests = false;
  processUpdateRequests();
  processDeleteRequests();
  request_mask = 0;
}

void AppImpl::processDeleteRequests() {
  // views
  for (unsigned int k = 0; k < del_view_list.size(); ++k) {
    ::operator delete(del_view_list[k]);    // enforces deletion
  }
  del_view_list.clear();
  
  // bricks
  for (unsigned int k = 0; k < del_obj_list.size(); ++k) {
    ::operator delete(del_obj_list[k]);    // enforces deletion
  }
  del_obj_list.clear();
  
  request_mask &= ~DELETE_REQUEST;
}


void AppImpl::addDeleteRequest(View* v) {
  del_view_list.push_back(v);
  request_mask |= DELETE_REQUEST;
}


void AppImpl::addDeleteRequest(UObject* b) {
  b->omodes.IS_DESTRUCTED = true;  // securite: normalement c'est deja le cas
  
  // si b est dans updatelist il faut l'enlever
  if (b->omodes.IS_UPDATING) removeUpdateRequests(b->toBox());
  /*
   if (b->omodes.IS_UPDATING) {  // si b est dans updatelist il faut l'enlever
     UpdateRequest* req = null;
     unsigned int k = 0;
     while ((req = findUpdateRequest(b, k))) {
       req->obj = null;         // remove from the updatelist
       k++;
     }
   }
   */  
  if (Application::conf.postpone_delete) {
    del_obj_list.push_back(b);    
    request_mask |= DELETE_REQUEST;    
  }
  else ::operator delete(b);    // enforces deletion
}


void AppImpl::processUpdateRequests() 
{  
  // NB: is_processing_update_requests: avoid infinite loops: this fct cant call itself
  if (is_processing_update_requests || (request_mask & UPDATE_REQUEST)==0 || is_terminated)
    return;
  
  is_processing_update_requests = true;
    
  // this will prevent View::updateWinPaint() to draw anything as the final
  // refresh is performed for the entire windows a few lines below
  if (Application::conf.is_using_gl) is_processing_layout_update_requests = true;
  
  // incorrect: voir plus bas
  //UpdateRequests::iterator p = update_list.begin();
  //UpdateRequests::iterator p_end = update_list.end();
  //for ( ; p != p_end; ++p) {

  // !!CAUTION: addUpdateRequest() may be called WHILE processUpdateRequests()
  // is being executed => the update_list vector may be reallocated at any time
  // (hence, is size() and the address of its elements may change)
  
  for (unsigned int k = 0; k < update_list.size(); ++k) {
    Box* obj = update_list[k].obj;
    if (obj) {     // obj == null if the obj was deleted in the meanwhile
      obj->doUpdate(update_list[k].upd, null);     // !!!&&& second arg should be disp !!!
      obj->omodes.IS_UPDATING = false;
      if (is_terminated) return;
    }
  }
  
  is_processing_layout_update_requests = false;

  update_list.clear();

  if (Application::conf.is_using_gl) {  // refresh des windows modifiees !!!A METTRE DANS Display
    // !!!! IL FAUDRAIT considere TOUS les Display  !!!@@@
    Display::HardwinList::iterator c = disp->hardwin_list.begin();
    Display::HardwinList::iterator c_end = disp->hardwin_list.end();
    for ( ; c != c_end; ++c) {
      UHardwinImpl* hw = *c;
      if (hw && hw->must_update && hw->win) {
        //cerr << "> processUpdateRequests: HW: " << hw <<endl;
        hw->win->doUpdate(Update::paint, disp);
        hw->must_update = false;
        //cerr << "< processUpdateRequests: HW: " << hw <<endl <<endl;
      }
    }
  }
  
  is_processing_update_requests = false;
  request_mask &= ~UPDATE_REQUEST;  // remove UPDATE_REQUEST
}


void AppImpl::addUpdateRequest(Box* obj, const Update& upd) {
  // don't update an object that has been destructed
  if (obj->omodes.IS_DESTRUCTED || obj->omodes.IS_DESTRUCTING || is_terminated) return;
  
  if (update_list.size() > 0) {   // same obj, same upd => nothing to update
    UpdateRequest& req = update_list[update_list.size()-1];
    if (req.obj == obj && req.upd == upd) return;
  }
  
  // ! addUpdateRequest() may be called WHILE processUpdateRequests() is being executed
  //if (is_processing_update_requests)
  //  cerr << "!!! is_processing_update_requests !!!"<<endl;
  
  bool remove_paint = false;
  
  if (Application::conf.is_using_gl) { 
    if (upd.modes == Update::PAINT) {
      for (View* v = obj->views; v != null; v = v->next) {
        if (v->hardwin) v->hardwin->must_update = true;
      }
      goto END;
    }
    else if (upd.modes & Update::PAINT) {
      for (View* v = obj->views; v != null; v = v->next) {
        if (v->hardwin) v->hardwin->must_update = true;
      }
      remove_paint = true;   // remove Update::PAINT
    }
  }
  
  update_list.push_back(UpdateRequest(obj, upd, remove_paint)); 
  
END:
  obj->omodes.IS_UPDATING = true;  // objects removed then from the upd list if deleted 
  request_mask |= UPDATE_REQUEST;
}

void AppImpl::removeUpdateRequests(Box* box) {
  if (is_terminated || box == null) return;
  for (unsigned int k = 0; k < update_list.size(); ++k) {
    if (box == update_list[k].obj) update_list[k].obj = null;
  }  
}

/*
UpdateRequest* AppImpl::findUpdateRequest(Box* obj, unsigned int& k) {
  if (is_terminated) return null;
  for (; k < update_list.size(); k++) {
    if (obj == update_list[k].obj) return &update_list[k];
  }
  return null;
}
*/

void Application::deleteNotify(Display* d) {
  for (unsigned int k = 0; k < impl.displist.size(); ++k) {
    if (impl.displist[k] == d) {
      // va tout decaler si on enleve de la liste !
      impl.displist[k] = null;
      break;
    }
  }
}

void Application::deleteNotify(View* deleted_view) {
  UFlowList& flist = impl.flowlist; 
  for (unsigned int k = 0; k < flist.size(); k++) {
    if (flist[k]) flist[k]->deleteNotify(deleted_view);
  }
}

void Application::deleteNotify(Element* deleted_group) {
  UFlowList& flist = impl.flowlist; 
  for (unsigned int k = 0; k < flist.size(); k++) {
    if (flist[k]) flist[k]->deleteNotify(deleted_group);
  }
}


const UDispList& Application::getDispList() {
  return impl.displist;
}

Display* Application::getDisp() {return impl.disp;}

// NB: un acces direct par [] serait possible
Display* Application::getDisp(int _id)  {
  for (unsigned int k = 0; k < impl.displist.size(); ++k) {
    if (impl.displist[k]->getID() == _id) return impl.displist[k];
  }
  return null;
}
 
Display* Application::openDisp(const String& _display_name) {
  //Display* d = new Display(_display_name);
  Display* d = Display::create(_display_name);
  if (d && d->isOpened()) return d;
  else {
    delete d;
    return null;
  }
}

void Application::closeDisp(Display* d) {
  delete d;
}

bool Application::hasTelePointers() {
  return (conf.tele_pointers && impl.displist.size() > 1);
}


const UFlowList& Application::getFlowList() {
  return impl.flowlist;
}

// NB: un acces direct par [] serait possible
EventFlow* Application::getFlow(int _id)  {
  for (unsigned int k = 0; k < impl.flowlist.size(); ++k) {
    if (impl.flowlist[k]->getID() == _id) return impl.flowlist[k];
  }
  return null;
}

Selection* Application::getSelection(int _id) {
  EventFlow* fl = getFlow(_id);
  if (!fl) return null;
  else return fl->getSelection();
}


Menu* Application::getOpenedMenu() {
  EventFlow* fl = impl.disp->obtainChannelFlow(0);  // DEFAULT IFLOW : A REVOIR
  return fl ? fl->menu_man.getDeepestMenu() : null; 
}

const String& Application::getImaPath() {
  return impl.imapath;
}

void Application::setImaPath(const String& value) {
  impl.imapath = value;
  String& path = impl.imapath;
  
  if (path[0] == '~' && (path[1] == '/' || path[1] == 0)) {
    //NB: ne PAS faire de free() sur un getenv!
    static char* home = ::getenv("HOME");
    if (home) path.replace(0, 1, home);   // virer ~ mais pas /
  }
  
  // ajouter / final
  if (path[-1] != '/') path.append('/'); 
}


void Application::setMotionLag(unsigned long app_lag, unsigned long nat_lag) {
  impl.app_motion_lag = app_lag;
  impl.nat_motion_lag = nat_lag;
}

unsigned long Application::getTime() {return UTimerImpl::getTime();}

void Application::postpone(UCall& c) {  // pas tout a fait correct si mthreads!
  addTimeout(0, 1, c);
  //if (pthread_self() != main_thread) {
  // Message::send(*impl.disp->getMainFrame(), "wakeup");    !!!@@@@!!!!
  // }
}

void Application::addTimeout(unsigned long _delay, int _ntimes, UCall& c) {
  Timer* t = new Timer(_delay, _ntimes, true); // true => auto_delete
  t->onTimeout(c);
  t->start();
}


MessagePortMap* Application::getMessagePortMap() {return impl.messmap;}

MessagePort* Application::findMessagePort(const String& name) {
  if (impl.messmap) return impl.messmap->findMessagePort(name);
  else return null;
}

MessagePort& Application::getMessagePort(const String& name) {
  if (!impl.messmap) impl.messmap = new MessagePortMap();
  return impl.messmap->getMessagePort(name);
}

void Application::onMessage(const String& name, UCall& c) {
  MessagePort& mp = getMessagePort(name);
  //mp.onChange(c); devrait etre onChange si c'etait une Node
  mp.add(c); 
}


void Application::error(const char* fun, const char* format, ...){
  va_list ap;
  va_start(ap, format);
  raiseError(Error::ERROR, null/*object*/, fun, format, ap);
  va_end(ap);
}

void Application::warning(const char* fun, const char* format, ...){
  va_list ap;
  va_start(ap, format);
  raiseError(Error::WARNING, null/*object*/, fun, format, ap);
  va_end(ap);
}

void Application::fatalError(const char* fun, const char* format, ...){
  va_list ap;
  va_start(ap, format);
  raiseError(Error::FATAL_ERROR, null/*object*/, fun, format, ap);
  va_end(ap);
}

void Application::internalError(const char* fun, const char* format, ...){
  va_list ap;
  va_start(ap, format);
  raiseError(Error::INTERNAL_ERROR, null/*object*/, fun, format, ap);
  va_end(ap);
}


void Application::raiseError(int errnum, const UObject* obj, const char* funcname, 
                        const char* format, ...){
  va_list ap;
  va_start(ap, format);
  getErrorHandler().raiseError(errnum, obj, funcname, format, ap);
  va_end(ap);
}

void Application::raiseError(int errnum, const UObject* obj, const char* funcname, 
                        const char* format, va_list ap){
  getErrorHandler().raiseError(errnum, obj, funcname, format, ap);
}

UErrorHandler& Application::getErrorHandler() {
  // this variable should be null even if this AppImpl constr was not called
  if (!appli_impl.error_handler) {
    appli_impl.error_handler = 
    new UErrorHandler((appli_impl.app_name ? *appli_impl.app_name : ""), &std::cerr);
  }
  
  if (appli_impl.error_handler->label().empty() && appli_impl.app_name)
    appli_impl.error_handler->label() = *appli_impl.app_name;
  
  return *appli_impl.error_handler;
}

void Application::setErrorHandler(UErrorHandler& eh) {
  appli_impl.error_handler = eh;
}


UErrorHandler::UErrorHandler(const String& _label, std::ostream* _fout) : 
plabel(ustr(_label)),
pbuffer(null),
fout(_fout) {
}

UErrorHandler::~UErrorHandler() {}

void UErrorHandler::setOutputStream(std::ostream* f) {
  fout = f;
}

void UErrorHandler::setOutputBuffer(String* s) {
  pbuffer = s;
}


void UErrorHandler::warning(const char* fun, const char* format, ...) const {
  va_list ap;
  va_start(ap, format);
  raiseError(Error::WARNING, null, fun, format, ap);
  va_end(ap);
}

void UErrorHandler::error(const char* fun, const char* format, ...) const {
  va_list ap;
  va_start(ap, format);
  raiseError(Error::ERROR, null, fun, format, ap);
  va_end(ap);
}

void UErrorHandler::error(int errnum, const UObject *obj, const char* fun, 
                          const char* format, ...) const {
  va_list ap;
  va_start(ap, format);
  raiseError(errnum, obj, fun, format, ap);
  va_end(ap);
}

void UErrorHandler::parserError(int errnum, const Char* tbuffer,
                                const char* msg1, const String& name,
                                const char* msg2, const Char* line) const {
  // eviter erreurs de positionnement en debut de buffer
  if (line < tbuffer) line = tbuffer;
  
  // calculer la position de line dans text
  int l = 1;
  for (const Char* t = tbuffer; t <= line; t++ ) {
    if (*t == '\n') l++;
  }
  
  // n'afficher que le debut de line et remplacer les \n par des \ suivis de n
  String line_beg;
  if (line) {
    line_beg.append(line, 60);
    for ( int k = 0; k < line_beg.length(); k++) {
      if (line_beg.at(k) == '\n') line_beg.replace(k, 1, "\\n");
    }
  }
  
  String& err = ustr("At line ");
  err &= l;
  err &= ":\n";
  err &= line_beg;
  err &= "\n";
  if (msg1) err.append(msg1);
  if (!name.empty()) err.append(name);
  if (msg2) err.append(msg2);
  
  raiseError(errnum, &err);
}

void UErrorHandler::raiseError(int errnum, const UObject* obj, const char* funcname, 
                               const char* format, va_list ap) const {
  Error e(errnum, obj, funcname);
  formatMessage(e, format, ap);
  if (fout && *fout) printOnStream(e);
  if (pbuffer) printOnBuffer(e);
  if (e.errnum < 0) throw e;
}

void UErrorHandler::raiseError(int errnum, String* msg) const {
  Error e(errnum, null, null);
    if (msg) {
      strncpy(e.message, msg->c_str(), sizeof(e.message));
      e.message[sizeof(e.message)-1] = 0;
  }
  if (fout && *fout) printOnStream(e);
  if (pbuffer) printOnBuffer(e);
  if (e.errnum < 0) throw e;
}


void UErrorHandler::formatMessage(Error& e, const char* format, va_list ap) const {
  // ICI traiter les translations !
  //char buf[2000] = "";
  char* p = e.message;
  int n = 0, len = 0;
  
  n = 0;
  if (plabel->empty()) sprintf(p, "(uninitialized) %n", &n);
  else sprintf(p, "(%s) %n", plabel->c_str(), &n);  
  len += n;
  p += n;
  
  n = 0;
  const char* errname = getErrorName(e.errnum);
  if (errname) sprintf(p, "%s %n", errname, &n); 
  else sprintf(p, "Custom Error #%d %n", e.errnum, &n); 
  len += n;
  p += n;
  
  if (e.function_name) {
    n = 0;
    if (e.object) sprintf(p, "in %s::%s()%n", 
                          e.object->getClassName().c_str(), e.function_name, &n);
    else sprintf(p, "in %s()%n", e.function_name, &n);
    len += n;
    p += n;
  }
  
  if (e.object) {
    n = 0;
    sprintf(p, " on object %p%n", e.object, &n);
    len += n;
    p += n;
  }
  
  n = 0;
  sprintf(p, "\n%n", &n);
  len += n;
  p += n;
  
  if (format) {
    vsnprintf(p, sizeof(e.message) - len, format, ap);
    //vsprintf(p, format, ap);
  }
  e.message[sizeof(e.message)-1] = 0;
}

const char* UErrorHandler::getErrorName(int errnum) const {
  switch (errnum) {
    case Error::WARNING: 
      return "Warning";
    case Error::ERROR: 
      return "Error"; 
    case Error::FATAL_ERROR: 
      return "Fatal Error";
    case Error::INTERNAL_ERROR: 
      return "Internal Error"; 
    case Error::STYLE_ERROR: 
      return "Style Error"; 
    case Error::CSS_ERROR: 
      return "CSS Error"; 
    case Error::XML_ERROR: 
      return "XML Error"; 
    default:
      return null;
  }
}

void UErrorHandler::printOnStream(const Error& e) const {
  if (!fout) {
    cerr <<  "UErrorHandler::printOnStream: can't print error because output stream is null! " << endl;
  }
  else {
    *fout << e.what() << endl << endl;
  } 
}

void UErrorHandler::printOnBuffer(const Error& e) const {
  if (!pbuffer) {
    cerr <<  "UErrorHandler::printOnBuffer: can't print error because output buffer is null! " << endl;
  }
  else {
    pbuffer->append(e.what());
  }
}

}
