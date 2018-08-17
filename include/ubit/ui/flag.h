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
   * const UFlag showStores, showMuseums;
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
   * !Warning: UFlag objects must not be deleted!
   * See also: UFlagdef, UPropdef.
   */
  class UFlag : public Condition {
  public:
    static const UFlag none;
    
    UFlag() {}
    virtual bool verifies(const UpdateContext&, const Element&) const;
  };
  

  // ==================================================== Ubit Toolkit =========
  /** [impl] negation of a UFlag condition.
   * see also: Condition& operator!(const Condition&)
   */
  class UNotFlag : public Condition {
  public:
    const UFlag& cond;
    UNotFlag(const UFlag& c) : cond(c) {}
    virtual bool verifies(const UpdateContext&, const Element&) const;
  };
  
  /** negation of a UFlag condition.
   */
  inline UNotFlag& operator!(const UFlag& c) {return *new UNotFlag(c);}
  
  
  // ==================================================== Ubit Toolkit =========
  /* [impl] Defines a flag that is inherited in the scene graph.
   *  See: UFlag.
   */
  class UFlagdef : public Attribute {
  public:
    UCLASS(UFlagdef)
    
    UFlagdef();
    UFlagdef(const UFlag&);
    ///< create a new flagdef (see UFlag.).
    
    UFlagdef& operator=(const UFlag& f) {return set(f);} 
    virtual UFlagdef& set(const UFlag&);
    virtual UFlagdef& clear();
    
    const UFlag* getFlag() const {return flag;}
    
    virtual void update();
    virtual void putProp(UpdateContext*, Element&);
    
  protected:
    const UFlag* flag;
  };
  
  inline UFlagdef& uflagdef() {return *new UFlagdef();}
  inline UFlagdef& uflagdef(const UFlag& f) {return *new UFlagdef(f);}
  
  
  // ==================================================== Ubit Toolkit =========
  /* [impl] Defines a property that is inherited in the scene graph.
   * use UAttrval to retreive the property in a (direct or indirect) child.
   */
  class UPropdef : public UFlagdef {
  public:
    UCLASS(UPropdef)
    UPropdef();
    UPropdef(const UFlag&);
    UPropdef(const UFlag&, Attribute&);
    
    virtual UPropdef& set(const UFlag&);
    virtual UPropdef& set(Attribute&);
    virtual UPropdef& set(const UFlag&, Attribute&);
    virtual UPropdef& clear();
    
    Attribute* getProp() const {return prop;}
    virtual void putProp(UpdateContext*, Element&);
    
  private:
    uptr<Attribute> prop;
  };
  
  /* [impl] gets the value of an inherited property.
   * use UPropdef to define a property in a (direct or indirect) parent.
   */
  class UPropval : public Attribute {
  public:
    UCLASS(UPropval)
    UPropval();
    UPropval(const UFlag&);
    
    const UFlag* getFlag() const {return flag;}
    
    virtual void update();
    virtual void putProp(UpdateContext*, Element&);
    
  private:
    const UFlag* flag;
  };
  
}
#endif


