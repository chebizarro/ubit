/*
 *  edit.cpp: Text Edition Attribute
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
#include <ctype.h>
#include <iostream>
#include <ubit/ustr.hpp>
#include <ubit/ucall.hpp>
#include <ubit/uon.hpp>
#include <ubit/ubox.hpp>
#include <ubit/uupdatecontext.hpp>
#include <ubit/ugraph.hpp>
#include <ubit/uedit.hpp>
#include <ubit/ufontmetrics.hpp>
#include <ubit/ucursor.hpp>
#include <ubit/uscrollpane.hpp>
#include <ubit/uscrollbar.hpp>
#include <ubit/Selection.hpp>
#include <ubit/ukey.hpp>
#include <ubit/uappli.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT


UEdit::UEdit() :
  calls(ucall(this, &UEdit::inputCB)),
  calls2(ucall(this, &UEdit::callbacks2)),
  caret_color(Color::red),
  caret_str(null), caret_pos(0),
  is_editable(true), is_visible(true), repainted(false) {
}

UEdit::~UEdit() {
  caret_str = null;
  destructs();
}


bool UEdit::isEditable() const {return is_editable;}
bool UEdit::isCaretVisible() const {return is_visible;}
Color* UEdit::getCaretColor() const {return caret_color;}

UEdit& UEdit::setEditable(bool b) {is_editable = b; return *this;}
UEdit& UEdit::setCaretVisible(bool b)  {is_visible = b; return *this;}
UEdit& UEdit::setCaretColor(Color* c) {caret_color = c; return *this;}


void UEdit::addingTo(Child& selflink, Element& parent) {
  Attribute::addingTo(selflink, parent);
  //if (getParentCount() > 1) {pourquoi pas ?
  //  Application::warning("UEdit::addingTo","UEdit objects can not have several parents; last parent: ",parent->cname());
   //}
  if (parent.emodes.IS_TEXT_EDITABLE) {
    const String* classname = &parent.getClassName();
    warning("UEdit::addingTo","This UEdit attribute has a parent (%s %p) that contains another UEdit attribute (an widget should not contain multiple UEdit objects", classname, &parent);
  }

  parent.emodes.IS_TEXT_EDITABLE = true;
  parent.emodes.IS_TEXT_SELECTABLE = true;
  //parent.emodes.CLOSE_MENU_MODE = parent.emodes.CANNOT_CLOSE_MENU;
  parent.disableMenuClosing(true);
  
  // ajouter handlers au box parent dans cache
  parent.addAttr(UOn::mpress / *calls);
  parent.addAttr(UOn::mrelease / *calls);
  parent.addAttr(UOn::kpress / *calls);
  parent.addAttr(UCursor::text);
  parent.addAttr(UOn::click / *calls2);
}

//NB: removingFrom() requires a destructor to be defined
//
void UEdit::removingFrom(Child& c, Element& parent) {
  parent.emodes.IS_TEXT_EDITABLE = false;
  parent.emodes.IS_TEXT_SELECTABLE = false;
  //parent.emodes.CLOSE_MENU_MODE = 0;
  parent.disableMenuClosing(false);

  // enlever les callbacks: !att: y'a 3 occurences de calls dans les attrs
  parent.removeAttr(*calls);
  parent.removeAttr(*calls);
  parent.removeAttr(*calls);
  parent.removeAttr(UCursor::text);
  parent.removeAttr(*calls2);
  Attribute::removingFrom(c, parent);
}


void UEdit::putProp(UpdateContext* ctx, Element& state) {
  ctx->edit = this;
}

// met a jour la string concernee laquelle mettra ainsi a jour ses parents
//  => ceci permet de synchroniser l'affichage de la String dans tous les parents
// Remarque:
// par contre les moveCaret sont locaux a chaque uedit (le caret peut avoir
// une position differente dans chaque parent)

void UEdit::update() {
  // NB: _parents.updateAutoParents() ne suffit pas car la string peut apparaitre
  // dans d'autres widgets, non editables
  if (caret_str) caret_str->updateAutoParents(Update::layoutAndPaint);
}

/* ==================================================== [Elc:] ======= */
/*
static void scroll(View* view, int dir) {
  UScrollpane* spane = null;
  view = view->getParentView();
  while (view) {
    Box* par = view->getBox();
    if (par && (spane = dynamic_cast<UScrollpane*>(par)))
      break;
    view = view->getParentView();
  }
  if (spane && spane->getVScrollbar())
    spane->getVScrollbar()->pressScrollButton(dir);
}
*/
/*
static int trueLength(const String *s) {
  cerr << s << endl;
  const char* c = s->c_str();
  if (!c) return 0;
  int ll = 0;
  for (const char *p = c; p != null; p++) {
    if (*p != '\n' && *p != '\r') ll++;
  }
  return ll;
}
*/

