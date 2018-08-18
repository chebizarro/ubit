/*
 *  font.cpp: Font Attribute
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
#include <ubit/ucall.hpp>
#include <ubit/uupdate.hpp>
#include <ubit/ui/updatecontext.h>
#include <ubit/ufont.hpp>
#include <ubit/draw/ufontImpl.hpp>
#include <ubit/uappli.hpp>
#include <ubit/ubox.hpp>
#include <ubit/uconf.hpp>
using namespace std;
namespace ubit {


static const float MEDIUM_FONT_SIZE = 12;


/* NOTES
 * - la specif 'normal' deconne sous certains Linux
 * - italique est tantot specifie par 'o' ou par 'i'
 * - fixed n'a pas forcement d'italique (ou alors d'une taille delirante)
static const FontSize FONT_SIZES[] = {
  {2,"2"}, 
  {6,"6"},        // XX_SMALL
  {7,"7"}, 
  {8,"8"},        // X_SMALL
  {9,"9"}, 
  {10,"10"},      // SMALL
  {11,"11"}, 
  {12,"12"},      // MEDIUM
  {14,"14"},      // LARGE
  //{15,"15"}, 
  {16,"16"},      // X_LARGE
  //{17,"17"}, 
  {18,"18"},
  //{19,"19"}, 
  {20,"20"},      // XX_LARGE
  // {22,"22"},   // word - degueu sur le Mac
  {24,"24"},      // word, X
  //{26,"26"},    // word - degueu sur le Mac
  //{28,"28"},    // word - degueu sur le Mac
  //{30,"30"},
  //{32,"32"},
  //{33,"33"},
  {34,"34"},      // X
  //{36,"36"},    // word - degueu sur le Mac
  {48,"48"},      // word
  {72,"72"}       // word
};
*/
//NB: FontFamily created in uconf.cpp to avoid init dependency problems 

Font Font::plain(null, -BOLD|-ITALIC, 0, UCONST); // NOT bold nor Italic
//Font& Font::normal = Font::plain;

Font Font::bold(null,      BOLD,      0, UCONST);
Font Font::italic(null,    ITALIC,    0, UCONST);
Font Font::fill(null,      FILL,      0, UCONST);
Font Font::underline(null, UNDERLINE, 0, UCONST);
Font Font::overline(null,  OVERLINE,  0, UCONST);
Font Font::strikethrough(null, STRIKETHROUGH, 0, UCONST);

Font Font::xx_small(null,  0, 6,  UCONST);
Font Font::x_small(null,   0, 8,  UCONST);
Font Font::small(null,     0, 10, UCONST);
Font Font::medium(null,    0, 12, UCONST);
Font Font::large(null,     0, 14, UCONST);
Font Font::x_large(null,   0, 16, UCONST);
Font Font::xx_large(null,  0, 20, UCONST);

Font Font::sans_serif(&FontFamily::sans_serif, 0,0, UCONST);
Font Font::serif(&FontFamily::serif, 0,0, UCONST);
Font Font::monospace(&FontFamily::monospace, 0,0, UCONST);
//Font Font::fixed(&FontFamily::fixed, 0,0, UCONST);

 
int FontFamily::family_count = 0;

FontFamily::FontFamily(const char* _name, UConst) {     // constr PRIVE
  // defs = defaults.defs; PAS pour ce constr PPRIVE
  ffindex = -1;
  ready = false;
  name = _name;
}

FontFamily::FontFamily(const String& _name) {   // constr PUBLIC
  defs = defaults.defs;  // recopier les defaults !
  ffindex = -1;
  ready = false;
  name = _name;
}

FontFamily& FontFamily::operator=(const FontFamily& f2) {
  defs = f2.defs;
  fsizes.clear();
  ready = false;
  return *this;
}

void FontFamily::setFamily(const char* _families) {
  defs.families = _families;
  ready = false;
}

void FontFamily::setStyles(const char* _normal_style, const char* _italic_style) {
  if (_normal_style) defs.normal_style = _normal_style;
  if (_italic_style) defs.italic_style = _italic_style;
  ready = false;
}
  
void FontFamily::setWeights(const char* _normal_weight, const char* _bold_weight) {
  if (_normal_weight) defs.normal_weight = _normal_weight;
  if (_bold_weight) defs.bold_weight = _bold_weight;
  ready = false;
}

void FontFamily::setCompression(const char* arg) {
  defs.compression = arg;
  ready = false;
}

void FontFamily::setEncoding(const char* arg) {
  defs.encoding = arg;
  ready = false;
}

void FontFamily::setPreferredSizes(const char* arg) {
  defs.preferred_sizes = arg;
  fsizes.clear();
  ready = false;
}

void FontFamily::setFreeType(const char* regular_files, 
                              const char* bold_files,
                              const char* italic_files, 
                              const char* bold_italic_files) {
  defs.fty_regular = regular_files;
  defs.fty_bold = bold_files;
  defs.fty_italic = italic_files;
  defs.fty_bold_italic = bold_italic_files;
  ready = false;
}

