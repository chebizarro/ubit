/*
 *  class.h: Ubit MetaClasses
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

#ifndef _uclass_hpp_
#define	_uclass_hpp_ 1

#include <ubit/udefs.hpp>

namespace ubit {
  
  
#define UCLASSDEF(NAME,C,CREATOR) \
struct MetaClass : public Class { \
MetaClass(): Class(NAME) {} \
virtual bool isInstance(Object& obj) const {return dynamic_cast<C*>(&obj);} \
virtual bool isInstance(Object* obj) const {return dynamic_cast<C*>(obj);} \
virtual C* newInstance() const {return CREATOR;} \
virtual Style* newStyle() const {return C::createStyle();} \
}; \
virtual const Class& getClass() const {static MetaClass& c = *new MetaClass; return c;}

  
  /** instanciable Object (and thus, Node) subclasses MUST invoke this macro in their public section
   * @see Class and UABSTRACT_CLASS().
   */
#define UCLASS(C) UCLASSDEF(#C, C, new C)
  
  /** abstract Object (and thus, Node) subclasses MUST invoke this macro in their public section
   * @see Class and UCLASS().
   */  
#define UABSTRACT_CLASS(C) UCLASSDEF(#C, C, null)
    
  
  /** Ubit class.
   *
   * All classes that derives from Object can have an associated Class.
   * Class is a metaclass that defines some useful data for the corresponding
   * Ubit class, such as the class name, a unique ID, a static constructor, 
   * a style def, etc.
   *
   * Object (and Node) subclasses MUST invoke the UCLASS macro in the public
   * section of their declaration as follows:
   *
   * <pre>
   *   class Color: public Attribute {
   *   public:
   *     UCLASS("ucolor", Color, new Color)
   *     .....
   *  };
   * </pre>
   *
   * The arguments of the UCLASS macro are:
   * - the ASCII name of the class (usually in lowercase letters)
   * - the C++ class name
   * - an instruction that would creates an instance of that class (such as: new Color)
   *   or NULL if this class is abstract
   *
   * The UCLASS macro redefines the getClass() and Class() methods which are,
   * respectively, an instance ad a class method. Both methods return a metaclass
   * that describes the C++ class.
   */
  class Class {  
  public:
    /** Parse Modes returned by Class::getParseModes():
     * - closing tags can be omitted for EMPTY_ELEMENTs (eg. <img>)
     *   if the 'permissive' mode of the XMLParser is set.
     * - the textual content of DONT_PARSE_CONTENT elements is not parsed
     *   and their comments are stored as a text element (eg. <style> <script>)
     * - the XML Parser collapses white spaces (and tabs and new lines)
     *   if the 'collapse_spaces' mode is on except for PRESERVE_SPACES 
     *   elements (eg. the <pre> element) 
     */
    enum ParseModes {
      EMPTY_ELEMENT      = 1<<0, 
      DONT_PARSE_CONTENT = 1<<1,
      PRESERVE_SPACES    = 1<<2
    };
        
    Class(const char* name);
    Class(const String& name);
    virtual ~Class();
    
    const String& getName() const {return *name;}
    ///< returns the class name.
    
    unsigned int getNo() const {return no;}
    /**< returns the class #.
     * the class No depends in which order classes are created (the first created class
     * has No 1 and so on)
     */
    
    virtual int getParseModes() const {return 0;}
    ///< returns an ORed combination of the parse modes (0 if none).
    
    virtual bool isInstance(Object& object) const {return false;}
    ///< returns true if this 'object' derives from this class.
    
    virtual Object* newInstance() const {return null;}
    /**< creates a new instance of the corresponding class (if applicable).
     * Ubit classes that derive from Object should have an associated Class that
     * redefines this method.
     */
    
    virtual Style* newStyle() const {return null;}
    /**< creates a new instance of the style of this class (if applicable).
     * Ubit classes that derive from Element should have an associated Class that
     * redefines this method.
     */
        
    Style* obtainStyle() const {return style ? style : (style = newStyle());}
    /**< returns the style prototype that is associated to this class.
     * Ubit classes that derive from Element can have a style prototype.
     * This function calls newStyle() the 1st type it is called to create the
     * style prototype
     * WARNING: the returned style must NOT be deleted.
     */
    
    AttributeList* getAttributes() const {return attributes;}
    /**< returns current class properties.
     * WARNING: the returned properties must NOT be deleted.
     */
    
    void setAttributes(AttributeList* alist) const {attributes = alist;}
    /**< changes the class properties.
     * WARNING: the properties given as argument are not copied and will be destroyed
     * by the Class destr. They must NOT be deleted by another function
     */
    
  private:
    static unsigned int count;
    unsigned int no;
    String* name;
    mutable Style* style;
    mutable AttributeList* attributes;
    
    Class(const Class&);   // assignment is forbidden.
    Class& operator=(const Class&);
  };

}
#endif