long UEdit::getCaretPos() const {
  Element* par = getParent(0);
  return par ? getCaretPos(*par) : 0;
}

long UEdit::getCaretPos(Element& par) const {
  //if (!par) return 0;
  String* s = null;
  long k = 0;
  bool found = false;

  for (ChildIter i = par.cbegin(); i != par.cend(); ++i) {
    if ((s = dynamic_cast<String*>(*i))) {
      if (s != caret_str) k += s->length(); //trueLength(s);
      else {k += caret_pos; found = true; break;}
    }
  }
  return (found ? k : 0);
}

UEdit& UEdit::setCaretPos(long pos) {
  Element* par = getParent(0);
  return par ? setCaretPos(pos, *par) : *this;
}

UEdit& UEdit::setCaretPos(long pos, Element& par) {
  //if (!par) return *this;
  String* s = null;
  long k = 0, found_pos = 0;
  String *found_s = null, *last_s = null;

  for (ChildIter i = par.cbegin(); i != par.cend(); ++i) {
    if ((s = dynamic_cast<String*>(*i))) {
      last_s = s;
      if (k + s->length() < pos) k += s->length();
      else {found_s = s; found_pos = pos -k; break;}
    }
  }

  if (found_s) setCaretStr(found_s, found_pos);
  else setCaretStr(last_s, -1);
  return *this;
}

/* ==================================================== [Elc:] ======= */

String* UEdit::getCaretStr(int& pos) const {
  pos = caret_pos;
  return caret_str;
}

String* UEdit::getCaretStr() const {
  return caret_str;
}

UEdit& UEdit::setCaretStr(String* _str) {
  setCaretStr(_str, 0, true, null);
  return *this;
}

UEdit& UEdit::setCaretStr(String* _str, int _pos) {
  setCaretStr(_str, _pos, true, null);
  return *this;
}

// NB1: la Str peut etre partagee a la fois par des Box editables et non editables
// NB2: strpos = pos in newstr / -1 means end of string

void UEdit::setCaretStr(String* newstr, int newpos, bool _update, View* view) {
  caret_str = newstr;
  if (!caret_str) caret_pos = 0;
  else {
    if (newpos >= 0 && newpos < caret_str->length()) {
      caret_pos = newpos;
    }
    else {
      caret_pos = caret_str->length();       // set to end-of-str
      
      // si newpos == -2 on place le caret avant le dernier char
      if (newpos < 0 && caret_str->c_str() && caret_pos > 0) {
        if (newpos == -2) caret_pos--;
        // att: exclusif: on est deja avant le dernier char si newpos = -2
        //else if (caret_str->c_str()[caret_pos-1] == '\n') caret_pos--;
      }
    }
  }

  if (_update && caret_str) {
    if (caret_str) caret_str->updateAutoParents(Update::paint);
  }
}


// devrait etre ailleurs car fct generique

// il faut parcourir la liste depuis le debut car les listes sont 
// simplement chainees, de plus 'from' et 'next' peuvent etre dans
// des descendants de type Element (mais PAS Box ni Window)

static String* _getPreviousStr(String* from, String*& last_str, Element& box) {
  if (/*!box || */ !from) return null;
  
  for (ChildIter ch = box.cbegin(); ch != box.cend(); ++ch) {
    String* s = null;
    Element* grp = null;
    
    if ((grp = (*ch)->toElem()) && !grp->toBox()) {
      s = _getPreviousStr(from, last_str, *grp);
      if (s) return s;
    }
    
    else if ((s = (*ch)->toStr())) {
      if (s == from) 
        return last_str;	               // skip the empty strings
      else if (s->c_str() && s->c_str()[0] != '\0')
        last_str = s;
    }
  }
  
  return null;
}


