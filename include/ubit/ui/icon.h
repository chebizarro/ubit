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
  
  /** UIcon (for UIconbox)
   */
  class UIcon : public UItem {
  public:
    UCLASS(UIcon)
    
    UIcon(const String& name = "", Args content = Args::none);
    ///< creates a new icon.
            
    virtual ~UIcon();
    static UStyle* createStyle();

    virtual int loadImage(const String& image_path);
    ///< load and shows the icon image, returns file loading status.
    
    const String& getName() const {return *pname;}
    
    bool isDir() const {return is_dir;}
    void setDir(bool state) {is_dir = state;}
    
  protected:
    uptr<Box> ima_box, text_box;
    uptr<String> pname;
    bool is_dir;
  };
  

    /** Icon box
   */
  class UIconbox : public UDocbox {
  public:
    UCLASS(UIconbox)
    
    UIconbox(Args = Args::none);
    virtual ~UIconbox();
    static UStyle* createStyle();

    virtual int readDir(const String& pathname, bool remote_dir = false);
    
    // inherited:
    // virtual Box& titlebar() {return *ptitle_bar;}
    // virtual Box& content()  {return *pcontent;}
    // virtual UScrollpane& scrollpane()  {return *pspane;}
    // virtual UScale& scale()  {return *pscale;}
    
    virtual String& pathname()  {return *ppathname;}
    virtual String& title()     {return *ptitle;}
    virtual UListbox& icons() {return *picons;}
    virtual UChoice& choice();
    virtual const UChoice& choice() const;
    
    virtual void addIcon(class UIcon&);
    virtual void removeIcon(class UIcon&, bool auto_delete=true);
    virtual void removeAllIcons(bool auto_delete = true);
    
    virtual class UIcon* getSelectedIcon();
    virtual class UIcon* getPreviousIcon();
    virtual class UIcon* getNextIcon();
    virtual class UIcon* getIcon(int) const;
    
    virtual void selectIcon(UIcon&);
    virtual void selectIcon(const String& name);
    virtual void selectPreviousIcon();
    virtual void selectNextIcon();
    
    // - - - impl.  - - - - - - - - - - - - - - - - - - - - - - - - - -
  protected:
    friend class UFinder;
    uptr<String> ppathname, ptitle;
    uptr<UListbox> picons;
    uptr<UHspacing> icon_hspacing;
    uptr<UVspacing> icon_vspacing;
    unsigned long filetime;
    bool show_parent_dir;
    virtual void okBehavior(UInputEvent&); 
  };
  
}
#endif


