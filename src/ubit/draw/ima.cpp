/*
 *  uima.cpp: images
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
#include <list>
#include <iostream>
#include <cstring>
#include <ubit/ufile.hpp>
#include <ubit/ucall.hpp>
#include <ubit/uupdatecontext.hpp>
#include <ubit/ugraph.hpp>
#include <ubit/uwin.hpp>
#include <ubit/uview.hpp>
#include <ubit/ucolor.hpp>
#include <ubit/ustr.hpp>
#include <ubit/uima.hpp>
#include <ubit/upix.hpp>
#include <ubit/core/event.h>
#include <ubit/uappli.hpp>
#include <ubit/ubox.hpp>
#include <ubit/uupdate.hpp>
#include <ubit/uconf.hpp>
#include <ubit/uhardima.hpp>
using namespace std;
namespace ubit {


Image::Image(const char* file_name, bool load_now) {
  name = null;
  show_unknown_ima = false;
  setImpl(file_name);
  if (load_now) loadNow();
}

Image::Image(const String& _filename, bool load_now) {
  name = null;
  show_unknown_ima = false;
  setImpl(_filename.c_str());
  if (load_now) loadNow();
}

Image::Image(const char** xpm_data, bool load_now) {
  name = null;
  show_unknown_ima = false;
  setImpl(xpm_data);
  if (load_now) loadNow();
}

Image::Image(const char** xpm_data, UConst m) : Data(m) {
  name = null;
  show_unknown_ima = false;
  setImpl(xpm_data);
}

Image::Image(int w, int h) {
  name = null;
  show_unknown_ima = false;
  setImpl(w, h);
}

Image::~Image() {
  if (name) free(name);
  cleanCache();
}

void Image::cleanCache() {
  // NB: on ne detruit pas data (data n'est pas copie et 
  // pointe donc sur l'original)
  for (list<UHardIma*>::iterator p = natimas.begin(); p != natimas.end(); p++) {
    delete (*p);
  }
  natimas.clear();
}

/* ==================================================== [Elc] ======= */

void Image::update() {
  _parents.updateAutoParents(Update::LAYOUT_PAINT);
}

//const char* Image::getNameChars() const {return name;}

bool Image::isRealized() const {
  return (natimas.empty() ? false : (*natimas.begin())->isRealized());
}

int Image::getHeight() const {
  return (natimas.empty() ? 0 : (*natimas.begin())->getHeight());
}

int Image::getWidth() const {
  return (natimas.empty() ? 0 : (*natimas.begin())->getWidth());
}

int Image::getBpp() const {
  return (natimas.empty() ? 0 : (*natimas.begin())->getBpp());
}

int Image::getTransparency() const {
  return (natimas.empty() ? 0 : (*natimas.begin())->getTransparency());
}

/*
unsigned long Image::getPixel(int x, int y) {
  return natimas.empty() ? 0 : (*natimas.begin())->getPixel(x, y);
}

void Image::setPixel(unsigned long p, int x, int y) {
  if (!natimas.empty()) (*natimas.begin())->setPixel(p, x, y);
}
*/
/* ==================================================== [Elc] ======= */

Image& Image::operator=(const Image& ima2) {
  if (checkConst()) return *this;
  if (&ima2 == this) return *this;
  
  setImpl((char*)null);
  name = ima2.name ? UCstr::dup(ima2.name) : null;
  stat = ima2.stat;

  if (ima2.natimas.empty()) {
    changed(true);
    return *this;
  }
  
  natimas.push_back((*ima2.natimas.begin())->createScaledClone(1.,1.));
  changed(true);
  return *this;
}


void Image::resize(int max_width, int max_height, bool preserve_ratio, bool dont_magnify) {
  if (checkConst()) return;// false;
  if (natimas.empty()) {
    if (stat==0) loadNow();
  }
  if (natimas.empty()) return;// false;
  if (getWidth()<=0 || getHeight()<=0) return;// false;

  float xscale = (max_width<=0)  ? 1. : (float)max_width/getWidth();
  float yscale = (max_height<=0) ? 1. : (float)max_height/getHeight();
  if (preserve_ratio) {
    float xyscale = min(xscale, yscale);   // preserve aspect ratio
    xscale = yscale = xyscale;
  }

  // case preserve_smaller : resize if ima is too large
  if (!dont_magnify || (xscale < 1. && yscale < 1.)) {
    UHardIma* ni = (*natimas.begin())->createScaledClone(xscale,yscale);
    cleanCache();
    natimas.push_back(ni);
    changed(true);
  }
  //return true;
}


