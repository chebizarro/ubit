/*
 *  hardfont.cpp
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


#include <cstdio>
#include <iostream>
#include <ubit/ubit_features.h>
#include <ubit/ustr.hpp>
#include <ubit/ufont.hpp>
#include <ubit/draw/ufontImpl.hpp>
#include <ubit/core/uappliImpl.hpp>
#include <ubit/udispX11.hpp>
#include <ubit/uhardfont.hpp>

#if UBIT_WITH_GL && UBIT_WITH_FREETYPE
#    include <FTGL/ftgl.h>  // FTGL
#    include <FTGL/FTFont.h>
#    include <FTGL/FTGLTextureFont.h>
#    include <FTGL/FTGLPolygonFont.h>
#    include <FTGL/FTGLPixmapFont.h>
#endif

namespace ubit {

UHardFont::UHardFont(Display* nd, const FontDescription& fd)  // glcontext dependent!!!
: status(NO_FONT), count(1)
#if WITH_2D_GRAPHICS
  ,sysf(0)
#endif
#if UBIT_WITH_GL
  ,ftf(null)
#endif
{
  if (Application::conf.is_using_freetype) {
#ifdef UBIT_WITH_GL
    ftf = loadFTGLFont(nd, fd);     // glcontext dependent!!!
    if (ftf) status = FTGL_FONT;
#else
    Application::error("UHardFont::UHardFont","FreeType requires OpenGL");
#endif
    return;
  }

#if WITH_2D_GRAPHICS
  sysf = loadSysFont(nd, fd);
  if (sysf) status = SYS_FONT;
  else return;

#if UBIT_WITH_X11 && UBIT_WITH_GL
  if (Application::isUsingGL()) {
    //glf = glGenLists(256);
    //glXUseXFont(sysf->fid, 0, 255, glf);
    glf = glGenLists(sysf->max_char_or_byte2 + 1);   // glcontext dependent!!!
    if (glf == 0) {
      Application::error("UHardFont::UHardFont","out of display lists");
      return;
    }
    glXUseXFont(sysf->fid, sysf->min_char_or_byte2, 
                sysf->max_char_or_byte2 - sysf->min_char_or_byte2 +1, 
                glf + sysf->min_char_or_byte2);
    status = GLX_FONT;
  }
#endif
#endif
}

UHardFont::~UHardFont() {     // glcontext dependent!!!
#if UBIT_WITH_GL
  if (status == FTGL_FONT) delete ftf;
  if (glf) glDeleteLists(glf, 256);
#endif
}


float UHardFont::getAscent() const {
#if UBIT_WITH_GL
  if (Application::conf.is_using_freetype) return ftf->Ascender();
#endif
#if WITH_2D_GRAPHICS
  return sysf->ascent;
#endif
  return 0; // toolkit compled with improper options
}

float UHardFont::getDescent() const {
#if UBIT_WITH_GL
  if (Application::conf.is_using_freetype) return -ftf->Descender();  //att: Descender() is < 0 !
#endif
#if WITH_2D_GRAPHICS
  return sysf->descent;
#endif
  return 0; // toolkit compled with improper options
}

float UHardFont::getHeight() const {
#if UBIT_WITH_GL
  if (Application::conf.is_using_freetype) return ftf->Ascender() - ftf->Descender(); //att: Descender() is < 0 !
#endif
#if WITH_2D_GRAPHICS
  return sysf->ascent + sysf->descent;
#endif
  return 0; // toolkit compled with improper options
}

#if (UBIT_WITH_X11 && WITH_2D_GRAPHICS)

// va savoir pourquoi: parfois default_char vaut n'importe quoi !
static int CharDefaultWidth(XFontStruct* f) {
  if (!f->per_char) return f->min_bounds.width;
  if (f->default_char >= f->min_char_or_byte2  && f->default_char <= f->max_char_or_byte2)
    return f->per_char[f->default_char].width;
  return f->per_char[0].width;
}

static int CharWidth(XFontStruct* f, char _c) {
  // !att: il FAUT unsigned char sinon valeur < 0 pour lettres accentuees
  unsigned char c = _c;
  if (f->per_char && c >= f->min_char_or_byte2 && c <= f->max_char_or_byte2) {
    XCharStruct* cs = &(f->per_char[c - f->min_char_or_byte2]);
    if (cs) return cs->width; else return CharDefaultWidth(f);
  }
  else return CharDefaultWidth(f);
}

#endif
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/* FTGL doc:
* void BBox( const char* string, float& llx, float& lly, float& llz, float& urx, float& ury, float& urz);
* 
* Get the bounding box for a string.
* @param string    a char string
* @param llx       lower left near x coord
* @param lly       lower left near y coord
* @param llz       lower left near z coord
* @param urx       upper right far x coord
* @param ury       upper right far y coord
* @param urz       upper right far z coord
*/
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

