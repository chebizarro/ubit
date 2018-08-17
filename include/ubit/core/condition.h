/*
 *  cond.h: Base class for the attributes of the Scene Graph
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

#ifndef UBIT_CORE_CONDITION_H_
#define	UBIT_CORE_CONDITION_H_

#include <ubit/udefs.hpp>

namespace ubit {
  
  /**
   * Base class for Ubit conditions.
   * 
   * main subclasses: UOn, Flag, UInscale.
   * Example:
   * <pre>
   *    ubutton("Click Me" + UOn::action / ucall(arg, myfunc))
   * </pre>
   * !Warning: Condition (and derived) objects must not be deleted.
   */
  class Condition {
  public:
    virtual ~Condition() {}
    
    virtual bool operator==(const Condition& c) const {return this == &c;}
    virtual bool operator!=(const Condition& c) const {return this != &c;}
    virtual const Condition* matches(const Condition& c) const {return this == &c ? this : 0;}
    virtual bool verifies(const UpdateContext&, const Element&) const = 0;
    
    virtual UOn* toOn() {return null;}
    virtual const UOn* toOn() const {return null;}
    
    virtual void setParentModes(Element& parent) const {};
  };
  
  // pose le pbm de la desctruction auto
  
  class MultiCondition : public Condition {
  public:
    MultiCondition& operator+=(const Condition& c) {return add(c);}
    virtual MultiCondition& add(const Condition&);
    virtual void remove(const Condition&);
    
    virtual const Condition* matches(const Condition& c) const;
    virtual bool verifies(const UpdateContext&, const Element&) const;
    virtual void setParentModes(Element& parent) const;
    
  private:
    typedef std::list<const Condition*> CondList;
    CondList condlist;
  };
  
}
#endif // UBIT_CORE_CONDITION_H_

