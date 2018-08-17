/*
 *  message.h: Ubit Inter-Client Messages: see umservice.hpp 
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

#ifndef _umessage_hpp_
#define	_umessage_hpp_
#include <map>
#include <string>
#include <ubit/uelem.hpp>
#include <ubit/uattr.hpp>

namespace ubit {

/**
 * Ubit Message.
 */
class Message {
public:
  static void send(UHardwinImpl&, const char* message);
  static void send(UHardwinImpl&, const String& message);
};

/**
 * Ubit Message Port.
 */
class MessagePort : public Element {
public:
  MessagePort(const String& name);
  const String& getName()  {return name;}
  const String& getValue() {return value;}

private:
  friend class MessagePortMap;
  String name, value;
};

/**
 * Implementation
 */
class MessagePortMap {
public:
  MessagePort& getMessagePort(const String& name);
  MessagePort* findMessagePort(const String& name);
  void fireMessagePort(const char* data);

private:
  struct Comp {
    bool operator()(const String* s1, const String* s2) const
    {return s1->compare(*s2) < 0;}
  };
  typedef std::map<const String*, MessagePort*, Comp> MessMap;
  MessMap mess_map;
  void _fireMessagePort(MessageEvent&, const char* buf);
};

}
#endif

