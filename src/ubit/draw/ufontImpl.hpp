/*
 *  ufontImpl.hpp
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

#ifndef _ufontImpl_hpp_
#define	_ufontImpl_hpp_ 1
namespace ubit {
  
  /** [impl] Internal representation for fonts.
   * should not be used directly
   */
  class UFontDesc {
  public:
    UFontDesc() {}
    UFontDesc(const UFont&);
    
    void set(const UFont&);
    void merge(const UFont&); // merge only: set NON default values and combine styles.
    void setScale(float xyscale); // MUST be called to init the FontDesc !
    
    const UFontFamily* family;
    short styles, def_size, actual_size;
    mutable short findex;
    float scaled_size;
  };
  
}
#endif
