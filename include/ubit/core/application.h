/*
 *  application.h: Application Context
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

#ifndef UBIT_CORE_APPLICATION_H_
#define	UBIT_CORE_APPLICATION_H_

#include <cstdio>
#include <cstdarg>

#include <ubit/ui/display.h>
#include <ubit/core/config.h>

namespace ubit {
  
  /**
   * Flow list (@see EventFlow and Application).
   */
  typedef std::vector<EventFlow*> FlowList;
  
  
  /** Display lists (@see Display and Application)
   */
  typedef std::vector<Display*> DisplayList;
  
      
  /**
   * The Application Context. 
   * A Ubit application must create one (and only one) Application object.
   * This Application must be created as soon as the program is started by main()
   * No Ubit function should be called before its creation nor after its deletion.
   *
   * A Application can be parameterized:
   * - through command line options: see Application(argc, arv)
   * - by means of a Config object: see Application(Config&)
   *
   * A Application can open windows on several Displays (that can be controlled
   * by remote machines): see Application::openDisp()). Window children can be 
   * shared by windows that belong to different displays. These children are
   * automatically replicated (and synchronized) on the corresponding displays.
   *
   * A Application can manage 1 or several Event Flows that are comptelety separated.
   * This is useful for Two-handed interaction or groupware (each user
   * controlling his own pointer on the screen(s)). Alternate event flows are 
   * automatically created when the UMS (Ubit Mouse Server) is running 
   * (see the 'umsd' program in directory ubit/ums). The UMS can manage several
   * sources of events and send them as separate event flows to the Application.
   * These event sources can be produced by a mouse or a MIMIo on the serial port
   * or by an application running on a remote machine (see the 'uremote' program
   * in ubit/ums). Finally, Event flows can be uniquely identified in callback 
   * functions through the getFlow() method of their Event& argument.
   *
   * Classes Timer and Source make it possible to fire callback functions after
   * a given timeout or when the application receives data on a file source
   * (such as a socket). Application::onTimeout() and Application::postpone() provide a 
   * simplified API for using timers. postpone() is used to fire CPU intensive 
   * methods when the event loop becomes idle.
   * 
   * Classes Socket and ServerSocket provide a simple mechanism to handle 
   * sockets. The are based on simple communication protocol that can be
   * implemented in non Ubit programs. In addition, asynchronous messages
   * can be exchanged between Ubit applications and the UMS server through the
   * the Message protocol. See: Message, Application::onMessage() and 
   * Application::getMessagePort().
   *
   * NOTE on colors: Ubit applications may not work properly on (obsolete) X11 
   * displays that do not support True Colors. See Display::setTrueColors() 
   * and Display::setPseudoColors() for details.
   * 
   * See also the Display class for inherited methods.
   */
  class Application {
  public:
    static Config config;
    /**< configuration of the application, must be set BEFORE the Application is created.
     * 'conf' is an object that is created when the program is launched. Its fields:
     * - are initialized to default values by Config::Config()
     * - can be changed BEFORE the Application is created, typically in the main() function
     * - are modified when the Application instance is created, according to (argc, argv)
     * @see: Config, Application::Application().
     * @see also: Config::parseOptions(), Config::readConfFile().
     */
    
    Application(int& argc, char** argv,
           Option* command_line_options = null, 
           const char* configuration_file = null);
    /**< constructor for creating the Application Context.
     * Arguments:
     * - 'argc' and 'argv' : the parameters of the main() function
     * - 'command_line_options' : optional command line options: @see Config for details
     * - 'configuration_file' : an optional configuration file: @see Config for details
     *
     * Exceptions:
     * - Error exception thrown if the X server can't be opened.
     *
     * Command line options:
     * - see class Config for details.
     * Examples:
     * <pre>
     *    $ myappli --help-x
     *    $ myappli --scale 0.75 --display hendrix.tsne.fr:0
     *</pre>
     * the first line prints out the help message. The second line specifies
     * a rendering scale and a X server display name.
     *
     * See also: Application::openDisp() and the man of classes Application and Display.
     */
    
    virtual ~Application();
    
    static Application& application();
    /**< returns a reference to the Application instance of the program.
     * There is *only one* Application for a given program.
     * Throws a Error exception if the Application has not yet been created.
     */
    
    static Application* getApplication();
    /**< returns a pointer the Application instance of the program.
     * same as Application::application() except that no exception is thrown.
     */
    
    static Config& getConfig() {return conf;}
    ///< returns the configuration of the application, @see Config.
    
    static const char* getVersion();
    ///< returns the version of the Ubit toolkit (for instance "6.0.0").
    
    static bool isUsingGL() {return conf.is_using_gl;}
    ///< returns true if OpenGL is used for rendering graphics.
    
    static bool isRunning();
    ///< returns true if the main loop is running.
    
    static bool isExiting();
    ///< returns true if the applicating is exiting.
    
    
    static void add(Window&);
    /**< adds a window (Frame, Dialog, etc) to the Application.
     * Note that:
     * - the first Frame that is shown or added to the Application is said to be the
     *   "Main Frame" of the application. It will be returned by Application::getMainFrame()
     *
     * - windows are initially hidden, their show() method must be called to make 
     *   them visible. If a window has not been added to a display by Display::add(), 
     *   it is implicitely added to the the Application when its show() method is called.
     * 
     * - the size of windows is computed when their show() method is called for the
     *   first time. It won't change afterwards excepts if Window::adjustSize() is called.
     *
     * @see also: Window:show(), Window:update() and Window, Frame and Dialog classes.
     */ 
    
    static void add(Window*);
    ///< calls add(Window&) if the argument is not null, produces an error otherwise.
    
    static void remove(Window& win, bool auto_delete = true);
    /**< removes or deletes a window from the Application.
     * this function works as follows:
     * - 1) the first occurence of 'win' is removed from the child list
     * - 2) if 'auto_delete' is true, this 'win' is DELETED if it has no other
     *      parent in the scene graph AND if it is not pointed by a UPtr<>.
     * - 3) destruction is RECURSIVE: if this 'win' is deleted, its descendants
     *      that have no other parents and are nor UPtr'ed are also destroyed.
     */
    
    static void remove(Window*, bool auto_delete = true);
    ///< calls remove(Window&) if the argument is not null, produces an error otherwise.
    
    static int start();
    /**< starts the event main loop of the application.
     * returns the value given to Application::quit()
     */
    
    static void quit(int status = 0);
    /**< quits the main loop of the application.
     * Application::start() will return the 'status' value
     */
    
    static Frame* getMainFrame();
    /**< returns the Main Frame (if any, null otherwise).
     * the main frame is the first Frame that was added to the Application by add().
     */
    
    static const String& getName();
    ///< returns argv[0], the name of the application.
    
    static String getTitle();
    ///< returns the title of the Main Frame (@see getMainFrame()).
    
    static void setTitle(const String& title);
    ///< changes the title of the Main Frame (@see getMainFrame()).
    
    static void setFocus(View*);
    /**< gives the input focus to this view.
     * no object gets the focus if argument is null. The focus is reset when the user 
     * clicks on a the view of widget that can handle input from the keyboard 
     */
    
    static void updateAll(const Update& mode = Update::layoutAndPaint); 
    /**< updates all windows.
     * see Window::update() for details.
     */
    
    
    // - - - Errors and warnings - - - - - - - - - - - - - - - - - - - - - - - -
        
    static ErrorHandler& getErrorHandler();
    ///< returns the current error handler (see ErrorHandler).
    
    static void setErrorHandler(ErrorHandler&);
    ///< changes the current error handler (see ErrorHandler).
        
    
    static void error(const char* funcname, const char* format, ...);
    ///< raises an error; shortcut for raiseError(Error::ERROR ...).
    
    static void warning(const char* funcname, const char* format, ...);
    ///< raises a warning; shortcut for raiseError(Error::WARNING, ...).
    
    static void fatalError(const char* funcname, const char* format, ...);
    ///< raises a fatal error; shortcut for raiseError(Error::FATAL_ERROR, ...).
     
    
    static void raiseError(int errnum, const Object*, 
                           const char* funcname, const char* format, ...);
    /**< raises an error: prints out a message and/or generate an exception.
     * this method:
     * - displays an error message (on std::cerr by default, see below)
     * - throws a Error exception if 'errnum' is < 0.
     *   Predefined errnums are Error::WARNING, ERROR, FATAL_ERROR, INTERNAL_ERROR 
     *   but other values may be used if needed.
     * - 'funcname' should be the name of the function where the error occured
     * - 'format'is a printf-like format
     * - '...' is a variadic list of arguments that correspond to 'format'.
     * 
     * see also:
     * - shortcut functions: error(), warning(), fatalError()
     * - ErrorHandler for configuring error management.
     */
    
    static void raiseError(int errnum, const Object*, 
                           const char* funcname, const char* format, va_list);
    ///< raises an error; see raiseError().

    
    // - - - Time and Timeouts - - - - - - - - - - - - - - - - - - - - - - - - -
    
    static unsigned long getTime();
    ///< returns the current time.
    
    static void setMotionLag(unsigned long app_lag, unsigned long nat_lag);
    /**< changes the motion lag.
     * motion (drag, move) events are withdrawn if the time lag is hihgher 
     * than these values (default are 15 and 100 ms respectively). 
     */
    
    static void addTimeout(unsigned long msec_delay, int ntimes, UCall& callback);
    /**< fire this callback after a given delay.
     * Args:
     * - 'delay' is in milliseconds. if 'delay' = 0, the timer is
     *   immediately fired when the main loop becomes idle.
     * - 'ntimes' specifies how many times the timer is fired (each time
     *   after the specified 'delay'). The timer will be fired repeatedly
     *   until the completion of the application if ntimes is -1.
     * - 'callback' is a ucall<> expression (see UCall). It is destroyed after
     *    completion, except if referenced by a unique_ptr<> or another node (see Node).
     * See also: postpone() and class Timer.
     */
    
    static void postpone(UCall& callback);
    /**< fire this callback when the main loop becomes idle.
     * this function is useful:
     * - to postpone the execution of callbacks that take some time
     * - to request the execution of callbacks in the main thread from another thread
     * 'callback' is a ucall<> expression (see UCall). It is destroyed after
     *  completion, except if referenced by a unique_ptr<> or another node (see Node).
     * See also: onTimeout() and Timer.
     */
    
    
    // - - - Displays and Event Flows  - - - - - - - - - - - - - - - - - - - - -
    
    static Display* getDisp();
    ///< returns the default display of the appplication.
    
    static Display* getDisp(int ID);
    /**< returns the display with this ID.
     * Ubit applications can open windows on multiple displays when X11 is used.
     * this function returns the Display that has this ID, and numm otherwise.
     * The ID of the default display is 0.
     * @see Application::openDisp().
     */
    
    static const DisplayList& getDispList();
    //< returns the list of Displays that are connected with this Application.
    
    static Display* openDisp(const String& display_name);
    /**< opens a connection on another X display.
     * returns null if the connection failed. Otherwise, windows (Frame,
     * Dialog...) can be added to the returned Display by using its add()
     * method. 
     * - 'display' is the name of the X Window server. It follows the syntax:
     *    hostname[:display_no[:screen_no]] such as in: hendrix.tsne.fr:0.0
     * - window children can be shared by windows that belong to different
     *   displays. They are automatically replicated (and synchronized) on
     *   these displays.
     */
    
    static void closeDisp(Display*);
    ///< closes a connection with another X display.  
    
    static EventFlow* getFlow(int ID = 0);
    /**< returns the Event Flow corresponding to this ID (null if not found).
     * A Application can manage several separate Event Flows on one or multiple displays.
     * This is useful for two-handed interaction or groupware, see Application for details.
     *
     * Each event flow has a unique ID. 0 is the ID of the native event flow on 
     * the main display (ie. the display where the Application was launched).
     * @see also: Display::getDispFlow()
     */
    
    static const FlowList& getFlowList();
    ///< returns the list of all event flows on all displays (see getFlow()).
    
    static Selection* getSelection(int ID = 0);
    /**< returns the text selection manager.
     * return the text selection of the native event flow of the main display
     * by default (ie. if ID = 0). However, an application may have several
     * selections (it has as many selections as its number of event flows). The ID
     * number then specifies the requested event flow. @see: getFlow() for details
     */
    
    
    // - - - Inter-client Messages - - - - - - - - - - - - - - - - - - - - - - -
    
    static void onMessage(const String& port_name, UCall& callback);
    /**< adds a callback that is fired when data is received on this port.
     * Ubit messages makes it simple to exchange data between applications.
     * See class MessagePort.
     */
    
    static MessagePort& getMessagePort(const String& name);
    ///< Ubit Messages: @see onMessage() and class MessagePort.
    
    static MessagePort* findMessagePort(const String& name);
    ///< Ubit Messages: @see onMessage() and class MessagePort.
    
    static MessagePortMap* getMessagePortMap();
    ///< [impl] Ubit Messages.
    
    
    static const String& getImaPath();
    ///< see setImaPath().
    
    static void setImaPath(const String& pathname);
    /**< get/set the default image path.
     * the default image pathn is prefixed to image file names that do not
     * start with / or .
     */
    
    static StyleSheet& getStyleSheet();
    ///< returns the style sheet of the application.
    
    static const Style& getDefaultStyle();
    ///< returns the default element style.
    
    Menu* getOpenedMenu();
    ///< [impl] returns the menu that is currently opened (null otherwise).
    
    static bool hasTelePointers();
    ///< [impl] returns true if the appli has telepointers.
    
    
    String& initConf(int& argc, char** argv, Option*, const char* confile);
    ///< [impl] parses the config specs, inits the global data of the appli, returns the display name.
    
    static void internalError(const char* fun, const char* format, ...);

    static void deleteNotify(Display*);
    ///< [impl] notifies that a display is being destroyed.
    
    static void deleteNotify(Element*);
    ///< [impl] notifies that an element is being destroyed.

    static void deleteNotify(View*);
    ///< [impl] notifies that a view is being destroyed.

private:
    //static class AppImpl& impl;
    ///< [impl] reference to the object that stores the application data.

	const AppImpl* impl() const { return impl_.get(); }
    AppImpl* impl() { return impl_.get(); }
	
	std::unique_ptr<AppImpl> impl_;


  };
  
}
#endif // UBIT_CORE_APPLICATION_H_

