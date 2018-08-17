/**
 *
 *  usymbol.hpp : graphical symbols that can be embedded in elements
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


#ifndef _usymbol_hpp_
#define	_usymbol_hpp_ 1

#include <ubit/udata.hpp>

namespace ubit {

/** Graphical Symbols.
 */
class Symbol: public Data {
public:
  UCLASS(Symbol)

  /// predefined symbols:
  static Symbol left, right, up, down, check, radio, square, circle;

  Symbol(const Symbol& = Symbol::left);
  ///< creates a new Symbol; @see also shortcut usymbol().
  
  virtual void set(const Symbol&);
  virtual void update();

  void setColor(const Color&);
  void setFrontShadowColor(const Color&);
  void setBackShadowColor(const Color&);

  // - impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

protected:
  friend class Box;
  enum  {
    S_LEFT, S_RIGHT, S_UP, S_DOWN, S_CHECK, S_RADIO, S_SQUARE, S_CIRCLE
  };

  // private constructor (internal implementation):
  Symbol(int index, UConst);
  int ix;  // internal index
  const class Color *color, *frontShadowColor, *backShadowColor;
  virtual void getSize(UpdateContext&, Dimension&) const;
  virtual void paint(Graph&, UpdateContext&, const Rectangle&) const;
};

inline Symbol& usymbol(const Symbol& s) {return *new Symbol(s);}
///< shortcut function that creates a new Symbol.

}
#endif
