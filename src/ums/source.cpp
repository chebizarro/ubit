/*************************************************************************
 *
 *  source.cpp - UMS Server
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2008 Eric Lecolinet / ENST Paris / www.enst.fr/~elc/ubit
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

#include <vector>
#include <algorithm>
#include <ubit/umservice.hpp>
#include <X11/X.h>
#include "umserver.hpp"
#include "source.hpp"
using namespace std;

#define ALLMODS (ShiftMask|LockMask|ControlMask|Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask)

/* ==================================================== ======== ======= */

UMSbutton::UMSbutton(int btn_number, unsigned int mod_mask,
		     unsigned int to_btn_mask, unsigned int to_mod_mask) {
  // input from the device
  in_btn_number = btn_number;
  in_mod_mask   = mod_mask;    // in_btn_mask not included

  // output to the event flow
  out_btn_mask = to_btn_mask;
  out_mod_mask = to_mod_mask;  // out_btn_mask not included
}

/* ==================================================== ======== ======= */

EventSource::~EventSource() {
  for (unsigned  int k = 0; k < button_map.size(); k++) delete button_map[k];
  fd = -1;
}

UMSbutton* EventSource::getButton(int btn_number) {
  for (unsigned  int k = 0; k < button_map.size(); k++) {
    if (button_map[k]->in_btn_number == btn_number)
      return button_map[k];
  }
  return null;
}

UMSbutton* EventSource::getButton(int btn_number, unsigned int mod_mask) {
  // NB: il ne faut prendre en compte QUE les modifieurs, pas les btns
  // dans le mask (celui-ci n'etant pas le meme au Press et au Release)
  mod_mask = (mod_mask & ALLMODS);

  for (unsigned int k = 0; k < button_map.size(); k++) {
    if (button_map[k]->in_btn_number == btn_number
        && ((button_map[k]->in_mod_mask & mod_mask) == mod_mask))
      return button_map[k];
  }
  return null;
}

void EventSource::setButton(int btn_number, unsigned int mod_mask,
			       unsigned int to_btn_mask, unsigned int to_mod_mask)
{
  UMSbutton* b = getButton(btn_number, mod_mask);
  if (b) {
    b->out_btn_mask = to_btn_mask;
    b->out_mod_mask = to_mod_mask;
  }
  else {
    b = new UMSbutton(btn_number, mod_mask, to_btn_mask, to_mod_mask);
    button_map.push_back(b);
  }
}

void EventSource::setButton(int btn_number, 
				unsigned int to_btn_mask, unsigned int to_mod_mask) {
  setButton(btn_number, 0, to_btn_mask, to_mod_mask);
}

/* ==================================================== [(c)Elc] ======= */
// EVENT SOURCES

bool UMServer::addEventSource(EventSource* so) {
  for (unsigned int k = 0; k < sources.size(); k++) {
    if (sources[k] == so) {
      cerr << "!addEventSource: this source is already registered" << endl;
      return false;
    }
  }

  bool stat = addSourceToMainLoop(so);
  if (stat) sources.push_back(so);
  //cout << "addSource: " << so->filedesc() << " status: "<< stat << endl;
  return stat;
}

bool UMServer::removeEventSource(EventSource* so) {
  unsigned int sz = sources.size();
  sources.erase( remove(sources.begin(), sources.end(), so), sources.end());

  if (sz == sources.size()) {
    cerr << "!removeEventSource: source not found" << endl;
    return false;
  }
  else return true;
}

