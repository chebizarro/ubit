/*************************************************************************
*
*  xwin.cpp
*  Ubit GUI Toolkit - Version 6.0
*  (C) 2003-2008 Eric Lecolinet / ENST Paris / www.enst.fr/~elc/ubit
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

#include <stdio.h>
#include <ctype.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xmu/WinUtil.h>
#include <ubit/umsproto.hpp>
#include <ubit/uwinImpl.hpp>
#include "umserver.hpp"
#include "flow.hpp"
#include "events.hpp"
using namespace std;

/* ==================================================== ======== ======= */

Win::Win(Window _win, Window _client_win) :
  is_prop_init(false), is_shown(false), is_override(false), 
  is_input_only(false), is_icon(false),
  win_sock(-1),
  win(_win), client_win(_client_win),
  ubit_prop(null) {
}

Win::~Win() {
  if (ubit_prop) XFree(ubit_prop);
  //if (command_argv) XFreeStringList(command_argv);
}

/* ==================================================== ======== ======= */

WinList::iterator UMServer::findWinIt(WindowID win) {
  if (win != None) {
    for (WinList::iterator k = wins.begin(); k != wins.end(); k++) {
      // chercher si c'est win OU client_win (ca varie selon les cas)
      if ((*k)->win == win || (*k)->client_win == win) return k;
    }
  }
  return wins.end();  // not found
}

Win* UMServer::findWin(Window win) {
  WinList::iterator k = findWinIt(win);
  return (k == wins.end()) ? null : *k;
}

/* ==================================================== ======== ======= */
/*
static bool isMatchingPattern(const char* pattern, const char* wname) {
  if (!wname) return false;
  //if (!pattern || !*pattern) return false;
  char p0 = tolower(pattern[0]);

  for (const char* w = wname; *w != 0; w++) {  
    if (tolower(*w) == p0) {
      const char* w1 = w+1;
      for (const char* p1 = pattern+1; *p1 != 0; p1++) {
	if (!w1 || tolower(*p1) != tolower(*w1)) goto next;
	w1++;
      }
      return true;
    next: ;
    }
  }
  return false;
}
*/

// NB: considere la version courante du wname (via XFetchName) laquelle
// peut changer si modifiee par l'application (par ex. Mozilla, etc.)

Win* UMServer::findWin(const char* pattern) {
  if (!pattern || !*pattern) return null;

  for (WinList::iterator k = wins.begin(); k != wins.end(); k++) {
    if (!(*k)->is_override && !(*k)->is_input_only && !(*k)->is_icon
	// ?? && (*k)->win->is_shown ??
	) {
      bool found = false;
      char* n = null;

      // respecter case sinon ambiguites!
      if (XFetchName(xdisplay,(*k)->win,&n) && n && strstr(n,pattern)) 
	found = true;
      if (n) XFree(n);

      if (!found) {
	n = null;
	if (XFetchName(xdisplay,(*k)->client_win,&n) && n && strstr(n,pattern)) 
	  found = true;
	if (n) XFree(n);
      }

      //if (found)fprintf(stderr, "FOUND: %x %x %s \n", (*k)->win, (*k)->client_win,n);
      if (found) return *k;
    }
  }
  return null;
}

/* ==================================================== ======== ======= */
/*
 Win* UMServer::findWinCommand(const char* pattern) {
   for (WinList::iterator k = wins.begin(); k != wins.end(); k++) {
     //if ((*k)->name && strstr((*k)->name, pattern)) return *k;
   }
   return null;
 }
 */

/* ==================================================== ======== ======= */
/* ==================================================== [(c)Elc] ======= */

