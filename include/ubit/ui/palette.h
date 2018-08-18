/*
 *  palette.h: movable palette box (internal frame)
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

#ifndef _upalette_hpp_
#define	_upalette_hpp_ 1

#include <ubit/ui/box.h>

namespace ubit {
  
  /** movable palette box (internal frame).
   */
  class UPalette : public Box {
  public:
    UCLASS(UPalette)
    static Style* createStyle();

    UPalette(Args content_args = Args::none);

    Box& content()  {return *pcontent;}
    ///< returns the content (the working area of the palette).

    Element& title()    {return *ptitle;}
    ///< returns the title (located in the left hand part of the titlebar()).

    Element& controls() {return *pcontrols;}
    ///< returns the controls area (located in the right hand part of the titlebar()).
        
    Box& titleBar()  {return *ptitle_bar;}
    ///< returns the title bar (which includes a title() and a controls() area).
    
    Box& resizeBtn() {return *presize_btn;}
    ///<returns the resize button (not show by default, resizeBtn().show() to make it appear).

    Position&   pos()   {return *ppos;}
    Size&  size()  {return *psize;}
    Scale& contentScale() {return *pcontent_scale;}  
  
    void setPosModel(Position*);
    void setPosControlModel(PositionControl*);

  protected:
    unique_ptr<Position> ppos;
    unique_ptr<PositionControl> ppos_ctrl;
    unique_ptr<Size> psize;
    unique_ptr<SizeControl> psize_ctrl;
    unique_ptr<Scale> pcontent_scale;
    unique_ptr<Box> pcontent;
    unique_ptr<Element> ptitle;
    unique_ptr<Element> pcontrols;
    unique_ptr<Box> ptitle_bar;
    unique_ptr<Box> presize_btn;
  };
  
  inline UPalette& upalette(Args content = Args::none) {return *new UPalette(content);}
  ///< shortcut function that returns *new UPalette(content).

}
#endif