float UHardFont::getWidth(char c) const {
#if UBIT_WITH_GL
  if (Application::conf.is_using_freetype) {
    char s[2];
    s[0] = c;
    s[1] = 0;
    return ftf->Advance(s);   // glcontext dependent!!!
  }
#endif
  
#if WITH_2D_GRAPHICS
# if UBIT_WITH_X11
  return CharWidth(sysf, c);
# endif
#endif
  
  return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

float UHardFont::getWidth(const char* s, int len) const {
#if UBIT_WITH_GL
  if (Application::conf.is_using_freetype) {
    return ftf->Advance(s, len);  // ELC: 'len' rajoute a Advance()
  }
#endif
  
#if WITH_2D_GRAPHICS
#if UBIT_WITH_X11
  return XTextWidth(sysf, s, len);
# endif
#endif
  return 0;   // GL and FreeType required
}

/* ==================================================== [Elc] ======= */

static bool getNext(String& item, String& list) {
  int pos = list.find(',');
  if (pos < 0) {
    item = list;
    list = "";
  }
  else {
    item = ""; item.append(list, pos);
    list.remove(0, pos+1);
    }
  item.trim();
  return !item.empty();
}

#if WITH_2D_GRAPHICS

XFontStruct* UHardFont::loadSysFont(Display* nd, const FontDescription& fd) {
  if (!fd.family) return null;
  const FontFamily* ff = fd.family;
  String family, families = ff->defs.families;
  
  while ((getNext(family, families))) {
    
    String weight, weights = 
    (fd.styles & Font::BOLD) ? ff->defs.bold_weight : ff->defs.normal_weight;

    while ((getNext(weight, weights))) {

      String style, styles =
      (fd.styles & Font::ITALIC) ? ff->defs.italic_style : ff->defs.normal_style;
      
      while ((getNext(style, styles))) {
        char font_name[500];
        // exple: "-*-times-medium-r-normal-*-12-*-*-*-*-*-*-1",
        sprintf(font_name, "-*-%s-%s-%s-%s-*-%d-*-*-*-*-*-*-%s",
                family.c_str(), weight.c_str(), style.c_str(), 
                ff->defs.compression.c_str(), fd.actual_size, ff->defs.encoding.c_str());

#if UBIT_WITH_X11
        XFontStruct* fs = null;
        fs = XLoadQueryFont(((UDispX11*)nd)->getSysDisp(), font_name);
#else
        Application::fatalError("UHardFont::loadSysFont","this function requires the Ubit toolkit to be compiled with X11 or GDK");
        fs = null; // !!!
#endif
        if (fs) return fs;
      }
    }
  }
  return null;
}

#endif
#if UBIT_WITH_GL

// FTGLBitmapFont / FTGLPixmapFont / FTGLExtrdFont /  FTGLOutlineFont * 
// FTGLPolygonFont plante: FTVectoriser::MakeMesh : GLU_TESS is not available 
// FTGLTextureFont 

// !!!!!!!!!!!!!!!!!  GESTION COUTEUSE ??? A REVOIR !!!! @@@@  !!!!!!!!!!!!!!!!!!!!!!!!

FTFont* UHardFont::loadFTGLFont(Display* nd, const FontDescription& fd) {
#if !defined(UBIT_WITH_FREETYPE)
  Application::fatalError("UHardFont::loadFTGLFont",
                     "FreeType font requested but Ubit was compiled without FreeType");
  return null;
  
#else
  if (!fd.family) return null;
  const FontFamily* ff = fd.family;
  String fnames;
  //int card = 0;
  //char ** list = XGetFontPath(nd->getSysDisp(), &card);
  //for (int k = 0; k < card; k++) cerr << list[k] << endl;
  //if (list) XFreeFontPath(list);
  
  if (fd.styles & Font::ITALIC) {
    if (fd.styles & Font::BOLD) fnames = ff->defs.fty_bold_italic;
    else  fnames = ff->defs.fty_italic;
  }
  else {
    if (fd.styles & Font::BOLD) fnames = ff->defs.fty_bold;
    else  fnames = ff->defs.fty_regular;
  }
  
  String fname;
  while ((getNext(fname, fnames))) {
    FTFont* f = (FTFont*)new FTGLTextureFont(fname.c_str());
    if (f->Error() != 0) delete f;
    else {
      f->FaceSize(fd.actual_size);
      return f;
    }
  }
  
  // not found!
  const char* font_name = fd.family->name.empty() ? "[Unknown]" : fd.family->name.c_str();
  Application::warning("UHardFont","Can't find FreeType font: %s", font_name);
  return null;
#endif
}

#endif
/* ==================================================== [Elc] ======= */

void UHardFont::drawString(const char* s, int len, float x, float y) const { // glcontext dependent!!!
#if UBIT_WITH_GL
  if (status == FTGL_FONT) {
#ifdef UBIT_WITH_FREETYPE
    // dans le cas de FTGL le glPushAttrib pourrait etre compacte
    //glPushAttrib(GL_ENABLE_BIT | GL_LIST_BIT | GL_CURRENT_BIT);  // GL_LINE_STIPPLE
    glEnable(GL_TEXTURE_2D); //indispensable ici!
    
    //glPushMatrix();    
    //EX: we cannot use glTranslate() because glClipPlane was called before
    //    => pen now contains absolute (x,y) coordinates
    //EX: glTranslatef(x, y, 0);

    ftf->Render(s, len, FTPoint(x, y));
    
    //glPopMatrix();
    
    glDisable(GL_TEXTURE_2D);
    //glPopAttrib();
    return;
#endif
  }
  
  // Freetype not available: use X11 Fonts
  if (status == GLX_FONT) {
    //glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT);
    glPushAttrib(GL_LIST_BIT);
    //if (alpha_graph) glEnable(GL_BLEND);
    glRasterPos2f(x, y);
    glListBase(glf);
    glCallLists(len, GL_UNSIGNED_BYTE, s);
    //glDisable(GL_BLEND);
    glPopAttrib();
    return;
  }
#endif // UBIT_WITH_GL
}  

}