/*
 *  border.h: widget border
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


#ifndef _uborder_hpp_
#define	_uborder_hpp_ 1

#include <ubit/core/attribute.h>
#include <ubit/core/length.h>

namespace ubit {
  
  class UBorderContent;
  
  /** Box Border.
   */
  class Border : public Attribute {
  public:
    UCLASS(Border)
    
    enum {NONE, LINE, SHADOW, ETCHED};
    ///< predefined decorations.
    
    static Border none, empty, line, shadowIn, shadowOut, etchedIn, etchedOut;
    ///< predefined borders.
    

    Border(int decoration = NONE);
    
    Border(int decoration, const Color& color, const Color& bgcolor,
            float width = 1, float height = 1);
    /**< creates a new Border with custom properties.
     * arguments:
     * - 'decoration' is one of NONE, +/-SHADOW, +/-ETCHED, +/-RECT (a negative 
     *    value sets an INwards shadow, a positive value an OUTwards shadow)
     * - 'color' and 'bgcolor' are the colors of the shadows. These values
     *    are shared and should not be deleted (by calling the delete operator)
     */
    
    Border(const Border&);
    ///< creates a new Border that is a copy of another Border.
     
    virtual ~Border();
    
    virtual Border& operator=(const Border&);

    
    int getDecoration() const {return decoration;}
     Border& setDecoration(int decoration);
    ///< changes the decoration.
    
    const Color& getColor() const {return *pcolor;}
    Border& setColor(const Color&);
    ///< changes foreground color; beware that the color arg is NOT duplicated and can't be freed.
    
    const Color& getBgcolor() const {return *pbgcolor;}
    Border& setBgcolor(const Color&);
    ///< changes background color; beware that the color arg is NOT duplicated and can't be freed.
         
    const PaddingSpec& getPadding() const {return padding;}
    Border& setPadding(const PaddingSpec&);
    
    bool isRounded() const {return is_rounded;}
    ///< returns true if the border is rounded.
    
    bool isOverlaid() const {return is_overlaid;}
    ///< returns true if the border is overlaid.
    
    Border& setOverlaid(bool state);
    
    //float getThickness() const {return thickness;}
    //Border& setThickness(float);

    virtual void update();

    virtual void getSize(const UpdateContext&, PaddingSpec&) const;
    virtual void paint(Graph&, const UpdateContext&, const Rectangle& r) const;
    virtual void paintDecoration(Graph& g, const Rectangle& r, Element& obj, 
                                 const Color& fg, const Color& bg) const;
    
  protected:
    short decoration;
    bool is_overlaid, is_rounded;
    PaddingSpec padding;
    unique_ptr<const Color> pcolor, pbgcolor;
    
    Border(int decoration, const Color& color, const Color& bgcolor,
            float width, float height, UConst);    
    virtual void constructs(int decoration, const Color& _color, const Color& _bgcolor);
    virtual void putProp(UpdateContext*, Element&);
  };
  
  inline Border& uborder(Border& b) {return *new Border(b);}
  
  
  /** Rounded border.
   */
  class RoundBorder : public Border {
  public:
    UCLASS(RoundBorder)

    RoundBorder(int decoration = NONE);
    RoundBorder(int decoration, const Color& color, const Color& bgcolor,
                 float width, float height, float arc_w, float arc_h);

    float getArcWidth()  const {return arc_w;}
    float getArcHeight() const {return arc_h;}
    
    virtual void paintDecoration(Graph& g, const Rectangle& r, Element& obj, 
                                 const Color& fg, const Color& bg) const;
  protected:
    float arc_w, arc_h;
  };

  
  
  /** Border that can contain other objects, including elements.
   */
class UCompositeBorder : public Border {
  public:
    UCLASS(UCompositeBorder)

    UCompositeBorder();

    UCompositeBorder(const Args& children);
    ///< creates a border that contains widgets.
    
    virtual Element* getSubGroup() const {return pchildren;}
    ///< [Impl] returns the children.
    
    virtual void putProp(UpdateContext*, Element&);

protected:    
    unique_ptr<Element> pchildren;
  };
  
}
#endif
