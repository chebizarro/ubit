/*
 *  dialogs.h: dialog boxes
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

#ifndef UBIT_UI_DIALOG_H_
#define	UBIT_UI_DIALOG_H_
#include <ubit/uwin.hpp>

namespace ubit {

  /**
   * Dialog box.
   * 
   * Dialogs can't be iconified and are always located on the top of the
   * "Main Frame" (@see Frame). Also, Dialogs are hidden when the Main Frame
   * is iconified
   *
   * @see class Window for inherited methods and other important remarks.
   */
  class Dialog : public Window {
  public:
    UCLASS(Dialog)
    
    Dialog(Args nodelist = Args::none);
    ///< creates a new Dialog Window; @see also shortcut function udialog().
        
    virtual ~Dialog() {destructs();}
    
    virtual Dialog& setTitle(const String& title) {Window::setTitle(title); return *this;}
    ///< changes the title of the dialog box.

    virtual void show(bool state, Display*); // redefined
    ///< shows/hides the dialog box on this display; @see: show(bool state);
    
    virtual void show(bool state = true) {Dialog::show(state, null);} // redefined
    /**< shows/hides this dialog box.
     * dialogs are not shown by default; show() must be called to make them visible.
     * Besides, their size do not change after the first call to show(), except if
     * the setSize() or adjustSize() method is called.
     *
     * @see also: showModal() to open a modal dialog.
     */
    
    static void showAlertDialog(const String& message_string);
    ///< pops up an Alert dialog on the screen (see showMessageDialog()).
    
    static void showAlertDialog(Args message_nodes);
    ///< pops up an Alert dialog on the screen (see showMessageDialog()).
    
    static void showErrorDialog(const String& message_string);
    ///< pops up an Error dialog on the screen (see showMessageDialog()).
    
    static void showErrorDialog(Args message_nodes);
    ///< pops up an Error dialog on the screen (see showMessageDialog()).
    
    static void showWarningDialog(const String& message_string);
    ///< pops up a Warning dialog on the screen (see showMessageDialog()).
    
    static void showWarningDialog(Args message_nodes);
    ///< pops up a Warning dialog on the screen (see showMessageDialog()).
    
    static void showMessageDialog(const String& title, const String& message_string,
                                  Args icon = Args::none);
    /**< pops up a Message dialog on the screen.
     * opens a modal dialog that display this message.
     * - the 'message_string' argument is copied (and can thus be freed).
     * - no 'icon' is displayed by defaut. 'icon' is an arbitrary arglist of nodes
     *   (see Args) that must not be destroyed (they won't appear otherwise)
     */
    
    static void showMessageDialog(const String& title, Args message_nodes,
                                  Args icon = Args::none);
    /**< pops up a Message dialog on the screen.
     * opens a modal dialog that display this message.
     * - 'message_nodes' is an arbitrary arglist of nodes (see Args) that must 
     *   not be destroyed (they won't appear otherwise)
     * - no 'icon' is displayed by defaut. 'icon' is an arbitrary arglist of nodes
     *   (see Args) that must not be destroyed (they won't appear otherwise)
     */
    
    static Style* createStyle();  // redefined
    virtual bool realize(); // redefined

#ifndef NO_DOC
    virtual void addingTo(Child&, Element& parent);
    virtual void removingFrom(Child&, Element& parent);  
  private:
    UCall* open_call;
    static class OptionDialog* messdialog;
#endif
  };
  
  inline Dialog& udialog(Args arglist = Args::none) {return *new Dialog(arglist);}
  ///< shortcut function that returns *new Dialog(args).
  
    /** Frame: toplevel and main window.
   * The main window of the application should usually be a Frame. This window, 
   * called the "Main Frame", has a special role for dialog boxes (@see Dialog)
   *
   * In contrast with Dialogs, Frames can be iconified separately. Most applications
   * have only 1 frame (the "Main Frame"). However, applications that require several 
   * iconfiable windows can create as many Frames as needed (the "Main Frame" is then
   * the first Frame that was added to the Application)
   *
   * @see class Window for inherited methods and other important remarks.
   */
  class Frame : public Dialog {
    friend class Application;
  public: 
    UCLASS(Frame)
    
    Frame(Args nodelist = Args::none);
    ///< creates a new Frame Window; @see also shortcut function uframe().
    
    bool isMainFrame() const {return wmodes.IS_MAINFRAME;}
    ///< is this frame the "main frame"? (@see: Application)
    
    virtual Frame& setTitle(const String& title) {Window::setTitle(title); return *this;}
    ///< changes the title of the dialog box.

    virtual void show(bool state, Display*);  // redefined
    virtual void show(bool state = true) {Frame::show(state, null);}  // redefined

    static Style* createStyle();  // redefined
    virtual bool realize();  // redefined
  };
  
  inline Frame& uframe(const Args& a = Args::none) {return *new Frame(a);}
  ///< shortcut that returns *new Frame(args).
  
  
    /** Predefined dialog box with optional content, icon and buttons area.
   * @see Dialog.
   */
  class OptionDialog : public Dialog {
  public:
    UCLASS(OptionDialog)

    OptionDialog(Args message_nodes = Args::none);
    OptionDialog(const String& title, Args message_nodes, Args icon, Args buttons);
       
    OptionDialog& setTitle(const String& title);
    OptionDialog& setMessage(const String& string);
    OptionDialog& setMessage(Args nodelist);
    OptionDialog& setIcon(Args icon);
    OptionDialog& setButtons(Args buttons);

  protected:
    unique_ptr<Box> picon, pmessage, pbuttons;
    void constructs(Args message);
  };
  
}
#endif // UBIT_UI_DIALOG_H_
