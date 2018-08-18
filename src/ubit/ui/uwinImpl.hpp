/*
 *  winImpl.h: ubit windows implementation
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

#ifndef _uwinImpl_hpp_
#define	_uwinImpl_hpp_ 1

#include <ubit/ui/window.h>

namespace ubit {
  
  /* [Impl] Window list.
   * Used for implementation purpose, UWinList elements are not counted has parents 
   * when auto deleting children. They are considered as genuine lists, not viewable
   * elements when displayingviews
   */
  class UWinList : public Element {
  public:
    UCLASS(UWinList)
    UWinList(const Args& a = Args::none) : Element(a) {}
    virtual int getDisplayType() const {return WINLIST;}   // redefined
  };
  
  
  
  class UWinImpl {
  public:
    /// window types.
    enum WinType {
      NONE,
      //NOTUSED,
      EXTWIN,
      PIXMAP,
      HARDWIN = 8,     // can't be instanciated
      SUBWIN,
      TOPLEVEL = 16,   // can't be instanciated
      FRAME,
      MAINFRAME, 
      DIALOG, 
      MENU
    };
    virtual ~UWinImpl() {};
  };
  

  /* [Impl] Soft (= simulated) Window.
   * abstract class: @see UHardwinX11, UHardwinGLUT, etc.
   */
  class USoftwinImpl : public UWinImpl {
  public:
    USoftwinImpl(Window&);
    virtual ~USoftwinImpl();
    
    UPos& pos() {return *ppos;}
    
    View* getActualView(View* winviews);
    void setActualView(View* winview);
    
    void doUpdate(const Update&, Window*, View* winview);
    
  private:
    unique_ptr<UPos> ppos;
    View* actual_view;  
  };
  

  /* [Impl] Hard (= real) Window.
   * abstract class: @see UHardwinX11, UHardwinGLUT, etc.
   */
  class UHardwinImpl : public UWinImpl {
  public:  
    virtual ~UHardwinImpl();
    
    Display* getDisp() const {return disp;}
    Window*  getWin()  const {return win;}
    WinType getWinType() const {return WinType(wintype);}  
    GLContext* getGlcontext() const {return glcontext;}
    
    bool isPixmap()   const {return wintype == PIXMAP;}  
    bool isSubwin()   const {return wintype == SUBWIN;}
    bool isHardwin()  const {return wintype > HARDWIN;}
    bool isTopLevel() const {return wintype > TOPLEVEL;}
    
    virtual void realize(WinType, float w, float h) = 0;
    virtual bool isRealized() const = 0;
    
    virtual void show(bool = true) = 0;
    virtual void toBack() = 0;
    virtual void toFront() = 0;
    
    virtual Point getScreenPos() const = 0;
    /**< returns window position relatively to the screen.
     * contrary to getPos(), this function returns the screen position for all windows
     * (even for subwindows embedded in another window)
     */
    
    virtual Point getPos() const = 0;  
    /**< returns window position relatively to its parent window (or the screen).
     * the screen position is returned for first level windows (ie. windows that
     * are not subwindows embedded in another window)
     */
    
    virtual void setPos(const Point&) = 0;
    /**< changes window position relatively to its parent window (or the screen).
     * changes position relatively to the screen for first level windows (ie. windows 
     * that are not subwindows embedded in another window)
     */
    
    virtual Dimension getSize() const = 0;
    ///< returns window size.
    
    virtual void setSize(const Dimension&) = 0;
    ///< changes window size.
    
    virtual String getTitle() const = 0;
    ///< returns window title.
    
    virtual void setTitle(const String&) = 0;
    ///< changes window title.
    
    virtual String getIconTitle() const = 0;
    ///< returns icon title.
    
    virtual void setIconTitle(const String&) = 0;  
    ///< changes icon title.
    
    virtual void setCursor(const Cursor*) = 0;
    ///< changes window cursor (can be null).
    
    virtual void setClassProperty(const String& instance_name, const String& class_name) = 0;
    ///< changes the WM_CLASS property when X11 is used.
    
  private:
    UHardwinImpl(const UHardwinImpl&);
    UHardwinImpl& operator=(const UHardwinImpl&);
    
  protected:
    UHardwinImpl(Display*, Window*);
    
  public:
    Children* getSoftwinList();
    UWinList*  getSoftwins();
    UWinList*  obtainSoftwins();
    void addSoftwin(Window* softwin, Window* hardwin, Display* disp, bool add_to_the_end);
    // add to the end add_to_the_end is true, to the beginning otherwise
    void removeSoftwin(Window* softwin);
    
    void doUpdate(const Update&, Window*, View* winview);
    void doUpdateImpl(const Update&, Window*, View* winview, const Dimension* size);
    
  protected:
    friend class Display;
    friend class Window;
    friend class View;
    friend class Graph;
    friend class PaintEvent;
    friend class InputEvent;
    friend class UAppliImpl;
    friend class GLCanvas;
    unsigned char wintype;       // one of UWinImpl::WinType
    unsigned char must_update;   // see UAppliImpl::processUpdateRequests
    Display* disp;
    Window* win;
    Children* softwin_list;
    // glcanvas windows and GLUT window have their own GLContext
    GLContext* glcontext;
  };  
   
}
#endif



