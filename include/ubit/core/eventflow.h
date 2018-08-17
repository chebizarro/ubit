/*
 *  eventflow.h: Event Flow
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

#ifndef _ueventflow_hpp_
#define	_ueventflow_hpp_  1

#include <ubit/uappli.hpp>
#include <ubit/core/event.h>
#include <ubit/ui/uviewImpl.hpp>

namespace ubit {
  
  class MenuManager;
  
  /** Event Flow.
   * a Ubit application can manage 1 or several Event Flows that are 
   * comptelety separated. This is useful for Two-handed interaction or 
   * groupware (each user controlling his own pointer on the screen(s)). 
   * See class Application and Application::getFlow().
   *
   * Note: the ID of the native Event Flow is 0 on all Displays.
   */
  class EventFlow {
  public:
    EventFlow(Display&, int channel);
    /**< creates a new Event Flow on this Display.
     * the ID of the native Event Flow is 0 on all Displays.
     */
    
    virtual ~EventFlow();
    
    int getID() const {return id;}
    /**< returns the global ID of this Event Flow.
     * this is a unique ID for the whole application.
     * returns 0 for the native Event Flow on the Default Display.
     * @see also: getChannel()
     */
    
    int getChannel() const {return channel;}
    /**< returns the channel of this Event Flow relatively to its Display.
     * this value is NOT unique for the whole application but depends on the display
     * that this event flow id connected to.
     * Remarks:
     * - returns 0 for the native Event Flow
     * - several Event Flows can have the same channel but on different displays 
     *   (for instance, each Display have an Event Flow on channel 0)
     * @see also: getID()
     */
    
    Display& getDisp() const {return disp;}
    ///< returns the display that controls this event flow.
    
    void setFocus(View*);
    /**< gives the input focus to this view.
     * no object gets the focus if argument is null. The focus is reset when the user 
     * clicks on a the view of widget that can handle input from the keyboard 
     */
    
    Selection* getSelection() {return &selection;}
    ///< returns the text selection of this event flow.
    
    MenuManager& getMenuManager() const {return menu_man;}
    
    UObject* getUserData() {return user_data;}
    ///< gets a handle to user data.  
    
    void setUserData(UObject* obj) {user_data = obj;}
    ///< sets a handle to user data.
    
    void closeAllMenus();
    ///< close all menus related to this eventflow.  
    
    void deleteNotify(View* deleted_view);
    void deleteNotify(Element* deleted_group);
    
    void redirectMousePress(MouseEvent&, View* winview);
    
    // - - - Impl.  - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    void mousePress(View* win_view, unsigned long time, int state,
                    const Point& win_pos, const Point& abs_pos, int btn);
    void mouseRelease(View* win_view, unsigned long time, int state,
                      const Point& win_pos, const Point& abs_pos, int btn);
    void mouseMotion(View* win_view, unsigned long time, int state,
                     const Point& win_pos, const Point& abs_pos);
    void wheelMotion(View* win_view, unsigned long time, int state,
                     const Point& win_pos, const Point& abs_pos, int type, int delta);
    void keyPress(View* win_view, unsigned long time, int state, int keycode, short keychar);
    void keyRelease(View* win_view, unsigned long time, int state, int keycode, short keychar);
    void winEnter(View* win_view, unsigned long time);
    void winLeave(View* win_view, unsigned long time);
    
    void setCursor(Event&, const Cursor*);
    void startAutoRepeat(Event&);
    void stopAutoRepeat(Event&);
    
  protected:
    void boxCross(View* box_view, unsigned long time, int state, const Cursor*, bool is_browsing);
    bool mustCloseMenus(View* source_view);
    void autoRepeatCB(TimerEvent&);
    void openTipCB(TimerEvent&);
    void openTipRequest(Event&);
    void closeTipRequest(Event&);
    Window* retrieveTelePointer(Display*);
    void showTelePointers(MouseEvent&, int mode = 0);  
    
  private:
    friend class Application;
    friend class Event;
    friend class Element;
    static int TIP_X_SHIFT;
    int id, channel;
    Display& disp;
    
    struct LastPressed {
      LastPressed() {reset();}
      void reset();
      void set(View* source_view, const Point& win_pos, const Point& screen_pos, const ViewFind&);
      View* view;
      Box* box;
      Point win_in_screen;        // pos of win in the screen (for drag and release events)
      3DCanvasView* canvas_view;  // != null if the source is in a 3Dwidget
      U3Dpos* refpos_in_canvas;    // 3Dpos of the 3Dwidget in canvas_view (if it is != null)
      UBehavior behavior;
    } lastPressed;
    
    View *lastEntered, *currentFocus;
    Element *lastArmed, *beingClicked, *dragSource, *dropTarget;  
    Point click_pos;
    int    click_count;
    unsigned long click_time;
    const class Cursor* lastCursor;    // cursor being currently shown
    uptr<Timer> auto_repeat_timer;     // timer fo auto_repeat actions
    uptr<Timer> tip_timer;             // timer for tool tips
    Window& tip_win;                      // window for tool tips
    MenuManager& menu_man;             // menubars & pulldown menus manager
    Selection& selection;              // text selection management
    std::vector<class Window*> tele_pointers;  // remote pointers
    UObject* user_data;
  };
  
}
#endif
