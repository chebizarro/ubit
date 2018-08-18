/*
 *  disp.cpp: Graphical Display (may be remotely located)
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

#include <iostream>
#include <algorithm>
#include <ubit/ubit_features.h>
#include <ubit/ucall.hpp>
#include <ubit/ui/window.h>
#include <ubit/ui/eventflow.h>
#include <ubit/ui/scrollbar.h>
#include <ubit/Selection.hpp>
#include <ubit/core/config.h>
#include <ubit/core/application.h>
#include <ubit/core/appimpl.h>
#include <ubit/ui/cursor.h>
#include <ubit/draw/font.h>
#include <ubit/draw/ufontImpl.hpp>
#include <ubit/uon.hpp>
#include <ubit/ui/timer.h>
#include <ubit/umsproto.hpp>
#include <ubit/udispX11.hpp>
#include <ubit/udispGLUT.hpp>
//#include <ubit/udispGDK.hpp>
#include <ubit/uhardfont.hpp>
#include <ubit/draw/rendercontext.h>
#include <ubit/uglcontext.hpp>
using namespace std;
namespace ubit {



Display* Display::create(const String& dname) {
  Display* d = null;
#if UBIT_WITH_GLUT
  d = new UDispGLUT(dname);
#elif UBIT_WITH_X11
  d = new UDispX11(dname);
//#elif UBIT_WITH_GDK
//  d = new UDispGDK(dname);
#else
  Application::fatalError("Display::create","no available underlying windowing toolkit")
#endif
  // ne peut pas etre dans Display constr, car ceux des subclasses doivent etre executes
  if (d && d->isOpened()) Application::impl.displist.push_back(d);
  return d;
}


Display::Display(const String& dname) :
id(Application::impl.displist.size()), 
conf(Application::conf), 
display_name(dname),
bpp(Application::conf.bpp),
depth_size(Application::conf.depth_size),
stencil_size(Application::conf.stencil_size),
screen_width(0), screen_height(0),
screen_width_mm(0), screen_height_mm(0),
is_opened(false), 
default_context(null), current_glcontext(null), 
app_motion_time(0), nat_motion_time(0),
red_mask(0), green_mask(0), blue_mask(0),
red_shift(0), green_shift(0), blue_shift(0), 
red_bits(0), green_bits(0), blue_bits(0),
IN_TO_PX(0.), CM_TO_PX(0.), MM_TO_PX(0.), PT_TO_PX(0.), PC_TO_PX(0.),
paste_str(null),
paste_pos(0) {
  // if :screen not found add ":0" (except if display_name is empty)
  if (!display_name.empty() && display_name.rfind(':') < 0) display_name &= ":0";
  
  if (Application::isUsingGL()) {
#ifndef UBIT_WITH_GL
    Application::error("Display","OpenGL mode requested but the Ubit GUI toolkit was not compiled with the OpenGL library");
#endif
  }
}

Display::~Display() {
  Application::deleteNotify(this);
} 

/*
 bool Display::isGLAvailable() const {
 #if UBIT_WITH_GL
 return true;
 #else
 return false;
 #endif
 }
 */

RenderContext* Display::getDefaultContext() {
  if (default_context) return default_context;
  
  // if default_context is not initialized, search (by order of preference):
  // the first mainframe, then the first frame, then the first window
  UHardwinImpl* main_hw = null;
  
  for (HardwinList::iterator k = hardwin_list.begin(); k != hardwin_list.end(); ++k) {
    UWinImpl::WinType wtype = (*k)->getWinType();
    if (wtype == UWinImpl::MAINFRAME) {
      main_hw = *k;
      break;
    }
    else if (wtype == UWinImpl::FRAME) {
      if (!main_hw) main_hw = *k;
    }
    else if (wtype > UWinImpl::HARDWIN) {
      if (!main_hw || main_hw->getWinType() != UWinImpl::FRAME) main_hw = *k;
    }
  }
  default_context = main_hw->getGlcontext();
  default_context->setDest(main_hw, 0, 0);
  return default_context;
}