static String* _getNextStr(String* from, bool& from_found, Element& box) {
  if (/*!box ||*/ !from) return null;

  for (ChildIter ch = box.cbegin(); ch != box.cend(); ++ch) {
    String* s = null;
    Element* grp = null;
 
    if ((grp = (*ch)->toElem()) && !grp->toBox()) {
      s = _getNextStr(from, from_found, *grp);
      if (s) return s;
    }
    
    else if ((s = (*ch)->toStr())) {
      if (s == from) 
        from_found = true;	               // skip the empty strings
      else if (from_found && s->c_str() && s->c_str()[0] != '\0')
        return s;
    }
  }

  return null;
}


String* UEdit::getPreviousStr(String* from, Element& par) {
  String* last_str = null;
  return _getPreviousStr(from, last_str, par);
}


String* UEdit::getNextStr(String* from, Element& par) {
  bool from_found = false;
  return _getNextStr(from, from_found, par);
}

/* ==================================================== [Elc:] ======= */
// devrait etre ailleurs car fct generique

String* UEdit::getParagraphFirstStr(String* from, int& pos, Element& par) {
  if (/*!par ||*/ !from) return null;
  if (pos == -1) pos = from->length(); // caret on end-of-str

  String* s = from;
  String* slast = from;
  bool first_str = true;

  while (true) {
    const char* ch = s->c_str();

    // le tout premier \n ne doit pas etre pris en compte
    // sinon on va bloquer dessus la le curseur est en fin de ligne
    if (first_str && ch && ch[pos]=='\n') pos--;
    first_str = false;

    if (ch) {
      while (pos > 0 && ch[pos]!='\n') pos--;

      if (ch[pos]=='\n') {
	// dans le CP ou on arrive sur un \n en fin de ligne
	// c'est slast avec pos=0 qu'il faut renvoyer
	if (pos < s->length()-1) {
	  pos++;  // skip the \n
	  return s;
	}
	else {
	  pos = 0;
	  return slast;
	}
      }
    }

    slast = s;
    String* last_str = null;
    s = _getPreviousStr(s, last_str, par);
    if (!s) {
      pos = 0;
      return slast;
    }
    pos = s->length();
  }
}


String* UEdit::getParagraphLastStr(String* from, int& pos, Element& par) {
  if (/*!par ||*/ !from) return null;
  if (pos == -1) pos = from->length(); // caret on end-of-str

  String* s = from;
  String* slast = from;

  while (true) {
    const char* ch = s->c_str();
    if (ch) {
      while (ch[pos] && ch[pos]!='\n') pos++;
      if (ch[pos]=='\n') return s;
    }

    slast = s;
    bool from_found = false;
    s = _getNextStr(s, from_found, par);
    pos = 0;
    if (!s) {
      pos = slast->length();
      return slast;
    }
  }
}

/* ==================================================== [Elc:] ======= */

void UEdit::deletePreviousChar(Element& par, View* view) {
  if (!caret_str) return;
  //if (!par) par = getParent(0);
  //if (!par) return;

  bool moved = previousChar(par, view);   // move to prev char
  if (moved) {
    // delete current char (nb: test = security)
    if (caret_pos >= 0 && caret_pos < caret_str->length())
      caret_str->replaceImpl(caret_pos, 1, null, false);
    update();
  }
}

void UEdit::deleteChar(Element& par, View* view) {   // !! INCOMPLET !! ne passe pas a str suivante
  if (!caret_str) return;
  //if (!par) par = getParent(0);
  //if (!par) return;

  if (caret_pos >= 0 && caret_pos < caret_str->length()) {
    caret_str->replaceImpl(caret_pos, 1, null, false);
    // NB: le caret ne doit pas bouger dans ce cas
    update();
  }
}


bool UEdit::previousChar(Element& par, View* view) {
  if (!caret_str) return false;
  //if (!par) par = getParent(0);
  //if (!par) return false;

  //UFlowView* fview = view->toFlowView();
  //bool repainted = false;  // !!!ATT: repainted est une var d'instance
  // false if no parent or could not be moved (first str)
  bool stat = false;

  if (caret_pos == -1) caret_pos = caret_str->length(); // caret on end-of-str
  if (caret_pos > 0) {
    setCaretStr(caret_str, caret_pos-1, true, view);
    stat = true;
  }
  // move to previous str is possible (and skip \n if any)
  else {
    String *s_prev = getPreviousStr(caret_str, par);
    if (s_prev) {
      // move to last-1 char (and skip final \n)
      setCaretStr(s_prev, -2, true, view); // avant dernier char
      stat = true;
    }
  }

  //if (fview && !repainted) scroll(view, -1);
  return stat;
}


