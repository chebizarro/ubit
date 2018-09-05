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

ErrorHandler::ErrorHandler(const String& _label, std::ostream* _fout) : 
plabel(std::make_unique<String>(ustr(_label))),
pbuffer(null),
fout(_fout) {
}

ErrorHandler::~ErrorHandler() {}

void ErrorHandler::setOutputStream(std::ostream* f) {
  fout = f;
}

void ErrorHandler::setOutputBuffer(String* s) {
  pbuffer = std::make_unique<String>(*s);
}


void ErrorHandler::warning(const char* fun, const char* format, ...) const {
  va_list ap;
  va_start(ap, format);
  raiseError(Error::WARNING, null, fun, format, ap);
  va_end(ap);
}

void ErrorHandler::error(const char* fun, const char* format, ...) const {
  va_list ap;
  va_start(ap, format);
  raiseError(Error::ERROR, null, fun, format, ap);
  va_end(ap);
}

void ErrorHandler::error(int errnum, const Object *obj, const char* fun, 
                          const char* format, ...) const {
  va_list ap;
  va_start(ap, format);
  raiseError(errnum, obj, fun, format, ap);
  va_end(ap);
}

void ErrorHandler::parserError(int errnum, const Char* tbuffer,
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

void ErrorHandler::raiseError(int errnum, const Object* obj, const char* funcname, 
                               const char* format, va_list ap) const {
  Error e(errnum, obj, funcname);
  formatMessage(e, format, ap);
  if (fout && *fout) printOnStream(e);
  if (pbuffer) printOnBuffer(e);
  if (e.errnum < 0) throw e;
}

void ErrorHandler::raiseError(int errnum, String* msg) const {
  Error e(errnum, null, null);
    if (msg) {
      strncpy(e.message, msg->c_str(), sizeof(e.message));
      e.message[sizeof(e.message)-1] = 0;
  }
  if (fout && *fout) printOnStream(e);
  if (pbuffer) printOnBuffer(e);
  if (e.errnum < 0) throw e;
}


void ErrorHandler::formatMessage(Error& e, const char* format, va_list ap) const {
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

const char* ErrorHandler::getErrorName(int errnum) const {
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

void ErrorHandler::printOnStream(const Error& e) const {
  if (!fout) {
    std::cerr <<  "ErrorHandler::printOnStream: can't print error because output stream is null! " << std::endl;
  }
  else {
    *fout << e.what() << std::endl << std::endl;
  } 
}

void ErrorHandler::printOnBuffer(const Error& e) const {
  if (!pbuffer) {
    std::cerr <<  "ErrorHandler::printOnBuffer: can't print error because output buffer is null! " << std::endl;
  }
  else {
    pbuffer->append(e.what());
  }
}
}
