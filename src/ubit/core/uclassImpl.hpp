/*
 *  classImpl.hpp
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

#ifndef _uclassImpl_hpp_
#define	_uclassImpl_hpp_ 1

#include <map>
#include <memory>

#include <ubit/core/attribute.h>
#include <ubit/core/element.h>
#include <ubit/ui/box.h>
#include <ubit/core/class.h>

namespace ubit {
  
    
  class DefaultAttribute : public Attribute {
  public:
    UCLASS(DefaultAttribute)
  };
  
    
  class DefaultInlineElement : public Element {
  public:
	UCLASS(DefaultInlineElement)
  };
  
    
  class DefaultBlockElement : public Element {
  public:
	UCLASS(DefaultBlockElement)
  };
  
    
  class AttributeClassMap {
  public:
    ~AttributeClassMap();
    
    void addClass(const Class&);
    ///< adds this class (if a class already exists with the same name it is removed).
    
    const Class* findClass(const String& name) const;
    ///< returns class (null if not found).
    
    const Class* obtainClass(const String& name);
    ///< finds class; creates a generic UAttrClass if not found.
    
    struct Comp {
      /// Attribute name comparator, case is ignored.
      bool operator()(const String*a,const String*b) const {return a->compare(*b,true) < 0;}
    };
    
    typedef std::map<const String*, const Class*, Comp> Map;
    Map map;
  };
  
    
  class ElementClassMap {
  public:
    ~ElementClassMap();
    
    void addClass(const Class&);
    ///< adds this class (if a class already exists with the same name it is removed).
    
    const Class* findClass(const String& name) const;
    ///< returns class (null if not found).
    
    const Class* obtainClass(const String& name);
    ///< finds class; creates a generic UElemntClass if not found.
    
    struct Comp {
      /// Element name comparator, case is ignored.
      bool operator()(const String*a,const String*b) const {return a->compare(*b,true)<0;}
    };
    typedef std::map<const String*, const Class*, Comp> Map;
    Map map;
  };
  
    
  class StyleSheet : public ElementClassMap {
  public:
    ~StyleSheet();
  };
  
}
#endif

