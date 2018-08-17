/**
 *
 *  udisp.cpp: Graphical Display (may be remotely located)
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


#ifndef UBIT_DRAW_DISPLAY_H_
#define	UBIT_DRAW_DISPLAY_H_

#include <list>
#include <ubit/udefs.hpp>
#include <ubit/ustr.hpp>
#include <ubit/uelem.hpp>
#include <ubit/ugeom.hpp>

namespace ubit {
  
class UGraphImpl;
class UCursorImpl;
  
/** Display Context.
  * A Application can open windows on several displays, which can be on remote machines
  * when X11 is used as a windowing systems. A Display contains the data related to 
  * a given display (or to a specific screen of a given display when screens are
  * logically separated, ie. when a window cannot move freely from one screen
  * to another one).
  *
  * Window objects (and UFrame, UDialog, UMenu subclasses) can be attached to a
  * specific Display by using Display::add(). Widgets that are contained in several 
  * Window objects ("contained" meaning they are a child of them) are AUTOMATICCALY
  * REPLICATED and synchronized on all these windows.
  *
  * Application derives from Display. The Application object (which must be unique) represents
  * the default display.
  */
class Display {
public:
  static Display* create(const String& display_name);
  /**< creates and opens a new Display.
  * 'display_name' is the name of the X Window server, it must be specified as follows:
  * - hostname:screen_no
  * or just:
  * - hostname  (screen 0 is taken in this case)
  *
  * NOTES:
  * - the Application is the default Display, its ID number is 0.
  * - the default BPP (number of bits per pixel) is 24, this can be changed globally
  *   for the whole application in UConf or for a given display by calling Display(UConf&)
  */
    
  virtual ~Display();

  bool isOpened() const {return is_opened;}
  ///< returns true if the Display was sucessufully opened.
  
  int getID() const {return id;}
  /**< returns the ID of this Display.
   * the ID of the default Display is 0 (the default Display is the Application).
   */
  
  const UConf& getConf() const {return conf;}
  ///< returns the configuration of this Display.

  int getBpp() const {return bpp;}
  /**< returns the number of bit per pixel of this Display.
   * when X11 is used, this bpp may differ from the default BPP of the X11 display.
   */
  
  const String& getDisplayName() const {return display_name;}
  ///< returns the name of the Display's X11 server (when X11 is used).
    
  int getScreenWidth() const {return screen_width;}
  ///< returns the width of the Display's screen.
  
  int getScreenHeight() const {return screen_height;}
  ///< returns the height of the Display's screen.
  
  void setPixelPerInch(double pixel_per_inch);
  ///< changes the pixel density (number of pixels per inch).
  
  void setPixelPerMM(double pixel_per_mm);
  ///< changes the pixel density (number of pixels per mm).
       
  // - - - windows - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  virtual void add(Window&);
  /**< add a window (UFrame, UDialog, etc) to this display.
   * Note that windows are initially hidden, their show() method must be called
   * to make them visible. Moreover, as show() calculates the window size, it should
   * called after adding the window children (see also: Window::adjustSize()).
   * @see: Application::add() and classes Window, UFrame, UDialog, UMenu.
   */ 
  
  void add(Window*);
  ///< see add(Window&).
    
  virtual void remove(Window&, bool auto_delete);
  /**< remove a window from this display.
   * for explanations on 'auto_delete' see: Element::remove()
   */ 
    
  void remove(Window*, bool auto_delete);
  ///< see remove(Window&, bool auto_delete).
  
  // - - - pointer - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  virtual void setPointerPos(const Point& screen_pos) = 0;
  ///< move the pointer to this position on the screen.

  virtual Point getPointerPos() const = 0;
  ///< returns the position of the pointer on the screen.

  virtual int getPointerState() const = 0;
  ///< returns the state of the modifier keys and pointer buttons.

  virtual bool grabPointer(const UCursor* = null) = 0;
  /**< starts a physical grab of the native pointer.
    * the UCursor arg is the cursor that is shown during the grab. A default
    * value (such as UCursor::crosshair) is used if this argument is null.
    * returns true if the pointer could be grabbed.
    * BEWARE that grabPointer() may lock the X server! @see ungrabPointer().
    */
  
  virtual void ungrabPointer() = 0;
  /**< ends a physical grab of the native pointer.
    * @see grabPointer() for details.
    */