bool UEdit::nextChar(Element& par, View* view) {
  if (!caret_str) return false;
  //if (!par) par = getParent(0);
  //if (!par) return false;

  UFlowView* fview = view->toFlowView();
  //bool repainted = true;     // !!!ATT: repainted est AUSSI une var d'instance
  bool stat = false;

  if (caret_pos == -1) caret_pos = caret_str->length(); // caret on end-of-str
  bool newline = false;

  if (!caret_str->c_str() || caret_str->length() == 0
      ||         // cas fin de str
      caret_pos+1 > caret_str->length()
      ||         // cas newline en fin de str
      (caret_pos+1 == caret_str->length()
       && caret_str->c_str()[caret_str->length()-1] == '\n'
       && (newline = true)
       )
      ) {
    // move to next str is possible (and skip \n if any)
    String* snext = getNextStr(caret_str, par);
    
    if (snext) {
      if (newline)
        setCaretStr(snext, 0, true, view); // avant premier char
      else
        setCaretStr(snext, 1, true, view); // apres premier char
      stat = true;
    }
    else if (fview) {
      // cas particulier d'un multi-line on est a la fin 
      // de la derniere String : il faut pouvoir passer a la ligne
      // finale (bien qu'il n'ait rien sur cette line)
      //repainted = false;
      setCaretStr(caret_str, caret_pos+1, true, view);
      stat = true;
    }
  }
  else {
    setCaretStr(caret_str, caret_pos+1, true, view);
    stat = true;
 }

  //if (fview && !repainted) scroll(view, +1);
  return stat;
}

/* ==================================================== [Elc:] ======= */

void UEdit::previousLine(Element& par, View* view) {
  UFlowView* fview = view->toFlowView();
  if (fview) {
    int cpos = getCaretPos(par);
    int x, y;
    fview->caretPosToXY(cpos, x, y);
    if (y > 0) {
      long cpos2;
      fview->xyToCaretPos(x, y-1, cpos2);
      repainted = false;
      setCaretPos(cpos2, par);
      /// marche pas if (!repainted) scroll(view, -1);
    }
  }
}

void UEdit::nextLine(Element& par, View* view) {
  UFlowView* fview = view->toFlowView();
  if (fview) {
    int cpos = getCaretPos(par);
    int x, y;
    fview->caretPosToXY(cpos, x, y);
    //if (y < 999999) {
      long cpos2;
      fview->xyToCaretPos(x, y+1, cpos2);
      repainted = false;
      setCaretPos(cpos2, par);
      ///  marche pas if (!repainted) scroll(view, +1);
    //}
  }
}

void UEdit::beginningOfLine(Element& par, View* view) {
  UFlowView* fview = view->toFlowView();
  if (fview) {
    int pos1 = getCaretPos(par);
    int x, y;
    fview->caretPosToXY(pos1, x, y);
    long pos2;
    fview->xyToCaretPos(0, y, pos2);
    setCaretPos(pos2, par);
  }
  else {
    int pos = caret_pos;
    String* s = getParagraphFirstStr(caret_str, pos, par);
    if (s) setCaretStr(s, pos, true, view);
  }
}

void UEdit::endOfLine(Element& par, View* view) {
  UFlowView* fview = view->toFlowView();
  if (fview) {
    int pos1 = getCaretPos(par);
    int x, y;
    fview->caretPosToXY(pos1, x, y);
    long pos2;
    fview->xyToCaretPos(-1, y, pos2);
    setCaretPos(pos2, par);
  }
  else {
    int pos = caret_pos;
    String* s = getParagraphLastStr(caret_str, pos, par);
    if (s) setCaretStr(s, pos, true, view);
  }
}

/*
void UEdit::beginningOfParagraph(Element* par) {
  int pos = caret_pos;
  String* s = getParagraphFirstStr(caret_str, pos, par);
  if (s) setCaretStr(s, pos, true, view);
}

void UEdit::endOfParagraph(Element* par) {
  int pos = caret_pos;
  String* s = getParagraphLastStr(caret_str, pos, par);
  if (s) setCaretStr(s, pos, true, view);
}
*/
/* ==================================================== [Elc:] ======= */

