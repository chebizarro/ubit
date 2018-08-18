/*
 *  styleparser.cpp
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
#include <clocale>
#include <ctype.h>
#include <string.h>
#include <ubit/ustyleparser.hpp>
#include <ubit/core/string.h>
#include <ubit/core/application.h>
using namespace std;
namespace ubit {


void StyleParser::skipSpaces() {                // skip comments
  while (true) {
    const Char* begin = p;

    while(*p && (*p==' ' || *p=='\n' || *p=='\r' || *p=='\t')) p++;
  
    if (*p == '/' && *(p+1) == '*') {    // comment
      p += 2;
      while (*p) {
        if (*p == '*' && *(p+1) == '/') {p = p+2; break;}
        else p++;
      }
    }

    if (p == begin) return;
  } 
}


bool StyleParser::readName(String& name) {
  if (!isalpha(*p) && *p!='_') return false; // 1st char must be in (alpha _ :)
  const Char* begin = p;
  p++;

  while (isalnum(*p) || *p == '-' || *p == '_' || *p == '.') p++;
  if (!*p) return false;

  name.append(begin, p-begin);
  if (permissive) name.lower();
  return true;
}


bool StyleParser::readValue(String& value) {
  if (iscntrl(*p)) return false;
  const Char* begin = p;

  // !iscntrl() fait planter s'il y a un newline ou un tab
  while (*p && /*!iscntrl(*p) &&*/ *p != ';' && *p != '}') {
    if (*p == '/' && *(p+1) == '*') {    // comment
      if (p > begin) value.append(begin, p-begin);
      skipSpaces();
      begin = p;
    }
    else p++;
  }
  //if (*p!= 0 && iscntrl(*p)) return false;
  if (p > begin) value.append(begin, p-begin);
  return true;
}

// starts on the 1st char of the name

bool StyleParser::readNameValuePair(String& name, String& value) {
  const Char* begin = p;

  if (!readName(name)) {
    error("invalid CSS attribute name", begin);
    return false;
  }

  skipSpaces();

  if (*p == ':') p++;
  else {
    if (permissive) {
      value.append("");
      return true;
    }
    else {
      error("invalid CSS attribute '", name,
            "': no : sign after attribute name",begin);
      return false;
    }
  }

  skipSpaces();

  if (readValue(value)) return true;
  else {
    error("invalid CSS attribute '", name,"': incorrect value", begin);
    return false;
  }
}

// reads the properties of the style definition, ie. the {} part after the selector

bool StyleParser::readStyleProps(StyleMaker& xxx) {
  const Char* begin = p;
  if (*p == '{') p++;

  while (true) {
    skipSpaces();

    if (*p == ';') {
      p++;
      skipSpaces();
    }

    else if (!*p || *p == '}') {         // end of style def
      return true;
    }

    else if (isalpha(*p)) {
      String name, value;
      if (readNameValuePair(name, value)) 
        xxx.addProp(name, value);
    }

    else {
      unexpected("in CSS style def", begin-1);
      return false;
    }
  }
}


static void readBracketSelector(const char* n, String& aux) {
  const char* pn = n;
  char* out = (char*)malloc(strlen(n));
  char* po = out;
  
  while (*pn) {
    if (*pn == '=') {
      if (pn > n && (*(pn-1) == '~' || *(pn-1) == '|')) {  // not taken into account
        *(po-1) = *pn++;     // enlever;
      }
      else *po++ = *pn++;
    }
    else if (*pn == '"') pn++;     // enlever (ne pas copier);
    else *po++ = *pn++;
  }
  *po = 0;
  aux &= out;
  delete out;
}


static bool readSingleSelector(String& name) {
  const char* n = name.c_str();
  if (!n) return false;

  // virer le wilcard puisqu'il est equivalent a rien
  if (*n == '*') {name.remove(0,1); n = name.c_str();}
    
  String aux;
  const char* pn = n;
  while (*pn) {
    if (*pn == '.') {
      // convertir aaa.bbb en aaa[class=bbb] 
      aux.append(n, pn-n);
      aux.append("[class=");
      aux.append(pn+1);
      aux.append(']');
      name = aux;
      break;
    }
    else if (*pn == '#') {
      // convertir aaa#bbb en aaa[id=bbb]
      aux.append(n, pn-n);
      aux.append("[id=");
      aux.append(pn+1);
      aux.append(']');
      name = aux;
      break;
    }
    else if (*pn == '[') {
      aux.append(n, pn-n);
      readBracketSelector(pn, aux);      
      name = aux;
      break;
    }
    else pn++;
  }

  return true;
}