  virtual bool pickWindow(int& x_in_win, int& y_in_win, UHardwinImpl* window,
                          UCursor* cursor = null, UCall* callback = null) = 0;
  /**< grabs the pointer and lets the user pick a window interactively with the mouse.
    * (note: this function only works when X11 is used).
    * - returns false if no window was picked
    * - 'x_in_win' and 'y_in_win' are the position of the pointer in the window
    * - 'window' must be previously created by createWinImpl(null). Its content is set
    *   so that it points to teh picked window
    * - 'cursor' specifies the cursor that is shown during the grab operation 
    *   (UCursor::crosshair is used is this arg. is null)
    * - if 'callback' is not null, it is fired when the mouse is pressed, moved, 
    *   released during the grab operation. 'callback' is a callback object created
    *   by one of the ucall<> templates (@see UCall).
    */
    
  // - - - event flow - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  EventFlow* getChannelFlow(int channel) const;
  /**< returns the Event Flow for this channel on this display (null if not found).
    * 0 is the channel of the native Event Flow on this display. Don't confuse
    * channels with event flow IDs: channels depend on displays, while IDs are
    * unique for the whole Application. For instance, if an appli is displayed on 2
    * displays, it will have (at least) 2 event flows, 1 on disp1 and 1 on disp2.
    * The channel of both event flow will be 0, but their IDs will differ.
    * @see: Application class and Application::getFlow()
    */
  
  EventFlow* obtainChannelFlow(int channel);
  /**< gets or creates the Event Flow that corresponds to this channel on this display.
    * calls getDispFlow() then creates a new flow if getDispFlow() returns null.
    * @see getDispFlow() for details.
    */
  
  Selection* getChannelSelection(int channel);
  /**< returns the text selection of a given event flow on this display.
    * @see getDispFlow() for details.
    */
  
  // - - - cut & paste - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 
  const String& getCopyBuffer() const {return copy_buffer;}
  ///< returns the copy buffer.

  String* getPasteTarget(int& pos) const;
  ///< [impl] returns the string (and the pos in this string) where the paste will take place.
    
  void clearPasteTarget();
  ///< [impl] clears the paste target.

  virtual void copySelection(UMouseEvent&, Selection&);
  /**< [impl] copies the content of the selection in the copy buffer and tells the X server we own the X selection.
  * note: there is 1 selection per EventFlow but only 1 copy_buffer per Display
  */
  
  virtual void pasteSelection(UMouseEvent&, String* paste_str, int paste_pos);
  /**< [impl] pastes the X selection into this String at this pos.
    * ATTENTION: c'est lie a *chaque* disp et c'est ASYNCHRONE: le paste
    * n'est pas fait immediatement mais par appel ulterieur d'un callback
    */
      
public:
  RenderContext* getDefaultContext();  
  void makeDefaultContextCurrentIfNeeded();
  
  void addHardwin(Window*);
  void removeHardwin(Window*);
  
  virtual unsigned long createColorPixel(const Rgba&) = 0;  // for 2D_GRAPHICS
  virtual UHardwinImpl* createWinImpl(Window*) = 0;
  virtual UCursorImpl*  createCursorImpl(int curtype) = 0;
  virtual void          deleteCursorImpl(UCursorImpl*) = 0;
  
  virtual UHardFont* getFont(const FontDescription*);
  /**< returns (possibily creating) the requested font (see NOTES).
   * IMPORTANT NOTES: 
   * - 1. fonts depends on the default Graphics Context (@see getDefaultGC()).
   *   Hence, when OpenGL is used, fonts can only be drawn by using this GC or by
   *   a GC that shares the displaylists with this GC (@see Graph::createGC)
   * - 2. this function may change the current GC: the default GC will then become
   *   the current GC (by calling getDefaultGC()->makeCurrent()). Programs that use
   *   multiple GC may need to change the current GC after calling getFont().
   */
  
  virtual UHardFont* realizeFont(const Font&);
  virtual UHardFont* realizeFont(const FontDescription&);  
  virtual void realizeFontFamily(const FontFamily&);
  
  // - - - WITHOUT OpenGL 
  
  unsigned long getBlackPixel() const {return black_pixel;}  
  unsigned long getWhitePixel() const {return white_pixel;}
  
