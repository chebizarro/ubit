/*
 *  zoom.h: semantic zooming
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

#ifndef _uzoom_hpp_
#define	_uzoom_hpp_ 1

#include <ubit/ucond.hpp>
#include <ubit/ubox.hpp>
#include <ubit/uctlmenu.hpp>

namespace ubit {
  
  class UZoommenu;
  
  /** Zoom pane for zooming and panning children.
   * - widgets to be zoomed must be added to the zoompane viewport (returned by viewport()).
   * - the zoompane creates a zoommenu that makes it possible to zoom and pane its
   *   children interactively. By default, this menu is opened by pressing the right
   *   button mouse OR the CTRL key and the left mouse button. 
   * - additionally, the scaling and panning values can be changed programmatically 
   *   through the scale() and pos() properties.
   */
  class UZoompane : public Box {
  public:
    UCLASS(UZoompane)
    
    UZoompane(Args arglist = Args::none);
    
    Box& viewport() {return *pviewport;}
    ///< returns the viewport that is controlled by this zoompane.
    
    UPos& viewportPos() {return *ppos;}
    /**< returns the position attribute of the viewport.
     * example: my_zoombox.viewportPos().set(25, 100);
     */
    
    UScale& viewportScale() {return *pscale;}
    /**< returns the scale attribute of the viewport.
     * example: my_zoombox.viewportScale() = 0.5;
     */    
       
    UZoommenu& zoommenu() {return *pmenu;}
    /**< returns the menu that controls this zoompane.
     * this menu is automatically created.  By default, it is opened by pressing
     *  the right button mouse OR the CTRL key and the left mouse button. 
     */
    
    void openMenuOn(int event_mask);
    
   private:
    uptr<Box> pviewport;
    uptr<UZoommenu> pmenu;
    uptr<UPos> ppos;
    uptr<UScale> pscale;
  };
  
  inline UZoompane& uzoompane(Args arglist = Args::none) {return *new UZoompane(arglist);}
  ///< shortcut function that creates a new UZoompane.


  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** Zoom menu for zooming and panning widgets.
   * used in UZoompane among other things.
   */
  class UZoommenu: public UCtlmenu {
  public:
    UCLASSDEF("UZoommenu", UZoommenu, null)  //  UZoommenu() not defined
    
    UZoommenu(Box& zoomed_box, Box& panned_box);
    /**< a Control menu that will zoom and pan a widget.
     * zoomed_box and panned_box may differ as one may pan a widget (e.g. a UPalette)
     * but only zoom its content (e.g. UPalette::content())
     */
    
    void openMenuOn(int event_mask);
    virtual void openMenuCB(UMouseEvent&);

  protected:
    uptr<UZoomAction> pzoom_action; 
    uptr<UPanAction> ppan_action;
    int event_mask;
  };
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** Zooming conditions (for semantic zooming).
   * objects prefixed by this range condition will only appear or be effective
   * if the scale (see UScale) is inside this range.
   * example:
   * <pre>
   *      uinscale(0.75,2.50) / ubox(Color::red + "Hello World!")
   * </pre>
   * this box will only be visible is the scale is between 0.75 and 2.50
   *
   * !Warning: UInscale objects must not be deleted!
   *
   * Note: UInscale was called USrange in older versions of the Ubit toolkit.
   */
  class UInscale : public Condition {
  public:
    UInscale() : smin(0.), smax(0.) {}
    UInscale(float _smin, float _smax) : smin(_smin), smax(_smax) {}
    
    void  setMin(float _smin) {smin = _smin;}
    void  setMax(float _smax) {smax = _smax;}
    float getMin() const {return smin;}
    float getMax() const {return smax;}
    
    virtual bool verifies(const UpdateContext&, const Element&) const;
  private:
    float smin, smax;
  };
  
  inline UInscale& uinscale(float smin, float smax) {return *new UInscale(smin,smax);}
  ///< shOrtcut for *new UInscale().
  
}
#endif
