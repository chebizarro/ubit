/*
 *  xmlgrammar.hpp
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

#ifndef UBIT_CORE_XMLGRAMMAR_H_
#define UBIT_CORE_XMLGRAMMAR_H_

#include <map>
#include <vector>
#include <ubit/core/dom.h>

namespace ubit {
  
  
  /**
   * XML Grammar.
   */
  class XmlGrammar {
  public:
    XmlGrammar();
    virtual ~XmlGrammar();
    
    virtual void addAttrClass(const Class&);
    virtual void addElementClass(const Class&);
    virtual void addCharEntityRef(const String& entity_name, unsigned short entity_value);
    
    virtual const Class* getAttrClass(const String& classname) const;
    virtual const Class* getElementClass(const String& classname) const;
    virtual unsigned short getCharEntityRef(const String& entity_name) const;
    
    static XmlGrammar& getSharedUndefGrammar();  ///< TO BE CHANGED !!!
    static Class* addUndefAttrClass(const String& classname); ///< TO BE CHANGED !!!
    static Class* addUndefElementClass(const String& classname); ///< TO BE CHANGED !!!
    virtual void createDefaultCharEntityRefs(); ///< TO BE CHANGED !!!
    
  protected:
    struct Comp {
      bool operator()(const String*a,const String*b) const {return a->compare(*b)<0;}
    };
    typedef std::map<const String*, unsigned short, Comp> CharEntityRefMap;
    
    CharEntityRefMap char_entity_refs;
    ElementClassMap element_classes;
    AttributeClassMap attr_classes;
  };
  
  /** List of XML Grammars.
   */
  class XmlGrammars {
  public:  
    void addGrammar(const XmlGrammar&);
    void addGrammars(const XmlGrammars&);
    
    const Class* getAttrClass(const String& classname) const;
    const Class* getElementClass(const String& classname) const;
    unsigned short getCharEntityRef(const String& entityname) const;
    
  protected:
    typedef std::vector<const XmlGrammar*> GrammarList;
    GrammarList glist;
  };
  
  
}
#endif // UBIT_CORE_XMLGRAMMAR_H_

