/*
 *  view.hpp : Box views
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

#ifndef _uview_hpp_
#define	_uview_hpp_ 1

#include <list>
#include <ubit/uelem.hpp>
#include <ubit/core/event.h>
#include <ubit/ugeom.hpp>

namespace ubit {
  
  class FlowView;
  class UTableView;
  class UPaneView;
  class ViewFind;
  class ViewLayout;
  class UViewUpdate;
  class UViewLayoutImpl;
  class UViewUpdateImpl;
  class UFlowLayoutImpl;
  class UFlowUpdateImpl;
  class UMultiList;
  class UViewProp;
  //struct UViewContext;
  
  /** Specifies the View Style of an Box.
   */
  class ViewStyle : public Attribute {
  public:
    UCLASSDEF("ViewStyle", ViewStyle, null)

    ViewStyle(View* (*)(Box*, View*, UHardwinImpl*), UConst);
    virtual ~ViewStyle() {destructs();}
    
    View* (*createView)(Box*, View* parview, UHardwinImpl*);
    ///< pointer to the corresponding View::createView static constructor.
    
    virtual void addingTo(Child&, Element& parent);
    virtual void removingFrom(Child&, Element& parent);
    ///< NOTE that this function require a specific destructor.
  };
  
  /** Box View.
   *  Box objects (and subclasses except Window) can have several views.
   *  these views can be retrieved by methods Box::getView(), Box::getViews(), etc.
   *  
   *  Note: Window objects (and subclasses) have only onse (shared) view that
   *  is called the "window view".
   */
  class View : public Rectangle {
  public:
    UCLASSDEF("View", View, null)
    
    static ViewStyle style;
    
    View(Box*, View* parview, UHardwinImpl*);
    virtual ~View();
    
    virtual FlowView*  toFlowView()  {return null;}
    virtual UTableView* toTableView() {return null;}
    virtual UPaneView*  toPaneView()  {return null;}
    
    bool isShown() const;
    ///< returns true if this view is currently shown.
    
    bool isRealized() const;
    ///< returns true if system resources have been allocated.
    
    float getWidth()  const {return width;}
    ///< returns the width of the view.
    
    float getHeight() const {return height;}
    ///< returns the height of the view.
    
    Dimension getSize() const;
    ///< returns the size of the view.
    
    void setSize(const Dimension&);
    /**< changes the size of the view (see details).
     * Note that most box views are layed out automatically. Calling setSize()
     * on such views won't have the expected effect because their size is 
     * controlled by the layout manager.
     */
    
    static Point convertPosTo(const View& to, const View& from, const Point& pos_in_from);
    /**< converts position in 'from' view to position in 'to' view.
     * both views must be on the same Display. returns false if the conversion failed.
     */
    
    float getX() const;
    ///< returns the x position of the view relatively to its parent origin. 
    
    float getY() const;
    ///< returns the y position of the view relatively to its parent origin. 
    
    Point getPos() const;
    ///< returns the position of the view relatively to its parent origin. 
    
    Point getPosIn(View& ref_view) const;
    ///< returns the position of the view relatively to the view given as an argument. 

    Point getScreenPos() const;
    ///< returns the position of the view relatively to the screen origin. 
    
    Point getGLPos() const;
    /**< returns the position of the view relatively to its hard window in GL coordinates.
     * y is relative to the BOTTOM left corner.
     * @see getHardwinPos() to obtain x,y in 'normal' coordinates
     */
    
    Point getHardwinPos() const;
    /**< returns the position of the view relatively to its hard window. 
     * y is relative to the TOP left corner
     * - @see getGLPos() to obtain x,y in OpenGL coordinates
     * - NB: coordinates are always relative to the topmost hard window (SubWindow
     *   hard windows are not taken into account)
     */
    
    float getScale() const {return scale;}
    ///< returns the scaling factor applied to this view.
    
    
    virtual bool caretPosToXY(long caret_pos, int& xcol, int& yline) const;
    /**< convert a caret position to XY coordinates.
     * returns false if the conversion did not succeeded.
     */
    
    virtual bool xyToCaretPos(int line, int col, long& caret_pos) const;
    /**< convert XY coordinates to a caret position.
     * returns false if the conversion did not succeeded.
     */
    
    Box* getBox() const {return box;}
    ///< returns the widget that controls this view.
    
    Box* getBoxParent() const;
    ///< returns the closest Box parent of getBox() (which is not necessarily a direct parent).
    
    View* getParentView() const {return parview;}
    ///< returns the parent view (if any, null for window views).
    
    Display* getDisp() const;
    ///< returns the Display where this view is displayed.
    
    Window* getWin() const;
    ///< returns the Window where this view is displayed.
    
    View* getWinView() const;
    ///< returns the view of the Window where this view is displayed.
    
    UHardwinImpl* getHardwin() const {return hardwin;}
    ///< returns the hard window that contains this view.
        
    bool isChildOf(const std::vector<View*>& parent_views);
    
    // - - - Impl - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  

    virtual bool containsWC(const Point& pos_in_win);
    ///< returns true if this point (in window coordinates) in contained in this view.
    
    virtual void updateLayout(const Dimension* force_size, bool upd_paint_data = true);
    ///< [Impl] updates layout coordinates.

    virtual void updatePaintData(const Rectangle* region = null);
    /**< [Impl] updates paint data (but does not paint).
     * certain coordinates (related to scrolling etc.) are updated while painting.
     * this function updates them without acutally painting
     */
    
    virtual void updatePaint(const Rectangle* region = null);
    /**< [Impl] paints this view or a part of this view.
     * note that 'region', must be in window coordinates if provided.
     */         

    virtual View* findSource(ViewFind&, Point& pos_in_src);
    ///< [Impl].
    
    virtual View* findSourceNext(ViewFind&, Point& pos_in_src);
    ///< [Impl].
    
    enum FindMode {FIND_CLIP, FIND_VIEW_CONTEXT, FIND_PARENT_CONTEXT};

    virtual bool findContext(UViewContext&, FindMode);
    ///< [Impl].
    
    virtual Data* findData(UDataContext&, const Point& pos,
                            const Data* searched_data, int strpos1, int strpos2);
    ///< [Impl].

#ifndef NO_DOC
    enum VMODES {
      // INITIALIZING is set at the 1st update, INITIALIZED at the 2nd update
      // (a combination of layout+update+layout is necessary for correct init)
      INITIALIZING = 1<<0,
      INITIALIZED  = 1<<1,
      DAMAGED      = 1<<2,      // this view has been damaged and must be repaint
      // this viewobject has been fully destructed (by all its destructors)
      // but the memory has not yet been freed (operator delete being redefined)
      DESTRUCTED   = 1<<3,
      FIXED_WIDTH  = 1<<4,      // this view has a fixed width.
      FIXED_HEIGHT = 1<<5,      // this view has a fixed height.
      REALIZED_CHILDREN = 1<<6, // the children of this view have been realized (for win views only)
      POS_HAS_CHANGED  = 1<<9,  // position has changed => geometry must be updated
      SIZE_HAS_CHANGED = 1<<10, // size has changed => geometry must be updated
      NO_DOUBLE_BUFFER = 1<< 11 
      // !BEWARE: no comma after last item!
    };
    
    typedef std::list<UViewProp*> UViewProps;
    
    static View* createView(Box*, View* parview, UHardwinImpl*);
    // createView() is a static constructor used by ViewStyle to make a new view.
    
    void operator delete(void*);
    // requests view deletion.
    
    int getVModes() const {return vmodes;}
    // returns the current V-Modes bitmask.
    
    bool hasVMode(int modes) const {return (vmodes & modes) != 0;}
    // true if ONE of these V-Modes is verified.
    
    void addVModes(long modes) {vmodes |= modes;}
    // add these modes to the V-Modes bitmask.
    
    void removeVModes(long modes) {vmodes &= ~modes;}
    // remove these modes from the V-Modes bitmask.
    
    virtual ViewStyle* getViewStyle() {return &style;}
    // returns the default view style.
    
    View* getNext() {return next;}
    // returns the next view in the list (if any, null otherwise).
    
    template <typename T> T* getProp(T*& p) { 
      p = null; 
      for (UViewProps::iterator i = props.begin(); i != props.end(); ++i)
      { if (dynamic_cast<T*>(*i)) {p = (T*)*i; return p;} }
      return null;
    }
    // returns the corresponding View Property; null if this property does not belong to the view.
    
    template <typename T> T& obtainProp(T*& p) {
      getProp(p); if (!p) {p = new T(); props.push_back(p);} return *p;
    }
    // returns or creates the corresponding View Property: creates and adds this property if it does not belong to the view.
    
    void incrHFlexCount() {++hflex_count;}
    void incrVFlexCount() {++vflex_count;}
    void setScale(float s) {scale = s;}
    
    virtual bool doLayout(UpdateContext&, ViewLayout&);
    virtual void doUpdate(UpdateContext&, Rectangle r, Rectangle clip, UViewUpdate&);

  protected:
    int vmodes;              // modes of this view
    float scale;
    float chwidth, chheight; // size occupied by children
    float edit_shift;        // for TextEdit
    unsigned short hflex_count, vflex_count; // number of horiz and vert flexible children
    View* parview;          // parent view
    Box* box;               // corresponding Box
    UHardwinImpl* hardwin;   // hard window 
    View* next;	           // next view
    UViewProps props;
    
    void setParentView(View* parent_view);
    void setNext(View* v) {next = v;}
            
    virtual void doLayout2(UViewLayoutImpl&, Element&, UpdateContext&, ViewLayout&);
    
    virtual void doUpdate2(UViewUpdateImpl&, Element&, UpdateContext&,
                           Rectangle& r, Rectangle& clip, UViewUpdate&);
    
    virtual bool updatePos(UViewUpdateImpl&, Element&, UpdateContext& curctx,
                           Rectangle& r, Rectangle& clip, UViewUpdate&);
    
    virtual void beginUpdate3d(UViewUpdateImpl&, Element&, UpdateContext& curctx);
    virtual void endUpdate3d(UViewUpdateImpl&, Element&, UpdateContext& curctx);
    
    virtual View* findInBox(Box*, const Point& winpos, const UpdateContext&, ViewFind&);
    virtual View* findInGroup(Element*, const Point& winpos, const UpdateContext&, ViewFind&);
    virtual View* findInChildren(Element*, const Point& winpos, const UpdateContext&, ViewFind&);
    
    virtual bool findDataV(UpdateContext&, ChildIter data_iter, ChildIter end_iter, 
                           const Rectangle&, UViewUpdate&);
    virtual bool findDataH(UpdateContext&, ChildIter data_iter, ChildIter end_iter, 
                           const Rectangle&, UViewUpdate&);
    virtual bool findDataPtr(UpdateContext&, ChildIter data_iter, ChildIter end_iter, 
                             const Rectangle&, UViewUpdate&);
    
    static void initLayoutH(UViewUpdateImpl& vd, const UpdateContext& curp, const Rectangle& r);
    static void initLayoutV(UViewUpdateImpl& vd, const UpdateContext& curp, const Rectangle& r);
    static void initLayoutViewport(UViewUpdateImpl& vd, const UpdateContext& curp, const Rectangle& r);
    static void layoutH(UViewUpdateImpl& vd, const UpdateContext& curp, ChildIter link,
                        const Dimension& dim, Element* chgrp, View* chview);
    static void layoutV(UViewUpdateImpl& vd, const UpdateContext& curp, ChildIter link,
                        const Dimension& dim, Element* chgrp, View* chview);
    static void layoutViewport(UViewUpdateImpl& vd, const UpdateContext& curp,
                               ChildIter link, const Dimension& dim, View* chview);
#endif
  private:
    friend class Box;
    friend class Window;
    friend class UHardwinImpl;
    friend class SubWindow;
    friend class Child;
    friend class UViewLayoutImpl;
    friend class UViewUpdateImpl;
    friend class UAppliImpl;
    friend class InputEvent;
    friend class EventFlow;
    friend class 3DCanvasView;
    View(const View&);
    View& operator=(const View&);  // assigment is forbidden
  };
  
  // ==================================================== ===== ======= //
  
  class FlowView: public View {
  public:
    UCLASSDEF("FlowView", FlowView, null)

    static ViewStyle style;  // renderer
    virtual ViewStyle* getViewStyle() {return &style;}
    
    FlowView(Box*, View* parview, UHardwinImpl*);
    virtual ~FlowView();
    
    static View* createView(Box*, View* parview, UHardwinImpl*);
    
    virtual FlowView* toFlowView() {return this;}
    ///< pseudo dynamic cast: returns this object if it a a FlowView.
    
    virtual bool caretPosToXY(long caret_pos, int& xcol, int& yline) const; // ex int
    virtual bool xyToCaretPos(int xcol, int yline, long& caret_pos) const;
    
    virtual bool doLayout(UpdateContext& parent_context, ViewLayout&);
    virtual void doUpdate(UpdateContext& parent_context, Rectangle r, Rectangle clip, UViewUpdate&);
    
    float getMaxWidth() const;
    
    // - - - Impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  private:
    friend class UFlowLayoutImpl;
    friend class UFlowUpdateImpl;
    class UFlowLine* lines;
    class UFlowCell* cells;
    int line_count, cell_count, lastline_strcell;
    int alloc_line_count, alloc_cell_count;
    
    //NB: pour des raiason historiquesflowview utilise des fonctions differentes
    // de sa superclasse View
    void flowDoLayout(UFlowLayoutImpl&, Element&, UpdateContext&, UMultiList&);
    void flowDoUpdate(UFlowUpdateImpl&, UpdateContext&, Element&, UMultiList&,
                      const Rectangle& r, Rectangle& clip, UViewUpdate&);
    
    //NB: NOT virtual: these functions are used instead of findDataV, findDataH, findDataPtr
    bool flowFindDataPos(UpdateContext&, ChildIter data_it, ChildIter end_it, 
                         UFlowCell*, const Rectangle&, UViewUpdate&);
    
    bool flowFindDataPtr(UpdateContext&, ChildIter data_it, ChildIter end_it,
                         UFlowCell*, const Rectangle&, UViewUpdate&);
  };
  
  
}
#endif
