/*
 *  cursor.h: Cursor Attribute
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

#ifndef _ucursor_hpp_
#define	_ucursor_hpp_ 1

#include <ubit/uattr.hpp>

namespace ubit {
  
  class UCursorImpl;
  
  /** Mouse cursor property.
   * this property specifies the shape of the cursor in a Box. It is inherited
   * by children, except if they have their own UCursor, or if their UStyle
   * specifies another UCursor.
   * As other Attribute(s), a UCursor instance must be added as an attribute 
   * (or as a child) of the Box. @see: Attribute.
   */
  class UCursor : public Attribute {
  public:
    UCLASS(UCursor)
    
    static UCursor none, pointer, crosshair, text, hand, pencil, wait, question, 
    hresize, vresize, move, dnd;
    ///< predefined cursors.
    
    UCursor(const UCursor& = UCursor::none);
    ///< creates a new Cursor from a UCursor (or a default UCursor).
    
    UCursor(int cursor_type);
    ///< creates a new Cursor from a cursor_type (impl dependent).
    
    UCursor(int cursor_type, UConst);
    ///< creates a new CONSTANT Cursor from a cursor_type (impl dependent).
    
    virtual ~UCursor();
    
    UCursor& operator=(const UCursor& c) {return set(c);}
    ///< copies another UCursor.
    
    virtual UCursor& set(const UCursor&);
    ///< copies another UCursor.
    
    bool operator==(const UCursor& c) const {return equals(c);}
    bool operator!=(const UCursor& c) const {return !equals(c);}
    virtual bool equals(const UCursor&) const;
    
    int getCursorType() const {return cursor_type;}
    
    // - - - impl  - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    virtual void update() {} 
    
    UCursorImpl* getCursorImpl(Display*) const;
    ///< [impl] returns internal data for this display.
    
#ifndef NO_DOC
    virtual void addingTo(Child&, Element& parent);
    virtual void removingFrom(Child&, Element& parent);
    virtual void putProp(UpdateContext*, Element&);
    ///< NOTE that removingFrom() requires a destructor to be defined.
    
  private:
    int cursor_type;
    mutable std::vector<UCursorImpl*> cimpl;
#endif
  };
  
}
#endif