  // attention: les masks varient suivant l'architecture (big endian ou little endian).
  unsigned long getRedMask() const {return red_mask;}  
  unsigned long getGreenMask() const {return green_mask;}  
  unsigned long getBlueMask() const {return blue_mask;}  
  int getRedShift()  const {return red_shift;}  
  int getGreenShift()const {return green_shift;}  
  int getBlueShift() const {return blue_shift;}  
  int getRedBits()   const {return red_bits;}  
  int getGreenBits() const {return green_bits;}  
  int getBlueBits()  const {return blue_bits;} 
  static void countBits(unsigned long mask, int& bits, int& shift);
       
  // - - - Event management 
  
  EventFlow* obtainFlow(unsigned int ev_state, int channel);
  ///< returns the corresponding Event Flow (creates it if does not already exist).
  
  virtual void onPaint(View* winview, float x, float y, float w, float h);
  virtual void onResize(View* winview, const Dimension& size);
  void onResizeCB(View* winview);

  virtual bool setSelectionOwner(UMouseEvent&) {return false;}
  ///< [X11 only] tells the X-server the appli owns the X selection.
  
  virtual void pasteSelectionRequest(UMouseEvent&) {}
  /**< [X11 only] asks the X selection.
    * cette fct ne recupere PAS tout de suite le contenu de la selection
    * mais entrainera un appel ulterieur de pasteSelectionCallback().
    * the Event must be a button event. 
    */
  
  virtual void copySelectionCB(void* system_event) {}
  /**< [X11 only] copie reellement la selection
    * (ou plus exactement envoie son contenu a l'appli qui la demande).
    * l'event doit etre un XSelectionRequestEvent
    */
  
  virtual void pasteSelectionCB(void* system_event) {}
  /**< [X11 only] recupere effectivement le contenu de la X selection.
    * l'appel de cette fct est provoque par pasteSelectionRequest()
    * the XEvent must be a XSelectionEvent event.
    */

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
private:
  friend class Application;
  friend class UAppliImpl;
  friend class Window;
  friend class UHardwinImpl;
  friend class View;
  friend class Graph;
  friend class Event;
  friend class FontFamily;
  friend class FontMetrics;
  friend class GLContext;
  friend class UGlcanvas;
  friend class Length;
  friend class Font;
  Display(const Display&);
  Display& operator=(const Display&); 
  
protected:
  Display(const String& display_name);

  virtual void startAppli() = 0;
  virtual void quitAppli() = 0;
  virtual void startLoop(bool main_loop) = 0;  // starts the mainloop or a subloop.
  virtual void quitLoop(bool main_loop) = 0;   // quits the mainloop or a subloop.

  // attention: l'ordre de destruction importe ici: winlist doit etre
  // detruit en premier ca ca entraine des destructions qui font reference
  // a flowlist => winlist doit etre declare en DERNIER
  int id;
  UConf& conf;             // configuration data (may be shared with the Application)
  String display_name;       // name of the X Display
  int bpp;                   // bit per pixel
  int depth_size, stencil_size;
  int screen_width, screen_height;
  int screen_width_mm, screen_height_mm;
  bool is_opened;
  RenderContext *default_context;
  const GLContext *current_glcontext;
  std::vector<UHardFont**> font_map;
  unsigned long app_motion_time, nat_motion_time;  // for lag control
  unsigned long black_pixel, white_pixel, red_mask, green_mask, blue_mask;
  int red_shift, green_shift, blue_shift, red_bits, green_bits, blue_bits;  
  double IN_TO_PX, CM_TO_PX, MM_TO_PX, PT_TO_PX, PC_TO_PX;
  
  typedef std::list<UHardwinImpl*> HardwinList;
  HardwinList hardwin_list;  // list of hardwins that are event managed

  // copySelection() copies the content of the selection in this buffer
  // note: there is 1 selection per EventFlow but only 1 copy_buffer per Display
  String copy_buffer;

  // pasteSelection() specifies that the selection will be pasted in this String
  // at this pos (uptr<> to avoid deletion)
  uptr<String> paste_str;
  int paste_pos;

  // attention: winlist doit etre detruit en premier!
  Element winlist;  // list of windows belonging to this display
    
  struct LastResize {
    LastResize() : count(0), winview(null) {}
    int count;
    View* winview;
    Dimension newsize;
  } last_resize;
  
};

}
#endif // UBIT_DRAW_DISPLAY_H_
