/*
 *  title.h: Class for Window Titles
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

#ifndef UBIT_UI_TITLE_H_
#define	UBIT_UI_TITLE_H_

#include <memory>

namespace ubit {
  /**
   * Window Title.
   */
  class Title: public Attribute {
  public:
    
    Title(const char* = "");
    /**< create a new Window Title (the argument is duplicated internally).
     * example:  udialog( utitle("Debug Window") + ... )
     */
    
    Title(const String&);
    /**< create a new Window Title (the argument is duplicated internally).
    * example:  udialog( utitle("Debug Window") + ... )
    */
    
    String& value() {return *pstring;}
    /** returns the value.
    * - example: Title t; t.value() = "Main Win"; String s = t.value();
    * - value() can be shared (but can't be deleted!)
    */
    
    virtual void update();
    
  private:
    std::unique_ptr<String> pstring;
  };
  
  
  inline Title& utitle(const char*_s) {return *new Title(_s);}
  inline Title& utitle(const String&_s) {return *new Title(_s);}

}
#endif // UBIT_UI_TITLE_H_
