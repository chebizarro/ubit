/**
 *  object.h: Base Class for all Ubit Objects
 *  Ubit GUI Toolkit - Version 8
 *  (C) 2018 Chris Daley
 *  (C) 2009 | Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
 *
 * ***********************************************************************
 * COPYRIGHT NOTICE :
 * THIS PROGRAM IS DISTRIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE
 * IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION;
 * EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 * SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
 * ***********************************************************************/

#ifndef UBIT_CORE_OBJECT_H_
#define	UBIT_CORE_OBJECT_H_

#include <stdlib.h>    // for size_t, new(), delete()

namespace ubit {
  
  /**
   * Base class of most Ubit objects.
   *
   * Object is the base class for (almost) all Ubit objects. Its main subclasses are
   * ubit::Node, ubit::Attribute, ubit::Element that play the same role as Node,
   * Attribute and Element in a XML/DOM model and ubit::Box and ubit::Window that are
   * the base classes for widgets and windows.
   *
   */
  class Object {
  public:
    
    Object();
    Object(const Object&);
    virtual ~Object();
    Object& operator=(const Object&);
             
    static Style* create_style() {return NULL;}

    bool is_auto_update() const {return !omodes.DONT_AUTO_UPDATE;}
    ///< return current update policy: @see setAutoUpdate().

    Object& set_auto_update(bool state = true);
    /**< changes the update policy of this object when its is modified.
     * if 'state' is true, the object if automatically updated when modified
     * @see Node::setAutoUpdate() for more details.
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
    ///< [impl] returns true if there is at least one scene graph parent (redefined by Node).
    
    void* operator new(size_t);
    ///< [impl] internal memory management.
    
    void operator delete(void*);
    ///< delete operator is forbidden on instances that derive from Object.
    
    void addPtr() const;       ///< [Impl] a uptr is added to this object.
    void removePtr() const;    ///< [Impl] a uptr is removed from this object.

  protected:
    friend class Node;
    friend class UpdateContext;
    friend class AppliImpl;
    friend class Selection;
    
    Modes omodes;
    State ostate;
  };
  
}

#endif // UBIT_CORE_OBJECT_H_
