/*
 *  flag.hpp
 *  (see also UAttrs.hpp for Attribute subclasses)
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

#ifndef _uflag_hpp_
#define	_uflag_hpp_ 1

#include <ubit/uattr.hpp>
#include <ubit/ucond.hpp>

namespace ubit {
  
  /** Flagging conditions.
   * for specifying conditional objets that are only active when this condition
   * is verified. Works in conbination with UFlagdef.
   * Example:
   * <pre>
   * const Flag showStores, showMuseums;
   * 
   *   Box& stores = ...;  // whatever
   *   Box& museums = ...; // whatever
   *   Box& scene = ubox( showStores/stores + showMuseums/museums );
   *   
   *   // the stores appear in this box (the museums are ignored)
   *   Box& scene_with_stores  = ubox( uflagdef(showStores) + scene);
   *
   *   // the museums appear in this box (the stores are ignored)
   *   Box& scene_with_museums = ubox( uflagdef(showMuseums) + scene);
   * </pre>
   *
   * !Warning: Flag objects must not be deleted!
   * See also: UFlagdef, UPropdef.
   */
  class Flag : public Condition {
  public:
    static const Flag none;
    
    Flag() {}
    virtual bool verifies(const UpdateContext&, const Element&) const;
  };
  

  /** [impl] negation of a Flag condition.
   * see also: Condition& operator!(const Condition&)
   */
  class NotFlag : public Condition {
  public:
    const Flag& cond;
    NotFlag(const Flag& c) : cond(c) {}
    virtual bool verifies(const UpdateContext&, const Element&) const;
  };
  
  /** negation of a Flag condition.
   */
  inline NotFlag& operator!(const Flag& c) {return *new NotFlag(c);}
  
  
  /* [impl] Defines a flag that is inherited in the scene graph.
   *  See: Flag.
   */
  class UFlagdef : public Attribute {
  public:
    UCLASS(UFlagdef)
    
    UFlagdef();
    UFlagdef(const Flag&);
    ///< create a new flagdef (see Flag.).
    
    UFlagdef& operator=(const Flag& f) {return set(f);} 
    virtual UFlagdef& set(const Flag&);
    virtual UFlagdef& clear();
    
    const Flag* getFlag() const {return flag;}
    
    virtual void update();
    virtual void putProp(UpdateContext*, Element&);
    
  protected:
    const Flag* flag;
  };
  
  inline UFlagdef& uflagdef() {return *new UFlagdef();}
  inline UFlagdef& uflagdef(const Flag& f) {return *new UFlagdef(f);}
  
  
  /* [impl] Defines a property that is inherited in the scene graph.
   * use UAttrval to retreive the property in a (direct or indirect) child.
   */
  class UPropdef : public UFlagdef {
  public:
    UCLASS(UPropdef)
    UPropdef();
    UPropdef(const Flag&);
    UPropdef(const Flag&, Attribute&);
    
    virtual UPropdef& set(const Flag&);
    virtual UPropdef& set(Attribute&);
    virtual UPropdef& set(const Flag&, Attribute&);
    virtual UPropdef& clear();
    
    Attribute* getProp() const {return prop;}
    virtual void putProp(UpdateContext*, Element&);
    
  private:
    unique_ptr<Attribute> prop;
  };
  
  /* [impl] gets the value of an inherited property.
   * use UPropdef to define a property in a (direct or indirect) parent.
   */
  class UPropval : public Attribute {
  public:
    UCLASS(UPropval)
    UPropval();
    UPropval(const Flag&);
    
    const Flag* getFlag() const {return flag;}
    
    virtual void update();
    virtual void putProp(UpdateContext*, Element&);
    
  private:
    const Flag* flag;
  };
  
}
#endif


