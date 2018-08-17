/*
 *  fontmetrics.cpp
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2008 | Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
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

#include <ubit/ubit_features.h>
#include <iostream>
#include <ctype.h>
#include <ubit/core/node.h>
#include <ubit/ufont.hpp>
#include <ubit/ufontmetrics.hpp>
#include <ubit/draw/ufontImpl.hpp>
#include <ubit/ustr.hpp>
#include <ubit/ugraph.hpp>
#include <ubit/uwin.hpp>
#include <ubit/ui/updatecontext.h>
#include <ubit/uappli.hpp>
#include <ubit/uhardfont.hpp>

namespace ubit {


// !!!CAUTION: disp->getFont() uses the default GC if it actually creates a new Font
// in OpenGL mode, the corresponding glcontext becomes the current context

#define NULL_FONT_ERROR(msg) Application::error(msg,"can't retreive font")

FontMetrics::FontMetrics() 
: disp(null), fd(null), own_fd(false) {}

FontMetrics::FontMetrics(const Font& _f, Display* _d) {
  set(_f, _d);
}

FontMetrics::FontMetrics(const FontDescription& _fd, Display* _d) {
  set(_fd, _d);
}

FontMetrics::FontMetrics(UpdateContext& _ctx) : 
disp(_ctx.getDisp()), 
fd(&_ctx.fontdesc), 
own_fd(false) {
  UHardFont* f = disp->getFont(fd);  // see note about getFont above
  if (!f) {
    Application::error("FontMetrics","can't retreive font");
  }
}

FontMetrics::~FontMetrics() {
  if (own_fd) delete &fd;
}


void FontMetrics::set(const Font& _f, Display* _d) {
  disp = (_d ? _d : Application::getDisp());
  fd = new FontDescription(_f);
  own_fd = true;
  UHardFont* f = disp->getFont(fd);
  if (!f) Application::error("FontMetrics","can't retreive font");
}

void FontMetrics::set(const FontDescription& _fd, Display* _d) {
  disp = (_d ? _d : Application::getDisp());
  fd = &_fd; 
  own_fd = false;
  UHardFont* f = disp->getFont(fd);
  if (!f) Application::error("FontMetrics","can't retreive font");
}


float FontMetrics::getWidth(char c) const {
  UHardFont* f = disp->getFont(fd);
  if (!f) return 0;
  return f->getWidth(c);
}

float FontMetrics::getWidth(const char* s, int len) const {
  UHardFont* f = disp->getFont(fd);
  if (!f) return NULL_FONT_ERROR("FontMetrics::getWidth"), 0;
  return f->getWidth(s, len);
}

float FontMetrics::getWidth(const String& s) const {
  return getWidth(s.c_str(), -1);
}

float FontMetrics::getWidth(const String& s, int pos, int len) const {
  return getWidth(s.c_str() + pos, len);
}


float FontMetrics::getAscent() const {
  UHardFont* f = disp->getFont(fd);
  if (!f) return NULL_FONT_ERROR("FontMetrics::getAscent"), 0;
  return f->getAscent();
}

float FontMetrics::getDescent() const {
  UHardFont* f = disp->getFont(fd);
  if (!f) return NULL_FONT_ERROR("FontMetrics::getDescent"), 0;
  return f->getDescent();
} 

float FontMetrics::getHeight() const {
  UHardFont* f = disp->getFont(fd);
  if (!f) return NULL_FONT_ERROR("FontMetrics::getHeight"), 0;
  return f->getHeight();
}

// - - - - - - - - - - - - - - -
/*
 bool FontMetrics::getCharDefaultSize(const Font& font, int& w, int& h) const{
 FontDescription fd(font);
 return getCharDefaultSize(fd, w, h);
 }
 
 bool Graph::getCharDefaultSize(const FontDescription& font, int& w, int& h) const {
 if (!wg) return false;
 USysFont f = wg->disp->natdisp->getFont(font);
 if (!f) {
 w = h = 0;
 return false;
 }
 w = CharDefaultWidth(f);
 h = f->ascent + f->descent;
 return true;
 }
 */
