/*
 *  scrollpane.hpp
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

#ifndef _uscrollpane_hpp_
#define	_uscrollpane_hpp_ 1

#include <ubit/ui/border.h>
#include <ubit/ui/view.h>

namespace ubit {
  
  /** A box with (optionnal) scrollbars that manages a viewport.
   * There are two ways to use a scroll pane:
   *
   * - a) to scroll a viewport: in this case the scrollpane must have one
   *   (and only one) child that derives from Box 
   *
   * - b) to superpose several layers: a scrollpane can have several Box children,
   *   in which case they are superimposed (but not scrolled, except for the last one).
   *   Transparent boxes can be superimposed in this way (see Alpha). CardBox can
   *   also be used for this purpose.
   *
   * Note: a scrollpane should only have children that derive from Box, Attribute, UCall
   * (it should not have Element or Data/String children)
   */ 
  class ScrollPane: public Box {
  public:
    UCLASS(ScrollPane)

    ScrollPane(Args = Args::none);
    ///< create a scroll pane with 2 scrollbars; see also shortcut functions uscrollpane().
    
    ScrollPane(int vert_scrollbar, int horiz_scrollbar, Args = Args::none);
    /**< create a scroll pane with 0, 1 or 2 scrollbars; see also shortcut functions uscrollpane().
     * - the vertical scrollbar is created if 'vert_scrollbar' is true 
     * - the horizontal scrollbar is created if 'horiz_scrollbar' is true 
     */
         
    virtual ~ScrollPane();
    
    static Style* createStyle();

    void setTracking(bool);
    ///< tracking mode updates the value while the scrollbar is being dragged (default is true).
    
    float getXScroll() const {return xscroll;}
    ///< returns the current X scroll value (float value in [0,100]).
    
    float getYScroll() const {return yscroll;}
    ///< returns the current Y scroll value (float value in [0,100]).
  
    ScrollPane& setXScroll(float _xscroll) {return setScroll(_xscroll, yscroll);}
    ///< changes the X scroll value (float value in [0,100]).
    
    ScrollPane& setYScroll(float _yscroll) {return setScroll(xscroll, _yscroll);}
    ///< changes the Y scroll value (float value in [0,100]).
    
    virtual ScrollPane& setScroll(float xscroll, float yscroll);
    ///< changes the X and Y scroll values (float values in [0,100]).

    virtual void makeVisible(Box& child);
    /**< scrolls the pane to make this child visible.
     * 'child' must be a direct or indirect child of the scrollpane
     */
      
    ScrollPane& showHScrollbar(bool state);
    ScrollPane& showVScrollbar(bool state);
    
    ScrollPane& showHScrollButtons(bool state);
    ScrollPane& showVScrollButtons(bool state);

    ScrollBar* getHScrollbar() {return hscrollbar;}
    ///< returns the horizontal scrollbar (if any; null otherwise)
    
    ScrollBar* getVScrollbar() {return vscrollbar;}
    ///< returns the vertical scrollbar (if any; null otherwise)
    
    Box* getScrolledBox();
    ///< returns the box that is scrolled by this pane.
    
    View* getScrolledView(View* pane_view);
    /**< returns the appropriate view of the box that is scrolled by this pane.
     * A Pane can have multiple parents and thus, can appear several times
     * on the GUI. This functions makes return the corresponding view
     * of the scrolled box.
     */
  
    virtual void setScrollImpl(float xscroll, float yscroll);    
  protected:
    friend class ScrollBar;
    ScrollBar *hscrollbar, *vscrollbar;
    float xscroll, yscroll;
    
    virtual void constructs(int vs_mode, int hs_mode, const Args&);
    virtual void unsetHScrollbar();
    virtual void unsetVScrollbar();
    virtual void resizeCB(UResizeEvent&);
    virtual void wheelCB(UWheelEvent&);
  };
  
  inline ScrollPane& uscrollpane(const Args& args = Args::none) 
  {return *new ScrollPane(args);}
  ///< shortcut function that returns *new ScrollPane(args).
  
  inline ScrollPane& uscrollpane(int vscrollbar, int hscrollbar, const Args& args = Args::none)
  {return *new ScrollPane(vscrollbar, hscrollbar, args);}
  ///< shortcut function that returns *new ScrollPane(vert_scrollbar, horiz_scrollbar, args).
  
  
    /** [Impl] UPane View.
   */
  class UPaneView: public View {
  public:
    static  ViewStyle style;  // renderer
    virtual ViewStyle* getViewStyle() {return &style;}
    
    UPaneView(Box*, View* parview, UHardwinImpl*);
    virtual ~UPaneView() {}
    static View* createView(Box*, View* parview, UHardwinImpl*);
    
    virtual UPaneView* toPaneView() {return this;}
    
    float getXScroll() {return xscroll;}
    float getYScroll() {return yscroll;}
    
    virtual void setXScroll(float scr) {xscroll = scr;}
    virtual void setYScroll(float scr) {yscroll = scr;}
    ///< note that using these functions is not a good idea if this pane is already controlled by scrollbars.
    
    void setPadding(const PaddingSpec& p) {padding = p;} // for View::initLayoutViewport
    
    friend class ScrollPane;
    PaddingSpec padding;
    float xscroll, yscroll;
  };
}
#endif

