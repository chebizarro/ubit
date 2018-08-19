/*
 *  str.cpp: active strings (can have callbacks)
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
#include <cstring>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ubit/udefs.hpp>
#include <ubit/core/on.h>
#include <ubit/core/file.h>
#include <ubit/ui/updatecontext.h>
#include <ubit/draw/graph.h>
#include <ubit/ui/fontmetrics.h>
#include <ubit/ui/view.h>
#include <ubit/core/string.h>
#include <ubit/ui/textedit.h>
#include <ubit/Selection.hpp>
#include <ubit/ui/window.h>
#include <ubit/core/application.h>
using namespace std;

namespace ubit {


//String String::none((char*)null, UMode::UCONST);
//String String::newline("\n", UMode::UCONST);
//String String::goBOL("\15", UMode::UCONST); // retour a la ligne si necessaire

const String& String::getNodeName() const {static String nn("#text"); return nn;}

String& String::onChange(UCall &c) {
  addChangeCall(c);
  return *this;
}

String::String(const char* chs, UConst m) : Data(m) {
  s = null; //!dont forget: checked by _set() for freeing memory!
  initImpl(chs, (chs ? strlen(chs) : 0));
}

String::String() {
  s = null; // !dont forget: checked by _set() for freeing memory!
  initImpl(null, 0);
}

String::String(const char* chs) {
  s = null; // !dont forget: checked by _set() for freeing memory!
  initImpl(chs, (chs ? strlen(chs) : 0));
}

String::String(const string& str) {
  s = null;
  initImpl(str.c_str(), str.length());  //pas optimal mais tant pis!
}

String::String(const String& str) {
  s = null;
  initImpl(str.s, str.len);
}

/*
String::String(const class UInteger& _n) {
  s = null;
  char _s[50] = "";
  sprintf(_s, "%d", _n.intValue()); // int!
  initImpl(_s, strlen(_s));
}

String::String(const class Float& _n) {
  s = null;
  char _s[50] = ""; 
  sprintf(_s, "%f", _n.doubleValue());  // double!
  initImpl(_s, strlen(_s));
}

String::String(const class Double& _n) {
  s = null;
  char _s[50] = "";
  sprintf(_s, "%f", _n.doubleValue());  // double!
  initImpl(_s, strlen(_s));
}
*/
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

