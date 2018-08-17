/*
 *  args.hpp : argument lists.
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

#ifndef UBIT_CORE_ARGS_H_
#define	UBIT_CORE_ARGS_H_

#include <ubit/core/uargsImpl.hpp>

namespace ubit {
  
  class UArgsImpl;
    
  /**< Argument list (for passing arguments to constructor or add functions).
   * Args makes it possible to create "additive" argument lists by using the + operator.
   *
   * A UArg object may either hold a single object pointer or reference, or a list 
   * of object pointers or references separated by + operators, example:
   * <pre>
   *   class Button : public Box {
   *     Button(Args arglist);
   *     ....
   *   };
   *
   *   void foo() {
   *     Button* b = new Button(UPix::disquette + "Save" + ucall(obj, saveFunc));
   *     ...
   *   }
   * </pre>
   *
   * Args::none is the empty arglist.
   */
  class Args {
  public:
    static const Args none;
    ///< the empty arglist.
    
    ~Args();
    Args();
    Args(Node*);
    Args(Node&);
    Args(const char*);
    Args(const Child&);
    Args(const Args&);
    Args(const UArgsImpl&);
      
    bool operator !() const;
    ///< returns true is this arglist is empty.
    
    bool empty() const;
    ///< returns true is this arglist is empty.
    
    Args& operator+=(const Args& arglist2);
    ///< adds the children contained in 'arglist2' to this arglist.
    
    friend const UArgsImpl& operator+(const UArgsImpl&, const char* c_string);
    ///< adds a C String to this arglist (note that the string content is duplicated).
    
    friend const UArgsImpl& operator+(const UArgsImpl&, Node*);
    ///< adds a Node to this arglist (Node is the base class for objects that can be part of the scene graph).
    
    friend const UArgsImpl& operator+(const UArgsImpl&, Node&);
    ///< adds a Node to this arglist (Node is the base class for objects that can be part of the scene graph).
    
    friend const UArgsImpl& operator+(const UArgsImpl&, const Child&);
    /**< adds a Child to this arglist.
     * a Child holds a Node (Node is the base class for objects that can be part
     * of the scene graph) and, possibly, a Condition (a condition that specifies if
     * this node is active). Example:
     * <pre>
     *    UOn::action / ucall(obj, &MyObj::foo)
     * </pre>
     * This statement is stored in a Child where UOn::action corresponds to the
     * condition and ucall(...) to the corresponding Node (which is, in this example
     * a callback object that will fire: obj->foo()).
     * Note however that the condition part is generally null in which case the
     * corresponding object is always active.
     */
    
    friend const UArgsImpl& operator+(const UArgsImpl&, const Args&);
    ///< adds the children contained in 'arglist2' to this arglist.
    
  private:  
    Args& operator=(const Args&); 
    ///< assigment is forbidden.
    
    friend class Element;
    friend class UArgsImpl;
    class UArgsChildren* children;
  };
  
}
#endif // UBIT_CORE_ARGS_H_
