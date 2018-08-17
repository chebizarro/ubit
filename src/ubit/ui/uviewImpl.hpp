/*
 *  viewImpl.hpp : internal implementation of View
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

#ifndef _uviewImpl_hpp_
#define	_uviewImpl_hpp_ 1

#include <ubit/core/event.h>
#include <ubit/uborder.hpp>
#include <ubit/uupdatecontext.hpp>

namespace ubit {
  
  class 3DCanvasView;
  class U3Dpos;
  
  // ==================================================== ===== =======
  // [Impl] Object behaviors.
  
  class UBehavior {
  public:
    enum InputType {MOUSE=0, BROWSE=1, KEY=2};
    
    UBehavior(InputType = MOUSE);
    
    InputType intype;
    bool SOURCE_IN_MENU;   ///< is the source (indirectly) included in a menu?.
    bool DONT_CLOSE_MENU;     ///< the source does not close menus.
    const UCursor* cursor;
    Element* browsing_group;
    Box* event_observer;
  };
  
  // ==================================================== ===== =======
  // [Impl] UViewProp: view property.
  
  class UViewProp {
  public:
    virtual ~UViewProp() {}
  };
  
  // used by motion/resize callbacks to check if pos/size has changed.
  struct UViewChangeCallbackProp : public UViewProp {
    Point pos;
    Dimension size;
  };
  
  // used by the keep size mode of USize
  struct UViewKeepSizeProp : public UViewProp {
    UViewKeepSizeProp() : width(-1), height(-1) {}
    float width, height;
  };
  
  // border padding of this view.
  struct UViewBorderProp : public UViewProp, public PaddingSpec {
    UViewBorderProp() : PaddingSpec(0,0) {}
  };
  
  // used by to force position.
  //struct UViewForcePosProp : public UViewProp {
  //  UViewForcePosProp() : x(0), y(0) {}
  //  float x, y;
  //};
  
  // ==================================================== ===== =======
  // [Impl] UViewContext: struct used to retrieve the view context.

  class UViewContext {
  public:
    UViewContext();
    ~UViewContext();    
    View::FindMode find_mode;
    bool is_clip_set;    // is clip valid ?
    Rectangle clip;
    UpdateContext* upd_context;  // set if get_upd_context is true
    View* layout_view;  // layout should take place from this view.
  };
  
  // ==================================================== ===== =======
  // [Impl] ViewFind: struct used to find event sources.
  
  class ViewFind {
  public:
    ViewFind(View* win_view, const Point pos_in_win,
              UBehavior::InputType intype, unsigned char catch_mask);
    void updateProps(View*, Element*, const UpdateContext& cur_ctx);
    
    View* ref_view;             // window or 3Dwidget view (if canvas_view != null)
    Point ref_pos;              // pos in ref_view (whatever it is)
    3DCanvasView* canvas_view;  // != null if the source is in a 3Dwidget
    U3Dpos* refpos_in_canvas;    // 3Dpos of the 3Dwidget in canvas_view (if it is != null)
    
    WindowUpdateContext win_ctx;
    UpdateContext found_ctx;
    unsigned char CATCH_MASK;
    Box* catched;
    Box* uncatchable;
    UBehavior bp;
  };
  
  // ==================================================== ===== =======
  // [Impl] UViewLayout: struct used to layout widget views.
  
  class UViewLayout {
  public:
    enum Strategy {BOXVIEW, GET_HINTS, IMPOSE_WIDTH, NESTED} strategy;
    Dimension dim;
    float spec_w, min_w, max_w;
    float spec_h, min_h, max_h;
    UViewLayout() : strategy(BOXVIEW) {}
  };
  
  class UViewLayoutImpl {
  public:
    UViewLayoutImpl(View*);
    void computeWidth(const UpdateContext& curp, const PaddingSpec&,
                      UViewLayout&, bool minmax_defined);
    void computeHeight(const UpdateContext& curp, const PaddingSpec&,
                       UViewLayout&, bool minmax_defined);
    
    View* view;
    int visibleElemCount;  // Box or Data (not Element, not a PosBox)
    float chwidth, pos_chwidth, chheight, pos_chheight;  // ex int
    unsigned char orient;
    bool mustLayoutAgain;
  };
  
  // ==================================================== ===== =======
  // [Impl] UViewUpdate: struct used to update widget views.
  
  class UViewUpdate {
  public:
    // !BEWARE: order is MEANINGFUL: all ITEM operations must be > ITEM_OPS
    enum Mode {
      PAINT_ALL,       // Paint all objects in clipping area
      PAINT_DAMAGED,   // Just paint damaged objects and their children,
      //MAIS AUSSI toutes les vues soeurs qui suivent (a cause des vues
      //evntuellement superposees, voir note dans UViewUpdateData ci-dessous)
      //PAINT_CHANGED, // Just paint objects whose size or pos has changed
      UPDATE_DATA,     // Just update data, do not paint
      FIND_DATA_POS,   // Locate an Data from its pos - do NOT paint it
      FIND_DATA_PTR    // Locate an Data from a link or a pointer
    };
    
    Mode mode;
    UDataContext* datactx;  // optional, used when searching UDatas
    // when applicable (mode PAINT_DAMAGED) : 0 before drawing any damaged,
    // > 0 No (depth) of the damaged view, < 0 after painting the first damaged
    int damaged_level;
    // true when one or several damaged views have been processed
    bool after_damaged;
    // number of views to repaint above damaged objects
    int above_damaged_count;
    
    UViewUpdate(Mode _mode) {
      mode = _mode;
      datactx = null;
      damaged_level = 0;
      after_damaged = false;
      above_damaged_count = 0;
    }
    
    void setMode(Mode _mode) {mode = _mode;}
    Mode getMode() {return mode;}    
  };
  
  
  class UViewUpdateImpl : public Rectangle {
  public:
    UViewUpdateImpl(View*, const Rectangle& r, UViewUpdate&);
    virtual ~UViewUpdateImpl();
    
    virtual void updateBackground(Graph&, UpdateContext&, 
                                  const Rectangle &r, const Rectangle& clip);
    virtual void setPadding(Graph&, const UpdateContext&, 
                            const Rectangle &r, bool add_frame_and_paddind);
    virtual void callPaintCallbacks(Element&, UpdateContext&);
    virtual void callMoveResizeCallbacks(Element&, UpdateContext&);
    
    View* view;
    PaddingSpec pad;
    Rectangle chr, chclip;
    float vflex_space, hflex_space;
    bool can_paint;
    unsigned char orient;
    UViewUpdate& upmode;
    class UTableView* tabview;
    class TextEdit* edit;  
  };
  
  // ==================================================== ===== =======
  
  class UMultiList {
  public:
    static const int MAXCOUNT = 2;
    
    // parses the child list of grp, finds the 1st visible element (Data or Element)
    // and adds it (and its followers) as a sublist
    // also checks HARDWIN and SOFTWIN_LIST
    UMultiList(UpdateContext&, Element&);
    
    ChildIter begin() {return sublists[current = 0].begin;}
    ChildIter end() {return sublists[current].end;}
    // att: end() doit etre celui de current car cette valeur est utilisee ds le code.
    
    void next(ChildIter& c);
    void addChildList(ChildIter begin, ChildIter end);
    
    //private:
    struct SubList {ChildIter begin, end;};
    SubList sublists[MAXCOUNT];
    int card, current;
    bool in_softwin_list;
  };
  
}
#endif

