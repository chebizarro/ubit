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
#include <ubit/core/errorhandler.h>
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
//std::unique_ptr<AppImpl> Application::impl_ = std::make_unique(appli_impl);


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
  config.readAttributes(cfile);
  // parseOptions(opts) first because they superseede Ubit options
  if (opts) config.parseOptions(argc, argv, opts);
  config.parseUbitOptions(argc, argv);
  config.freeze_options = true;

  return config.display;
}


Application::Application(int& argc, char** argv, Option* opts, const char* confile) {
  String s = argv[0];
  impl_->app_name = new String(s.basename());
  impl_->disp = Display::create(initConf(argc, argv, opts, confile));

  if (!impl_->disp->isOpened()) {   // throws an exception
    Application::fatalError("Application","The Application display can't be opened (is the X11 Server running?)");
  }

  if (impl_->application) {
    Application::fatalError("Application","The Application is already created (an application must have only one Application)");
    return;
  }
  impl_->application = this;
  impl_->sources = new Element();
  // UWinList elements are not counted has parents when auto deleting children
  impl_->modalwins = new UWinList();
  
  // creates the default flow (action required by certain toolkits)
  impl_->disp->obtainChannelFlow(0);
  
  impl_->mainloop_running = impl_->subloop_running = false; 
  impl_->disp->startAppli(); 
}


Application::~Application() {
  impl_->is_terminated = true;
  //pose probleme: des destructeurs peuvent etre appeles juste apres
  //-> ne pas faire d'exception dans un tel cas!  
  // impl_->application = null;
  impl_->disp->quitAppli();
}

/*  APPELE OU ?
void Application::realize() {
  Display::realize();
  //if (config.locale) setlocale(LC_CTYPE, config.locale);
  if (!config.locale.empty()) setlocale(LC_ALL, config.locale.c_str());
  //impl_->natappli->setProperties(conf);
  const String& name = getAppName();
  static const String res_class = "Ubit";
  natdisp->getMainFrame().setNames(res_class, name, name, name);
} 
*/
  //bool Application::isRealized() const {return natdisp->isRealized();}
  //const char* Application::getCommandName() const {return config.app_name;}
  //const char* Application::getCommandPath() const {return (config.app_argc>0 ? config.app_argv[0] : null);}
  
Application& Application::application() {
  if (!impl_->application)    // throws an exception
    Application::fatalError("Application::application()","no Application instance (not created yet?, already destroyed?)");
  return *impl_->application;
}

Application* Application::getApplication() {return impl_->application;}

bool Application::isExiting() {return impl_->is_terminated;}

const char* Application::getVersion() {return UBIT_VERSION;}

//const char* Application::getWindowingToolkit() {return config.windowing_toolkit;}

bool Application::isRunning() {return impl_->mainloop_running;}

Frame* Application::getMainFrame() {return impl_->main_frame;}

const String& Application::getName() {
  if (!appli_impl.app_name) appli_impl.app_name = new String();
  return *appli_impl.app_name;
}

StyleSheet& Application::getStyleSheet() {
  return impl_->stylesheet;
}

const Style& Application::getDefaultStyle() {
  static Style* default_style = new Style();
  return *default_style;
}


String Application::getTitle() {
  if (impl_->main_frame) return impl_->main_frame->getTitle();
  else return "";
}

void Application::setTitle(const String& title) {
  if (impl_->main_frame) impl_->main_frame->setTitle(title);
}

void Application::setFocus(View* v) {
  getFlow(0)->setFocus(v);
}


// En arg: n'importe quelle Window; mais seul le MainFrame (= le 1er Frame 
// ajoute a Application) est automatiquement affichee par defaut
void Application::add(Window& win) {
  impl_->disp->add(win);
  Frame* frame = dynamic_cast<Frame*>(&win);
  if (frame && !impl_->main_frame) {    // makes the first Frame be the 'Main Frame'
    impl_->main_frame = frame; 
    impl_->main_frame->wmodes.IS_MAINFRAME = true;
  }
}

void Application::add(Window* win) {
  if (!win) error("Application::add","null argument");
  else add(*win);
}

void Application::remove(Window& win, bool remove_mode) {
  if (&win == impl_->main_frame) impl_->main_frame = null;
  impl_->disp->remove(win, remove_mode);
}

void Application::remove(Window* win, bool remove_mode) {
  if (!win) error("Application::remove","null argument");
  else remove(*win, remove_mode);
}


static void updateAll2(Element* grp, const Update& mode) {
  // NB: du fait des parents multiples, il est possible de remettre
  // a jour plusieurs fois la meme window
  // ==> il faudrait tenir compte du flag must-update
  
  // TODO
  // if (grp->toWin()) grp->toWin()->update(mode);
  
  for (ChildIter c = grp->cbegin(); c != grp->cend(); ++c) {
    Element* g = reinterpret_cast<Element*> (*c);
    if (g) updateAll2(g, mode);    // in any case
  }
}

//  updates all visible windows recursively
void Application::updateAll(const Update& mode) {
  for (ChildIter c = impl_->disp->winlist.cbegin(); c != impl_->disp->winlist.cend(); ++c) {
    Element* g = reinterpret_cast<Element*> (*c);
    if (g) updateAll2(g, mode);
  }
}


int Application::start() {
  // updateAll() remet l'affichage a jour avant d'entrer dans la mainloop
  // pour eviter des pbms d'initialisation du layout, en part. avec le texte
  impl_->application->updateAll();

  //if (impl_->app_title.empty()) setTitle(*impl_->app_name);
  //else setTitle(impl_->app_title);
  String t = getTitle();
  if (t.empty()) setTitle(*impl_->app_name);

  impl_->main_status = 0;
  impl_->disp->startLoop(true);  // main loop

  // detruire ce qui doit l'etre mais ne pas appeler les requetes d'affichage
  impl_->processDeleteRequests();
  return impl_->main_status;
}


