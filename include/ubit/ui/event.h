/*
 *  event.hpp
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


#ifndef uevent_hpp
#define	uevent_hpp  1

#include <ubit/ui/box.h>
#include <ubit/core/geometry.h>
#include <ubit/draw/key.h>

namespace ubit {
  
  class UDataContext;
  
  /** Ubit Event class.
   * note that this class inherits from class Modifier that defines modifier masks
   * such as LeftButton, RightButton, etc. 
   */
  class Event {
  public:
    Event(const Condition& c, Object* source, Object* aux = null);
    
    virtual ~Event();
    
    const Condition& getCond() const {return *cond;}
    /**< returns the event condition that detected this event.
     * e.g.: returns 'UOn::change' if the expression was: UOn::change / ucall(...)
     */
    
    virtual Object* getSource() const {return source;}
    ///< returns the object that received this event.
    
    virtual Object* getAux() const {return aux;}
    ///< returns auxilliary data.
    
    /// dynamic cast: returns 'this' converted to the requested class or null.
    virtual InputEvent*   toInputEvent()  {return null;}
    virtual MouseEvent*   toMouseEvent()  {return null;}
    virtual KeyEvent*     toKeyEvent()    {return null;}
    virtual UWheelEvent*   toWheelEvent()  {return null;}
    virtual UViewEvent*    toViewEvent()   {return null;}
    virtual PaintEvent*   toPaintEvent()  {return null;}
    virtual UResizeEvent*  toResizeEvent() {return null;}  
    virtual TimerEvent*   toTimerEvent()  {return null;}
    virtual UWinEvent*     toWinEvent()    {return null;}
    virtual MessageEvent* toMessageEvent(){return null;}  
    virtual USysWMEvent*   toSysWMEvent()  {return null;}  
    virtual UserEvent*    toUserEvent()   {return null;}
    
    void setCond(const Condition&);
    
  protected:
    const Condition* cond;  // event condition
    Object* source;    // the object that produced this event
    Object* aux;       // auxilliary data
  };
  

  /** Base class for MouseEvent and KeyEvent
   * Note that this class inherits from class Modifier that defines modifier masks 
   * such as LeftButton, RightButton... 
   */
  class InputEvent : public Event, public Modifier {     
  public:
    InputEvent(const Condition&, View* source_view, EventFlow*, 
                unsigned long when, int state);

     virtual InputEvent* toInputEvent() {return this;}
    
    virtual Element* getSource() const;  
    ///< returns the object that received this event.
    
    View* getView() const {return source_view;}
    /**< returns the object's view that received this event.
     * if getSource() derives from Box, getView() returns the box's view where
     * this event occured. null is returned otherwise.
     */
    
    View* getViewOf(const Box& box) const;
    /**< returns the view of 'box' that (indirectly) received this event.
     * returns the view of 'box' that contains getView() if any (null otherwise)
     */
    
    unsigned long getWhen() const {return when;}
    ///< returns the time when this event was produced.
    
    virtual int getModifiers() const;
    /**< returns an ORed mask of mouse buttons and modifiers that are pressed.
     * Modifier masks are defined in class Modifier. They are inherited by Event,
     * and thus, InputEvent. Exemple:
     * <pre>
     * void callbackFunc(Event& e) {
     *    if (e.getModifiers() & (e.LeftButton | e.ControlDown))
     *        // done when the left mouse button AND the control key are pressed
     * }
     * </pre>
     * alternatively, Event::LeftButton or Modifier::LeftButton can be used 
     * instead of e.LeftButton
     */
    
    bool isShiftDown() const;
    bool isControlDown() const;
    bool isMetaDown() const;
    bool isAltDown() const;
    bool isAltGraphDown() const;
    
    Window* getWin() const;
    ///< returns the hard window that received this event.
    
    View* getWinView() const;
    ///< returns the view of the hard window that received this event.
    
    EventFlow* getFlow() const {return eflow;}
    /**< returns the Event Flow that received this event (for two-handed interaction or groupware).
     * a Ubit application can manage 1 or several Event Flows. This is useful 
     * for two-handed interaction or groupware (each user controlling his own 
     * pointer on the screen(s)). @see Application and Application::getFlow().
     */
    
    Display* getDisp() const;
    /**< returns the display that received this event.
     * A Ubit application can open windows on multiple displays.
     * The default display is the Application.
     * @see Application and Application::getDisp().
     */
    
    bool dontCloseMenu() const {return modes.DONT_CLOSE_MENU;}
    
  protected:
    friend class Element;
    friend class EventFlow;
    friend class View;
    friend class FlowView;
    friend class ViewFind;
    
    struct Modes {
      unsigned PROPAGATE:1;
      unsigned IS_MDRAG:1;
      unsigned IS_FIRST_MDRAG:1;
      unsigned IS_BROWSING:1;
      unsigned DONT_CLOSE_MENU:1;
      unsigned SOURCE_IN_MENU:1;
    } modes;
        
    int state;
    unsigned long when;
    View* source_view;    // the view that contains the mouse
    EventFlow* eflow;     // the event flow that produced this event
    Element* event_observer;
  };
  
  /** mouse events
   * @see UOn::mpress, UOn::mrelease, UOn::mdrag, UOn::mmove for explanations.
   */
  class MouseEvent : public InputEvent {
  public:
    MouseEvent(const Condition&, View* source, EventFlow*, unsigned long time, int state,
                const Point& pos, const Point& abs_pos, int btn);
    
    virtual MouseEvent* toMouseEvent() {return this;}
    
    float getX() const {return pos.x;}
    ///< returns the X coordinate of the mouse in getView().
    
    float getY() const {return pos.y;}
    ///< returns the Y coordinate of the mouse in getView().
    
    const Point& getPos() const {return pos;}
    ///< returns the coordinates of the mouse in getView().
    
    const Point& getScreenPos() const {return abs_pos;}
    ///< returns the screen coordinates of the mouse.
    
    Point getPosIn(const View& view) const;
    ///< returns the coordinates of this event in this 'view'.
    
    Point getPosIn(const Box& parent_box) const;
    /**< returns the coordinates of this event in parent_box.
     * returns point (0,0) if the mouse is not located inside 'parent_box'.
     */
    
    Point getPosAndViewIn(const Box& parent, View*& parent_view) const;
    /**< returns the coordinates of this event in 'parent' and the corresponding 'parent's view.
     * returns point (0,0) and 'parent_view' == null if the mouse is not located inside 'parent'.
     */
    
    int getClickCount() const {return click_count;}
    ///< returns the number of mouse clicks.
    
    int getButton() const {return button;}
    /**< returns the button that was just pressed or released.
     * returns one of: Event::LeftButton, MidButton, RightButton for UOn::mpress 
     * and UOn::mrelease events. Returns 0 for UOn::mmove or UOn::mdrag events.
     * Example:
     *
     * <pre>
     * class MyClass {
     * public:
     *    Myclass();  // contructor
     * private:
     *    void pressCB(MouseEvent&);
     * };
     *
     * MyClass::MyClass() {
     *    ....
     *    ubutton("Press Me" + ucall(this, &MyClass::pressCB))
     *    ....
     * }
     *
     * void MyClass::pressCB(MouseEvent& e) {
     *    if (e.getButton() == e.RightButton && e.isControlDown()) 
     *         // done when the left mouse button AND the control key are pressed
     * }
     * </pre>
     */
    
    int getButtons() const;
    /**< returns an ORed mask of mouse buttons *after* the event occured.
     * button mask are Event::LeftButton, MidButton, RightButton. They are defined
     * in class Modifier and inherited by Event.
     * see InputEvent::getModifiers() for examples.
     */
    
    void translatePos(float x, float y);
    ///< translates the mouse position.
    
    void propagate() {modes.PROPAGATE = true;}
    ///< propagates events in children: @see Element::catchEvents().
    
    void setFirstDrag(bool s)  {modes.IS_FIRST_MDRAG = s;}
    bool isFirstDrag() const   {return modes.IS_FIRST_MDRAG;}
    bool isBrowsing()  const   {return modes.IS_BROWSING;}
    
      
    String* getStr();
    ///< returns the String object that is under the mouse (@see getData(UDataContext&)).
    
    String* getStr(UDataContext&);
    ///< returns the String object that is under the mouse and related properties (@see getData(UDataContext&)).
    
    Data* getData();
    ///< returns the Data object that is under the mouse (@see getData(UDataContext&)).
    
    Data* getData(UDataContext&);
    /**< returns the Data object that is under the mouse and related properties.
     * - returns null if no Data object was under the mouse when this event was generated
     * - the UDataContext argument is filled with useful info on the retreived object.
     * - use getStr() if you are only interested in Data objects that are strings.
     * @see also: UDataContext
     */
    
  protected:
    friend class EventFlow;
    friend class Element;
    friend class View;
    friend class FlowView;
    int button, click_count;
    Point pos, abs_pos;
  };
  
  /** wheel events
   * @see UOn::wheel;
   */
  class UWheelEvent : public MouseEvent {
  public:
    enum {WHEEL_DELTA = 120};
    
    UWheelEvent(const Condition&, View* source, EventFlow*,
                unsigned long time, int state, 
                const Point& pos, const Point& abs_pos,
                int wheel_btn, int wheel_delta);
    
    virtual UWheelEvent* toWheelEvent() {return this;}
    
    bool isVertical() const {return (button & 1) == 0;}
    
    int getWheelDelta() const {return delta;}
    /**< returns the amount of wheel movement.
     * - a positive value indicates that the wheel was rotated forward,
     *    away from the user;
     * - a negative value indicates that the wheel was rotated backward, 
     *    toward the user.
     * - One wheel click is defined as WHEEL_DELTA, which is 120.
     */
    
  private:
    int delta;
  };
  
  /** keyboard events
   * @see UOn::kpress, UOn::krelease, UOn::ktype for explanations.
   */
  class KeyEvent : public InputEvent {
  public:
    KeyEvent(const Condition&, View* source, EventFlow*, 
              unsigned long time, int state, 
              int keycode, short keychar);
    
    virtual KeyEvent* toKeyEvent() {return this;}
    
    short getKeyChar() const {return keychar;}
    ///< returns the typed character (in ISO Latin).
    
    int getKeyCode() const {return keycode;}
    ///< returns the key code (@see Key constants).
    
    virtual void setKeyChar(short keychar);
    /**< changes the typed character (in ISO Latin).
     * Note: this can be useful to hide actual text when typing a password etc.
     */
    
    virtual void setKeyCode(int keycode);
    ///< changes the key code (@see Key constants).
    
  private:
    friend class Element;
    int keycode;
    short keychar;
  };
  
  /** view event: base class for PaintEvent and UResizeEvent.
   */
  class UViewEvent : public Event {
  public:
    UViewEvent(const Condition&, View* source_view);
    virtual UViewEvent* toViewEvent() {return this;}
    
    Box* getSource() const;
    ///< returns the widget that received this event.
    
    View* getView() const {return source_view;}
    ///< returns the view of getSource() that received this event.
    
    Display* getDisp() const;
    /**< returns the display that received this event.
     * A Ubit application can open windows on multiple displays.
     * The default display is the Application.
     * @see Application and Application::getDisp().
     */
    
  protected:
    View* source_view;
  };
  
  /** Paint event.
   * @see:
   * - condition UOn::paint that triggers callback functions when the view is
   *   repainted. These functions can have an optional PaintEvent argument
   * - Graph for drawing Ubit graphics in these callback functions
   * - Graph::GLPaint drawing OpenGL graphics in these callback functions
   */
  class PaintEvent : public UViewEvent {
  public:
    PaintEvent(const Condition&, View* source_view, const Rectangle* win_clip = null);
    
    virtual PaintEvent* toPaintEvent() {return this;}  

    bool isClipSet() {return is_clip_set;}
    ///< true if the clipping zone is vaild.
    
    void getClip(Rectangle& r) const;
    /**< returns the drawing zone relatively to the origin of getView().
     * returns (0, 0, getView()->width, getView()->height) if isClipSet() is false.
     * and and empty rectangle if getView() is null
     */
    
    const Rectangle getClip() const {Rectangle r; getClip(r); return r;}
    ///< shortcut for getClip(Rectangle& r).
    
    virtual bool setSourceAndProps(View*);
    void setContext(UpdateContext& c) {current_context = &c;}
    
  protected:
    friend class Box;
    friend class Display;
    friend class Graph;
    
    bool is_clip_set;   // is redraw_clip valid ?
    Rectangle  redraw_clip;       // in window coordinates!
    UpdateContext* current_context;
    View* layout_view;
    
    virtual void setProps(const class UViewContext&);
  };
  
  /** Resize event.
   * @see:
   * - condition UOn::resize that triggers callback functions with this argument
   */
  class UResizeEvent : public UViewEvent {
  public:
    UResizeEvent(const Condition&, View* source_view);
    virtual UResizeEvent* toResizeEvent() {return this;}  
  };
    
  
  class UWinEvent : public Event {
  public:
    UWinEvent(const Condition&, View* source_view, int type, int state);
    
    virtual UWinEvent* toWinEvent() {return this;}
  
    View* getView() const {return source_view;}    
    int getType()  const {return type;}
    int getState() const {return state;}
    
  private:
    View* source_view;
    int type, state;
  };
  
  /** Platform-dependent window manager events.
   */
  class USysWMEvent : public Event {
  public:
    USysWMEvent(const Condition&, View* source_view, void* system_event);
    
    virtual USysWMEvent* toSysWMEvent() {return this;}
    
    View* getView() const {return source_view;}
    void* getSysEvent() {return sys_event;}
    
  private:
    View* source_view;
    void* sys_event;
  };
  
  /** Ubit Message events.
   * @see Application::onMessage()
   */
  class MessageEvent : public Event {
  public:
    MessageEvent(const Condition&, MessagePort* port);
    
    virtual MessageEvent* toMessageEvent() {return this;}
    
    //virtual Object* getSource() const {return (Object*)port;}
    
    const String* getMessage() const;
    ///< returns the message string.
    
    MessagePort* getMessagePort() const {return (MessagePort*)source;}
    ///< returns the corresponding messagge port.
    
  private:
    friend class MessagePortMap;
    //MessagePort* port;
  };
  
  /** timer event.
   * @see Timer;
   */
  class TimerEvent : public Event {
  public:
    TimerEvent(const Condition&, Timer* timer, unsigned long when);
    
    virtual TimerEvent* toTimerEvent() {return this;}
    
    //virtual Object* getSource() const {return (Object*)timer;}
    Timer* getTimer() const {return (Timer*)source;}
    unsigned long getWhen() const {return when;}
    
  protected:
    //Timer* timer;
    unsigned long when;
  };
  
  /** User events.
   * this class is intented to be subclassed.
   */
  class UserEvent : public Event {
  public:
    UserEvent(const Condition&, Object* source, int type, void* data);
    
    virtual UserEvent* toUserEvent() {return this;}
    
    //virtual Object* getSource() const {return source;}
    int   getType() const {return type;}
    void* getData() const {return data;}
    
  private:
    //Object* source;
    int type;
    void* data;  
  };
  
  /** information on Data objects that are being retrieved 
   *  by Event::getData() or Event::getStr().
   */
  class UDataContext {
  public:
    UDataContext();
    ~UDataContext();
    
    Rectangle getBounds() const;
    ///< return the location and the size of the data object relatively to e.getView().
    
# ifndef NO_DOC
  //private:
    friend class Box;
    friend class View;
    friend class FlowView;
    friend class TextEdit;
    friend class Selection;
     
    Rectangle win_clip;
    // in the case of multine String objects, the region does not contain the entire
    // string but just the line that is beneath the mouse
    
    bool exact_match;
    // true if this data object is exactly located under the Mouse.
    // false if this data is the last data object before the Mouse position
    
    long strpos, strpos2;
    // strpos = position of the mouse in the string if data is an String.
    
    ChildIter it, it2;
    // points to the data object that is located under the mouse when calling 
    // MouseEvent::getData() or MouseEvent::getStr().
    
    Data* data;
    View* source_view;
    Point win_eventpos;

    void set(UpdateContext&, Data*, ChildIter data_iter, ChildIter end_iter, 
             const Rectangle&, int _strpos, bool exact_match);
    void merge(UpdateContext&, Data*, ChildIter data_iter, ChildIter end_iter, 
               const Rectangle&, bool exact_match);    
# endif
  };
  
}
#endif

