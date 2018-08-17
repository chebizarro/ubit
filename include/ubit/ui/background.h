/*
 *  background.h: Widget background and transparency.
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

#ifndef UBIT_UI_BACKGROUND_H_
#define	UBIT_UI_BACKGROUND_H_

#include <ubit/core/attribute.h>

namespace ubit {
  
  /**
   * Widget background.
   * 
   * This attributes controls the background of widgets (objects deriving from Box).
   * If not added to the widget's attribute list, the default background specified
   * in the widget's ubit::Style is used. This default background if transparent for most
   * widgets, so that the background of the first non transparent parent widget
   * will appear.
   */
  class Background : public Attribute {
  public:
    
    static Background none, inherit,
    metal, velin, white, black, grey, lightgrey, darkgrey, navy, lightblue, blue, 
    red, green, yellow, orange, wheat, teal;
    ///< predefined background constants.
    
    ~Background();
    
    Background();
    ///< creates an empty background attribute.
    
    Background(const Background&);
    ///< creates a background from another one.
    
    Background(const Color&);
    ///< creates a background prop with a solid color (the Color argument is copied).
    
    Background(Image&);  
    ///< creates a background prop with an image (the Image argument is copied).
    
    Background& operator=(const Background&);
    ///< copies a background attribute from another one.

    bool operator==(const Background&);
    ///< compares two background attributes.

    virtual Background& setColor(const Color& color);
    /**< changes the background color.
     * the Color argument is copied internally so that this background will remain
     * valid if this argument is destroyed.
     */
    
    virtual Background& setRgba(const Rgba&);
    ///< changes the background color.
     
    virtual Background& setRbgaF(float r, float g, float b, float a = 1.);
    ///< changes the background color, arguments are floats in range [0,1].
    
    virtual Background& setRbgaI(unsigned int r, unsigned int g, unsigned int b,
                                      unsigned int a = 255);
    ///< changes the background color, arguments are integers in range [0,255].

    virtual Background& setNamedColor(const String& color_name, float a = 1.);
    ///< changes the background color, arguments are a color name and an alpha value in range [0,1].
    
    virtual Background& setIma(Image& image, bool tiling = true);
    /**< changes the background image (WARNING: see details).
     * WARNING: the Image argument is NOT copied internally: this background will
     * NOT be valid if this argument is destroyed.
     */
    
    Background& setTiling(bool);
    ///< sets tiling on or off.
    
    const Color* getColor() const;
    ///< returns the current color if any; null otherwise.
    
    const Image* getIma() const;
    ///< returns the current image if any; null otherwise.
    
    bool isTiled() const;
    ///< returns true if the image is tiled.
    
    virtual void update();
    virtual void putProp(UpdateContext*, Element&);
        
    // - impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
    Background(Color&, UConst);
    Background(Image&, UConst);
  protected:
    uptr<Color> pcolor;
    uptr<Image> pima;
    float alpha;
    bool tiling;
#endif
  };
  
  inline Background& ubackground() {return *new Background();}
  ///< shortcut for *new Background().
  
  inline Background& ubackground(const Color& col) {return *new Background(col);}
  ///< shortcut for *new Background(col).

  inline Background& ubackground(Image& ima) {return *new Background(ima);}
  ///< shortcut for *new Background(ima).

  /** Widget transparency.
   * value must be in range [0.0, 1.0]. A value of 1. makes the widget opaque,
   * a value of 0. makes it fully transparent
   */
  class UAlpha : public Attribute {
  public:
    UCLASS(UAlpha)
    
    UAlpha(float = 1.0);
    /**< creates an alpha blending property; see also shortcut function ualpha().
     * specifies if the widget (ubit:Box and subclass) that contains this prop
     * is opaque (value = 1.0), fully stransparent (value = 0.0)
     * or translucent (value in range ]0.0, 1.0[)
     */
    
    operator float() const {return val;}
    ///< type conversion: returns value  which is in range [0., 1.].
    
    UAlpha& operator=(float v)         {return set(v);}
    UAlpha& operator=(const Float& v) {return set(v);}
    UAlpha& operator=(const UAlpha& v) {return set(v.val);}
    virtual UAlpha& set(float);  
    ///< changes alpha value. must be in range [0., 1.].
    
    bool operator==(float v)         const {return val == v;}  
    bool operator==(const Float& v) const {return val == v;}  
    bool operator==(const UAlpha& v) const {return val == v.val;}
    
    virtual void update();
    virtual void putProp(UpdateContext*, Element&);
    
  private:
    float val;
  };
  
  inline UAlpha& ualpha(float value = 1.0) {return *new UAlpha(value);}
  ///< shortcut function that returns *new UAlpha(value).
  
}
#endif