Win* UMServer::addWin(WindowID win, bool notify) {

  // att: ne pas mettre les pointer wins dans la liste !
  if (isPointer(win)) return null;

  WinList::iterator k = findWinIt(win);
  if (k != wins.end()) return *k;       // already in the list

  // dans certains cas, "for some reason", client_win n'est pas la
  // fenetre esperee ...
  WindowID client_win = XmuClientWindow(xdisplay, win);

  // int command_argc = 0;
  // char** command_argv = null;
  // XGetCommand(xdisplay, client_win, &command_argv, &command_argc);
  
  // imposer ums_cursor pour chaque client_win s'il est specifie
  if (root_cursor != None) XDefineCursor(xdisplay, client_win, root_cursor);
  
  /* malheureusement client_win est parfois erronee dont il faut faire
   * cela dans getPos() a chaque pressMouse
    * XGetWindowProperty(xdisplay, client_win, _UMS_WINDOW,
                         req_offset, req_length,
                         False,        // dont delete property
                         XA_STRING,    // req_type
                         &type, &format, &nitems, &bytes_after,
                         &ubit_win_prop);
  */

  Win* w = new Win(win, client_win);
  wins.push_back(w);

  XWindowAttributes a;
  if (XGetWindowAttributes(xdisplay, win, &a)) {
    if (a.map_state == IsViewable) w->is_shown = true;
    if (a.override_redirect) w->is_override = true;
    if (a.c_class == InputOnly) w->is_input_only = true;
  }

  XWMHints* wm_hints = XGetWMHints(xdisplay, win);
  if (wm_hints) {
    if (wm_hints->flags & IconWindowHint) {
      for (WinList::iterator k = wins.begin(); k != wins.end(); k++) {
	if ((*k)->win == wm_hints->icon_window) {
	  (*k)->is_icon = true;
	  break;
	}
      }
    }
    XFree(wm_hints);
  }

  if (notify) sendWindowState(w, Win::CREATE);
  return w;
}

/* ==================================================== ======== ======= */

void UMServer::removeWin(Window win, bool notify) {
  WinList::iterator k = findWinIt(win);
  if (k != wins.end()) {       // in the list
    if (notify) sendWindowState(*k, Win::DESTROY);
    delete *k;
    wins.erase(k);
  }
}

/* ==================================================== ======== ======= */

void UMServer::reorderWin(WinList::iterator& from, 
			  const WinList::iterator& to) {
  // on transfere l'objet pointe de from a to
  // on evite ainsi une creation et une destruction d'objet
  wins.insert(to, *from);
  *from = null;          // from ne pointe plus sur rien
  wins.erase(from);
}

/* ==================================================== ===== ======= */
/* ==================================================== [Elc] ======= */
// NB: peut etre appele plusieurs fois pour re-synchronisation eventuelle

void UMServer::retrieveXWindows() {
  Window root_win = None;
  Window parent_win = None;
  Window* windows = NULL;
  unsigned int wcount = 0;

  wins.clear();

  // The children are listed in current stacking order, from bottom-most 
  // (first) to top-most (last).

  if (XQueryTree(xdisplay, xrootwin, &root_win, &parent_win, &windows, &wcount)
      && wcount != 0 && windows != null) {

    for (unsigned int k = 0; k < wcount-1; k++) {
      addWin(windows[k], false);
    }
    XFree(windows);
  }
}

/* ==================================================== ===== ======= */

void UMServer::raiseXWindows() {
  for (WinList::iterator k = wins.begin(); k!=wins.end(); k++) {
    //if ((*k)->win != grab_win && (*k)->client_win != grab_win) {
      XRaiseWindow(xdisplay, (*k)->win);
      //XRaiseWindow(xdisplay, (*k)->client_win);
    //}
  }
}

/* ==================================================== ======== ======= */

void UMServer::sendWindowState(Win* w, int state) {
  if (w->is_override || w->is_input_only || w->is_icon) return;

  for (CnxList::iterator c = cnxs.begin(); c != cnxs.end(); c++) {
    if ((*c)->browse_windows) {   // n'envoyer qu'a ceux qui le demandent
      char msg[300];
      sprintf(msg, "_umsWin %ld\t%ld\t%d", w->win, w->client_win, state);
      events.sendMessage((*c)->sock, msg);
    }
  }
}

void UMServer::sendWindowList(Cnx* to) {
  for (WinList::iterator k = wins.begin(); k != wins.end(); k++) {
    if (!(*k)->is_override && !(*k)->is_input_only && !(*k)->is_icon) {
      char msg[300];
      sprintf(msg, "_umsWin %ld\t%ld\t%d", 
	      (*k)->win, (*k)->client_win, (*k)->is_shown);
      events.sendMessage(to->sock, msg);
    }
  }
}

