/*
 *  font.h: Font Attribute
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

#ifndef _ufont_hpp_
#define	_ufont_hpp_ 1

#include <ubit/uattr.hpp>
#include <ubit/ustr.hpp>

namespace ubit {
  
  /** Font of a Element or Box container.
   * this property specifies the font in a Element or a Box. It is inherited
   * by children, except if they have their own Font, or if their UStyle
   * specifies another font.
   * As other Attribute(s), a UScale instance must be added to the attribute list
   * or child list of the container: @see: Attribute.
   */
  class Font : public Attribute {
  public:
    UCLASS(Font)
    
    enum Style {
      PLAIN         = 0,
      BOLD          = 1<<0,	// DON'T CHANGE VALUE!
      ITALIC        = 1<<1,	// DON'T CHANGE VALUE!
      FILL          = 1<<2,
      UNDERLINE     = 1<<3,
      OVERLINE      = 1<<4,
      STRIKETHROUGH = 1<<5
    };
    ///< bitmask values for setStyle().
    
    static Font sans_serif, serif, monospace;
    ///< default font family modifiers.
    
    static Font xx_small, x_small, small, medium, large, x_large, xx_large;
    ///< default font size modifiers.
    
    static Font plain, bold, italic, fill, underline, overline, strikethrough;
    /**< font style modifiers.
     * - 'plain' means not bold, not italic, not underlined
     * -  mofifiers can be combined: example:
     *    <pre>   ubox( Font::bold + Font::italic + "abcd" )  </pre>
     */
    
    Font();
    ///< create a new Font; @see also shortcut ufont().
    
    Font(const Font&);
    ///< create a new Font; @see also shortcut ufont().
    
    Font(const Font& font, int style, float pixel_size);
    /**< create a new Font.
     * keeps the same characteristics as 'font' with a specific 'style' and 'point_size'
     * style' is an ORed combination of Font::FontStyle constants such as BOLD|ITALIC
     * or -BOLD|-ITALIC (negative values deactivate these styles)
     * @see also shortcut ufont().
     */
    
    Font(const FontFamily&, int style, float point_size);
    /**< create a new Font.
     * style' is an ORed combination of Font::FontStyle constants such as BOLD|ITALIC
     * or -BOLD|-ITALIC (negative values deactivate these styles)
     * @see also shortcut ufont().
     */
    
    virtual Font& operator=(const Font& f) {set(f); return *this;}
    ///< copies font.
    
    virtual void set(const Font&);
    ///< copies font.
    
    virtual bool operator==(const Font& f) const {return equals(f);}
    virtual bool operator!=(const Font& f) const {return !equals(f);}
    virtual bool equals(const Font&) const;
    ///< compares fonts.
    
    const FontFamily& getFamily() const {return *family;}
    ///< returns the font family.
    
    float getPixelSize() const {return fsize;}
    ///< returns the font size (in pixels).
    
    float getPointSize() const;
    ///< returns the font size (in points).
    
    int getStyles() const {return on_styles;}
    ///< returns the font styles.
    
    Font& setFamily(const FontFamily&);
    ///< changes the font family.
    
    Font& setFamily(const String& font_families);
    ///< changes the font family (takes the closest one in a comma separated list).
    
    Font& setSize(const String& size);
    ///< changes the font size.
    
    Font& setPixelSize(float pixel_size);
    ///< changes the font size (in pixels).
    
    Font& setPointSize(float point_size);
    ///< changes the font size (in points).
    
    Font& setStyles(int styles);
    ///< changes the font styles.
    
    Font& changeStyles(int styles, bool add);
    /**< changes the font style.
     * 'styles' is an ORed bitmask of Font::Style values
     * 'styles' can be negative, which means 'disable these styles'
     */
    
    bool isPlain() const;
    bool isBold() const;
    bool isItalic() const;
    bool isFilled() const;
    bool isUnderlined() const;
    bool isOverlined() const;
    bool isStrikethrough() const;
    
    Font& setBold(bool = true);
    Font& setItalic(bool = true);
    Font& setFilled(bool = true);
    Font& setUnderlined(bool = true);
    Font& setOverlined(bool = true);
    Font& setStrikethrough(bool = true);
    
    void merge(const Font&);
    /**< merge font characteristics.
     * sets NON default values and combines styles
     */
    
    virtual void update();
    ///< update parents' graphics.
    
    // - impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    virtual void realize(Display*);
    /**< [impl] allocates physical resources.
     * this function is automatically called when this font is used
     * for the first time. There no need to call it in client code except
     * for loading fonts in advance.
     * - note: the argument can be the Application (the Application Context)
     */
    
    virtual void putProp(UpdateContext*, Element&);
    
  private:
    friend class Display;
    friend class FontDescription;
    const FontFamily* family;
    //short size;
    float fsize;
    short on_styles, off_styles;  // activated and disactivated styles
    Font(const FontFamily*, int styles, float pixel_size, UConst);
  };
  
  inline Font& ufont(const Font& f) {return *new Font(f);}
  ///< shortcut for *new Font(font).
  
  
  /** A Font Family represents a collection of fonts for all possible sizes and styles.
   * Note that actual fonts are only loaded when necessary
   */
  class FontFamily : public UObject {
  public:
    static FontFamily sans_serif, serif, monospace;
    ///< predefined font families that can be used in Font(s).
    
    static FontFamily defaults;
    /**< specifies default characteristics used when FontFamilies can't be found
     * should be changed before creating the Application
     */
    
    FontFamily(const String& logical_name);
    ///< create a new font family.
    
    FontFamily& operator=(const FontFamily&);  
    
    const String& getName() const {return name;}
    
    void setFamily(const char* native_families);
    ///< changes font characteristics for this family (args are coma separated lists).
    
    void setStyles(const char* normal_styles, const char* italic_styles);
    ///< changes font characteristics for this family.
    
    void setWeights(const char* normal_weights, const char* bold_weights);
    ///< changes font characteristics for this family.
    
    void setCompression(const char* compressions);
    
    void setEncoding(const char* encodings);
    
    void setPreferredSizes(const char* sizes);
    
    void setFreeType(const char* regular_files, 
                     const char* bold_files,
                     const char* italic_files, 
                     const char* bold_italic_files);
    /**< specifies FreeType font files for this family.
     * each argument is a comma separated list of of font pathnames (the first 
     * available one is used).
     */
    
    // - impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  protected:
    friend class FontDescription;  
    friend class UHardFont;  
    friend class Display;  
    
    FontFamily(const char* logical_name, UConst);  // private constr
    FontFamily(const FontFamily&); // forbidden
    
    void initSizes() const;
    int sizeToIndex(float fsize) const;
    
    static int family_count;
    mutable int ffindex;
    mutable bool ready;
    String name;
    mutable std::vector<int> fsizes;
    struct Defs {
      String
      families, 
      normal_style,
      italic_style,
      normal_weight,
      bold_weight,
      compression,
      encoding,  
      preferred_sizes,
      fty_regular, fty_bold, fty_italic, fty_bold_italic;  // freetype
    } defs;
  };
  
}
#endif
