/*
 *  ctlmenu.hpp : Control menus (see upiemenu.hpp)
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

#ifndef _uctlmenu_hpp_
#define _uctlmenu_hpp_ 1

#include <ubit/upiemenu.hpp>
#include <ubit/ucall.hpp>

namespace ubit {
  
  /** Control menu.
   * a control menu makes it possible to select an operation then to control one
   * or two continous values in a single gesture. Control menus have to modes:
   *
   * - novice mode: the user presses the mouse and waits for getShowDelay() msec.
   *   The menu appears and the user selects one of the menu items by dragging the
   *   mouse. When the mouse reaches this item, it is selected and the menu 
   *   disappears. The user can then control one or two values (that correspond 
   *   to the horizontal and vertical directions) by dragging the mouse.
   *
   * - expert mode: the user performs the same gesture, but faster so that he
   *   does not wait for getShowDelay() msec. The same actions are performed
   *   except that the menu does not show up.
   *
   * @see: PieMenu class for inherited methods.
   *
   * Event management:
   * - each menu item can have callbacks as explained in PieMenu class
   * - a major difference with UPiemenus(s) is that items cannot be deselected
   *   one they have been selected by dragging the mouse inside their area
   * - callbacks are fired in the same way in the expert and novice mode. The only
   *   difference is that the menu does not appear in expert mode. In novice
   *   mode, the menu disappears when an item is selected.
   * - ControlAction class and its subclasses can be used to make code simpler. 
   *   A ControlAction object has a mdrag() method that is called when the mouse is 
   *   dragged as shown in the example below:
   *
   * <pre>
   *   class Demo {
   *   public:
   *       void scrollMore(MouseEvent& e);
   *       ....
   *   };
   *  
   *   Demo* d = new Demo();
   *   ControlMenu* cmenu = new ControlMenu(); 
   *
   *   // scrollMore() is fired if item(0) has been selected and the mouse 
   *   // is being dragged (even if it is now outside of item(0))
   *
   *   cmenu->item(0)->add(" Scroll More " + UOn::mdrag / ucall(this, &Demo::scrollMore))
   *
   *   // here we use the predefined ScrollAction class (that derives from 
   *   // ControlAction) to perform the same kind of behavior in a simpler way
   *
   *   ScrollAction* a = new ScrollAction(...);
   *   cmenu->item(4)->add(" Scroll Less " + *a)
   * </pre>
   */  
  class ControlMenu : public PieMenu {
  public:
    UCLASS(ControlMenu)
    static Style* createStyle();

    ControlMenu();
    ///< creates a new Control Menu; @see also shortcut function uctlmenu().
  };
  
  inline ControlMenu& uctlmenu() {return *new ControlMenu;}
  ///< shortcut function that creates a new control menu.
  
  
  /** Callback object for ControlMenu items (base class).
   * This class is intended to be subclassed by implementation classes that
   * redefine the mdrag() method.
   */
  class ControlAction : public UCall {
  public:
    ControlAction(float _gain) : xmag(_gain), ymag(_gain) {}
    virtual ~ControlAction() {}
    
    void  setGain(float _gain) {xmag = ymag = _gain;}
    void  setGain(float _xgain, float _ygain) {xmag = _xgain, ymag = _ygain;}
    float getXGain() const {return xmag;}
    float getYGain() const {return ymag;}
    
  protected:
    virtual void mdrag(MouseEvent& e, ControlMenu&) = 0;
    /**< called when the mouse is dragged (must be redefined by subclasses).
     * This method is called repeatedly when one of the menu items has been
     * selected and the mouse is being dragged. Note that e.isFirstDrag() returns
     * true at the beginning of a drag sequence. @see: ControlMenu::item() for details,
     */
    
    virtual void operator()(Event&);
    ///< [impl] calls mdrag() with appropriate arguments (no need to redefine this method).
    
    virtual void addingTo(Child&, Element& parent);
    ///< [impl] adds the UOn::mdrag event condition  (no need to redefine this method).
    
    float xmag, ymag;
  };
  
  
  /** Centred zoom (Callback object for ControlMenu items).
   */
  class ZoomAction : public ControlAction {
  public:
    ZoomAction(Box& zoomed_box, float gain = 0.5);
  protected:
    unique_ptr<Box> zbox;     // zoomed box
    unique_ptr<Position> posAttr; // box pos attribute (NB: its value is scale independent)
    unique_ptr<Scale> scaleAttr; // box scale attribute
    float  zbox_scale0;    // initial scale of box 
    Point zbox_pos0;     // initial (scale indep) pos of box in its container
    Point mouse_in_zbox0;
    float mouse_delta;
    virtual void mdrag(MouseEvent&, ControlMenu&);
  };
  
  
  /** Panning (Callback object for ControlMenu items).
   */
  class PanAction : public ControlAction {
  public:
    PanAction(Box& panned_box, float gain = 1.);
  protected:
    unique_ptr<Box> box;     // panned box
    unique_ptr<Position> posAttr; // box pos attribute (NB: its value is scale independent)
    Point pos0;        // initial (scale indep) pos of box in its container
    //float container_scale;
    virtual void mdrag(MouseEvent&, ControlMenu&);
  };

  
  /** Scrolling (Callback object for ControlMenu items).
   */
  class ScrollAction : public ControlAction {
  public:
    ScrollAction(Scrollpane&, float gain = 0.33);
    ScrollAction(float gain = 0.3);

    void setPane(Scrollpane&);
    Scrollpane* getPane() const {return pane;}
    
  protected:
    virtual void mdrag(MouseEvent&, ControlMenu&);
    Scrollpane* pane;
    float arm_xpos, arm_ypos, xpos, ypos;
  };
  
}
#endif