void Display::makeDefaultContextCurrentIfNeeded() {
  if (!Application::isUsingGL()) return;
  
  // note that getDefaulContext() inits 'default_context' if not already done
  RenderContext* dc = getDefaultContext();
  if (!current_glcontext || !current_glcontext->isSharedWith(dc)) dc->makeCurrent(); 
}


void Display::add(Window& win) {  
  winlist.add(win);   // add to the window list

  // !!ATT: marche pas si meme win ajoutee plusieurs fois !!
  //Child* c = null;
  //if (win.pbegin() != win.pend()) c = &win.pbegin().parent().getChild();
  win.initViewImpl(/*c,*/ null, this);

  // ne pas afficher de fenetre avant lancement de la mainloop
  if (Application::isRunning() && win.isShowable()) win.show(true);
}

void Display::remove(Window& win, bool auto_delete) {
  win.show(false);
  winlist.remove(win, auto_delete);
}

void Display::add(Window* win) {  
  if (!win) Application::error("Display::add","null Window* argument");
  else add(*win);
}

void Display::remove(Window* win, bool auto_delete) {  
  if (!win) Application::error("Display::remove","null Window* argument");
  else remove(*win, auto_delete);
}


void Display::addHardwin(Window* win) {
  if (!win) return;
  UHardwinImpl* hw = win->hardImpl();
  //UHardwinImpl* hw = win->getHardwin(this);  PAS BON !!!
  //cerr << "addHardwin " <<win->hardImpl() << " "<< hw << endl;
  if (!hw) return;
  if (find(hardwin_list.begin(), hardwin_list.end(), hw) == hardwin_list.end())
    hardwin_list.push_back(hw);      // ne pas ajouter 2 fois
}

void Display::removeHardwin(Window* win) {
  if (!win) return;
  UHardwinImpl* hw = win->hardImpl();
  // UHardwinImpl* hw = win->getHardwin(this); PAS BON !!!
  //cerr << "removeHardwin "  <<win->hardImpl() << " "<< hw << endl;
  if (!hw) return;
  HardwinList::iterator i = find(hardwin_list.begin(), hardwin_list.end(), hw);
  if (i != hardwin_list.end()) hardwin_list.erase(i);
}


void Display::setPixelPerMM(double pixel_per_mm) {
  MM_TO_PX = pixel_per_mm;
  CM_TO_PX = pixel_per_mm * 10;
  IN_TO_PX = pixel_per_mm * 25.4;  // generally close to 72 
  PT_TO_PX = pixel_per_mm * 25.4 / 72.;       //  pt = 1/72 in
  PC_TO_PX = 12 * pixel_per_mm * 25.4 / 72.;  // 1 pc = 12 pt  
}

void Display::setPixelPerInch(double pixel_per_inch) {
  MM_TO_PX = pixel_per_inch / 25.4;
  CM_TO_PX = pixel_per_inch / 25.4 * 10;
  IN_TO_PX = pixel_per_inch;
  PT_TO_PX = pixel_per_inch / 72.;       //  pt = 1/72 in
  PC_TO_PX = 12 * pixel_per_inch / 72.;  // 1 pc = 12 pt  
}

String* Display::getPasteTarget(int& pos) const {
  pos = paste_pos; 
  return paste_str;
}

void Display::clearPasteTarget() {
  paste_str = null;
  paste_pos = 0;
}

void Display::copySelection(MouseEvent& e, Selection& textsel) {
  textsel.copyText(copy_buffer);
  setSelectionOwner(e);
}

void Display::pasteSelection(MouseEvent& e, String* _paste_str, int _paste_pos) {
  // il faut effacer toutes les selections avant de faire un paste (sinon tout
  // va se melanger n'importe comment)  
  const UFlowList& flist = Application::getFlowList();
  for (UFlowList::const_iterator it = flist.begin(); it != flist.end(); ++it) {
    if (&(*it)->getDisp() == this && (*it)->getSelection()) 
      (*it)->getSelection()->clear();
  }
  
  // NB: pour interdire un improbable delete de paste_str avant
  // pasteSelectionCallback() paste_str est un unique_ptr<>
  paste_str = _paste_str;
  paste_pos = _paste_pos;
  pasteSelectionRequest(e);
}


