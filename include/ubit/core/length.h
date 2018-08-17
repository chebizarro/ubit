/*
 *  length.h: lengths and units
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

#ifndef _ulength_hpp_
#define	_ulength_hpp_ 1

#include <ubit/uobject.hpp>

namespace ubit {
  
  class Unit {
  public:
    enum Type {PX, EM, EX, IN, CM, MM, PT, PC, PERCENT, PERCENT_CTR, AUTO, IGNORE};
    
    Unit(const Unit& u) : type(u.type) {}
    explicit Unit(Unit::Type t) : type(t) {}
    
    Unit& operator=(const Unit& u) {type = u.type; return *this;}
    Unit& operator=(Unit::Type t) {type = t; return *this;}

    Unit::Type getType() const {return (Unit::Type)type;}

    bool operator==(Unit u) const {return u.type == type;}
    bool operator==(Unit::Type t) const {return t == type;}

    bool operator!=(Unit u) const {return u.type != type;}
    bool operator!=(Unit::Type t) const {return t != type;}
    
    unsigned char type;
  };
  
  extern const Unit 
  UPX,  ///< Pixel unit.
  UEM,  ///< EM unit (1em is equal to the current font size).
  UEX,  ///< EX unit (1ex is the x-height of a font, which is about half the font-size)
  UIN,  ///< Inch unit.
  UCM,  ///< Centimeter unit.
  UMM,  ///< Millimeter unit.
  UPT,  ///< Point unit (1pt is the same as 1/72 inch).
  UPC,  ///< Pica unit (1pc is the same as 12pt).
  UPERCENT,     ///< percentage of the parent size in range [0f,100f].
  UPERCENT_CTR; ///< same as UPERCENT but coordinates correspond to the widget center.
  
  
  /**
   * Length of positions (UPos), sizes (USize) and padding (UPadding) 
   */
  class Length {
  public:
    struct Modes {
      enum {NONE=0};
      unsigned char val;
      explicit Modes(unsigned char m) : val(m) {}
    };
         
    Length(float value = 0.0) : val(value), unit(Unit::PX), modes(0) {}
    ///< creates a new length with the specified value, unit = PX and modes = 0.
    
    Length(float value, Unit u) : val(value), unit(u), modes(0) {}
    ///< creates a new length with the specified value and unit (modes = 0).
    
    Length(float value, Unit u, Modes m) : val(value), unit(u), modes(m) {}
    ///< creates a new length with the specified value, unit and modes.
    
    Length(const String& text_spec); 
    ///< creates a new length with a value and a unit taken from a textual specification (modes = NONE).

    bool operator==(const Length&) const;
    bool operator!=(const Length&) const;
    
    operator float() const {return val;}
    
    float getValue() const {return val;}
    Unit getUnit()  const {return unit;}
    Modes getModes() const {return modes;}
    
    Length& setValue(float value) {val = value; return *this;}
    Length& setUnit(Unit u) {unit = u; return *this;}
    Length& setUnit(const char* unit);
    Length& setModes(Modes m) {modes = m; return *this;}
    
    Length& set(const String& text_spec); 
    ///< changes the value and the unit from a textual specification (modes are set to NONE).
        
    String toString() const;
    
    float toPixels(Display*, const UFontDesc&, float view_len, float parview_len) const;
    
    friend std::ostream& operator<<(std::ostream& s, const Length& l);
    friend Length operator|(float len, Unit u);
    friend Length operator|(Length len, Unit u);
    friend Length operator|(Length len, Length::Modes m);
    
    float val;
    Unit unit;
    Modes modes;
  };
  

  std::ostream& operator<<(std::ostream&, const Length&);
  ///< prints out this Length on an output stream.
  
  inline Length operator|(float len, Unit u) {return Length(len, u);}
  /**< | makes its possible to specify the Unit of a Length in simple way (see exemples).
   * Exemples: 
   * - 50|UPX means a length of 50 pixels
   * - 50|UPERCENT means a length of 50% of the parent
   * Impl notes:
   * operator|(float, enum) wouldn't work properly because C++ silently converts enums to ints
   */
  
  inline Length operator|(Length len, Unit u) {return Length(len).setUnit(u);}
  
  inline Length operator|(Length len, Length::Modes m) {return Length(len).setModes(m);}
  
  extern const Length
  UIGNORE,    ///< ignore this specification.
  UAUTO;      ///< let Ubit compute this value.
  
  
  
  struct PosSpec {    
    PosSpec(Length _x, Length _y) : x(_x), y(_y) {}
    Length x, y;
  };
  
  
  struct SizeSpec {
    SizeSpec(Length w, Length h) : width(w), height(h) {}
    Length width, height;
  };
  
  /** Box padding.
   */
  struct PaddingSpec {
    PaddingSpec(Length horiz, Length vert) : top(vert), right(horiz), bottom(vert), left(horiz) {}
    // attention: doit etre refenie si cette classe est heritee
    void set(Length horiz, Length vert) {left = right = horiz; top  = bottom = vert;}
    void add(const PaddingSpec&);
    Length top, right, bottom, left;
  };
  
}
#endif