bool Image::rescale(float xscale, float yscale) {
  if (checkConst()) return false;
  if (natimas.empty()) {
    if (stat==0) loadNow();
  }
  if (natimas.empty()) return false;

  UHardIma* ni = (*natimas.begin())->createScaledClone(xscale,yscale);
  cleanCache();
  natimas.push_back(ni);
  changed(true);
  return true;
}


int Image::read(const String& fname, int max_w, int max_h) {
  setImpl(fname.c_str());
  realize(max_w, max_h);
  // ne fait rien si UHardwin::read() tient compte du max pour ce fichier
  if (max_w > 0 || max_h > 0) resize(max_w, max_h, true, true);
  changed(true);  // *apres* le realize
  return stat;
}

int Image::read(const char* fname, int max_w, int max_h) {
  setImpl(fname);
  realize(max_w, max_h);
  // ne fait rien si natima::read() tient compte du max pour ce fichier
  if (max_w > 0 || max_h > 0) resize(max_w, max_h, true, true);
  changed(true); // *apres* le realize
  return stat;
}

int Image::loadNow() {
  if (!name && !data) return stat = UFilestat::CannotOpen;
  realize(0,0);
  changed(true);  // *apres* le realize
  return stat;
}

int Image::loadFromData(const char** xpm_data) {
  setImpl(xpm_data);
  realize(0,0);
  changed(true); // *apres* le realize
  return stat;
}


void Image::set(const String& fname) {
  if (checkConst()) return;
  setImpl(fname.c_str());
  changed(true);
}

void Image::set(const char* fname) {
  if (checkConst()) return;
  setImpl(fname);
  changed(true);
}

void Image::set(const char** xpm_data) {
  if (checkConst()) return;
  setImpl(xpm_data);
  changed(true);
}


void Image::setImpl(const char* fname) {
  cleanCache();
  if (name) free(name);
  name = fname ? UCstr::dup(fname) : null;
  data = null;
  stat = UFilestat::NotOpened;
  mode = READ_FROM_FILE;
}

void Image::setImpl(const char** xpm_data) {
  cleanCache();
  if (name) free(name);
  name = null;
  data = xpm_data;  //!!att: NOT copied!
  stat = UFilestat::NotOpened;
  mode = READ_FROM_DATA;
}

// ATT: pas pret pour OpenGL!! (a cause de createImage)
void Image::setImpl(int w, int h) {
  cleanCache();
  if (name) free(name);
  name = null;
  data = null;
  stat = UFilestat::NotOpened;
  mode = CREATE;
  UHardIma* ni = null;
  
  if (w > 0 && h > 0) {
#if UBIT_WITH_GL
    if (Application::isUsingGL()) {
      ni = new UHardImaGL(Application::getDisp(), w, h);
      goto DONE;
    }
#endif
#if WITH_2D_GRAPHICS
    ni = new UHardIma2D(Application::getDisp(), w, h);
    goto DONE;   // just to avoid a compiler warning
#endif
  }
  
  DONE: if (ni) {
    stat = UFilestat::Opened;
    natimas.push_back(ni);
  }
}

/* ==================================================== [Elc] ======= */

void Image::getSize(UpdateContext& ctx, Dimension& dim) const {
  Display* d = ctx.getDisp();

  if (stat == UFilestat::NotOpened && natimas.empty()) {
    realize(0 ,0, d, false);
  }

  // !!ATTENTION: l'image peut etre partage par plusieurs objets,
  // d'ou la necessite d'initialiser TOUS les links correctement
  // meme si l'image en elle-meme n'est charge qu'une seule fois

  if (natimas.empty()) {
    if (show_unknown_ima) {            // empecher recursion infinie
      if (this != d->getConf().unknow_image) {
        d->getConf().unknow_image->getSize(ctx, dim);
      }
      else dim.width = dim.height = 0;
    }
    else dim.width = dim.height = 0;
  }

  else {
    // it's useless to rescale the image at this stage : we'll
    // do it when the image is actually painted (hence hidden images,
    // for instance clipped images, wont be rescaled)
    UHardIma* ni = (*natimas.begin());
    //dim.width  = ni->getWidth()  * ctx.xyscale + 0.5;   // ex int()
    //dim.height = ni->getHeight() * ctx.xyscale + 0.5;
    dim.width  = ni->getWidth()  * ctx.xyscale;   // ex int()
    dim.height = ni->getHeight() * ctx.xyscale;

  }
}

/* ==================================================== [Elc] ======= */