Selection* Display::getChannelSelection(int _channel) {
  EventFlow* fl = getChannelFlow(_channel);
  if (!fl) return null;
  else return fl->getSelection();
}

EventFlow* Display::getChannelFlow(int _channel) const {
  UFlowList& flist = Application::impl.flowlist; 
  for (unsigned int k = 0; k < flist.size(); ++k) {
    if (&flist[k]->getDisp() == this && flist[k]->getChannel() == _channel)
      return flist[k];
  }
  return null;
}

EventFlow* Display::obtainChannelFlow(int _channel) {
  EventFlow* fl = getChannelFlow(_channel);
  if (!fl) fl = new EventFlow(*this, _channel);
  return fl;
}


void Display::countBits(unsigned long mask, int& bits, int& shift) {
  bool in_tab = true; shift = 0; bits = 0;
  while (mask != 0) {
    if (in_tab) {
      if ((mask & 0x1) == 0) shift++; else {in_tab = false; bits++;}
    }
    else {
      if ((mask & 0x1) == 0) break; else bits++;
    }
    mask = mask >> 1;
  }
}

// Fonts

UHardFont* Display::getFont(const FontDescription* f) { // !NOTE: may change the glcontext!
  if (!f) return null;
  makeDefaultContextCurrentIfNeeded();
   
  const FontFamily& ff = *(f->family);
  if (!ff.ready || ff.ffindex >= (signed)font_map.size() || !font_map[ff.ffindex] 
      || f->findex < 0 || !font_map[ff.ffindex][f->findex])
    return realizeFont(*f);
  else return font_map[ff.ffindex][f->findex];
}

void Display::realizeFontFamily(const FontFamily& fam) {
  if (fam.ffindex < 0) {      // new FontFamily
    fam.ffindex = fam.family_count++;
  }
  
  // increase the fontmap vector
  if (fam.ffindex >= (signed)font_map.size()) {
    for (int k = (signed)font_map.size(); k <= fam.ffindex; ++k) {
      font_map.push_back(null);
    }
  }
  
  // this font family has changed => remove entries in fontmap
  if (!fam.ready && font_map[fam.ffindex]) {    
    unsigned int fontmap_size = 4 * (fam.fsizes.size() + 1);
    UHardFont** fam_map = font_map[fam.ffindex];
    
    for (unsigned int k = 0; k < fontmap_size; ++k) {
      // delete if not shared
      if (fam_map[k] && fam_map[k]->count <= 1) delete fam_map[k];
    }
    delete [] fam_map;
    font_map[fam.ffindex] = null;
  }
  
  // note that the size vector may have been changed
  fam.initSizes();
  
  // allocates the corresponding font_map element
  if (!font_map[fam.ffindex]) {
    unsigned int fontmap_size = 4 * (fam.fsizes.size() + 1);
    UHardFont** fam_map = new UHardFont*[fontmap_size];
    
    for (unsigned int k = 0; k < fontmap_size; ++k) fam_map[k] = null;
    font_map[fam.ffindex] = fam_map;
  }
  
  fam.ready = true;
}


UHardFont* Display::realizeFont(const Font& font) {
  FontDescription f(font);
  return realizeFont(f);
}

