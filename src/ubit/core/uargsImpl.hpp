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
  class ArgsImpl;
  class _UAttrArgs;
  
  const ArgsImpl& operator+(const ArgsImpl&, const char*);
  const ArgsImpl& operator+(const ArgsImpl&, Node*);
  const ArgsImpl& operator+(const ArgsImpl&, Node&);
  const ArgsImpl& operator+(const ArgsImpl&, const Child&);
  const ArgsImpl& operator+(const ArgsImpl&, const Args&); 
  
  const _UAttrArgs& operator,(const _UAttrArgs&, Attribute*);
  const _UAttrArgs& operator,(const _UAttrArgs&, Attribute&);
  const _UAttrArgs& operator,(const _UAttrArgs&, UCall*);
  const _UAttrArgs& operator,(const _UAttrArgs&, UCall&);
  //const _UAttrArgs& operator,(const _UAttrArgs&, const UAttrChild&);


  class ArgsImpl {
  public:
    ArgsImpl();
    ArgsImpl(Node*);
    ArgsImpl(Node&);
    ArgsImpl(const char*);
    ArgsImpl(const Child&);  
    ArgsImpl(const Args&);
    ArgsImpl(const ArgsImpl&);
    ~ArgsImpl();

    friend const ArgsImpl& operator+(const ArgsImpl&, const char*);
    friend const ArgsImpl& operator+(const ArgsImpl&, Node*);
    friend const ArgsImpl& operator+(const ArgsImpl&, Node&);
    friend const ArgsImpl& operator+(const ArgsImpl&, const Child&);
    friend const ArgsImpl& operator+(const ArgsImpl&, const Args&);
    
    friend const ArgsImpl& operator+(const _UAttrArgs&, const ArgsImpl&);  // !!!!!

  private:
    ArgsImpl& operator=(const ArgsImpl&);  // assigment is forbidden
    friend class Element;
    friend class Args;
    class ArgsChildren* children;
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
    
    friend const ArgsImpl& operator+(const _UAttrArgs&, const ArgsImpl&);  // !!!!!

  private:
    _UAttrArgs& operator=(const _UAttrArgs&);  // assigment is forbidden
    friend class Element;
    friend class Args;
    class ArgsChildren* children;
  };
    
  
  class ArgsChildren : public Children {
    friend class Args;
    friend class ArgsImpl;
    friend class _UAttrArgs;
    int refcount;
    //ArgsChildren(); test for counting creation/deletion balance
    //~ArgsChildren();
  };
  
}
#endif
