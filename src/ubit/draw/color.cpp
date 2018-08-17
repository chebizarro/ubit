/*
 *  color.cpp: Color Attribute
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
#include <map>
#include <iostream>
#include <climits>
#include <cstdio>   // sscanf()
#include <ubit/ucall.hpp>
#include <ubit/uupdate.hpp>
#include <ubit/ui/updatecontext.h>
#include <ubit/ustr.hpp>
#include <ubit/ubox.hpp>
#include <ubit/ucolor.hpp>
#include <ubit/uappli.hpp>
#include <ubit/ubox.hpp>
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

#define UNSET_PIXEL ULONG_MAX
enum {RGBA_COLOR = 1, NONE_COLOR = 8, INHERIT_COLOR = 9};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct NamedColor {
  const char* colname;
  unsigned char r, g, b;
};
  
class NamedColorMap {
public:
  NamedColorMap();
  void add(const NamedColor&);  // adds or replaces
  const NamedColor* find(const char* colorname) const;
private:
  struct Comp {
    bool operator()(const char*a, const char*b) const {return UCstr::compare(a,b,true) < 0;}
  };
  typedef std::map<const char*, const NamedColor*, Comp> Map;
  Map map;
};
  
static NamedColorMap* named_colors = null;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Rgba::setRgbaF(float r, float g, float b, float a) {
  comps[0] = (unsigned char) (r*255); 
  comps[1] = (unsigned char) (g*255); 
  comps[2] = (unsigned char) (b*255); 
  comps[3] = (unsigned char) (a*255);
}

/*
bool Rgba::equals(float r, float g, float b, float a) const {
return (comps[0]==r*255 && comps[1]==g*255 
        && comps[2]==b*255 && comps[3]==a*255);
}
 */

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Color Color::none(NONE_COLOR, UCONST);
Color Color::inherit(INHERIT_COLOR, UCONST);
Color Color::disabled(0x8c, 0x8c, 0x8c, 255, UCONST);

Color Color::black(0, 0, 0, 255, UCONST);
Color Color::white(255, 255, 255, 255, UCONST);
Color Color::lightgrey(0xd9, 0xd9, 0xd9, 255, UCONST);
Color Color::grey(0xda, 0xda, 0xda, 255, UCONST);
Color Color::darkgrey(0x80, 0x80, 0x80, 255, UCONST);
Color Color::navy(0, 0, 128, 255, UCONST);
Color Color::lightblue(160, 210, 245, 255, UCONST);
Color Color::blue(0x44, 0x7A, 0xC5, 255, UCONST);
Color Color::red(255, 0, 0, 255, UCONST);
Color Color::green(0, 0xaa, 0, 255, UCONST);
Color Color::yellow(255, 255, 0, 255, UCONST);
Color Color::orange(255, 165, 0, 255, UCONST);
Color Color::wheat(245, 222, 179, 255, UCONST);
Color Color::teal(95, 158, 160, 255, UCONST);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef WITH_2D_GRAPHICS
# define unsetPixels() 
#endif

Color::Color() : rgba(0u, 0u, 0u, 255u) {  // opaque black
  ostate = RGBA_COLOR;
  unsetPixels();
}

Color::Color(const Color& c) : rgba(c.rgba) {
  ostate = c.ostate;
#if WITH_2D_GRAPHICS
  pixels = c.pixels;
#endif
}

Color::Color(const Color& c, float a) : rgba(c.rgba) {
  ostate = RGBA_COLOR;
  rgba.comps[3] = (unsigned char)(a*255);
  unsetPixels();
}

Color::Color(const Color& c, unsigned int a) : rgba(c.rgba) {
  ostate = RGBA_COLOR;
  rgba.comps[3] = a;
  unsetPixels();
}

Color::Color(const Rgba& comps) : rgba(comps) {
  ostate = RGBA_COLOR;
  unsetPixels();
}

Color::Color(float r, float g, float b, float a) : rgba(r, g, b, a) {
  ostate = RGBA_COLOR;
  unsetPixels();
}

Color::Color(unsigned int r, unsigned int g, unsigned int b, unsigned int a) 
: rgba(r,g,b,a) {
  ostate = RGBA_COLOR;
  unsetPixels();
}

Color::Color(unsigned int r, unsigned int g, unsigned int b, unsigned int a, UConst m) 
: Attribute(m), rgba(r,g,b,a) {
  ostate = RGBA_COLOR;
  unsetPixels();
}