float UEdit::getXpos(UpdateContext& ctx, const Rectangle& r, int offset, int cellen) const {
  float xpos = r.x;
  if (!caret_str) return xpos;

  //!att: meme si !(str->s) penser a afficher le Caret
  const char* s = caret_str->c_str();
  if (s) {
    xpos += UFontMetrics(ctx).getXPos(s + offset, cellen, caret_pos - offset);
  }
  
  // xpos-1 pour eviter que la barre s'affiche sur le char suivant 
  // sauf si on est deja au min (cad r.x)
  xpos = std::max(xpos-1, (float)r.x);
  return xpos;
}

float UEdit::getXpos(UpdateContext& ctx, const Rectangle& r) const {
  return getXpos(ctx, r, 0,  (caret_str ? caret_str->length() : 0));
}

void UEdit::paint(Graph& g, UpdateContext& ctx, const Rectangle& r, int offset, int cellen) const {
  if (!is_visible) return;
  float xpos = getXpos(ctx, r, offset, cellen);  // ex int

  // -1 sinon ca deborde de la zone de background!
  float y2 = r.y + UFontMetrics(ctx).getHeight() -1;  // ex int

  if (caret_color) g.setColor(*caret_color);
  else if (ctx.color) g.setColor(*ctx.color);
  
  g.drawLine(xpos, r.y, xpos, y2);
  //g.drawLine(xpos+1, r.y, xpos+1, y2);

  Rectangle c;
  g.getHardwinClip(c);
  repainted = (r.y > c.y) && (y2 < c.y+c.height);
}

void UEdit::paint(Graph& g, UpdateContext& ctx, const Rectangle& r) const {
  paint(g, ctx, r, 0, (caret_str ? caret_str->length() : 0));
}

// NB: press et non arm car arm provoque un reaffichage 
//     (on reafficherait donc 2 fois)

void UEdit::inputCB(UInputEvent& e) {
  if (e.getCond() == UOn::mpress)
    mpressed((UMouseEvent&)e);
  else if (e.getCond() == UOn::mrelease)
    mreleased((UMouseEvent&)e);
  else if (e.getCond() == UOn::kpress)
    kpressed((UKeyEvent&)e);
}


void UEdit::mpressed(UMouseEvent& e) {
  UDataContext dc;
  String* s = null;
  int btn = e.getButton();

  if ((btn == Application::conf.mouse_select_button || btn == Application::conf.mouse_alt_button)
      && (s = e.getStr(dc))) {

    setCaretStr(s, dc.strpos, true, e.getView()/*ex:false*/); // no hscroll
    
    if (btn == Application::conf.mouse_alt_button) {
      // APRES setCaretPosition !
      if (caret_str && is_editable) {
        // ATTENTION: c'est lie a *chaque* disp et c'est ASYNCHRONE: le paste
        // n'est pas fait immediatement mais par appel ulterieur d'un callback
        // -> pbm si jamais la caret_str est detruite entre temps!
        e.getDisp()->pasteSelection(e, caret_str, caret_pos);
      }
    }
  }
}


void UEdit::mreleased(UMouseEvent& e) {
  UDataContext dc;
  String* newstr = null;
  // mrelease1 necessaire pour que le Drag selection fonctionne
  if (e.getButton() == Application::conf.mouse_select_button && (newstr = e.getStr(dc))) {
    setCaretStr(newstr, dc.strpos, true, e.getView() /*ex: false*/);  // no hscroll
  }
}


void UEdit::callbacks2(UMouseEvent& e) {  // multiple clicks
  int btn = e.getButton();

  if (btn == Application::conf.mouse_select_button) {
    if (caret_str && e.getClickCount() == 2) {
      String sel_txt;
      Selection* sel = getSelection(e, sel_txt);
      if (sel) {
        sel->start(e);
        sel->fromPos = 0;
        sel->toPos = caret_str->length()-1;
        sel->complete(e);
      }
    }
  }
}


Selection* UEdit::getSelection(UInputEvent& e, String& sel_text) {
  sel_text.clear();
  Display* d = e.getDisp(); 
  Selection* sel = d ? d->getChannelSelection(0) : null;  // on devrait tenir compte du flow !!!
  if (sel) sel->copyText(sel_text);
  return sel;
}

void UEdit::deleteSelection(Selection* sel, String& sel_text, Element& container) {
  int pos_in_s = 0;
  String* s = getCaretStr(pos_in_s);
  
  if (!sel || !sel->in_obj || sel->oldLink == sel->in_obj->cend()
      || s != *(sel->oldLink)) 
    return;
  
    if (pos_in_s >= sel->toPos) {
    long pos = getCaretPos(container);
    setCaretPos(pos - sel_text.length(), container);
  }
  sel->deleteText();
}

