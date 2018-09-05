/*
 *  attribute.h: Base class for the attributes of the Scene Graph
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

#ifndef UBIT_CORE_ATTRIBUTE_LIST_H_
#define	UBIT_CORE_ATTRIBUTE_LIST_H_

#include <memory>

namespace ubit {
  /**
   * Attribute list.
   * used by UConf and by the style management system (among other things).
   */
  class AttributeList : public Attribute {
  public:
    
    void addAttr(Attribute&);
    void addAttr(const String& name, const String& value);
    
    String* getAttr(const String&, bool ignore_case = true);
    
    template <class AttrClass> 
    AttrClass* getAttr() const {AttrClass* a; _attributes.findClass(a); return a;}
    /**< returns the attribute that derives from AttrClass.
     * null is returned if there is no such attribute in the element's ATTRIBUTE list.
     */
    
    template <class AttrClass> 
    AttrClass& obtainAttr() const {AttrClass* a; _attributes.findClass(a); return a ? *a : *new AttrClass;}
    /**< returns the attribute that derives from AttrClass.
     * null is returned if there is no such attribute in the element's ATTRIBUTE list.
     */
    
    virtual void update();
    virtual void putProp(UpdateContext*, Element&);  
  };


  /**
   * Window Title.
   */
  class Title: public Attribute {
  public:
    UCLASS(Title)
    
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
#endif // UBIT_CORE_ATTRIBUTE_LIST_H_
