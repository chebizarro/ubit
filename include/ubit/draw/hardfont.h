/*
 *  hardfont.hpp
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


#ifndef UBIT_UHARDFONT_HPP
#define UBIT_UHARDFONT_HPP

#include <ubit/ubit_features.h>

#if UBIT_WITH_X11
#  include <X11/Xlib.h> 
#endif

class FTFont;

namespace ubit {
  
/** [Impl] Native Font.
*/
class UHardFont {
public:
  enum {NO_FONT=0, SYS_FONT, GLX_FONT, FTGL_FONT};

  UHardFont(Display*, const FontDescription&);
  ~UHardFont();

  void  drawString(const char* str, int str_len, float x, float y) const;
  
  bool  isReady() {return status > NO_FONT;}
  int   getStatus() {return status;}
  float getAscent() const;
  float getDescent() const;
  float getHeight() const;
  float getWidth(char) const;  
  float getWidth(const char* str, int len = -1) const;

private:
  friend class Display;
  friend class Graph;
  short status, count;

#if UBIT_WITH_GL
  union {
    unsigned int glf;  //GLuint  glf;
    FTFont* ftf;
  };
  FTFont* loadFTGLFont(Display*, const FontDescription&);
  ///< loads a FTGL font; requires OpenGL and TrueType.
#endif

#if UBIT_WITH_X11
  friend class UX11context;
  XFontStruct* sysf;
  XFontStruct* loadSysFont(Display*, const FontDescription&);
#endif
};

}
#endif // UBIT_UHARDFONT_HPP

