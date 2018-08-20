/*
 *  object.h: Base Class for all Ubit Objects
 *  Ubit GUI Toolkit - Version 8
 *  (C) 2018 Chris Daley
 *  (C) 2009 | Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
 *
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

#ifndef UBIT_CORE_OBJECT_H_
#define	UBIT_CORE_OBJECT_H_

#include <stdlib.h>    // for size_t, new(), delete()
#include <exception>
#include <ubit/udefs.hpp>
#include <ubit/core/class.h>
#include <ubit/core/error.h>

namespace ubit {
  
  /**
   * Base class of most Ubit objects
   *
   * Object is the base class for (almost) all Ubit objects. Its main subclasses are
   * UNode, UAttr, UElem that play the same role as Node, Attribute and Element in a
   * XML/DOM model and UBox and UWin that are the base classes for widgets and windows.
   *
   *
   * 1. SUBCLASSING: Object subclasses MUST invoke the UCLASS macro in the public section
   * of their declaration:
   * <pre>
   *   class UColor: public UAttr {
   *   public:
   *     UCLASS("UColor", UColor, new UColor)
   *     .....
   *  };
   * </pre>
   *
   * UCLASS redefines getClass() and Class() which return a metaclass that describes
   * the corresponding Ubit class (see Class). getClass() is an instance method,
   * Class() a static method. 
   *
   *
   */
  class Object {
  public:
    struct MetaClass : public Class {
      MetaClass() : Class("Object") {}
      //virtual bool isInstance(Object& obj) const {return dynamic_cast<Object&>(obj);}
      virtual bool isInstance(Object* obj) const {return dynamic_cast<Object*>(obj);}
      virtual Object* newInstance() const {return null;} 
      virtual Style* newStyle() const {return null;}
    };
    
    Object();
    Object(const Object&);
    virtual ~Object();
    Object& operator=(const Object&);
    
    static const char* getVersion();
    ///< returns the version of the Ubit package.

    //static const Class& Class() {static MetaClass& c = *new MetaClass; return c;}
    ///< class method that returns the metaclass of this class. @see getClass().

    virtual const Class& getClass() const {static MetaClass& c = *new MetaClass; return c;}
    /**< instance method that returns the metaclass of this object.
     * Class() and getClass() return a metaclass (see Class) that describes this
     * class. Class() is a class method, while getClass() is an instance method.
     *
     * WARNING: Class() and getClass() must be redefined by ALL Ubit classes.
     * This can be done by invoking the UCLASS macro (see UNode) in the public section
     */
    
    const String& getClassName() const;
    ///< returns getClass().getName().
         
    static Style* createStyle() {return null;}

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    virtual void error(const char* function_name, const char* format, ...) const;
    /**< prints out an error message.
     * this method displays an error message (on std::cerr by default) :
     * - 'function_name' = the name of the function where the error occured
     * - 'format' = a printf-like format
     * - '...'    = a variadic list of arguments that correspond to 'format'.
     * @see also: warning() and UAppli::error().
     */
    
    virtual void warning(const char* function_name, const char* format, ...) const;
    ///< prints out a warning message: @see error().
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    bool isDeletable() const {return omodes.IS_DYNAMIC;}
    /**< checks if this object can be destroyed by 'delete'.
     * objects that were allocated in the heap or in static memory, or that are
     * plain fields of other objects cannot be destroyed by using 'delete'. 
     */
    
    bool isConst() const {return omodes.IS_CONST;}
    ///< checks whether the content of this object can be modified (@see setConst()).
    
    Object& setConst();
    /**< specifies that the content of this object cannot be modified.
     * an attempt to modify the content of this object will then produce an error. 
     * Notes:
     * - constness can't be changed once set. 
     * - this feature makes it possible to specify that an object is constant even if 
     *   it is not declared 'const'. This makes it possible to have constant and non
     *   constant objects included in the same container. Constness is thus checked
     *   at run time instead of compilation time.
     * @see also: UCONST.
     */
     
    bool isAutoUpdate() const {return !omodes.DONT_AUTO_UPDATE;}
    ///< return current update policy: @see setAutoUpdate().

    Object& setAutoUpdate(bool state = true);
    /**< changes the update policy of this object when its is modified.
     * if 'state' is true, the object if automatically updated when modified
     * @see UNode::setAutoUpdate() for more details.
     */
    
    bool isIgnoringChangeCallbacks() const {return omodes.IGNORE_CHANGE_CALLBACKS;}
    ///< return true if change callbacks are fired.
    
    Object& ignoreChangeCallbacks(bool state);
    ///< change callbacks are not fired if argument is true.
    
    
    typedef unsigned char State;
    typedef short PtrCount;
    
    State getState() const {return ostate;}
    ///< returns the current object state.
    
    void setState(State s) {ostate = s;}
    ///< changes the object state.
    
    PtrCount getPtrCount() const {return ptr_count;}  
    ///< [impl] returns the number of uptr<> smart pointers that are pointing to this object.
    
    /// [Impl] internal object modes.
    struct Modes {
      unsigned IS_DYNAMIC:1;         // this obj was allocated in dynamic memory
      unsigned IS_CONST:1;           // this obj is const, its value can't be changed
      unsigned IS_DESTRUCTED:1;      // this obj has been destructed
      unsigned IS_DESTRUCTING:1;     // this obj is being destructed
      unsigned DONT_AUTO_UPDATE:1;   // set(), add() and remove() do NOT call update().
      unsigned IS_UPDATING:1;        // is being updated (repainted or re-layed out).
      unsigned IS_IN_SELECTION:1;    // is part of a text selection
      unsigned IGNORE_CHANGE_CALLBACKS:1;
    };
    
    struct UConst {};
    
    static UConst UCONST;
    /**< UCONST can be given as an argument to some constructors to make the object constant.
     * an attempt to modify the value of this object will produce an error. 
     * Note that constness can't be changed once set. @see also: setConst().
     */
    
    bool checkConst() const;
    ///< [impl] produces an error if this object is logically constant (@see setConst()).
    
    bool isDestructed()  const {return omodes.IS_DESTRUCTED;}
    ///< [impl] this object has been destructed (but memory has not been freed yet).
    
    bool isDestructing() const {return omodes.IS_DESTRUCTING;}
    ///< [impl] this object is being destructed (one of its destructors has been called).
    
    virtual bool hasSceneGraphParent() const {return false;};    // !MUST be virtual!
    ///< [impl] returns true if there is at least one scene graph parent (redefined by UNode).
    
    void* operator new(size_t);
    ///< [impl] internal memory management.
    
    void operator delete(void*);
    ///< delete operator is forbidden on instances that derive from Object.
    
    void addPtr() const;       ///< [Impl] a uptr is added to this object.
    void removePtr() const;    ///< [Impl] a uptr is removed from this object.

  protected:
    friend class UNode;
    friend class UUpdateContext;
    friend class UAppliImpl;
    friend class USelection;
    friend class UPtr;
    
    mutable PtrCount ptr_count;  // uptr<> reference count
    Modes omodes;
    State ostate;
  };
  
}
#endif // UBIT_CORE_OBJECT_H_
