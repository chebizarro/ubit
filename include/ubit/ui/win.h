/*
 *  win.h: ubit windows
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

#ifndef _uwin_hpp_
#define	_uwin_hpp_ 1

#include <ubit/ubox.hpp>
#include <ubit/uview.hpp>

namespace ubit {
  
  class UWinPlacement;
  class UWinImpl;
  
  /** Base class for windows and menus.
   * Window is the base class for all windows (UFrame, UDialog...) and menus
   * (UMenu, UPopmenu, UCtlmenu...)
   *
   * A Window can be added as a child of any widget. If this widget is "armable", such as
   * a button, this window is automatically opened when the widget is clicked.
   * A Window can also be added as a child of the Application or another Display if the application
   * manges multiple displays. As other UNodes, windows can have several parents. 
   *
   * CLOSING BEHAVIOR:
   * - by default, windows are hidden when they are closed (@see close()), except for
   *   the "Main Frame" (@see UFrame) that quits the application.
   * - this behavior can be changed by adding a UOn::close callback (@see: UOn)
   *   to the window (e.g. to save useful data before quitting)
   *
   * LAYOUT: 
   * - the window size is computed the first time it appears on the screen (@see: show())
   * - once computed, the size DOES NOT CHANGE automatically when children are added to/
   *   removed from the window => children should be added BEFORE the first call to show()
   * - to recompute size programatically, call: adjustSize() or setSize()
   */
  class Window : public Box {
  public:
    UCLASS(Window)
    
    Window(Args nodelist = Args::none);
    ///< creates a new Window; @see class Window for details.
    
    virtual ~Window();
    
    virtual Window* toWin() {return this;}  // redefined
    virtual const Window* toWin() const {return this;}  // redefined
    
    static UStyle* createStyle();  // redefined
    virtual int getDisplayType() const {return wmodes.IS_HARDWIN ? HARDWIN : SOFTWIN;}
    
    // - - - position - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    virtual Window& setTitle(const String& title);
    ///< changes the title of the window.
    
    virtual String getTitle() const;
    ///< returns the title of the window.
    
    virtual void setPos(UMouseEvent&, float x = 0, float y = 0);
    /**< moves the upper left corner of the window to the event location.
     * (x, y) are added to the event location.
     */
    
    virtual void setPos(const Window& win, const Point&, Display* = null);
    ///< moves the upper left corner of the window relatively to this position in 'win'.
    
    virtual void setPos(const View& view, const Point&);
    ///< moves the upper left corner of the window relatively to this position in 'view'.
    
    virtual void setPos(const View& view, const UWinPlacement&);
    ///< moves the window according to this UWinPlacement and relatively to 'view' .
    
    virtual void setScreenPos(const Point&, Display* = null);
    ///< moves the upper left corner of the window to this position on the screen.
    
    virtual void centerOnScreen(Display* = null);
    // centers the window on the screen.
    
    virtual Point getPos(const Window& win, Display* = null) const;
    ///< returns the position of the upper left corner of the window relatively to 'win'.
    
    virtual Point getPos(const View& view) const;
    ///< returns the position of the upper left corner of the window relatively to 'view'.
    
    virtual Point getScreenPos(Display* = null) const;
    ///< returns the position of the upper left corner of the window on the screen.
    
    // - - - size and stacking order - - - - - - - - - - - - - - - - - - - - - - -
    
    virtual float getWidth(Display* = null) const;
    ///< returns the window width.
    
    virtual float getHeight(Display* = null) const;
    ///< returns the window height.
    
    virtual Dimension getSize(Display* = null) const;
    ///< returns the window size on this display.
    
    virtual void setSize(const Dimension&, Display* = null);
    ///< changes the window size on this display.
    
    virtual void adjustSize();
    /**< adjusts the size of the window according to its content.
     * By default, the size of a window does not change when children are added or
     * removed. This function adjusts the size of a window to the size of its children 
     */
    
    void pack() {adjustSize();}
    ///< synonym for adjustSize().
    
    virtual void setFullScreen(bool state, Display* = null);
    ///< set full screen mode on or off.
    
    virtual void toBack(Display* = null);
    ///< sends this window to the back on this display.
    
    virtual void toFront(Display* = null);
    ///< brings this window to the front on this display.
    
    
    virtual bool isShown() const;
    ///< is the window currently shown?
    
    virtual bool isShown(Display*) const;
    ///< is the window currently shown on this display?
    
    virtual bool isModal() const;
    ///< is this dialog modal?.
    
    virtual void setModal(bool = true) ;
    ///< sets this dialog in modal or non modal mode.  
    
    virtual void show(bool state = true) {Window::show(state, null);}
    /**< shows/hides the window.
     * windows are not shown by default; show() must be called to make them visible.
     * Besides, their size do not change after the first call to show(), except if
     * the setSize() or adjustSize() method is called.
     *
     * @see also: showModal() to open a modal window.
     */
    
    virtual void show(bool state, Display*);
    ///< shows/hides the window on this display; @see: show(bool state);
    
    virtual int showModal(Display* = null);
    /**< opens a window and locks the interaction with other widgets until this window is closed.
     * this function:
     * - returns when the window is unshown by calling: show(false) or close(status)
     * - returns a value that indicates which button button was clicked in the modal
     *   window. This value is 0 by default; @see close() for more details.
     */
    
    virtual void close(int status = 0);
    /**< closes the window.
     * this function is called when the close button of the window title is clicked.
     * It can also be fired by a button of a modal window to specify a status that
     * will be returned by showModal().
     *
     * Behavior of close():
     * - by default, close() hides the window, except if it is  the "Main Frame"
     *   (@see UFrame) in which case close() quits the application
     * - if the window has at least one UOn::close callback (@see: UOn), these callbacks
     *   are fired (the default behaviors are not performed in this case)
     *
     * close() and showModal():
     * - buttons of a modal window can call close(status) to specify the value returned
     *   by showModal() if this function was used. Typically, the "Cancel" button
     *   (or the close button of the window title) return 0, the "OK" button returns 1, etc.
     * - by default, showModal() returns 0 if the window was closed by show(false)
     * 
     * notes:
     * - Element::closeWin() can be used as a callback of any (indirect) child of
     *   a window to close this window with the appropriate status
     * - if close(status) is redefined in a subclass, this redefinition should call 
     *   Window::close(status) so that callbacks are correctly fired
     */
    
    void repaint() {update(Update::paint);}          // redefined
    void update() {update(Update::layoutAndPaint);}  // redefined
        
    virtual void update(const Update& update_options, Display* = null);
    /**< indicates that the layout and/or the paint of this window will be updated.
     * this will be done when the main loop becomes idle.
     * - the 1st arg. specify what must be updated and misc options (see Update)
     * - the 2nd arg. should be null (it will be used for differenciating window
     *   views on different displays in the future)
     *
     * REMARK: This function does NOT change the size of the window. adjustSize() or
     * setSize() must be called to do that
     *
     * @see also: repaint(), update(), show().
     */
    
    virtual void doUpdate(const Update&, Display* = null);  // redefined

    
    bool isHardwin() const {return wmodes.IS_HARDWIN;}
    /**< returns true if this is a "hard" window.
     * a hard window is native window that has its own graphics context.
     * @see also: isSoftwin(). 
     */
    
    bool isSoftwin() const {return !wmodes.IS_HARDWIN;}
    /**< returns true if this is a "soft" window.
     * a soft window simulates an window that is displayed inside a "hard" window
     * (which is a UFrame or a UDialog)
     * @see also: isHardwin(). 
     */
    
    virtual Window& setSoftwin(bool state = true);
    /**< sets the "soft window" mode if arg is true.
     * this mode can only be set *before* window initialization
     * (ie. before the window is added to a visible object)
     * see isSoftwin() for details.
     */
    
    Window& setSoftWin(bool state = true) {return setSoftwin(state);}
    ///< setSoftwin() synonym.
    
    Window& softwin(bool state = true) {return setSoftwin(state);}
    ///< setSoftwin() synonym.
    
    bool isMapped() const {return wmodes.IS_MAPPED;}
    void setMapped(bool s) {wmodes.IS_MAPPED = s;}
    
    // - - - Impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    virtual bool realize();
    /**< [impl] initializes the window and its children. 
     * this function called when the window is shown for the first time
     */
    
    View* getWinView(Display* = null) const;
    ///< [impl] returns the corresponding view.
    
    UHardwinImpl* getHardwin(Display* = null) const;
    /**< [impl] returns the related hard window.
     * returns the associated hard window if isHardwin() is true, and the hard window
     * that contains this soft window otherwise
     */
    
#ifndef NO_DOC
    UHardwinImpl* hardImpl() const;
    USoftwinImpl* softImpl() const;  
    virtual void highlight(bool state);
    void addWinView(View*);
    virtual void initView(View* parent_view);
    virtual View* initViewImpl(View* parent_view, Display*);
    virtual void deleteViewsInside(const std::vector<View*>& parent_views);
    
  protected:
    friend class Box;
    friend class Application;
    friend class UAppliImpl;
    friend class UHardwinImpl;
    friend class USoftwinImpl;
    
    struct Modes {
      unsigned IS_HARDWIN:1;    // has a UHardwinImpl implementation (USoftWin otherwise) 
      unsigned IS_MENU:1;
      unsigned IS_DIALOG:1;
      unsigned IS_FRAME:1;
      unsigned IS_MAINFRAME:1; // main frame window?
      unsigned IS_MODAL:1;      // this window is modal
      unsigned IS_AUTO_OPENED:1; // automatically opened if parent is ARMable
      unsigned IS_MAPPED:1;    // this window is currently mapped on the screen
    };
    Modes wmodes;
    std::vector<UWinImpl*> wimpl; // liste pour multiple displays
        
    virtual View* realizeSoftwin(Window* hardwin, View* hardwin_view,
                                  Display*, bool add_to_the_end);
    
    virtual bool realizeHardwin(char window_type);
    // internal realization function (window_type as defined in UNatWin).
    
    virtual void realizeChildren(View* winview);
    // called by realize() to create the views of the children
#endif
  };
  

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** Constraints for Window Placement.
   * Examples (R = reference object, T = this object)
   * - RR TT   hplace = uright(),  hoppositeBorder = true,  hdist = 9
   *   RR      vplace = utop(),    hoppositeBorder = false, vdist = 0
   *
   * - RR      hplace = uleft(),   hoppositeBorder  = false, hdist = 0
   *   RR      vplace = ubottom(), hoppositeBorder  = true,  vdist = 1
   *   TT
   *
   *  - TT      hplace = uleft(),   hoppositeBorder = true,  hdist = 0
   *   TT RR    vplace = ubottom(), hoppositeBorder = false, vdist = 0
   */
  class UWinPlacement {
  public:
    UWinPlacement();
    
    // Horizontal placement. 
    // -- Explicit if &uleft() or &uright()
    // -- Default placement if null or other Halign values.
    Halign* halign;
    
    // vertical placement. Explicit if &utop() or &ubottom()
    Valign* valign;
    
    // Relative location from reference object:
    //
    // if oppositeBorder is true:
    //    the specified border (eg. uleft()) of the reference object is 
    //    aligned with the OPPOSITE border of 'this' object (eg. right)
    //    --> this implies that 'this' object will be located OUTSIDE
    //    the ref. object (exemple: a cascaded submenu located on the
    //    right side of its parent menu)
    //
    // if oppositeBorder is false:
    //    same borders are used for both objects (eg. both left
    //    sides are aligned together)
    //    --> this implies that 'this' object will be located INSIDE
    //    the ref. object (in the same way as for usual UH/Valign rules)
    
    bool hoppositeBorder, voppositeBorder;
    
    // relative 'distance' between reference and 'this' object borders.
    // Usually 1 or 0. Can be < 0 as it is a relative 'distance'.
    int hdist, vdist;
  };
  
}
#endif