void FontFamily::initSizes() const {
  if (fsizes.size() > 0) return;
  const char* p1 = defs.preferred_sizes.c_str();
  char *p2 = 0;
  while (true) {
    long l = strtol(p1, &p2, 0);
    if (l == 0 || p2 == p1) break;
    fsizes.push_back(int(l));
    if (*p2 == ',') p2++;
    p1 = p2;
  }
}

//ATT: faut retourner la plus proche QUI EST PLUS PETITE !
// (si on retourne une taille plus grande le texte va etre clippe
//  ou ca va entrainer des passages a la ligne

int FontFamily::sizeToIndex(float size) const {
  if (size <= 0) return 0;
  if (fsizes.size() == 0) initSizes();

  // NB: CECI IMPLIQUE UNE LIMITE MAX DE TAILLE DES FONTS !
  if (size >= fsizes[fsizes.size()-1]) 
    return fsizes.size()-1;
  
  // NB: pas tres efficace!
  int smaller = 0;
  for (unsigned int n = 0; n < fsizes.size(); ++n) {
    if (fsizes[n] > size) break;
    smaller = n;
  }
  
  return smaller;
}


//private constr
Font::Font(const FontFamily* ff, int _styles, float pixsize, UConst m) : Attribute(m) {
  family = ff;
  if (pixsize <= 0) fsize = 0; else fsize = pixsize;
  if (_styles > 0) {on_styles = _styles; off_styles = 0;}
  else {on_styles = 0; off_styles = -_styles;}
}

Font::Font(const FontFamily& ff, int _styles, float pixsize) {
  family = &ff;
  if (pixsize <= 0) fsize = 0; else fsize = pixsize;
  if (_styles > 0) {on_styles = _styles; off_styles = 0;}
  else {on_styles = 0; off_styles = -_styles;}
}

Font::Font(const Font& f, int _styles, float pixsize) {
  family = f.family;
  if (pixsize <= 0) fsize = 0; else fsize = pixsize;
  if (_styles > 0) {on_styles = _styles; off_styles = 0;}
  else {on_styles = 0; off_styles = -_styles;}
}

Font::Font(const Font& f) {
  family = f.family;
  fsize = f.fsize;
  on_styles = f.on_styles;
  off_styles= f.off_styles;
}

Font::Font() {
  Font& f = *Application::conf.default_font;
  family = f.family;
  fsize = f.fsize;
  on_styles = f.on_styles;
  off_styles = f.off_styles;
}


void Font::set(const Font& f) {
  if (checkConst()) return;
  if (equals(f)) return;  
  family = f.family;
  fsize = f.fsize;
  on_styles = f.on_styles;
  off_styles = f.off_styles;
  changed(true);
}

void Font::merge(const Font& f) {
  if (checkConst()) return;
  
  if (f.family) family = f.family;
  if (f.fsize > 0) fsize = f.fsize;   // 0 means don't impose size

  on_styles  = on_styles  | f.on_styles;  // combines styles
  off_styles = off_styles | f.off_styles;
  changed(true);
}

bool Font::equals(const Font& f) const {
  if (this == &f) return true;
  else return (family == f.family && fsize == f.fsize
               && on_styles == f.on_styles && off_styles == f.off_styles);
}


/*
Font& Font::setFamily(const Font &f) {
  if (checkConst()) return *this;
  family = f.family;
  changed(true);
  return *this;
}
 Font& Font::setPixelSize(const Font &f) {
 if (checkConst()) return *this;
 size = f.size;
 changed(true);
 return *this;
 }
 Font& Font::setStyles(const Font &f) {
 if (checkConst()) return *this;
 on_styles  = f.on_styles;
 off_styles = f.off_styles;
 changed(true);
 return *this;
 }
*/
  
Font& Font::setFamily(const FontFamily& ff) {
  if (checkConst()) return *this;
  family = &ff;
  changed(true);
  return *this;
}

Font& Font::setFamily(const String& font_families) {
  if (checkConst()) return *this;
  //family = ...;
  changed(true);
  return *this;
}
/*
 void StyleProperties::setFontFamily(Font& f, const String& _val) {
 String v;
 int pos = _val.find(',');
 if (pos > 0) v = _val.substring(0, pos);
 else v = _val;
 v.trim();
 
 if (isEq(v,"sans-serif") || isEq(v,"helvetica") || isEq(v,"arial"))
 f.setFamily(Font::sans_serif.getFamily());
 else if (isEq(v,"serif") || isEq(v,"times"))
 f.setFamily(Font::serif.getFamily());
 else if (isEq(v,"monospace") || isEq(v,"courier"))
 f.setFamily(Font::monospace.getFamily());
 //else if (isEq(v,"fixed"))
 //  f.setFamily(Font::fixed);
 }
 
 */


Font& Font::setSize(const String& size) {
  if (checkConst()) return *this;
  float val = 0;
  const char* unit = null;
  if (!size.scanValue(val, unit)) return *this;   // no value specified

  if (!unit || !*unit || UCstr::equals(unit,"px",true))
    return setPixelSize(val); 
  else if (UCstr::equals(unit,"pt",true))
    return setPointSize(val); 
  else if (UCstr::equals(unit,"%",true))
    return setPointSize(MEDIUM_FONT_SIZE * val / 100.);
  else {
    error("Font::setSize","wrong font unit");
    return *this;
  }
}