void Image::paint(Graph& g, UpdateContext& props, const Rectangle& r) const {
  if (natimas.empty()) {
    if (show_unknown_ima) {                //empecher recursion infinie
      if (this != g.getDisp()->getConf().unknow_image) 
        g.getDisp()->getConf().unknow_image->paint(g, props, r);
    }
    //else nop;
    return;
  }
  
  g.drawIma(*this, r.x, r.y, props.xyscale);

/*
#if UBIT_WITH_GL
  if (Application::isUsingGL()) {
    // une seule natima dans ce cas (la premiere)
    // ATT: on ne gere pas les multidisp             !!!!!!!
    if (!natimas.empty()) {
      UHardIma* ni = *(natimas.begin());
      if (ni) g.drawNatIma(*(UHardImaGL*)ni, r.x, r.y, props.xyscale);
    }
    return;
  }
#endif
#if WITH_2D_GRAPHICS
  UHardIma* ni = getOrCreateIma(g.getDisp(), props.xyscale);
  if (ni) g.drawNatIma(*ni, r.x, r.y);
#endif
*/
}

/* ==================================================== [Elc] ======= */
#if WITH_2D_GRAPHICS

UHardIma* Image::getOrCreateIma(Display* d, float xyscale) const {
  UHardIma* ni = findImaInCache(d, xyscale);
  if (!ni) ni = addImaInCache(d, xyscale);
  return ni;
}

UHardIma* Image::findImaInCache(Display* d, float xyscale) const {
  // search in the cache if this image was already created with the same size
  // AND THE SAME DISPLAY !!
  for (list<UHardIma*>::iterator p = natimas.begin(); p != natimas.end(); ++p) {
    if ((*p)->getScale() == xyscale && (*p)->disp == d)
      return *p;
  }
  return null; // not found
}

// we destroy all copies except the 1st one which is the original image
// and the last one as it is likely to be used again soon

UHardIma* Image::addImaInCache(Display* d, float xyscale) const {
  if (natimas.empty()) return null;
  
  list<UHardIma*>::iterator p = natimas.begin();
  p++;                                // skip 1st = the original
  while (p != natimas.end()) {
    if ((*p)->disp != d) p++;
    else { 
      list<UHardIma*>::iterator p2 = p; p2++;
      if (p2 == natimas.end()) p++;    // dont destroy the last one !!
      else {
        delete *p;
        natimas.erase(p);
        p = p2;
      }
    }
  }

  // NB image depth must be <= to the NatDisp depth
  UHardIma* ni = (*natimas.begin())->createScaledClone(xyscale, xyscale, d);
  if (ni) {
    if (Application::isUsingGL()) {
      Application::internalError("Image::addImaInCache","invalid when using OpenGL");
      return null;
    }
    ((UHardIma2D*)ni)->scale = xyscale;   // !! att au cast !
    natimas.push_back(ni);
  }

  return ni; 
}

#endif
/* ==================================================== [Elc] ======= */

void Image::getFullPath(String& path, const char* name) {
  if (!name || name[0] == '\0')
    path = "";
  else if (name[0] == '.' || name[0] == '/')
    path = name;
  else       // no prefix ==> add imap_path
    path = Application::getImaPath() & name;
}

int Image::realize(int max_w, int max_h, Display* disp, bool force_reload) const {
  if (!disp) disp = Application::getDisp();
  // ne pas essayer de recharger une seconde fois quelque soit
  // le resultat de cet essai (pour ne pas boucler sur le load)
  if (force_reload || (stat == UFilestat::NotOpened && natimas.empty())) {
    UHardIma* ni = null;
    
#if UBIT_WITH_GL
    if (Application::isUsingGL()) {
      ni = new UHardImaGL(disp, 0, 0);
      goto DONE;
    }
#endif
#if WITH_2D_GRAPHICS
    ni = new UHardIma2D(disp, 0, 0);
    goto DONE;   // just to avoid a compiler warning
#endif

  DONE:
    if (mode == READ_FROM_DATA) {
      if (!data) stat = UFilestat::InvalidData;
      else {
        stat = ni->readFromData(data, max_w, max_h);
        if (stat <= 0) delete ni; else natimas.push_back(ni);
      }
    }
    
    else if (mode == READ_FROM_FILE) {
      String fpath;
      getFullPath(fpath, name);
      stat = ni->read(fpath, null, max_w, max_h);
      if (stat <= 0) delete ni; else natimas.push_back(ni);
    }
    
    /*
    else if (mode == CREATE) {
      ... dans setImpl();
    }
    */
    
    else delete ni;  // ni cree inutilement
  }

  return stat;
}

}
