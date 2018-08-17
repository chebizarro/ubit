/*
 *  tip.h: Tool Tip class
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

#ifndef UBIT_UI_TIP_H_
#define	UBIT_UI_TIP_H_

namespace ubit {
  /**
   * Box tool tip.
   */
  class Tip: public Attribute {
  public:
    
    Tip(const char* label = null);
    /**< create a new Tool Tip with a string label.
     * 'label' is duplicated internally and can be freed.
     * Example:
     *    ubutton( "Build" + utip("Build active target") )
     */
    
    Tip(const Args& arglist);
    /**< create a new Tool Tip with an arglist.
     * 'arglist' is NOT duplicated internally. The tooltip will show nothing if
     * it is destroyed.
     * Example:
     *    ubutton( "Build"
     *            + utip(Font::bold + "Build" 
     *                   + Element(Color::red + "active")
     *                   + "target") )
     */
    
    virtual ~Tip();
    
    //String& value() {return *pvalue;}
    // returns the string value.
    
    virtual Tip& set(const char* label);
    virtual Tip& set(const Args& arglist);
    
    virtual void addingTo(Child&, Element& parent);
    virtual void removingFrom(Child&, Element& parent);
    //NB: removingFrom() impose un destructeur
    unique_ptr<Node> content;
  };
  
  inline Tip& utip(const char* label = null) {return *new Tip(label);}
  ///< shortcut function that returns *new Tip(label).
  
  inline Tip& utip(const Args& args) {return *new Tip(args);}
  ///< shortcut function that returns *new Tip(args).

}
#endif // UBIT_UI_TIP_H_