void Application::quit(int status) {
  impl_->main_status = status;
  impl_->is_terminated = true;
  impl_->disp->quitLoop(true);  // main loop
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


void AppImpl::addDeleteRequest(Object* b) {
  b->omodes.IS_DESTRUCTED = true;  // securite: normalement c'est deja le cas
  
  // si b est dans updatelist il faut l'enlever
  if (b->omodes.IS_UPDATING) removeUpdateRequests(reinterpret_cast<Box*>(b));
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
  if (Application::config.postpone_delete) {
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
  if (Application::config.is_using_gl) is_processing_layout_update_requests = true;
  
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

  if (Application::config.is_using_gl) {  // refresh des windows modifiees !!!A METTRE DANS Display
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
  
  if (Application::config.is_using_gl) { 
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
  for (unsigned int k = 0; k < impl_->displist.size(); ++k) {
    if (impl_->displist[k] == d) {
      // va tout decaler si on enleve de la liste !
      impl_->displist[k] = null;
      break;
    }
  }
}

void Application::deleteNotify(View* deleted_view) {
  FlowList& flist = impl_->flowlist; 
  for (unsigned int k = 0; k < flist.size(); k++) {
    if (flist[k]) flist[k]->deleteNotify(deleted_view);
  }
}

void Application::deleteNotify(Element* deleted_group) {
  FlowList& flist = impl_->flowlist; 
  for (unsigned int k = 0; k < flist.size(); k++) {
    if (flist[k]) flist[k]->deleteNotify(deleted_group);
  }
}


const DisplayList& Application::getDispList() {
  return impl_->displist;
}

Display* Application::getDisp() {return impl_->disp;}

// NB: un acces direct par [] serait possible
Display* Application::getDisp(int _id)  {
  for (unsigned int k = 0; k < impl_->displist.size(); ++k) {
    if (impl_->displist[k]->getID() == _id) return impl_->displist[k];
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
  return (config.tele_pointers && impl_->displist.size() > 1);
}


const FlowList& Application::getFlowList() {
  return impl_->flowlist;
}

// NB: un acces direct par [] serait possible
EventFlow* Application::getFlow(int _id)  {
  for (unsigned int k = 0; k < impl_->flowlist.size(); ++k) {
    if (impl_->flowlist[k]->getID() == _id) return impl_->flowlist[k];
  }
  return null;
}

Selection* Application::getSelection(int _id) {
  EventFlow* fl = getFlow(_id);
  if (!fl) return null;
  else return fl->getSelection();
}


Menu* Application::getOpenedMenu() {
  EventFlow* fl = impl_->disp->obtainChannelFlow(0);  // DEFAULT IFLOW : A REVOIR
  return fl ? fl->menu_man.getDeepestMenu() : null; 
}

const String& Application::getImaPath() {
  return impl_->imapath;
}

void Application::setImaPath(const String& value) {
  impl_->imapath = value;
  String& path = impl_->imapath;
  
  if (path[0] == '~' && (path[1] == '/' || path[1] == 0)) {
    //NB: ne PAS faire de free() sur un getenv!
    static char* home = ::getenv("HOME");
    if (home) path.replace(0, 1, home);   // virer ~ mais pas /
  }
  
  // ajouter / final
  if (path[-1] != '/') path.append('/'); 
}


void Application::setMotionLag(unsigned long app_lag, unsigned long nat_lag) {
  impl_->app_motion_lag = app_lag;
  impl_->nat_motion_lag = nat_lag;
}

unsigned long Application::getTime() {return UTimerImpl::getTime();}

void Application::postpone(UCall& c) {  // pas tout a fait correct si mthreads!
  addTimeout(0, 1, c);
  //if (pthread_self() != main_thread) {
  // Message::send(*impl_->disp->getMainFrame(), "wakeup");    !!!@@@@!!!!
  // }
}

void Application::addTimeout(unsigned long _delay, int _ntimes, UCall& c) {
  Timer* t = new Timer(_delay, _ntimes, true); // true => auto_delete
  t->onTimeout(c);
  t->start();
}


MessagePortMap* Application::getMessagePortMap() {return impl_->messmap;}

MessagePort* Application::findMessagePort(const String& name) {
  if (impl_->messmap) return impl_->messmap->findMessagePort(name);
  else return null;
}

MessagePort& Application::getMessagePort(const String& name) {
  if (!impl_->messmap) impl_->messmap = new MessagePortMap();
  return impl_->messmap->getMessagePort(name);
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


void Application::raiseError(int errnum, const Object* obj, const char* funcname, 
                        const char* format, ...){
  va_list ap;
  va_start(ap, format);
  getErrorHandler().raiseError(errnum, obj, funcname, format, ap);
  va_end(ap);
}

void Application::raiseError(int errnum, const Object* obj, const char* funcname, 
                        const char* format, va_list ap){
  getErrorHandler().raiseError(errnum, obj, funcname, format, ap);
}

ErrorHandler& Application::getErrorHandler() {
  // this variable should be null even if this AppImpl constr was not called
  if (!appli_impl.error_handler) {
    appli_impl.error_handler = std::make_unique<ErrorHandler>((appli_impl.app_name ? *appli_impl.app_name : ""), &std::cerr);
  }
  
  if (appli_impl.error_handler->label().empty() && appli_impl.app_name)
    appli_impl.error_handler->label() = *appli_impl.app_name;
  
  return *appli_impl.error_handler;
}

void Application::setErrorHandler(ErrorHandler& eh) {
  appli_impl.error_handler = std::unique_ptr<ErrorHandler>(&eh);
}

}