// reads the selector(s) of the style definition.

bool StyleParser::readStyleSelectors(StyleMaker& xxx) {
  const Char* begin = p;
  bool multiple_sel = false;

  String sel;

  while (*p && *p != '{') {
    if (*p == '/' || *(p+1) == '*') {    // comment
      if (p > begin) sel.append(begin, p-begin);
      skipSpaces();
      begin = p;
    }
    else if (*p == ',') {  // multiple selectors
      multiple_sel = true;
      p++;
    }
    else p++;
  }

  if (!*p) return false;

  sel.append(begin, p-begin);
  sel.trim();
  if (permissive) sel.lower();

  if (!multiple_sel) {
    readSingleSelector(sel);
    xxx.count = 1;
    *(xxx.selectors[0]) = sel;
  }
  else {
    xxx.count = sel.tokenize(xxx.selectors, ",");
    for (unsigned int k = 0; k < xxx.count; k++)
      readSingleSelector(*(xxx.selectors[k]));
  }
 
  return true;
}

// reads one complete style definition (= the selector and the value part).

bool StyleParser::readStyleDef(StyleMaker& xxx) {
  const Char* begin = p;

  xxx.count = 0;
  //xxx.sel.clear();
  xxx.begin();

  if (!readStyleSelectors(xxx)) {
    error("invalid style selector", begin);
    return false;
  }
    
  skipSpaces();
  
  if (*p == '{') p++;
  else {
    error("invalid style declaration '", *xxx.selectors[0],
          "': no { sign after style name", begin);
    xxx.end(false);
    return false;
  }

  skipSpaces();

  xxx.create();

  if (readStyleProps(xxx)) {
    xxx.end(true);
    return true;
  }
  else {
    xxx.end(false);
    error("invalid style declaration '", *xxx.selectors[0],
          "': incorrect value", begin);
    return false;
  }
}

// reads all style definitions included in the buffer.

int StyleParser::parseImpl(StyleMaker& xxx, const String& _buffer) {
  p = text_buffer = _buffer.c_str();
  if (!p || !*p) {
    p = text_buffer = null;
    return 0;
  }
  
  stat = 1;
  
  while (*p) {
    skipSpaces();
    if (!*p) break;
    
    if (readStyleDef(xxx)) p++;
    else {
      stat = 0;
      break;
    }
  }

  p = text_buffer = null;
  return stat;
}


StyleParser::StyleParser() :
  permissive(true),
  stat(0),
  text_buffer(null),
  p(null),
  perrhandler(Application::getErrorHandler()) {
}

// une fois pour toutes: 
// les destr DOIVENT etre definis dans les .ccp qunad il y a des unique_ptr<XXX> 
// pour eviter d'etre oblige d'inclure la def de XXX dans le .hpp

StyleParser::~StyleParser() {}


StyleParser::StyleMaker::StyleMaker() {
  // il faut TOUJOURS au moins un element sinon plantage !
  selectors.push_back(new String());
}

StyleParser::StyleMaker::~StyleMaker() {
  for (unsigned int k = 0; k < selectors.size(); k++)
    delete selectors[k];
}


void StyleParser::unexpected(const char* msg, const Char* line) {
  if (!*p) {
    error("premature end of file ", "", msg, line);
  }
  else if (iscntrl(*p)) {
    char s[] = "invalid character ' '";
    s[sizeof(s)-3] = *p;
    // NB: + avec un char deconne avec g++
    error(s, "", msg, line);
    p++;
  }
  else  {
    char s[] = "unexpected character ' '";
    s[sizeof(s)-3] = *p;
    error(s, "", msg, line);
    p++;
  }
}

void StyleParser::error(const char* msg, const Char* line) {
  error(msg, "", 0, line);
}

void StyleParser::error(const char* msg1, const String& name,
                      const char* msg2, const Char* line) {
  perrhandler->parserError(UError::CSS_ERROR, text_buffer, msg1, name, msg2, line);
}


}