Color::Color(const String& color_name, float a) {
  setNamedColor(color_name, a);
}

Color::Color(unsigned char special, UConst m) 
: Attribute(m), rgba(0u, 0u, 0u, 0u) {
  ostate = special;
}

Color::~Color() {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 
Color& Color::operator=(const Color& c) {
  if (checkConst()) return *this;
  if (equals(c)) return *this;
  ostate = c.ostate;
  rgba = c.rgba;
#if WITH_2D_GRAPHICS
  pixels = c.pixels;
#endif
  changed(true);
  return *this;
}
  
Color& Color::setRgba(const Rgba& comps) {
  if (checkConst()) return *this;
  if (equals(comps)) return *this;
  ostate = RGBA_COLOR;
  rgba = comps;
  unsetPixels();
  changed(true);
  return *this;
}

Color& Color::setRgbaF(float r, float g, float b, float a) {
  return setRgba(Rgba(r, g, b, a));
}

Color& Color::setRgbaI(unsigned int r, unsigned int g, unsigned int b, unsigned int a) {
  return setRgba(Rgba(r, g, b, a));
}

Color& Color::setNamedColor(const String& colname, float a) {
  if (checkConst()) return *this;
  Rgba comps;
  bool found = parseColor(colname.c_str(), comps);
  comps.comps[3] = (unsigned char)(a*255); 
  if (found) return setRgba(comps);
  else {
    warning("Color::setNamedColor","color name '%s' not found", 
            (colname.empty() ? "null" : colname.c_str()));
    return *this;
  }
}

bool Color::equals(const Color& c) const {
  return (ostate == c.ostate && rgba == c.rgba);
}

bool Color::equals(const Rgba& comps) const {
  return (ostate == RGBA_COLOR && rgba == comps);
}

bool Color::equals(float r, float g, float b, float a) const {
  return (ostate == RGBA_COLOR && rgba == Rgba(r, g, b, a));
}

bool Color::equals(unsigned int r, unsigned int g, unsigned int b, unsigned int a) const {
  return (ostate == RGBA_COLOR && rgba == Rgba(r,g,b,a));
}

void Color::update() {
  updateAutoParents(Update::paint);    //size unchanged
}

void Color::putProp(UpdateContext* props, Element&) {
  props->color = this;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if WITH_2D_GRAPHICS

void Color::unsetPixels() {
  for (int k = 0; k < (int)pixels.size(); ++k) pixels[k] = UNSET_PIXEL;
  static bool first_time = true;
  if (rgba.comps[3] != 255 && !Application::isUsingGL() && first_time) {
    first_time = false;
    warning("Color","alpha value ignored because the application was not launched in OpenGL mode");
  }
}

unsigned long Color::getPixel(Display* d) const {
  int id = d->getID();
  if (id < (int)pixels.size() && pixels[id] != UNSET_PIXEL) return pixels[id];
  
  if (id >= (int)pixels.size()) {      // agrandir la table
    for (int k = pixels.size(); k <= id; ++k) pixels.push_back(UNSET_PIXEL);
  }
  // color allocation & conversion to pixel value
  return pixels[id] = d->createColorPixel(rgba);
}

#endif
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool Color::parseColor(const char* name, Rgba& c) {
  if (!name || !*name) {
    c.setRgbaI(0, 0, 0); // not found
    return false;
  }
  
  // #rgb or #rrggbb or #rrrrggggbbbb specification
  if (name[0] == '#') {
    int r = 0, g = 0, b = 0;
    int len = ::strlen(name+1);
    int n = 0;
    
    if (len == 6) {
      n = sscanf(name+1, "%2x%2x%2x", &r, &g, &b);
    }
    else if (len == 3) {
      n = sscanf(name+1, "%1x%1x%1x", &r, &g, &b);
      r *= 16 + r; g *= 16 +g; b *= 16 + b; 
    }
    else if (len == 12) {
      n = sscanf(name+1, "%2x%*2x%2x%*2x%2x%*2x", &r, &g, &b);
    }
    //cerr << "col " << name<<" "  << r << " " << g << " " <<b << endl;
    
    if (n == 3) {
      c.setRgbaI(r,g,b);
      return true;
    }
  }
   
  // grayXXX specification
  else if (name[0]=='g' && name[1]=='r'&& (name[2]=='a' || name[2]=='e') 
           && name[3]=='y' && name[4]!=0) {
    int percent = atoi(name+4);
    int col = int(percent * 2.56);
    c.setRgbaI(col, col, col);
    return true;
  }
    
  // rgb(r,g,b) specification
  //else if (name[0] == 'r' && name[1] == 'g' && name[2] == 'b' && name[3] == '(') {
  //                      !!!! A COMPLETER @@@@
  //}
    
  else {      // color name
    if (!named_colors) named_colors = new NamedColorMap();
    const NamedColor* nc = named_colors->find(name);
    if (nc) {
      c.setRgbaI(nc->r, nc->g, nc->b);
      return true;
    }
  }
    
  c.setRgbaI(0, 0, 0); // not found
  return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

const NamedColor* NamedColorMap::find(const char* cname) const {
  if (!cname || !*cname) return null;
  Map::const_iterator k = map.find(cname);
  if (k == map.end()) return null;
  else return k->second;
}

void NamedColorMap::add(const NamedColor& cspec) {
  //Map::iterator k = map.find(cspec->colname);
  //if (k != map.end()) map.erase(k);
  map[cspec.colname] = &cspec;
}

void Color::addNamedColor(const char* name, const Rgba& c) { 
  if (!named_colors) named_colors = new NamedColorMap();
  NamedColor* nc = new NamedColor;
  nc->colname = strdup(name);
  nc->r = c.comps[0];
  nc->g = c.comps[1];
  nc->b = c.comps[2];
  named_colors->add(*nc);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

NamedColorMap::NamedColorMap() {
  static NamedColor tab[] = {
  {"aliceblue",	240, 248, 255},
  {"antiquewhite",	250, 235, 215},
  {"aqua",	 0, 255, 255},
  {"aquamarine",	127, 255, 212},
  {"azure",	240, 255, 255},
  {"beige",	245, 245, 220},
  {"bisque",	255, 228, 196},
  {"black",	 0, 0, 0},
  {"blanchedalmond",	255, 235, 205},
  {"blue",	 0, 0, 255},
  {"blueviolet",	138, 43, 226},
  {"brown",	165, 42, 42},
  {"burlywood",	222, 184, 135},
  {"cadetblue",	 95, 158, 160},
  {"chartreuse",	127, 255, 0},
  {"chocolate",	210, 105, 30},
  {"coral",	255, 127, 80},
  {"cornflowerblue",	100, 149, 237},
  {"cornsilk",	255, 248, 220},
  {"crimson",	220, 20, 60},
  {"cyan",	 0, 255, 255},
  {"darkblue",	 0, 0, 139},
  {"darkcyan",	 0, 139, 139},
  {"darkgoldenrod",	184, 134, 11},
  {"darkgray",	169, 169, 169},
  {"darkgreen",	 0, 100, 0},
  {"darkgrey",	169, 169, 169},
  {"darkkhaki",	189, 183, 107},
  {"darkmagenta",	139, 0, 139},
  {"darkolivegreen",	 85, 107, 47},
  {"darkorange",	255, 140, 0},
  {"darkorchid",	153, 50, 204},
  {"darkred",	139, 0, 0},
  {"darksalmon",	233, 150, 122},
  {"darkseagreen",	143, 188, 143},
  {"darkslateblue",	 72, 61, 139},
  {"darkslategray",	 47, 79, 79},
  {"darkslategrey",	 47, 79, 79},
  {"darkturquoise",	 0, 206, 209},
  {"darkviolet",	148, 0, 211},
  {"deeppink",	255, 20, 147},
  {"deepskyblue",	 0, 191, 255},
  {"dimgray",	105, 105, 105},
  {"dimgrey",	105, 105, 105},
  {"dodgerblue",	 30, 144, 255},
  {"firebrick",	178, 34, 34},
  {"floralwhite",	255, 250, 240},
  {"forestgreen",	 34, 139, 34},
  {"fuchsia",	255, 0, 255},
  {"gainsboro",	220, 220, 220},
  {"ghostwhite",	248, 248, 255},
  {"gold",	255, 215, 0},
  {"goldenrod",	218, 165, 32},
  {"gray",	128, 128, 128},
  {"grey",	128, 128, 128},
  {"green",	 0, 128, 0},
  {"greenyellow",	173, 255, 47},
  {"honeydew",	240, 255, 240},
  {"hotpink",	255, 105, 180},
  {"indianred",	205, 92, 92},
  {"indigo",	 75, 0, 130},
  {"ivory",	255, 255, 240},
  {"khaki",	240, 230, 140},
  {"lavender",	230, 230, 250},
  {"lavenderblush",	255, 240, 245},
  {"lawngreen",	124, 252, 0},
  {"lemonchiffon",	255, 250, 205},
  {"lightblue",	173, 216, 230},
  {"lightcoral",	240, 128, 128},
  {"lightcyan",	224, 255, 255},
  {"lightgoldenrodyellow",	250, 250, 210},
  {"lightgray",	211, 211, 211},
  {"lightgreen",	144, 238, 144},
  {"lightgrey",	211, 211, 211},
  {"lightpink",	255, 182, 193},
  {"lightsalmon",	255, 160, 122},
  {"lightseagreen",	 32, 178, 170},
  {"lightskyblue",	135, 206, 250},
  {"lightslategray",	119, 136, 153},
  {"lightslategrey",	119, 136, 153},
  {"lightsteelblue",	176, 196, 222},
  {"lightyellow",	255, 255, 224},
  {"lime",	 0, 255, 0},
  {"limegreen",	 50, 205, 50},
  {"linen",	250, 240, 230},
  {"magenta",	255, 0, 255},
  {"maroon",	128, 0, 0},
  {"mediumaquamarine",	102, 205, 170},
  {"mediumblue",	 0, 0, 205},
  {"mediumorchid",	186, 85, 211},
  {"mediumpurple",	147, 112, 219},
  {"mediumseagreen",	 60, 179, 113},
  {"mediumslateblue",	123, 104, 238},
  {"mediumspringgreen",	 0, 250, 154},
  {"mediumturquoise",	 72, 209, 204},
  {"mediumvioletred",	199, 21, 133},
  {"midnightblue",	 25, 25, 112},
  {"mintcream",	245, 255, 250},
  {"mistyrose",	255, 228, 225},
  {"moccasin",	255, 228, 181},
  {"navajowhite",	255, 222, 173},
  {"navy",	 0, 0, 128},
  {"oldlace",	253, 245, 230},
  {"olive",	128, 128, 0},
  {"olivedrab",	107, 142, 35},
  {"orange",	255, 165, 0},
  {"orangered",	255, 69, 0},
  {"orchid",	218, 112, 214},
  {"palegoldenrod",	238, 232, 170},
  {"palegreen",	152, 251, 152},
  {"paleturquoise",	175, 238, 238},
  {"palevioletred",	219, 112, 147},
  {"papayawhip",	255, 239, 213},
  {"peachpuff",	255, 218, 185},
  {"peru",	205, 133, 63},
  {"pink",	255, 192, 203},
  {"plum",	221, 160, 221},
  {"powderblue",	176, 224, 230},
  {"purple",	128, 0, 128},
  {"red",	255, 0, 0},
  {"rosybrown",	188, 143, 143},
  {"royalblue",	 65, 105, 225},
  {"saddlebrown",	139, 69, 19},
  {"salmon",	250, 128, 114},
  {"sandybrown",	244, 164, 96},
  {"seagreen",	 46, 139, 87},
  {"seashell",	255, 245, 238},
  {"sienna",	160, 82, 45},
  {"silver",	192, 192, 192},
  {"skyblue",	135, 206, 235},
  {"slateblue",	106, 90, 205},
  {"slategray",	112, 128, 144},
  {"slategrey",	112, 128, 144},
  {"snow",	255, 250, 250},
  {"springgreen",	 0, 255, 127},
  {"steelblue",	 70, 130, 180},
  {"tan",	210, 180, 140},
  {"teal",	 0, 128, 128},
  {"thistle",	216, 191, 216},
  {"tomato",	255, 99, 71},
  {"turquoise",	 64, 224, 208},
  {"violet",	238, 130, 238},
  {"wheat",	245, 222, 179},
  {"white",	255, 255, 255},
  {"whitesmoke",	245, 245, 245},
  {"yellow",	255, 255, 0},
  {"yellowgreen",	154, 205, 50},
  {null,	0, 0, 0}
  };
  
  for (int k = 0; tab[k].colname != null; ++k) map[tab[k].colname] = &tab[k];
}

}
