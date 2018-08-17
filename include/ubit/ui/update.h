/*
 *  update.hpp : graphics update
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


#ifndef _update_hpp_
#define	_update_hpp_ 1

#include <ubit/udefs.hpp>

namespace ubit {

/** specifies how to update Element, Box, Window objects and subclasses.
 * See also: Element::update(), Box::update(), Window::update().
 */
class Update {
public:
  enum {
    PAINT = 1<<0,
    ///< updates the paint (not the layout).
    
    LAYOUT = 1<<1,
    ///< updates the layout (not the paint).
    
    LAYOUT_PAINT = (LAYOUT | PAINT),
    ///< updates the layout and the paint.
    
    HIDDEN_OBJECTS = 1<<3,
    /**< updates objects even if they are not shown.
     * by default, hidden objects (when isShown() is false) are not updated.
     */

    SHOW = 1<<6,
    HIDE = 1<<7,
    ADJUST_WIN_SIZE = 1<<8
  };
  
  static const Update paint;
  ///< paint only: prefined constant for Update(PAINT);
  
  static const Update layoutAndPaint;
  ///< layout then paint: prefined constant for Update(LAYOUT|PAINT);
  
  ~Update();

  Update(unsigned int mode_mask = LAYOUT|PAINT);  
  /**< creates an object that specifies how to update Element(s) and subclasses.
    * This object is given as an argument to Element::update(), Box::update(),
    * Window::update()
    * 'mode_mask' is a OREd combination of the constants defined in the Update enum
    * example:
    * <pre>
    *     ;
    *     Box* box = ...;
    *     box->update(Update upd(Update::LAYOUT | Update::PAINT));
    * </pre>
    */
    
  Update(const Update&);
  Update& operator=(const Update&);
  
  bool operator==(const Update&);
  
  void addModes(unsigned int m) {modes |= m;}

  long getModes() const {return modes;}
  ///< returns the current mode mask.

  bool isHiddenObjectsMode() const {return (modes & HIDDEN_OBJECTS) != 0;}
  ///< if true, hidden objects (which are normally ignored) will be updated.

  void setPaintData(const Data*);
  ///< [impl] set the data to be painted.
  
  void setPaintStr(const String*, int strpos1, int strpos2);
  ///< [impl] set the string to be painted.
  
  void setMove(float delta_x, float delta_y, bool x_percent, bool y_percent);
  ///< [impl] used to move objects.
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  enum {
    ADD_REMOVE = 1<<11,
    STR_DATA = 1<<12,  // doit impliquer PAINT
    //SCROLL = 1<<13,    // appel direct de updateView() par Scrollpane sans update()
    MOVE   = 1<<14     // appel en NO_DELAY par UPos
  };
  
private:
  friend class Element;
  friend class Box;
  friend class Window;
  friend class View;
  friend class UAppliImpl;
  friend class UpdateRequest;
  long modes;

public:  // a cause d'un bug de CC
  struct ScrollMove {
    float delta_x, delta_y;
    bool xpercent, ypercent;
  };
  
  struct StrData {
    const Data* data;
    mutable Rectangle* region;  // mutable impose par Box::doUpdate()
    int pos1, pos2;
  };
  
private:
  // IMPL NOTE: il ne DOIT pas y avoir de subclasses car Update est copie
  // directement dans UpdateRequest (voir uappliImpl.hpp)  
  union /*UpdateImpl*/ {
    StrData sd;
    ScrollMove sm;
  } props;
};


}
#endif