/* ==================================================== [Elc] ======= */
/* ==================================================== ===== ======= */
/*
void UMServer::showWindowName(Window win) {
  Window client_win = XmuClientWindow(xdisplay, win); 
  char hex[20];
  sprintf(hex, "%lx", win);
  cerr << " win: " << win << " 0x"<< hex;
  sprintf(hex, "%lx", client_win);
  cerr << " client_win: " << client_win << " 0x"<< hex;

  char* window_name = 0;
  if (XFetchName(xdisplay, client_win, &window_name))
    cerr << " / name: " << (window_name ?  window_name : "Null") << endl;
  else cerr << " / no name " << endl;
  if (window_name) XFree(window_name);
}

void UMServer::showWindows() {
  Window root_win = None;
  int win_x, win_y;
  unsigned int win_width, win_height;
  unsigned int win_border;
  unsigned int win_depth;

  for (WinList::iterator k = wins.begin(); k!=wins.end(); k++) {
    if ((*k)->win != grab_win && (*k)->client_win != grab_win) {
    if (XGetGeometry(xdisplay, (*k)->win,
                     &root_win,  &win_x, &win_y,
                     &win_width, &win_height,
                     &win_border, &win_depth)) {
      showWindowName((*k)->win);
      cerr << " / x,y = " << win_x << " " << win_y
        << " / w,h = " << win_width << " " << win_height << endl;
    }
  }
}
*/
/* ==================================================== ===== ======= */
/* ==================================================== [Elc] ======= */
// trouver window du server X contenant le point (rx,ry).
// input:  point (rx,ry) dans root
// output: pos.win = window trouvee et coords = pos.{rx,ry,wx,wy}

bool UMServer::getXWin(int rx, int ry, Pos& pos, bool check_props) {
  Win* found_w = null;
  Window root_win = None;
  int x, y;
  unsigned int width, height, border, depth;

  pos.win = None;
  pos.win_sock = -1;
  pos.wx = pos.wy = 0;
  pos.rx = rx;
  pos.ry = ry;
  pos.in_ubitwin = false;

  Window parwin = None;
  Window win_child = None;
  int x_in_win, y_in_win;

  // !ATT: cette liste ne doit contenir que les window filles de xrootwin
  // (cad les window du WM qui contiennent celles des applis
  // ou les window override_redirect des menus)

  for (WinList::iterator k = wins.begin(); k != wins.end(); k++) {

    // !ATT: x et y sont relatifs a la parent window. il ne faut donc
    // considerer que les filles de root_win (ce qui est le cas ici)
    // sinon il faudrait translater les coords par XTranslateCoordinates

    if ((*k)->is_shown
        && XGetGeometry(xdisplay, (*k)->win, &root_win,
                        &x, &y, &width, &height, &border, &depth)
        && root_win == xrootwin      // tjrs vrai si 1 seul screen
        && rx >= x && rx < int(x + width)
        && ry >= y && ry < int(y + height)
        ) {

      // trouver la subwindow de root_children[k] qui contient rx,ry
      // et coords dans cette subwindow
      found_w = *k;
      Window win = (*k)->win;

#if TST
      Window clientw = XmuClientWindow(xdisplay, win);
      //printf("win %x client %x \n", win, clientw);

      if (clientw != None
          && XTranslateCoordinates(xdisplay,/*src*/root_win,/*dst*/clientw,
                                   rx,ry, &x_in_win,&y_in_win, &win_child)
          ) {
        pos.win = clientw;
        pos.subwin = win_child;  // new
        pos.wx = x_in_win;
        pos.wy = y_in_win;
      }
#endif

      while (XTranslateCoordinates(xdisplay, /*src*/root_win, /*dst*/win,
                                   rx,ry,&x_in_win,&y_in_win,&win_child)){
        pos.win = win;
        pos.wx = x_in_win;
        pos.wy = y_in_win;
        if (win_child == None) break;
        else {
          parwin = win;
          win = win_child;
        }
      }
    }
  }

  if (pos.win != None && found_w) {
    if (!found_w->is_prop_init || check_props) {
      found_w->is_prop_init = true;

      long req_offset = 0;
      long req_length = XMaxRequestSize(xdisplay);
      Atom type;
      int format;
      unsigned long nitems = 0, bytes_after = 1;

      if (found_w->ubit_prop) XFree(found_w->ubit_prop);
      found_w->ubit_prop = null;

      // NB: getProperty rajoute toujours le 0 final d'une string retourne
      // dans prop_return (meme si elle est nulle)
      XGetWindowProperty(xdisplay, pos.win, _UMS_WINDOW,
                         req_offset, req_length,
                         False,        // dont delete property
                         XA_STRING,    // req_type
                         &type, &format, &nitems, &bytes_after,
                         &(found_w->ubit_prop));
    }
    // fprintf(stderr, "winprop: win: %x / props: %s \n",
    //         pos.win, (found_w->ubit_prop ? "YES": "NO"));
    if (found_w->ubit_prop) {
      pos.in_ubitwin = true;
      if (found_w->ubit_prop[0] == UWinImpl::SUBWIN) {
        if (parwin) {
          pos.win = parwin;
          XTranslateCoordinates(xdisplay,/*src*/root_win,/*dst*/parwin,
                                rx,ry, &x_in_win,&y_in_win, &win_child);
          pos.wx = x_in_win;
          pos.wy = y_in_win;
        }
      }
    }
    /*
    cerr << "%%%%FOUND " << found_w->win 
      << " " << pos.win
      << " " << pos.in_ubitwin << " type " 
      << (found_w->ubit_prop ? int(found_w->ubit_prop[0]) : -1)
      << " " << UNatWin::CHILDWIN << endl;
    */
  }
  return pos.win;
}