UHardFont* Display::realizeFont(const FontDescription& _f) {
  const FontFamily* fam = _f.family;
  // on ne peut pas modifier _f directement car _f.family doit
  // rester null dans les cas des modifiers Font::small etc
  FontDescription f = _f;
  
  // sets and initializes the font family if needed
  if (!fam) f.family = fam = Application::conf.default_font->family;
  
  if (!fam->ready || fam->ffindex >= (signed)font_map.size() || !font_map[fam->ffindex])
    realizeFontFamily(*fam);
  
  if (f.findex < 0) f.setScale(1.);      // sinon index serait a -1 !
  
  UHardFont* nf = font_map[fam->ffindex][f.findex];
  if (nf && nf->isReady()) return nf;  // font already loaded
  
  // ALL fonts are created in the default GC of the display !!!
  // => fonts won't be usuable with other OpenGL GCs except those created in sharelists mode
  //if (current_rc != getDefaultRC()) getDefaultRC()->makeCurrent();
  makeDefaultContextCurrentIfNeeded();

  nf = new UHardFont(this, f);
  
  if (!nf->isReady()) delete nf;
  else return font_map[fam->ffindex][f.findex] = nf;  // DONE!
  
  // otherwise...
  String s = "Can't load font '"; s &= fam->name;
  if (f.styles & Font::BOLD) s &= " bold";
  if (f.styles & Font::ITALIC) s &= " italic";
  s &= "' (using alternate font)";
  Application::warning("Display::realizeFont", s.c_str());
  
  // try the same font without stylistic options
  if (f.styles != 0)  {
    FontDescription fd2 = f;
    fd2.styles = 0;
    fd2.setScale(1.);    // sinon fd2.index serait incorrect
    UHardFont* nf2 = getFont(&fd2);
    if (nf2 && nf2->isReady()) {
      nf2->count++;   // shared font
      return font_map[fam->ffindex][f.findex] = nf2;
    }
  }
  
  // try the default font 
  if (f.family->name != FontFamily::defaults.name)  {
    FontDescription fd2 = f;
    fd2.family = &FontFamily::defaults;
    //fd2.styles = 0;
    fd2.setScale(1.);    // sinon fd2.index serait incorrect
    UHardFont* nf2 = getFont(&fd2);
    if (nf2 && nf2->isReady()) {
      nf2->count++;  // shared font
      return font_map[fam->ffindex][f.findex] = nf2;
    }
  }
  
  // try another size
  const int LAST_RESORT_FONT_SIZE = 12;
  
  if (f.actual_size != LAST_RESORT_FONT_SIZE)  {
    FontDescription fd2 = f;
    fd2.styles = 0;
    fd2.def_size = LAST_RESORT_FONT_SIZE;
    fd2.setScale(1.);    // sinon fd2.index serait incorrect
    UHardFont* nf2 = getFont(&fd2);
    if (nf2 && nf2->isReady()) {
      nf2->count++;   // shared font
      return font_map[fam->ffindex][f.findex] = nf2;
    }
  }
  
  // plus aucun espoir...
  Application::fatalError("Display::realizeFont","can't load any appropriate font");
  return null;
}


EventFlow* Display::obtainFlow(unsigned int estate, int channel) {
  if (estate & UMS_EVENT_MASK) {                         // ???? !!! PBM si pas X11 ???
    // create a new flow or returns an existing flow if ID already used.
    return obtainChannelFlow(channel);
  }
  return Application::impl.disp->obtainChannelFlow(0);  // default
}

void Display::onResize(View* winview, const Dimension& size) {
  // ce test est pas bon car winview peut avoir ete change sans que onResize()
  // ait ete mis a jour (c'est le cas avec GLUT par exple)
  //if (winview->getWidth() != size.width || winview->getHeight() != size.height) {
    
    // different winview: purge requests on previous winview
    if (last_resize.winview != null && last_resize.winview != winview) {
      last_resize.count = 1;  // do it now
      onResizeCB(last_resize.winview);
      last_resize.winview = null;
      last_resize.count = 0;
    }
    
    last_resize.count++;
    last_resize.winview = winview;
    last_resize.newsize = size;
    Application::postpone(ucall(this, winview, &Display::onResizeCB));
  //}
}

void Display::onResizeCB(View* winview) {
  if (last_resize.winview == winview && last_resize.count == 1) {
    last_resize.winview = null;
    Dimension& size = last_resize.newsize;
    
    Window* win = winview->getWin();
    UHardwinImpl* hw = win->getHardwin(this);
    //cerr << "onResizeCB " <<win->hardImpl() << " "<< hw << endl;
    hw->doUpdateImpl(Update::LAYOUT, win, winview, &size);
    win->repaint();
  }
  last_resize.count--;
}

void Display::onPaint(View* winview, float x, float y, float w, float h) {
  if (last_resize.count == 0) {  
    Rectangle clip(x, y, w, h);
    winview->updatePaint(&clip);
  }
}

}
