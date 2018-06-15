/*************************************************************************
 *
 *  umserver.cpp: UMS Server [Ubit Multiple-Mouse/Message Server]
 *  Provides multiple event flows, pseudo pointers, two-handed interaction,
 *  RendezVous identification, remote control facilities, etc.
 *
 *  NOTE: -lsocket -lnsl linking directives required on Solaris
 *
 *  Ubit Project
 *  Author: Eric Lecolinet
 *  Part of the Ubit Toolkit: A Brick Construction Game Model for Creating GUIs
 *  (C) 2003-2008 Eric Lecolinet / ENST Paris / http://www.enst.fr/~elc/ubit
 *
 * ***********************************************************************
 * COPYRIGHT NOTICE : 
 * THIS PROGRAM IS DISTRIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE 
 * IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. 
 * YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER THE TERMS OF THE GNU 
 * GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION; 
 * EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 * SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
 * ***********************************************************************/

#ifndef _umserver_hpp_
#define	_umserver_hpp_
#include <vector>
#include <list>
#include <iostream>
#include <string>
#include <ubit/udefs.hpp>
#include <ubit/usocket.hpp>
using namespace ubit;

#if (defined(__MACH__) && defined(__APPLE__))
#  define MACOSX 1
#endif

/* ==================================================== [(c)Elc] ======= */

typedef unsigned long WindowID;
typedef unsigned long AtomID;
typedef unsigned long CursorID;
typedef unsigned long TimeID;
typedef union  _XEvent   XEvent;
typedef struct _XDisplay XDisplay;
typedef struct XScreen;

enum {
  MouseLeftID   = (1<<8),
  MouseMiddleID = (1<<9),
  MouseRightID	= (1<<10),
  WheelUpID     = (1<<11),
  WheelDownID   = (1<<12),
  ALT_GRAPH_MASK = 0x2000
};

class EventSource;
class EventFlow;
class MouseFlow;
struct RemoteUMS;
typedef std::list<RemoteUMS*> RemoteUMSList;

/* ==================================================== ======== ======= */
/* Key mapping.
 */
struct ActionKey {
  ActionKey(unsigned int keycode, int modifier,
               void (*press_action)(class UMServer&, XEvent&),
               void (*release_action)(class UMServer&, XEvent&));

  unsigned int keycode;
  int modifier;
  void (*press_action)(class UMServer&, XEvent&);
  void (*release_action)(class UMServer&, XEvent&);
};

/* ==================================================== ======== ======= */
/** position on the screen
*/
struct Pos {
  Pos();
  Pos(int rx, int ry, int wx, int wy, WindowID win,
      bool in_ubitwin, int win_sock);

  int rx, ry;
  int wx, wy;
  WindowID win, subwin;
  // att ubit_win n'est mis a jour que par getWinPos()
  // cad quand on interagit avec un mouse flow non natif
  bool in_ubitwin;
  int win_sock;
};

/* ==================================================== ======== ======= */
/** window (on the same display).
*/
struct Win {
  enum State {HIDE=0, SHOW=1, CREATE=2, DESTROY=3};

  ~Win();
  Win(WindowID win, WindowID client_win);
  ///< att: aucune duplication des arguments!.
  
  bool is_prop_init, is_shown, is_override, is_input_only, is_icon;  
  int win_sock;
  WindowID win, client_win;
  unsigned char* ubit_prop;
  
private:
  Win(const Win&);
  Win& operator=(const Win&);
};

typedef std::list<Win*> WinList;

/* ==================================================== ======== ======= */
/** connection with a remote appli.
*/
struct Cnx {
  Cnx(USocket*);
  ~Cnx();

  class USocket* sock;
  bool browse_servers, browse_neigbors, browse_windows;
};

typedef std::list<Cnx*> CnxList;

/* ==================================================== [(c)Elc] ======= */
/** screen edges.
*/
struct Edges {
  static const int EDGE_THICKNESS = 1;
  Edges(UMServer*, int neighbor_mode);
  bool isEdge(WindowID);
  void enterEdge(WindowID, int x_root, int y_root);
  void raiseEdges();
  void moveNativePointerBackHome(bool center);
  
