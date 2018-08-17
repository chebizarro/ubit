/*
 *  ucolor.h: Color Attribute
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


#ifndef _ucolor_hpp_
#define	_ucolor_hpp_ 1

#include <string.h>
#include <ubit/uattr.hpp>

namespace ubit {
  
  /** Specifies a RGBA value for defining Color(s).
   */
  class URgba {
  public:
    URgba() {comps[0] = comps[1] = comps[2] = 0; comps[3] = 255u;}
        
    URgba(unsigned int r, unsigned int g, unsigned int b, unsigned int a = 255u) 
    {comps[0] = r; comps[1] = g; comps[2] = b; comps[3] = a;}
    
    URgba(float r, float g, float b, float a = 1.0f) {setRgbaF(r,g,b,a);}

    void setRgbaI(unsigned int r, unsigned int g, unsigned int b, unsigned int a = 255u) 
    {comps[0] = r; comps[1] = g; comps[2] = b; comps[3] = a;}
    
    void setRgbaF(float r, float g, float b, float a = 1.0f);

    bool operator==(const URgba& c) const {
      return (comps[0]==c.comps[0] && comps[1]==c.comps[1] 
              && comps[2]==c.comps[2] && comps[3]==c.comps[3]);
    }    

    //bool equals(unsigned int r, unsigned int g, unsigned int b, unsigned int a) const {
    //return components[0]==r && components[1]==g && components[2]==b && components[3]==a;}
    
    //bool equals(float r, float g, float b, float a) const;

    unsigned int getRedI()   const {return comps[0];}
    unsigned int getGreenI() const {return comps[1];}
    unsigned int getBlueI()  const {return comps[2];}
    unsigned int getAlphaI() const {return comps[3];}
    
    void setRedI(unsigned int c)   {comps[0] = c;}
    void setGreenI(unsigned int c) {comps[1] = c;}
    void setBlueI(unsigned int c)  {comps[2] = c;}
    void setAlphaI(unsigned int c) {comps[3] = c;}
    
    unsigned char comps[4];  // must be compatible with GLubyte
  };
  
  
  /** Color attribute of an element or a widget.
   * This attributes specifies the foreground color of an object that derives
   * from Element (i.e. element nodes and widgets). This color will be inherited
   * by the children of this element if they dont't specify a color of their own.
   *
   * Color attributes can either be added to the element's attribute or child list.
   *
   * The alpha value is ignored if OpenGL is not used: see Application::isUsingGL().
   *
   * @see also: Attribute.
   */
  class Color : public Attribute {
  public:
    UCLASS(Color)
    
    static Color none, inherit, white, black, grey, lightgrey, darkgrey, 
    navy, lightblue, blue, red, green, yellow, orange, wheat, teal, disabled;
    /**< predefined colors.
     * the value of these color are predefined and unmutable (attempting to change
     * them would produce an error)
     */
    
    Color();
    ///< creates a solid black color.
    
    Color(const Color& color);
    ///< creates a new color object that is a copy of 'color'.
        
    Color(const Color& color, float a);
    ///< creates a new color object that is a copy of 'color' but with a different alpha.

    Color(const Color& color, unsigned int a);
    ///< creates a new color object that is a copy of 'color' but with a different alpha.

    Color(const URgba&);
    ///<creates a new color object from a URgba value.
    
    Color(float r, float g, float b, float a = 1.0f);
    /**< creates a new color object with a RGBA value; see also shortcut ucolor(r,g,b,a).
     * r,g,b,a are floats in the range [0 - 1]. To avoid ambiguities, they MUST be
     * explicitely specified as floats, for instance: Color(1f, 0f, 0f, 0.5f).
     */
    
    Color(unsigned int r, unsigned int g, unsigned int b, unsigned int a = 255u);
    /**< creates a new color object with a RGBA value; see also shortcut ucolor(r,g,b,a).
     * r,g,b,a are unsigned integers in the range [0 - 255]. To avoid ambiguities, they
     * MUST be explicitely specified as unsigned ints, e.g.: Color(255u, 0u, 0u, 125u).
     */
  
    Color(const String& color_name, float a = 1.);
    ///<creates a new color object from a color name (see setNamedColor()).

    ~Color();
    
    Color& operator=(const Color& c);
    ///< changes color from another color.

    Color& setRgba(const URgba&);
    ///< changes color, arguments is a URgba struct.
         
    Color& setRgbaF(float r, float g, float b, float a = 1.0f);
    ///< changes color, arguments are floats in range [0 - 1].
    
    Color& setRgbaI(unsigned int r, unsigned int g, unsigned int b, unsigned int a=255u);
    ///< changes color, arguments are unsigned ints in range [0 - 255].
    
    Color& setNamedColor(const String& color_name, float a = 1.);
    /**< changes color, arguments are a color name and an alpha value in range [0,1].
     * 'color_name' can be on of the following:
     * - #rgb or #rrggbb or #rrrrggggbbbb with r,g,b being hexadecimal digits
     * - grayXXX with XXX being a number from 0 to 100
     * - a HTML color name
     * the alpha value is ignored if OpenGL is not used for rendition: see Application::isUsingGL()
     */
    
    const URgba& getRgba() const {return rgba;}
     
    bool operator==(const Color& c) const {return equals(c);}
    bool operator!=(const Color& c) const {return !equals(c);}
    
    virtual bool equals(const Color&) const;
    virtual bool equals(const URgba&) const;
    virtual bool equals(float r, float g, float b, float a) const;
    virtual bool equals(unsigned int r, unsigned int g, unsigned int b, unsigned int a) const;
    
    virtual void update();  // redefinition

    virtual void putProp(UpdateContext*, Element&);   // redefinition
    
    static void addNamedColor(const char* name, const URgba&);
    ///< adds a color to the database of named colors.
    
    static bool parseColor(const char* name, URgba&);
    ///< returns the URgba corresponding to this color name, if found.
    
    // - - - impl - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
  protected:
    friend class Graph;
    URgba rgba;
#if WITH_2D_GRAPHICS
    mutable std::vector<unsigned long> pixels;
    void unsetPixels();
    unsigned long getPixel(Display*) const;
#endif
    Color(unsigned int r, unsigned int g, unsigned int b, unsigned int a, UConst);
    //[Impl] creates a new CONSTANT color with a RGB value; @see UObject::UCONST.
    
    Color(unsigned char special, UConst);
    //[Impl] creates a new CONSTANT color with a special value; @see UObject::UCONST.
    
#endif
  };
  
  
  inline Color& ucolor() {return *new Color();}
  ///< shortcut function that performs *new Color();
  
  inline Color& ucolor(float r, float g, float b, float a = 1.) 
  {return *new Color(r,g,b,a);}
  ///< shortcut function that performs *new Color(r, g, b, a);
  
  inline Color& ucolor(unsigned int r, unsigned int g, unsigned int b, unsigned int a = 255) 
  {return *new Color(r,g,b,a);}
  ///< shortcut function that performs *new Color(r, g, b, a);
  
  inline Color& ucolor(const String& color_name, float alpha = 1.) 
  {return *new Color(color_name, alpha);}
  ///< shortcut function that performs *new Color(color_name, alpha);

}
#endif