Font& Font::setPixelSize(float s) {
  if (checkConst()) return *this;
  if (s <= 0) fsize = 0;    // 0 means don't impose a specific size
  else fsize = s;
  changed(true);
  return *this;
}

Font& Font::setPointSize(float s) {
  if (checkConst()) return *this;
  if (s <= 0) fsize = 0;
  else fsize = s * Application::getDisp()->PT_TO_PX;  // !!! devrait etre adapté au Display
  //cerr << "fsize " << fsize << " " << Application::getDisp()->PT_TO_PX<<endl;
  // Point: A unit of measure used by printers, equal to 1/72 inch.
  // EX: static const double POINT_TO_PIXEL = 1.33;  // conventionnel, a la louche
  changed(true);
  return *this;
}

float Font::getPointSize() const {
  return fsize / Application::getDisp()->PT_TO_PX;
}


Font& Font::setStyles(int s) {
  if (checkConst()) return *this;
  if (s > 0) {on_styles = s; off_styles = 0;}
  else {on_styles = 0; off_styles = -s;}
  changed(true);
  return *this;
}

Font& Font::changeStyles(int s, bool add) {
  if (checkConst()) return *this;
  if (s > 0) {
    off_styles = off_styles & ~s;
    if (add) on_styles = on_styles | s;
    else on_styles = on_styles & ~s;
  }
  else {
    s = -s;
    on_styles = on_styles & ~s;
    if (add) off_styles = off_styles | s;
    else off_styles = off_styles & ~s;
  }
  changed(true);
  return *this;
}


bool Font::isPlain()  const {return on_styles == 0;}
bool Font::isBold()   const {return (on_styles & BOLD) != 0;}
bool Font::isItalic() const {return (on_styles & ITALIC) != 0;}
bool Font::isFilled() const {return (on_styles & FILL) != 0;}
bool Font::isUnderlined() const {return (on_styles & UNDERLINE) != 0;}
bool Font::isOverlined()  const {return (on_styles & OVERLINE) != 0;}
bool Font::isStrikethrough() const {return (on_styles & STRIKETHROUGH) != 0;}

Font& Font::setBold(bool state) {
  return changeStyles(Font::BOLD, state);
}

Font& Font::setItalic(bool state) {
  return changeStyles(Font::ITALIC, state);
}

Font& Font::setFilled(bool state) {
  return changeStyles(Font::FILL, state);
}

Font& Font::setUnderlined(bool state) {
  return changeStyles(Font::UNDERLINE, state);
}

Font& Font::setOverlined(bool state) {
  return changeStyles(Font::OVERLINE, state);
}

Font& Font::setStrikethrough(bool state) {
  return changeStyles(Font::STRIKETHROUGH, state);
}


void Font::update() {
  updateAutoParents(Update::LAYOUT_PAINT);  // size changed in both directions
}

// This method initializes a Font for a given UDisplay (or Application)
// It returns true if the font could be found and false otherwise
// (a default font will be used in this case)
// NOTE: it is not necessary to call this function explicitely since
// version 99.10 (it will automatically be called the first time 
// this Font is used for drawing a string on the screen)
//
void Font::realize(Display* d) {
  if (!d) d = Application::getDisp();
  d->realizeFont(*this);
}


void Font::putProp(UpdateContext* props, Element&) {
  FontDescription& fd = props->fontdesc;
  if (family) fd.family = family;
  
  // ajouter les 'on_styles' et enlever les 'off_styles'
  fd.styles = (fd.styles | on_styles) & ~off_styles;

  //NB: size = 0 means "any size" and thus, does not change df.size 
  if (fsize > 0) fd.def_size = fsize;
  fd.setScale(props->xyscale);
}


FontDescription::FontDescription(const Font& f) {
  set(f);
}

void FontDescription::set(const Font& f) {
  family = f.family;
  styles = f.on_styles;
  def_size = f.fsize; 
  scaled_size = def_size;
  findex = -1; 
  actual_size = 0;  // NOT INIT!
}

void FontDescription::merge(const Font& f) {
  if (f.family) family = f.family;
  // NB: size = 0 means "any size" and thus, does not change f.size 
  if (f.fsize > 0) {def_size = f.fsize; scaled_size = def_size;}  
  styles = styles | f.on_styles;   // combine styles
  findex = -1; 
  actual_size = 0;  // NOT INIT!
}

void FontDescription::setScale(float xyscale) {
  if (def_size == 0) def_size = Font::medium.getPixelSize();
  scaled_size = def_size * xyscale;     // scaled pixel size
  if (!family) {
    Application::internalError("FontDescription::setScale","null font family; UFontDef: %p", this);
    family = Application::conf.default_font->family;
  }
  // find font index from scaled pixel size
  findex = family->sizeToIndex(scaled_size);
  actual_size = family->fsizes[findex];

  // calculate font index and actual rescaled point size
  findex = (findex << 2) + (styles & Font::BOLD) + (styles & Font::ITALIC);
}

}

