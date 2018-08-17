/*
 *  argsImpl.cpp : class for creating argument lists ("additive notation").
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

#ifndef _uargsImpl_hpp_
#define	_uargsImpl_hpp_  1

#include <ubit/core/node.h>

namespace ubit {
  
    // implementation of Args (should not be used directly by clients).

  class Args;
  class UArgsImpl;
  class _UAttrArgs;
  
  const UArgsImpl& operator+(const UArgsImpl&, const char*);
  const UArgsImpl& operator+(const UArgsImpl&, Node*);
  const UArgsImpl& operator+(const UArgsImpl&, Node&);
  const UArgsImpl& operator+(const UArgsImpl&, const Child&);
  const UArgsImpl& operator+(const UArgsImpl&, const Args&); 
  
  const _UAttrArgs& operator,(const _UAttrArgs&, Attribute*);
  const _UAttrArgs& operator,(const _UAttrArgs&, Attribute&);
  const _UAttrArgs& operator,(const _UAttrArgs&, UCall*);
  const _UAttrArgs& operator,(const _UAttrArgs&, UCall&);
  //const _UAttrArgs& operator,(const _UAttrArgs&, const UAttrChild&);


  class UArgsImpl {
  public:
    UArgsImpl();
    UArgsImpl(Node*);
    UArgsImpl(Node&);
    UArgsImpl(const char*);
    UArgsImpl(const Child&);  
    UArgsImpl(const Args&);
    UArgsImpl(const UArgsImpl&);
    ~UArgsImpl();

    friend const UArgsImpl& operator+(const UArgsImpl&, const char*);
    friend const UArgsImpl& operator+(const UArgsImpl&, Node*);
    friend const UArgsImpl& operator+(const UArgsImpl&, Node&);
    friend const UArgsImpl& operator+(const UArgsImpl&, const Child&);
    friend const UArgsImpl& operator+(const UArgsImpl&, const Args&);
    
    friend const UArgsImpl& operator+(const _UAttrArgs&, const UArgsImpl&);  // !!!!!

  private:
    UArgsImpl& operator=(const UArgsImpl&);  // assigment is forbidden
    friend class Element;
    friend class Args;
    class UArgsChildren* children;
  };
  
  
  class _UAttrArgs {
  public:
    _UAttrArgs();
    _UAttrArgs(Attribute*);
    _UAttrArgs(Attribute&);
    _UAttrArgs(UCall*);
    _UAttrArgs(UCall&);
    _UAttrArgs(const Child&);  
    //_UAttrArgs(const _UAttrArgs&);
    ~_UAttrArgs();

    friend const _UAttrArgs& operator,(const _UAttrArgs&, Attribute*);
    friend const _UAttrArgs& operator,(const _UAttrArgs&, Attribute&);
    friend const _UAttrArgs& operator,(const _UAttrArgs&, UCall*);
    friend const _UAttrArgs& operator,(const _UAttrArgs&, UCall&);
    //friend const _UAttrArgs& operator,(const _UAttrArgs&, const UAttrChild&);
    
    friend const UArgsImpl& operator+(const _UAttrArgs&, const UArgsImpl&);  // !!!!!

  private:
    _UAttrArgs& operator=(const _UAttrArgs&);  // assigment is forbidden
    friend class Element;
    friend class Args;
    class UArgsChildren* children;
  };
    
  
  class UArgsChildren : public Children {
    friend class Args;
    friend class UArgsImpl;
    friend class _UAttrArgs;
    int refcount;
    //UArgsChildren(); test for counting creation/deletion balance
    //~UArgsChildren();
  };
  
}
#endif