/* ==================================================== [Elc] ======= */

int FontMetrics::getCharPos(const char* s, int len, float x) const {
  UHardFont* f = disp->getFont(fd);
  if (!f) return 0;
  
  int charpos = 0;
  float xpos = 0;
  
  for (charpos = 0; charpos < len; ++charpos) {
    char c = s[charpos];
    float cw = f->getWidth(c);
    if (x <= xpos + cw / 2)  break;
    else xpos += cw;
  }
  
  // si on tombe sur un separateur a la fin
  // alors il faut renvoyer position precedente car ce separateur n'est 
  // pas affiche car il y a un saut de ligne implicite
  
  // bug corrige: le test charpos==len manquait
  if (len > 0 && charpos==len && isspace(s[len-1])) charpos--;
  return charpos;
}

// - - - - - - - - - - - - - - -

float FontMetrics::getXPos(const char *s, int len, int charpos) const {
  UHardFont* f = disp->getFont(fd);
  if (!f) return 0;
  
  if (charpos > len || charpos < 0) charpos = len;
  float xpos = 0;
  
  for (int k = 0; k < charpos; ++k) {
    //char c = s[k]; float cw = f->getWidth(c); xpos += cw;
    xpos += f->getWidth(s[k]);
  }
  return xpos;
}


bool FontMetrics::
getSubTextSize(const char* s, int len, Dimension& dim, float available_width,
               int& s_sublen, int& change_line) const {
  dim.width = dim.height = 0;
  s_sublen = 0;
  change_line = 0;
  
  UHardFont* f = disp->getFont(fd);
  if (!f) {
    Application::error("FontMetrics::getSubTextSize","can't retreive font");
    return false;
  }
  
  //int cw_moyen = (f->min_bounds.width + f->max_bounds.width) / 2;
  dim.height = f->getHeight();
  int last_pos = -1;
  float lw = 0, last_lw = 0;
  
  for (int pos = 0; pos < len; ++pos) {
    char c = s[pos];
    //float cw = f->getWidth(c);    //int cw;
    //lw += cw;
    lw += f->getWidth(c);
    
    if (c == '\n') {
      dim.width = lw;
      s_sublen = pos+1;
      change_line = 2;
      return true;
    }
    
    else if (isspace(c)) {
      last_lw = lw;
      last_pos = pos;
    }
    
    else if (lw > available_width) {
      if (last_pos >= 0) {
        dim.width = last_lw;
        s_sublen = last_pos+1;
        change_line = 1;
        return true;
      }
    }
  }
  
  // sinon: tout retourner
  dim.width = lw;
  s_sublen = len;
  change_line = (lw > available_width ? 1 : 0);
  return true;
}

// utilise par drawString en mode OpenGL pour eviter de dessiner
// ce qui sort de la zone de clipping

bool FontMetrics::getClippedText(const UHardFont* f, float clip_x, float clip_width,
                                  const char* s, int len, float x0, float y0,
                                  int& charpos_begin, int& charpos_end,
                                  float& xpos_begin, float& xpos_end) {
  charpos_begin = -1; // -1 means undef or invalid
  charpos_end = -1;
  xpos_begin = 0;
  xpos_end = 0;
  if (!f) return false;
  
  float xc = x0;  
  for (int pos = 0; pos < len; pos++) {
    if (charpos_begin < 0 && xc >= clip_x) {
      charpos_begin = pos; 
      xpos_begin = xc;
    }
    char c = s[pos];
    float cw = f->getWidth(c); 
    xc += cw;
    // <= sinon le dernier char est jete dans certains cas
    if (xc <= clip_x + clip_width + 1) {  // +1 mandatory, wrong clipping otherwise
      charpos_end = pos; 
      xpos_end = xc;
    }
    else break;   //1nov07
  }
  
  return true;
}

}
/* ==================================================== [Elc] ======= */