  UMServer& ums;
  bool is_outside;
  int x_crossings, y_crossings;
  int x_delta, y_delta;
  WindowID left_edge, top_edge, right_edge, bottom_edge, h_where_win, v_where_win;
};

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */
/** UMS server main class
*/
class UMServer {
public:
  /// attempt to re-connect (if connection failed) after this delay
  static const TimeID NEIGHBOR_RETRY_DELAY = 6*1000;  // 1 mn

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

  UMServer(const char* display_name, int ums_port, bool reuse_address,
           int cursor_mode, int neighbor_mode);
  /**< constructor.
   * Args:
   * - ums_port: the port used by the UMS; uses UMS_DEFAULT_PORT if 0
   * - display_name == NULL means "local host"
   */

  ~UMServer();

  bool isInit() const {return is_init;}
  /**< checks if the UMS is initialized.
   * NB: returns true if the UMS port and the X display could be opened.
   */

  void start();
  /**< starts the main loop of the UMS server.
   * NB: the UMS must be initialized (see: isInit())
   */

  void setPublicDir(const char* s);
  const std::string& getPublicDir() {return public_dir;}

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Emulation of mouse buttons 2 & 3, etc.

  void setNativeButton(int raw_btn_number, unsigned int raw_mod_mask,
		       unsigned int to_btn_mask, unsigned int ro_mod_mask);
  /* changes the events generated by the buttons of the native mouse.
   * for instance: setNativeButton(Button1, ControlMask, Button3Mask, 0);
   * will emulate Button3 when ControlMask+Button1 are pressed/released
   *
   * this is useful for machines that do not have 3 buttons such as the IPAQ.
   * see also: EventSource::SetButton() (to configurate other event sources)
   */

  void setActionKey(unsigned int keycode, int modifier, 
		    void (*press_action)(class UMServer&, XEvent&),
		    void (*release_action)(class UMServer&, XEvent&));
  /**< this key will perform some useful action when pressed and/or released.
   */

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  // Browsed UMSs and Neighbor UMSs

  RemoteUMSList::iterator findRemoteUMS(const char* name);
  RemoteUMS* addRemoteUMS(const char* name, const char* address,
                          int port, int pos);
  void removeRemoteUMS(const char* name);

  RemoteUMS* getNeighborUMS(int pos) const;
  RemoteUMS* setNeighborUMS(int pos, const char* name);

  void sendRemoteUMSState(RemoteUMS*, int state);
  ///< sends an added/removed state when a RemoteUMS is created or deleted.

  void sendRemoteUMSList(Cnx* to);
  ///< sends the list of RemoteUMSs that are currently browsed to a client.

  void sendNeighborList(Cnx* to);
  void sendNeighborState(RemoteUMS*, int pos);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  // Event Sources

  std::string resolveDeviceName(const char*);
  ///< returns the /dev/ttxx name from a shortcut.

  bool addEventSource(EventSource*);
  /**< adds an event source.
   * NB: - several event sources can be created
   * Return value:
   *     - true if this source could be added (ie. if this source
   *       was not previously registered)
   */

  bool removeEventSource(EventSource*);
  /**< removes an event source.
   * NB: - this fct does not delete the source.
   * Return value:
   *     - true if this source could be removed (= if this source
   *       was previously registered
   */

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  // Event Flows

  MouseFlow& getNativeMouseFlow() const;
  /**< returns the native mouse flow.
    * equivalent to getMouseFlow(0)
    */

  MouseFlow* getMouseFlow(int id) const;
  /**< returns this mouse flow if 'id' is defined and it is a mouse flow.
    * equivalent to dynamic_cast<MouseFlow*>(getEventFlow(id)
    * the id of the native mouse flow is 0.
    */

  MouseFlow* getOrCreateMouseFlow(int id);
  /**< returns this mouse flow or creates it if it does not exist
    */
  
  EventFlow* getEventFlow(int id) const;
  /**< returns this mouse flow if 'id' is defined and it is a mouse flow.
    * equivalent to dynamic_cast<MouseFlow*>(getEventFlow(id))
    * the id of the native mouse flow is 0.
    */
 
