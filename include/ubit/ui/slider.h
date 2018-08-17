/*
 *  slider.hpp
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

#ifndef _uslider_hpp_
#define	_uslider_hpp_ 1

#include <ubit/unumber.hpp>
#include <ubit/ubox.hpp>
#include <ubit/ustyle.hpp>

namespace ubit {
  
  struct USliderStyle : public UStyle {
    USliderStyle();
    const UStyle& getStyle(UpdateContext* ctx) const;
    // default is hstyle
    UStyle vstyle;
  };
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** Slider widget.
   * the value is a percentage in the range [0., 100.]
   *
   * Geometry:
   *  - horizontal orientation by default 
   *  - addAttr(UOrient::vertical) makes slider vertical
   *  - shortcut functions uhslider() and uvslider() specify the orientation
   *
   * Callbacks
   *  - UOn::action (default callback) is fired when the value is changed
   *  - UOn::change is fired continuously while the knob is dragged
   */
  class USlider: public Box {
  public:
    UCLASS(USlider)
    
    USlider(const Args& = Args::none);
    /**< creates a new slider (@see also shortcut uslider(Args&)).
     * Default orientation is horizontal. new USlider(UOrient::vertical) creates
     * a vertical slider. The slider's value is in the range [0., 100.]
     */
    
    USlider(Float& value, const Args& = Args::none);
    /**< creates a new slider with a model value (@see also shortcut uslider(Float&, Args&)).
     * the slider is synchronized with this value. Several widgets (eg. sliders)
     * that share the same value are synchronized.
     * Default orientation is horizontal. new USlider(value, UOrient::vertical) creates
     * a vertical slider. The slider's value is in the range [0., 100.]
     */
  
    float getValue() const {return *pvalue;}
    ///< returns the current value (a float between 0 and 100).
    
    USlider& setValue(float val) {*pvalue = val; return *this;}
    /**< changes the current value (a float between 0 and 100).
     * UOn::change and UOn::action callbacks are fired when
     * the value is changed (see class USlider).
     */
    
    Float& value() {return *pvalue;}
    /**< return the internal value model.
     * UOn::change and UOn::action callbacks are fired when
     * the value is changed (see class USlider).
     * this object can be shared but can't be deleted.
     */
    
    // - - - impl.  - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    static UStyle* createStyle() {return createSliderStyle();}
    static USliderStyle* createSliderStyle() {return new USliderStyle;}
    
    virtual Box* createKnob();
    ///< [impl] creates the knob; may be redefined by subclasses.
    
    virtual Box* createRail();
    ///< [impl] creates the rail; may be redefined by subclasses.
    
  protected:
    uptr<Float> pvalue;        // corresponding value (percent)
    uptr<Box> prail, pknob;
    uptr<UPos> pknob_pos;
    uptr<class UPosControl> pknob_ctrl;
    
    void constructs();
    virtual void gotoPosCB(UMouseEvent&);
    virtual void actionCB(Event&);  //UNodeEvent
    virtual void changeCB(Event&);  //UNodeEvent
  };
  
  
  USlider& uslider(const Args& a = Args::none);
  ///< shortcut that creates a new slider (that is horizontal by default).
  
  USlider& uslider(Float& value, const Args& a = Args::none);
  ///< shortcut that creates a new slider (that is horizontal by default) with a model value.
  
  USlider& uhslider(const Args& a = Args::none);
  ///< shortcut that creates a new horizontal slider.
  
  USlider& uvslider(const Args& a = Args::none);
  ///< shortcut that creates a new vertical slider.
  
}
#endif

