/*
 *  style.h: Element and Box styles
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

#ifndef _ustyle_hpp_
#define	_ustyle_hpp_ 1

#include <ubit/udefs.hpp>
#include <ubit/uborder.hpp>
#include <ubit/ufont.hpp>
#include <ubit/ugeom.hpp>
#include <ubit/uon.hpp>

namespace ubit {
  
  struct ULocalProps {
    ULocalProps();
    
    SizeSpec size;                // object size
    PaddingSpec padding;          // space beween content and border (if any)
    const Border* border;         // object border
    const Background* background; // image or color background of the component
    float alpha;                   // for alpha blending
    // const Shape* shape;        // shape of the component
    // if this group is not null the objects it contains are added to children for 
    // display (e.g. for adding list-item or checkboxes markers)
    Element* content;    // NB: = null in most cases    
  };
  
  
  /** Compiled Object Style.
   * specify the appearance of Element/Box/Window objects and subclasses.
   */
  class UStyle {
  public:
    UStyle();
    virtual ~UStyle();
    virtual const UStyle& getStyle(UpdateContext*) const {return *this;}
    
    void setAlpha(float a) {local.alpha = a;}
    //void setBackground(const Background*);
    void setBorder(const Border* b) {local.border = b;}
    void setCursor(const UCursor* c) {cursor = c;}
    void setFont(const Font* f) {font = f;}
    void setSize(Length width, Length height);
    void setPadding(Length horiz, Length vert);
    void setHorizPadding(Length left, Length right);
    void setVertPadding(Length top, Length bottom);

    const Color* getColor(const Element&) const;
    const Color* getBgcolor(const Element&) const;
    void setColors(Color& all_colors);
    void setColors(Color& unselected_colors, Color& selected_colors);
    void setBgcolors(Color& all_colors);
    void setBgcolors(Color& unselected_colors, Color& selected_colors);
    void setColor(int action, Color&);
    void setBgcolor(int action, Color&);
    
    // champs locaux a l'object, pas heritables
    ULocalProps local;
    
    // champs heritables
    char orient;
    char  valign, halign;      
    short vspacing, hspacing;
    const ViewStyle* viewStyle;
    //TextEdit* edit;
    const Font* font;
    const UCursor* cursor;
    const String* textSeparator;  // text separator between children when printing
    Color* colors[2 * UOn::ACTION_COUNT];
    Color* bgcolors[2 * UOn::ACTION_COUNT];
    //const Color *color, *bgcolor;
    //class AttributeList* attrs;
  };
  
}
#endif