void UEdit::kpressed(UKeyEvent& e) {
  if (!caret_str) return;

  // en fait ca retourne pas un Element mais toujours le Box qui le
  // contient. c'est une bonne chose car ca permet aux diverses fonctions
  // de descendre dans les UElems imbriques qunad il y en a et de traiter
  // ainsi tous les descendants du Box de la meme maniere qu'ils soient
  // ou non inclus dans des UElems intermediaires
  if (!e.getSource()) return;
  Element& box = *e.getSource();
  View* view = e.getView();
  
  long  keycode = e.getKeyCode();
  unsigned short keychar = e.getKeyChar();
  bool done  = false;  

  String sel_txt;
  Selection* sel = getSelection(e, sel_txt);

  // si la selection n'est pas sur l'objet ou on interagit on l'efface
  if (sel && sel->getObj() != &box) sel->clear();
  
  if (keycode == UKey::BackSpace) {
    if (is_editable) {
      if (sel_txt.empty()) deletePreviousChar(box, view);
      else deleteSelection(sel, sel_txt, box);
    }
  }
  else if (keycode == UKey::Delete) {     // cf aussi XK_KP_Delete a convertir !!!!
    if (is_editable) {
      if (sel_txt.empty()) deleteChar(box, view);
      else deleteSelection(sel, sel_txt, box);
    }
  }
  else if (keycode == UKey::Left) {     // cf aussi XK_KP_Left a convertir !!!!
    if (sel) sel->clear();
    previousChar(box, view);
  }
  else if (keycode == UKey::Right) {     // cf aussi XK_KP_Right a convertir !!!!
    if (sel) sel->clear();
    nextChar(box, view);
  }
  else if (keycode == UKey::Up) {     // cf aussi XK_KP_Up
    if (sel) sel->clear();
    previousLine(box, view);
  }
  else if (keycode == UKey::Down) {     // cf aussi XK_KP_Down
    if (sel) sel->clear();
    nextLine(box, view);
  }
  else if (keycode == UKey::Home) {     // cf aussi XK_KP_Home, XK_Begin, XK_KP_Begin
    if (sel) sel->clear();
    beginningOfLine(box, view);
  }
  else if (keycode == UKey::End) {     // cf aussi XK_KP_End
    if (sel) sel->clear();
    endOfLine(box, view);
  }
  else if (keycode == UKey::Enter) {
    // the "Enter" key can break the line if the boxent is a FlowView
    if (is_editable && view->toFlowView()) {
      if (!sel_txt.empty()) deleteSelection(sel, sel_txt, box);
      keychar = '\n';
      done = caret_str->insertImpl(caret_pos, keychar, false);
      if (done) nextChar(box, view);
      update();
    }
  }
  else {
    switch(keychar) {
    case '\004':  // ^D : delete
      if (is_editable) {
        if (sel_txt.empty()) deleteChar(box, view);
        else deleteSelection(sel, sel_txt, box);
      }        
      break;

    case '\002':  // ^B : backwards
      if (sel) sel->clear();
      previousChar(box, view);
      break;

    case '\006':  // ^F : backwards
      if (sel) sel->clear();
      nextChar(box, view);
      break;

    case '\001':  // ^A : begin
      if (sel) sel->clear();
      beginningOfLine(box, view);
      break;

    case '\005':  // ^E : end
      if (sel) sel->clear();
      endOfLine(box, view);
      break;

#if MARCHE_PAS
      // ce fct necessitent un type particulier pour e
    case 3:  // ^C : Copy
      if (sel) {
        // NB: ne fait le copy sur le Mac, que sur X
        e.getDisp()->copySelection(e, *sel);
      }
      break;
      
    case 22:  // ^V : Paste
      if (caret_str && is_editable) {
        sel->clear();
        e.getDisp()->pasteSelection(e, caret_str, caret_pos);
      }
      break;
#endif
      
    default:
      // si le char est affichable
      //EX: if (isprint(kk))... for some reason isprint elime les chars 
      //accentues sauf avec certaines locales quand elles existent => pas sur!

      if (is_editable && keychar != '\0' && !iscntrl(keychar)) {
        if (!sel_txt.empty()) deleteSelection(sel, sel_txt, box);
        done = caret_str->insertImpl(caret_pos, keychar, false);
        if (done) nextChar(box, view);
        update();
      }
    }
  }
}

}

