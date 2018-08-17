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
    virtual ~Object();
    
    Object(Object const &) = delete;
    Object& operator=(Object const &) = delete;

  protected:
    Object();
 
  };
  
}

#endif // UBIT_CORE_OBJECT_H_
