/*
 *  source.cpp: file and socket data management
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
#include <iostream>
#include <unistd.h>       // darwin
#include <sys/stat.h>
#include <ubit/core/application.h>
#include <ubit/core/appimpl.h>
#include <ubit/uon.hpp>
#include <ubit/net/source.h>
#include <ubit/ucall.hpp>
using namespace std;
namespace ubit {


Source::Source(int _source) : is_opened(false), source(_source) {
# ifdef UBIT_WITH_GDK
  gid = 0;
# endif 
  if (source > -1) open(_source);
}

Source::~Source() {
# ifdef UBIT_WITH_GDK
  if (is_opened) gdk_input_remove(gid);
  gid = 0;
# endif 
  if (is_opened) fireClose();
  is_opened = false;
}

#if UBIT_WITH_GDK
static void inputCB(gpointer input, gint source, GdkInputCondition) {
  Source* i = (Source*)input;
  i->fireInput();
}
# endif

void Source::open(int _source) {
  source = _source;
  is_opened = true;
  Element* sources = Application::impl.sources;
  ChildIter i = sources->children().find(*this); 
  // ne pas mettre 2 fois dans la liste!
# if UBIT_WITH_GDK
  if (i != sources->cend()) g_source_remove(gid);
  gid = gdk_input_add(source, GDK_INPUT_READ, inputCB, (gpointer)this);
# endif
  if (i == sources->cend()) sources->add(this);
}

void Source::close() {
# if UBIT_WITH_GDK
  gdk_input_remove(gid);
  gid = 0;
# endif  
  Element* sources = Application::impl.sources;
  sources->remove(*this, false);
  if (is_opened) fireClose();
  is_opened = false;
}

void Source::onAction(UCall& c) {
  Child* l = new Child(&c);
  l->setCond(UOn::action);
  _addAttr(*l);
}

void Source::onClose(UCall& c) {
  Child* l = new Child(&c);
  l->setCond(UOn::close);
  _addAttr(*l);
}

void Source::fireInput() {
  if (isDestructed()) return;  // securite
  Event e(UOn::action, this);  //UNodeEvent
  //e.setWhen(Application::getTime());
  fire(e);
}

void Source::fireClose() {
  if (isDestructed()) return;  // securite
  Event e(UOn::close, this);  //UNodeEvent
  //e.setWhen(Application::getTime());
  fire(e);
}


void AppImpl::resetSources(Element* sources, fd_set& read_set, int& maxfd) {
  if (!sources) return;
  for (ChildIter c = sources->cbegin(); c != sources->cend(); ++c) {
    Source* i = static_cast<Source*>(*c);
    int fd = 0;
    if (i && ((fd = i->source) >= 0)) {
      FD_SET(fd, &read_set);
      maxfd = std::max(maxfd, fd);
    }
  }
}

void AppImpl::cleanSources(Element* sources) {
  if (!sources) return;
  struct stat statbuf;
  for (ChildIter c = sources->cbegin(); c != sources->cend(); ) {
    int fd = 0;
    Source* i = static_cast<Source*>(*c);
    ++c;  // possible destruction => faire ++ avant
    if (!i) continue;
    if (!i->isOpened() || (fd=i->getSource())<=0 || fstat(fd, &statbuf) < 0) {
      i->is_opened = false;
      i->close();
    }
  }
}

void AppImpl::fireSources(Element* sources, fd_set& read_set) {
  if (!sources) return;
  for (ChildIter c = sources->cbegin(); c != sources->cend(); ) {
    Source* i = static_cast<Source*>(*c);
    ++c;  // stop() peut detruire le Source => faire ++ avant
    
    if (!i || i->isDestructed()) {
      // cas malheureux ou fireInput() a detruit celui d'apres
      // => sortir sinon plantage
      return;
    }
    
    int fd = 0;
    if (i->is_opened && ((fd=i->source) >= 0) && FD_ISSET(fd,&read_set)) {
      i->fireInput();
    }
  }
}


}