   bool addEventFlow(EventFlow*);
  /**< adds an event flow.
     * NB: - several event flows can be created
     *     - their IDs MUST be != 0 (and have different values)
     *     - the native event flow (that corresponds to the standard X mouse)
     *       is created implicitely by the UMServer. Its ID is always 0.
     * Return value:
     *     - true if this flow could be added (ie. ID is != 0 and not flow
     *       was registered with the same ID)
     */

  bool removeEventFlow(EventFlow*);
  /**< removes an event flow.
    * NB: - this fct does not delete the flow.
    *     - the native event flow (whose ID = 0) can't be removed.
    * Return value:
    *     - true if this flow could be removed (= ID is != 0 and this flow
    *       was previously registered
    */

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Connections

  CnxList::iterator findCnxIt(USocket*);
  Cnx* findCnx(USocket*);
  Cnx* addCnx(USocket*);
  void removeCnx(USocket*);
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  // X Display and windows

  XDisplay* getDisplay() const {return xdisplay;}
  XScreen*  getScreen()  const {return xscreen;}
  WindowID  getRoot()    const {return xrootwin;}
  int  getXConnection()  const {return xconnection;}
  long getScreenWidth()  const;
  long getScreenHeight() const;
  int  getScreenDepth()  const;

  // Calibration for the MIMIO
  struct Calibration* getCalibration();

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  // Current (X) Windows

  Win* addWin(WindowID, bool notify);
  void removeWin(WindowID, bool notify);
  void reorderWin(WinList::iterator& from, const WinList::iterator& to);

  void retrieveXWindows();
  void raiseXWindows();

  void sendWindowState(Win*, int state);
  void sendWindowList(Cnx* to);

  Win* findWin(const char* name);
  Win* findWin(WindowID);
  WinList::iterator findWinIt(WindowID);
  ///< searches a window from its name or its ID in the X window list.

  bool getXWin(int root_x, int root_y, Pos&, bool check_props);
  ///< returns the window and pos that contains this point.

  bool getXSubwin(const Pos& wp, int x, int y, Pos&);
  ///< trouver subwindow de wp.win contenant le point (x,y) relatif a wp.win.

  bool getXWinPos(const Win* w, Pos&);
  ///< trouver position de w dans root.
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Pointers
  
  bool isPointer(WindowID);
  ///< true if this window is a pseudo pointer.

  void moveNativePointerBackHome(bool center);
  ///< move the native pointer back to the home server.

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  // Implementation

  static TimeID getTime();     ///< in millisec.

  class UServerSocket* getServerSocket() {return serv_sock;}
  WinList& getWins() {return wins;}
  CnxList& getCnxs() {return cnxs;}
  
  void processRequest(USocket*, class UInbuf&);

  // private:
  AtomID _WM_DELETE_WINDOW;
  AtomID _UMS_WINDOW;
  AtomID _UMS_MESSAGE;

  bool initX(const char* display_name, int cursor_mode, int neighbor_mode);
  void getAndProcessXEvents();

  bool addSourceToMainLoop(USocket*);
  bool addSourceToMainLoop(EventSource*);
  void initMainLoop();
  void runMainLoop();

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

  bool is_init;
  class UServerSocket* serv_sock;
  int xconnection;
  XDisplay* xdisplay;
  XScreen*  xscreen;
  WindowID  xrootwin;
  CursorID root_cursor, grab_cursor;
  struct Events& events;
  struct Edges* edges;
  struct Calibration* calib;
  struct Mdns* mdns;
  std::string hostname, system, public_dir;
  EventSource& natsource;   ///< the native event source.

  /// list of event sources (= the devices that are connected to the eflows)
  std::vector<EventSource*> sources;

  /// list of event flows (eflows[0] corresponds to the standard X server)
  std::vector<EventFlow*> eflows;

  /// lists of connections (= connected applications) and their opened windows
  CnxList cnxs;

  /// list of the windows of this X server
  WinList wins;

  /// the list of RemoteUMS that are currently browsed.
  RemoteUMSList remotes;

  /// neigbhors of this UMS server.
  RemoteUMS *l_neighbor, *r_neighbor, *t_neighbor, *b_neighbor;

  /// predefined keys that perform some action
  std::vector<ActionKey> action_keys;
};

#endif
/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */
