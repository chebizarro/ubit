/*
 *  data.h: all viewable objects that can be inserted in an element
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


#ifndef UBIT_CORE_DATA_H_
#define	UBIT_CORE_DATA_H_

#include <ubit/core/node.h>

namespace ubit {

/** Base class for Viewable Objects.
  *
  * Data objects can have callback functions that are fired when their value is
  * changed. Data parents can also be notified when data value are modified
  * by using the UOn::dataChange condition (see below).
  *
  * see an example in class String. The only difference is that UOn::dataChange
  * must be used instead of UOn::strChange for specifing callbacks in parents. 
 */
class Data: public Node {
public:
  //UCLASS("#data", Data, null);
  UABSTRACT_CLASS(Data);

  Data() {}
  Data(UConst) {omodes.IS_CONST = true;}

  virtual int getDisplayType() const {return DATA;}

  virtual Data* toData() {return this;}
  virtual const Data* toData() const {return this;}

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  virtual Data& onChange(UCall&);
  /**< adds a callback that is fired when the value is modified.
    * @see Data doc.
    */
  
  virtual void changed(bool update = true);
  /**< [impl] called when object's content is changed.
    * This function updates graphics (if 'update' is true) then fires 
    * parents' UOn::dataChange callbacks
    */

  virtual void update() = 0;
  ///< updates graphics.

  friend class Args;
  friend class Box;
  virtual void getSize(UpdateContext&, Dimension&) const = 0;
  virtual void paint(Graph&, UpdateContext&, const Rectangle&) const = 0;
};
}
#endif // UBIT_CORE_DATA_H_
