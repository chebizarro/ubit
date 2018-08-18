/*
 *  icon.h: Icon widget
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

#ifndef _uicon_hpp_
#define _uicon_hpp_
#include <ubit/udoc.hpp>

namespace ubit {
  
  /** Icon (for IconBox)
   */
  class Icon : public Item {
  public:
    UCLASS(Icon)
    
    Icon(const String& name = "", Args content = Args::none);
    ///< creates a new icon.
            
    virtual ~Icon();
    static Style* createStyle();

    virtual int loadImage(const String& image_path);
    ///< load and shows the icon image, returns file loading status.
    
    const String& getName() const {return *pname;}
    
    bool isDir() const {return is_dir;}
    void setDir(bool state) {is_dir = state;}
    
  protected:
    unique_ptr<Box> ima_box, text_box;
    unique_ptr<String> pname;
    bool is_dir;
  };
  

    /** Icon box
   */
  class IconBox : public DocumentBox {
  public:
    UCLASS(IconBox)
    
    IconBox(Args = Args::none);
    virtual ~IconBox();
    static Style* createStyle();

    virtual int readDir(const String& pathname, bool remote_dir = false);
    
    // inherited:
    // virtual Box& titlebar() {return *ptitle_bar;}
    // virtual Box& content()  {return *pcontent;}
    // virtual Scrollpane& scrollpane()  {return *pspane;}
    // virtual Scale& scale()  {return *pscale;}
    
    virtual String& pathname()  {return *ppathname;}
    virtual String& title()     {return *ptitle;}
    virtual ListBox& icons() {return *picons;}
    virtual Choice& choice();
    virtual const Choice& choice() const;
    
    virtual void addIcon(class Icon&);
    virtual void removeIcon(class Icon&, bool auto_delete=true);
    virtual void removeAllIcons(bool auto_delete = true);
    
    virtual class Icon* getSelectedIcon();
    virtual class Icon* getPreviousIcon();
    virtual class Icon* getNextIcon();
    virtual class Icon* getIcon(int) const;
    
    virtual void selectIcon(Icon&);
    virtual void selectIcon(const String& name);
    virtual void selectPreviousIcon();
    virtual void selectNextIcon();
    
    // - - - impl.  - - - - - - - - - - - - - - - - - - - - - - - - - -
  protected:
    friend class Finder;
    unique_ptr<String> ppathname, ptitle;
    unique_ptr<ListBox> picons;
    unique_ptr<HSpacing> icon_hspacing;
    unique_ptr<VSpacing> icon_vspacing;
    unsigned long filetime;
    bool show_parent_dir;
    virtual void okBehavior(InputEvent&); 
  };
  
}
#endif