/* ==================================================== ======== ======= */
// trouver subwindow de winpos contenant le point (rx,ry).
// input:  win et point (rx,ry) dans root
// output: pos.win = subwindow trouvee ou win et coords = pos.{rx,ry,wx,wy}

bool UMServer::getXSubwin(const Pos& winpos, int x, int y, Pos& pos) {
  if (!winpos.win) return false;
  pos.win = None;
  pos.win_sock = -1;
  pos.wx = pos.wy = 0;
  x += winpos.rx, y += winpos.ry;
  pos.rx = x;
  pos.ry = y;

  Window win_child = None;
  int x_in_win, y_in_win;

  // trouver la subwindow de win qui contient rx,ry
  // et coords dans cette subwindow
  WindowID win = winpos.win;
  while (XTranslateCoordinates(xdisplay, /*src*/xrootwin, /*dst*/win,
                               x, y, &x_in_win, &y_in_win, &win_child)
         ) {
    pos.win = win;
    pos.wx = x_in_win;
    pos.wy = y_in_win;

    if (win_child == None) break;
    else win = win_child;
  }

  return (pos.win != None);
}

/* ==================================================== ===== ======= */
// trouver position de win dans root.
// input:  win
// output: pos.rx, pos.ry  (pos.win = win et le reste a 0)

bool UMServer::getXWinPos(const Win* w, Pos& pos) {
  if (!w || !w->win) return false;
  int x, y;
  unsigned int win_width, win_height, win_border, win_depth;
  Window root_win = None;
  Window win_child = None;
  pos.win = None;
  pos.win_sock = -1;
  pos.wx = pos.wy = 0;
  pos.rx = pos.ry = 0;

  // !NB: XGetGeometry ne donne pas les coords absolues dans le cas des Shells
  // car ceux-ci sont inclus dans une fenetre intermediaire cree par le WM
  if (XGetGeometry(xdisplay, w->win, &root_win,
                   &x, &y, &win_width, &win_height,
                   &win_border, &win_depth)

      && XTranslateCoordinates(xdisplay, /*src*/w->win, /*dst*/root_win,
                               0, 0, &x, &y, &win_child)) {
    pos.win = w->win;
    pos.rx = x;
    pos.ry = y;
    return true;
  }
  else return false;
}

/* ==================================================== [TheEnd] ======= */