String::~String() {
  destructs();     // necessaire car removingFrom specifique
  if (s) free(s);
  s = null; len = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void String::addingTo(Child& c, Element& parent) {
  Data::addingTo(c, parent);

  // dans ce mode les Str de n'importe quel object peuvent etre selectees
  if (Application::conf.generic_textsel) {
    parent.emodes.IS_TEXT_SELECTABLE = true;
  }
  
  // il y a un caret et il ne pointe sur rien -> le faire pointer sur cette String
  if (parent.emodes.IS_TEXT_EDITABLE) {
    TextEdit* ed = null;
    parent.attributes().findClass(ed);
    if (!ed) parent.children().findClass(ed);
    if (ed && !ed->getCaretStr()) ed->setCaretStr(this, 0);
  }
}

void String::removingFrom(Child& c, Element& parent) {
  // la Str est enlevee et un caret pointait dessus -> le faire pointer sur null
  if (parent.emodes.IS_TEXT_EDITABLE) {
    TextEdit* ed = null;
    parent.attributes().findClass(ed);
    if (!ed) parent.children().findClass(ed);
    if (ed && ed->getCaretStr() == this) ed->setCaretStr(null, 0);
  }
  Data::removingFrom(c, parent);
}

// NB: duplicates the char string (!ATT: length MUST be the EXACT length)

void String::initImpl(const char *_s, int _len) {
  if (!_s) syncVals(null, 0);
  else {
    s = (char*)malloc((_len + 1) * sizeof(char));
    if (s) {strncpy(s, _s, _len); s[_len] = 0; syncVals(s, _len);}
    else {
      syncVals(null, 0);
      Application::fatalError("String::initImpl","No more memory; String object %p",this);
    }
  }
}

void String::setImpl(const char *_s, int _len) {
  if (checkConst()) return;
  if (s) free(s);
  
  if (!_s) syncVals(null, 0);
  else {
    s = (char*)malloc((_len + 1) * sizeof(char));
    if (s) {strncpy(s, _s, _len); s[_len] = 0; syncVals(s, _len);}
    else {
      syncVals(null, 0);
      Application::fatalError("String::setImpl","No more memory; String object %p",this);
    }
  }
  changed(true);
}

void String::setImplNoCopy(char *_s, int _len) {
  if (checkConst()) return;
  if (s) free(s);
  
  if (!_s) syncVals(null, 0);
  else {
    s = _s;
    s[_len] = 0; 
    syncVals(s, _len);
  }
  changed(true);
}


String& String::clear() {
  if (!s) return *this;
  setImpl((char*)null, 0);
  return *this;
}

String& String::operator=(const char* _s) {
  if (equals(_s)) return *this;
  setImpl(_s, (_s ? strlen(_s) : 0));
  return *this;
}

String& String::operator=(const String& _s) {
  if (equals(_s)) return *this;
  setImpl(_s.s, _s.len);
  return *this;
}

String& String::operator=(const string& _s) {
  if (equals(_s)) return *this;
  setImpl(_s.c_str(), _s.length());
  return *this;
}

/*
String& String::set(const UInteger& _n) {
  return setInt(_n.intValue());
}

String& String::set(const Float& _n) {
  return setFloat(_n.floatValue());
}

String& String::set(const Double& _n) {
  return setDouble(_n.doubleValue());
}
*/

String& String::setInt(int _n) {
  if (checkConst()) return *this;
  char _s[50] = ""; 
  sprintf(_s, "%d", _n);
  return operator=(_s); //cf autoupdate
}

String& String::setLong(long _n) {
  if (checkConst()) return *this;
  char _s[50] = ""; 
  sprintf(_s, "%ld", _n);
  return operator=(_s); //cf autoupdate
}

String& String::setFloat(float _n) {
  if (checkConst()) return *this;
  char _s[50] = ""; 
  sprintf(_s, "%f", _n);
  return operator=(_s); //cf autoupdate
}

String& String::setDouble(double _n) {
  if (checkConst()) return *this;
  char _s[50] = "";
  sprintf(_s, "%f", _n);
  return operator=(_s); //cf autoupdate
}

bool String::scanValue(float& val, String& unit) const {
  const char* u = null;
  bool valfound = scanValue(val, u);
  if (u && *u) unit = u; else unit.clear();
  return valfound;
}

bool String::scanValue(float& val, const char*& unit) const {
  val = 0.;
  if (!s || !*s) return false;
  const char* p = s; 
  // virer tout les caracteres speciaux jusqua ' ' inclus
  // attention cast necessaire sinon suppression des accentues
  while (*p && (unsigned char)*p <= ' ') p++;
  
  bool positive = true;
  if (*p == '-') {
    positive = false;
    ++p;
  }
  
  const char* pnum = p;
  double x = 0.;
  
  while (isdigit(*p)) {
    x = x * 10 + (*p - '0');
    p++;
  }
  
  if (*p=='.') {
    p++;
    double n = 10.;
    while (isdigit(*p)) {
      x += (*p - '0') / n;
      n *= 10;
      p++;
    }
  }
  
  val = positive ? x : -x;
  unit = p;
  return (p != pnum);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

ostream& operator<<(ostream& os, const String& s) {
  if (s.s) return (os << s.s);
  else return os;
}

istream& operator>>(istream& is, String& s) {
  char* c_s = null;
  is >> c_s;
  s.operator=(c_s);
  return is;
}


bool String::equals(const char* _s, bool ignore_case) const {
  if (s == _s) return true;
  else if (!_s) return isEmpty();
  else if (!s) return (!_s || !*_s); 
  else return (UCstr::compare(s, _s, ignore_case) == 0);
}

bool String::equals(const String& _s, bool ignore_case) const {
  return equals(_s.s, ignore_case);
}

bool String::equals(const string& _s, bool ignore_case) const {
  return equals(_s.c_str(), ignore_case);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int String::compare(const char* _s, bool ignore_case) const {
  if (s == _s) return 0;
  else if (!_s) return isEmpty() ? 0 : 1;
  else if (!s) return (!_s || !*_s) ? 0 : -1;
  else return UCstr::compare(s, _s ,ignore_case);
}

int String::compare(const String& _s, bool ignore_case) const {
  return compare(_s.s, ignore_case);
}

int String::compare(const string& _s, bool ignore_case) const {
  return compare(_s.c_str(), ignore_case);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int String::compareN(const char* _s, unsigned int n, bool ignore_case) const {
  if (s == _s) return 0;
  else if (!_s) return isEmpty() ? 0 : 1;
  else if (!s) return (!_s || !*_s) ? 0 : -1;
  else return UCstr::compareN(s, _s, n, ignore_case);
}

int String::compareN(const String& _s, unsigned int n, bool ignore_case) const {
  return compareN(_s.s, n, ignore_case);
}

int String::compareN(const string& _s, unsigned int n, bool ignore_case) const {
  return compareN(_s.c_str(), n, ignore_case);
}


int String::toInt() const {
  return s ? atoi(s) : 0;
}

long String::toLong() const {
  //return s ? strtol(s, null, 0) : 0;
  return s ? atol(s) : 0;
}

float String::toFloat() const {
  return float(s ? atof(s) : 0);
}

double String::toDouble() const {
  return s ? atof(s) : 0;  // == strtod(s, null) 
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

String String::toUpper() const {
  String s2 = s;
  if (!s2.s) return s2;
  for (char* p = s2.s; *p; p++) *p = toupper(*p);
  return s2;
}

String String::toLower() const {
  String s2 = s;
  if (!s2.s) return s2;
  for (char* p = s2.s; *p; p++) *p = tolower(*p);
  return s2;
}

void String::upper() {
  if (!s) return;
  for (char* p = s; *p; p++) *p = toupper(*p);
}

void String::lower() {
  if (!s) return;
  for (char* p = s; *p; p++) *p = tolower(*p);
}

void String::capitalize() {
  if (!s) return;
  if (!*s) return; else *s = toupper(*s);
  for (char* p = s+1; *p; p++) *p = tolower(*p);
}


bool String::checkFormat(int pos, int _c) {
  if (_c == '\0') return false;      //don't insert the NUL char!
  //else if (model) return model->checkFormat(pos, _c);
  else return true;
}

bool String::checkFormat(int pos, const char *_s) {
  if (_s == null) return false;      //don't insert the NULL string!
  //else if (model) return model->checkFormat(pos, _s);
  else return true;
}

void String::syncVals(char *news, int newlen) {
  s = news;
  len = newlen;
  /*
  if (model) model->set(this);
  void String::setStrModel(class UEditable *m) {
  model = m;
  chainage et cas null et enlever existant;
  */
}

void String::update() {
  _parents.updateAutoParents(Update::layoutAndPaint);
}

// calls UOn::change callbacks of this object when its value is changed
// then UOn::changeData AND UOn::changeStr callbacks of its parents
//
void String::changed(bool upd) {
  if (upd && !omodes.DONT_AUTO_UPDATE) update();

  if (!omodes.IGNORE_CHANGE_CALLBACKS) {
    if (_abegin() != _aend()) {
      Event e(UOn::change, this);  //UNodeEvent
      fire(e);
    }
    // ensuite on lance les callbacks des parents
    // container callbacks
    fireParents(UOn::dataChange, this);  // utile ?
    fireParents(UOn::strChange, this);
  }
}


char String::operator[](int pos) const {
  if (!s || pos >= len) return 0;
  if (pos < 0) pos = len-1; // last char
  return s[pos];
}

char String::at(int pos) const {
  if (!s || pos >= len) return 0;
  if (pos < 0) pos = len-1; // last char
  return s[pos];
}
 
char String::setCharAt(int pos, char newchar, bool upd) {
  if (checkConst()) return 0;
  if (!s || pos >= len) return 0;
  if (!checkFormat(pos, newchar)) return false; // !!

  if (pos < 0) pos = len-1; // last char
  s[pos] = newchar;

  syncVals(s, len);
  changed(upd);
  return true; 
}


String operator&(const String& s1, const String& s2) {
  String str = s1; str.append(s2); return str;
}

String operator&(const String& s1, const char* s2) {
  String str = s1; str.append(s2); return str;
}

String operator&(const char* s1, const String& s2) {
  String str = s1; str.append(s2); return str;
}

String operator&(const String& s1, const string& s2) {
  String str = s1; str.append(s2); return str;
}

String operator&(const string& s1, const String& s2) {
  String str = s1; str.append(s2); return str;
}

String operator&(const String& _s, int _v) {
  String str(_s); str.append(_v); return str;
}

String operator&(int _v, const String& _s) {
  String str; str.setInt(_v); str.append(_s); return str;
}

String operator&(const String& _s, double _v) {
  String str(_s); str.append(_v); return str;
}

String operator&(double _v, const String& _s) {
  String str; str.setDouble(_v); str.append(_s); return str;
}

/*
String operator&(const String& _s, const UInteger& _v) {
  String str(_s); str.append(_v); return str;
}
String operator&(const UInteger& _v, const String& _s) {
  String str; str = _v; str.append(_s); return str;
}
String operator&(const String& _s, const Double& _v) {
  String str(_s); str.append(_v); return str;
}
String operator&(const Double& _v, const String& _s) {
  String str; str = _v; str.append(_s); return str;
}
*/

void String::append(const char c) {
  insertImpl(-1, c, true);
}

void String::append(const char *s2) {
  insertImpl(-1, s2, 0, npos, true);
}

void String::append(const char *s2, unsigned int nbchars) {
  insertImpl(-1, s2, 0, nbchars, true);
}

void String::append(const String &s2) {
  insertImpl(-1, s2.s, 0, npos, true);
}

void String::append(const String &s2, unsigned int nbchars) {
  insertImpl(-1, s2.s, 0, nbchars, true);
}

void String::append(const string& s2) {
  insertImpl(-1, s2.c_str(), 0, npos, true);
}

void String::append(const string& s2, unsigned int nbchars) {
  insertImpl(-1, s2.c_str(), 0, nbchars, true);
}

void String::append(int v2) {
  char _s2[50] = {0}; 
  sprintf(_s2, "%d", v2);
  insertImpl(-1, _s2, 0, npos, true);
}

void String::append(double v2) {
  char _s2[50] = {0}; 
  sprintf(_s2, "%f", v2);
  insertImpl(-1, _s2, 0, npos, true);
}

/*
void String::append(const UInteger& v2) {
  append(v2.intValue());
}

void String::append(const Double& v2) {
  append(v2.doubleValue());
}
*/

bool String::insert(int to_pos, char c) {
  return insertImpl(to_pos, c, true);
}

bool String::insert(int to_pos, const String &s2, int from_pos,
                  unsigned int from_nbchars) {
  return insertImpl(to_pos, s2.s, from_pos, from_nbchars, true);
}

bool String::insert(int to_pos, const char *s2, int from_pos,
                  unsigned int from_nbchars) {
  return insertImpl(to_pos, s2, from_pos, from_nbchars, true);
}

bool String::insert(int to_pos, const std::string& s2, int from_pos,
                  unsigned int from_nbchars) {
  return insertImpl(to_pos, s2.c_str(), from_pos, from_nbchars, true);
}

// from_nbchars < 0 means whole string s2
// return : true if changed

bool String::insertImpl(int to_pos, const char *s2, int from_pos,
                      unsigned int from_nbchars, bool upd) {
  if (checkConst()) return false;
  if (!checkFormat(to_pos, s2) || to_pos > len) return false;

  if (to_pos < 0) to_pos = len; // append

  int len_s2 = strlen(s2);
  if (from_pos < 0) from_pos = len_s2 -1;
  
  int nbc = (from_nbchars == npos) ? len_s2 : from_nbchars;
  if (from_pos + nbc > len_s2) nbc = len_s2 - from_pos;
  if (nbc <= 0) return false;

  char* news = (char*) malloc(len + nbc + 1); // 0 final
  if (!news) {
    Application::fatalError("String::insertImpl","No more memory; String object %p",this);
    return false;		// str et strLen inchanges !
  }

  if (!s) strncpy(news, s2+from_pos, nbc);
  else {
    if (to_pos > 0) strncpy(news, s, to_pos);
    strncpy(news+to_pos, s2+from_pos, nbc);
    strcpy(news+to_pos+nbc, s+to_pos);
    free(s);
  }
  news[len+nbc] = 0;  // 0 final

  syncVals(news, len+nbc);
  changed(upd);
  return true;
}


bool String::insertImpl(int pos, char c, bool upd) {
  if (checkConst()) return false;
  if (!checkFormat(pos, c)) return false; //!!
  if (pos < 0 || pos > len) pos = len; // append

  char *news = (char*) malloc(len + 1 + 1); // 0 final
  if (!news) {
    Application::fatalError("String::insertImpl","No more memory; String object %p",this);
    return false;		// str et strLen inchanges !
  }

  if (!s) {
    news[0] = c;
    news[1] = '\0';
  }
  else {
    if (pos>0) strncpy(news, s, pos);
    news[pos] = c;
    strcpy(news+pos+1, s+pos);
    free(s);
  }

  syncVals(news, len+1);
  changed(upd);
  return true;
}


void String::remove(int pos, unsigned int nbchars) {
  replaceImpl(pos, nbchars, (char*)null, true);
}

void String::replace(int pos, unsigned int nbchars, const String& s2) {
  replaceImpl(pos, nbchars, s2.s, true);
}

void String::replace(int pos, unsigned int nbchars, const char *s2) {
  replaceImpl(pos, nbchars, s2, true);
}

void String::replace(int pos, unsigned int nbchars, const std::string& s2) {
  replaceImpl(pos, nbchars, s2.c_str(), true);
}


bool String::replaceImpl(int pos, unsigned int nbchars, const char *str, bool upd) {
  if (checkConst()) return false;
  if (pos < 0) pos = len-1;  // last char (new 26jt)

  int nbc = (nbchars == npos) ? len : nbchars;
  
  // revient a un insert() ou un append() dans ce cas
  if (pos >= len) return insertImpl(pos, str, 0, npos, upd);

  // ne pas depasser taille actuelle de str
  if (pos + nbc > len) nbc = len - pos;

  int nadd = str ? strlen(str) : 0;
  int newlen = len - nbc + nadd;

  if (newlen <= 0) {	  // theoriquement jamais < 0 mais == 0
    syncVals(null, 0);
  }
  
  else {
    char* news = (char*)malloc(newlen + 1); // 0 final
    if (!news) {
      Application::fatalError("String::replaceImpl","No more memory; String object %p",this);
      return false;		  // str et strLen inchanges !
    }

    // cas (!s ou !*s)  ==>  equivalent a remove()
    if (nadd==0) {	
      if (!s) return false;	  // securite: deja teste par: newlen <= 0
      if (pos>0) strncpy(news, s, pos);
      strcpy(news+pos, s+pos+nbc);
      free(s); s = null;
    }

    // il y a qq chose a ajouter
    else {
      if (!checkFormat(pos, str)) return false; //!!

      if (!s) strcpy(news, str);
      else {
	if (pos>0) strncpy(news, s, pos);
	strncpy(news+pos, str, nadd);
	strcpy(news+pos+nadd, s+pos+nbc);
	free(s); s = null;
      }
    }

    syncVals(news, newlen);
  }

  changed(upd);
  return true;
}


bool String::toString(string& str, int from_pos, unsigned int nbchars) const {
  if (!s || from_pos >= len) // hors bornes
    return false;
  else {
    if (from_pos < 0) from_pos = len-1;
    int nbc = (nbchars == npos) ? len : nbchars;
  
    // ne pas depasser taille actuelle de str
    if (from_pos + nbc > len) nbc = len - from_pos;
    if (nbc <= 0) return false;
    else {
      str.append(s + from_pos, nbc);
      return true;
    }
  }
}

std::string String::toString(int pos, unsigned int nbchars) const {
  std::string substr;
  toString(substr, pos, nbchars);
  return substr;
}

String String::substring(int pos, unsigned int nbchars) const {
  String substr;
  substr.insert(0, *this, pos, nbchars);
  return substr;
}


void String::trim(bool strip_beginning, bool strip_end) {
  if (checkConst()) return;
  if (!s || !*s) return;

  char* p1 = s;
  if (strip_beginning) {	// enlever les \n \t et spaces au debut
    // virer tout les caracteres speciaux jusuq'a ' ' inclus
    // attention cast necessaire sinon suppression des accentues
    while (*p1 && (unsigned char)*p1 <= ' ') p1++;
    if (!*p1) {
      free(s);
      syncVals(null, 0);
      return;
    }
  }

  char* p2 = s + len - 1;
  if (strip_end) {		// enlever les \n \t et spaces a la fin
    //while (p2 >= p1 && (*p2 == ' ' || *p2 == '\n' || *p2 == '\t')) p2--;
    while (p2 >= p1 && (unsigned char)*p2 <= ' ') p2--;
    *(p2+1) = 0;
  }

  char* s2 = UCstr::dup(p1);
  free(s);
  syncVals(s2, p2 - p1 + 1);
}


int String::tokenize(std::vector<String*>& tokens, const String& separators,
		   bool trim_beginning, bool trim_end) const
{
  if (empty()) return 0;
  char* p1 = s;
  unsigned int ix = 0;

  while (p1) {
    String* t;
    // on recupere les tokens existants si c'est le cas
    if (ix < tokens.size()) {
      t = tokens[ix];
      if (t) t->clear(); 
      else {t = new String(); tokens[ix] = t;}
    }
    else {
      t = new String(); tokens.push_back(t);
    }
    ix++;

    char* p2 = ::strpbrk(p1, separators.c_str());
    if (p2) t->append(p1, p2-p1);
    else t->append(p1);
    
    t->trim(trim_beginning, trim_end);
    if (t->empty()) ix--;
    
    if (p2) p1 = p2 + 1;
    else break;
  }
  return ix;
}


String String::split(int pos, bool delete_char_at_pos) {
  //if (checkConst()) return String::none;
  if (checkConst()) return "";
  String res;

  if (!s || pos<0 || pos>=len) return res;

  if (delete_char_at_pos) res = s+1+pos; else res = s+pos;

  if (pos == 0) {
    free(s);
    syncVals(null, 0);
  }
  else {
    len = pos;
    s = (char*)realloc(s, sizeof(char)*(pos+1));
    s[pos] = '\0';
    syncVals(s, len);
  }

  changed(true);
  return res;
}


int String::find(char c, int pos) const {
  if (empty() || pos >= len) return -1;
  char* p = ::strchr(s + pos, c);
  return (p ? p-s : -1);
}

int String::find(const char* s2, int pos) const {
  if (empty() || !s2 || pos >= len) return -1;
  char* p = ::strstr(s + pos, s2);
  return (p ? p-s : -1);
}

int String::find(const String& s2, int pos) const {
  return find(s2.c_str(), pos);
}

int String::find(const std::string& s2, int pos) const {
  return find(s2.c_str(), pos);
}

int String::rfind(char c) const {
  if (empty()) return -1;
  char* p = ::strrchr(s, c);
  return (p ? p-s : -1);
}


void String::getSize(UpdateContext& ctx, Dimension& dim) const {
  // NB: conserver une hauteur stable etfaire en sorte que la surface ne soit 
  // jamais de taille nulle (sinon on ne pourra plus la selecionner en cliquant)  
  FontMetrics fm(ctx);
  dim.width = (s && *s) ? fm.getWidth(s, len) : 1;   // 1 pour afficher le caret
  dim.height = fm.getHeight();
}

// prototype for warped text (UFlowview)
void String::getSize(UpdateContext& ctx, Dimension& dim, float available_width,
                   int offset, int& str_sublen, int& change_line) const {
  FontMetrics fm(ctx);
  if (s && *s) {
    fm.getSubTextSize(s + offset, len - offset, dim, available_width, 
                      str_sublen, change_line);
    // sinon il n'y aura pas de place pour afficher le caret en fin de ligne
    // *w += 1; ???
  }
  else { // voir NOTES ci-dessus
    dim.width = 1;
    dim.height = fm.getHeight();
    str_sublen = 0;
  }
}


void String::paint(Graph& g, UpdateContext& ctx, const Rectangle& r) const {
  paint(g, ctx, r, 0, len);
}

void String::paint(Graph& g, UpdateContext& ctx, const Rectangle& r,
                 int offset, int cellen) const {
  // !! don't confuse cellen and len!
  if (!s || !*s) return;
  
  Selection* ts = g.getDisp() ? g.getDisp()->getChannelSelection(0) : null;

  //ce qui serait cool c'est d'economiser les setColor et setFont!!!
  g.setFont(ctx.fontdesc);
  if (ctx.color) g.setColor(*ctx.color);

  // cas ou il n'y a pas de TextSel, ou l'objet pointe est null
  // ou ce n'est pas le parent de str 
  // -> affichage de base sans selection
  
  //if ((bmodes & UMode::IN_SELECTION) == 0
  if (!omodes.IS_IN_SELECTION
      || !ts || !ts->in_obj
      // permettre l'affichage des objets inclus dans ts.in_obj
      || ts->fromLink == Selection::null_link
      || ts->toLink == Selection::null_link
      || ts->fromLink == ts->endPressLink
      || ts->toLink == ts->endPressLink
      ) {
    g.drawString(s + offset, cellen, r.x, r.y);
  }

  // -> sinon affichage avec selection
  else {
    FontMetrics fm(ctx);

    int deb = 0, fin = cellen;
    float ww = 0;    // ex int
    // IL FAUDRAIT tester LEs LIENs (pas les String)
    // sinon les strings partagees vont s'allumer a plusieurs endroits!!

    if (this == dynamic_cast<const String*>(*(ts->fromLink))) {
      deb = ts->fromPos - offset;
      if (deb < 0) deb = 0;  //inclus dans selection
      else if (deb >= cellen) deb = cellen;   //exclus (avant) selection
    }

    // pas de else: this peut etre a la fois fromLink et toLink
    // (c'est meme le cas le plus frequent!)

    if (this == dynamic_cast<const String*>(*(ts->toLink))) {
      fin = ts->toPos - offset;
      if (fin < 0) fin = 0;   //exclus (apres) selection
      else if (fin >= cellen) fin = cellen; //inclus dans selection
    }

    if (deb > 0) {
      g.drawString(s+offset, deb, r.x, r.y);
      ww = fm.getWidth(s+offset, deb);
    }
    
    if (fin > deb) {
      if (ts->pcolor) g.setColor(*ts->pcolor);
      if (ts->pbgcolor) g.setBackground(*ts->pbgcolor);
      if (ts->pfont) {
        FontDescription fd = ctx.fontdesc;
        fd.merge(*ts->pfont);
        fd.setScale(ctx.xyscale);
        g.setFont(fd);
        g.drawString(s+deb+offset, fin-deb, r.x + ww, r.y);
        ww += FontMetrics(fd,ctx.getDisp()).getWidth(s+deb+offset, fin-deb);
      }
      else {
        g.drawString(s+deb+offset, fin-deb, r.x + ww, r.y);
        ww += fm.getWidth(s+deb+offset, fin-deb);
      }
    }

    if (fin < cellen) {
      g.setFont(ctx.fontdesc);
      if (ctx.color) g.setColor(*ctx.color);
      g.drawString(s+fin+offset, cellen-fin, r.x + ww, r.y);
    }
  }
} 


String String::basename(bool with_suffix) const {
  String res;
  if (len == 0) return res;
  
  char* dir_name, *base_name, *suffix;
  int dir_len, base_len, suffix_len;
  UCstr::parsePath(s, len, dir_name, dir_len, base_name, base_len, suffix, suffix_len);
  
  if (with_suffix) res = base_name;
  else res.insert(0, base_name, 0, base_len);
  return res;
}


String String::dirname(bool with_slash) const {
  String res;
  if (len == 0) res = ".";
  else {
    char* dir_name, *base_name, *suffix; 
    int dir_len, base_len, suffix_len;
    UCstr::parsePath(s, len, dir_name, dir_len, base_name, base_len, suffix, suffix_len);
    
    if (dir_len == 0) res = ".";
    else res.insert(0, dir_name, 0, dir_len);
  }
  
  if (with_slash && res.at(-1) != '/') res.insert(-1, '/');
  return res;
}

String String::suffix() const {
  String res;
  if (len == 0) return res;
  
  char* dir_name, *base_name, *suffix;
  int dir_len, base_len, suffix_len;
  UCstr::parsePath(s, len, dir_name, dir_len, base_name, base_len, suffix, suffix_len);
  res.insert(0, suffix, 0, suffix_len);
  return res;
}

String String::expand() const {
  if (empty()) return "";
  String res = *this;
  
  if (s[0] == '~' && s[1] == '/') {
    //NB: ne PAS faire de free() sur un getenv!
    char* home = getenv("HOME");
    if (home) res.replace(0, 1, home); // virer ~mais pas /
  }
  // on pourrait aussi gerer les variables....
  return res;
}


int String::read(const String& filename) {
  if (checkConst()) return 0;
  String fname = filename.expand();
  if (fname.empty()) return UFilestat::CannotOpen;
  
  char* buffer = null;
  struct stat finfo;
  int fd = -1;
  int res = UFilestat::NotOpened;
  int ll = 0;
  
  if ((fd = ::open(fname.c_str(), O_RDONLY, 0)) == -1) {
    res = UFilestat::CannotOpen;
  }
  else if (::fstat(fd, &finfo) == -1
           || (finfo.st_mode & S_IFMT) != S_IFREG) {
    res = UFilestat::CannotOpen;
  }
  else if (!(buffer = (char*)::malloc((finfo.st_size + 1) * sizeof(char)))) {
    res = UFilestat::NoMemory;
  }
  else if (::read(fd, buffer, finfo.st_size) <= 0) {
    res = UFilestat::InvalidData;
    if (buffer) ::free(buffer);
    buffer = null;
  }
  else {
    // trouver le 1et \0 et s'arreter la pour eviter plantages avec
    // les executables, etc.
    for (ll = 0; ll < finfo.st_size; ll++)
      if (buffer[ll] == 0) break;
    
    // be sure it's NULL terminated (nb: on a fait malloc (size + 1))
    if (ll >= finfo.st_size) {
      ll = finfo.st_size;
      buffer[ll] = '\0';
    }
    
    res = UFilestat::Opened;
  }
  
  if (fd != -1) close(fd);
  setImplNoCopy(buffer, ll);
  return res;
}


int String::readLines(const String& filename, std::vector<String*>& lines,
                    bool strip_beginning, bool strip_end) {
  String fname = filename.expand();
  if (fname.empty()) return UFilestat::CannotOpen;
  
  ifstream in(fname.c_str());
  if (!in) return UFilestat::CannotOpen;
  
  string line;
  while ((std::getline(in, line))) {
    String* l = new String(line);
    l->trim(strip_beginning, strip_end);
    if (l->empty()) delete l; else lines.push_back(l);
  }
  
  return UFilestat::Opened;
}


int String::write(const String& filename) {
  String fname = filename.expand();
  if (fname.empty()) return UFilestat::CannotOpen;
  
  int fd = -1;
  int res = UFilestat::NotOpened;
  
  if ((fd = ::open(fname.c_str(), O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) == -1)
    res = UFilestat::CannotOpen;
  else if (::write(fd, s, len) <= 0)
    res = UFilestat::InvalidData;
  else
    res = UFilestat::Opened;
  
  if (fd != -1) close(fd);
  return res;
}

// useful functions 

char *UCstr::dup(const char *s1) {
  if (!s1) return null;
  else {
    char *p = (char*)::malloc((strlen(s1) + 1) * sizeof(char));
    if (p) ::strcpy(p, s1);
    return p;
  }
}

char *UCstr::dup(const char *s1, const char *s2) {
  if (!s1 || !*s1) return UCstr::dup(s2);
  else if (!s2 || !*s2) return UCstr::dup(s1);
  else {
    int l1 = strlen(s1);
    char *p = (char*)::malloc((l1 + strlen(s2) + 1) * sizeof(char));
    if (p) {
      ::strcpy(p, s1);
      ::strcpy(p+l1, s2);
    }
    return p;
  }
}

char* UCstr::dup(const char *s1, char sep, const char *s2) {
  if (!s1 || !*s1) return UCstr::dup(s2);
  else if (!s2 || !*s2) return UCstr::dup(s1);
  else {
    int l1 = strlen(s1);
    char *p = (char*)::malloc((l1 + strlen(s2) + 2) * sizeof(char));
    if (p) {
      ::strcpy(p, s1);
      p[l1] = sep;
      ::strcpy(p + l1 + 1, s2);
    }
    return p;
  }
}

bool UCstr::equals(const char* s1, const char* s2, bool ignore_case) {
  if (s1 == s2) return true;
  else return compare(s1, s2, ignore_case);
};

int UCstr::compare(const char* s1, const char* s2, bool ignore_case) {
  if (s1 == s2) return 0;
  register const unsigned char *p1 = (const unsigned char *) s1;
  register const unsigned char *p2 = (const unsigned char *) s2;
  unsigned char c1, c2;

  if (ignore_case) {
    do {
      c1 = isupper(*p1) ? tolower(*p1) : (*p1);
      c2 = isupper(*p2) ? tolower(*p2) : (*p2);
      if (c1 == '\0') break;
      ++p1; ++p2;
    } while (c1 == c2);
  }
  else {
    do {
      c1 = *p1;
      c2 = *p2;
      if (c1 == '\0') break;
      ++p1; ++p2;
    } while (c1 == c2);    
  }
  
  return c1 - c2;
}

int UCstr::compareN(const char* s1, const char* s2, unsigned int n, bool ignore_case) {
  if (s1 == s2 || n == 0) return 0;  
  register const unsigned char *p1 = (const unsigned char *) s1;
  register const unsigned char *p2 = (const unsigned char *) s2;
  unsigned char c1, c2;

  if (ignore_case) {
    do {
      c1 = isupper(*p1) ? tolower(*p1) : (*p1);
      c2 = isupper(*p2) ? tolower(*p2) : (*p2);
      if (--n == 0 || c1 == '\0') break;
      ++p1; ++p2;
    }
    while (c1 == c2);
  }
  else {
    do {
      c1 = *p1;
      c2 = *p2;
      if (--n == 0 || c1 == '\0') break;
      ++p1; ++p2;
    } while (c1 == c2);    
  }
  
  return c1 - c2;
}


//const char *UCstr::suffix(const char *path) {
//  return UCstr::suffix((char*) path);
//}

const char *UCstr::suffix(const char *path) {
  const char *p = path + ::strlen(path);
  //probleme: etre sur que le . est apres le dernier /
  while (p >= path) {
    if (*p == '.' || *p == '/') break;
    p--;
  }
  // be sure its a . and eliminate . and .xxxrc and ..
  if (p <= path || *p != '.') return null;
  else if (p == path+1 && *path == '.') return null;
  else return p+1;
}


void UCstr::parsePath(char* path, int len,
                      char*& dir_name, int& dir_len,
                      char*& base_name, int& base_len,
                      char*& suffix, int& suffix_len) {
  dir_name = null;
  dir_len = 0;
  base_name = null;
  base_len = 0;
  suffix = null;
  suffix_len = 0;
  
  if (len == 0 || !path || !*path) return;
  
  char *p = path + len;
  
  // chercher le /
  while (p > path) {
    if (*p == '/') break;
    p--;
  }
  
  if (*p == '/') {
    dir_name = path;
    if (p == path) dir_len = 1; // CP du "/"
    else dir_len = p - path;
    base_name = p+1;
    base_len = len - dir_len -1;
  }
  else {
    base_name = path;
    base_len = len;
  }
  
  if (*base_name == 0) {
    base_name = null;
    base_len = 0;
    return;
  }
  
  // chercher le . (be sure its a . and eliminate . and .xxxrc and ..)
  char *f = base_name;
  bool normal_char_found = false;
  while (*f != 0) {
    if (*f != '.') normal_char_found = true;
    else if (normal_char_found) break;
    f++;
  }
  
  if (*f == '.' && normal_char_found) {
    suffix = f+1;
    suffix_len = len - (f+1 - path);
    base_len -= (suffix_len + 1);
  }
}

}


