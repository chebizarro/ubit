/*
 *  umservice.h: acess to the UMS (Ubit Mouse/Message Server) services 
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

#ifndef _umservice_hpp_
#define	_umservice_hpp_
#include <ubit/udefs.hpp>
#include <ubit/usocket.hpp>

namespace ubit {
  
  /** MessageService: Ubit Mouse/Message Service.
   */
  class MessageService : public Socket {
  public:
    MessageService(const String& host, int port = 0);
    /**< opens a connection with a remote UMS server (Ubit Mouse/Message Server).
     * This makes it possible to control the pointer(s) of a remote X display,
     * to send text or events to the windows that are located on this display,
     * and many other things (see umsproto.hpp and directory ubit/ums)
     *
     * Args:
     *  - host: the host where the UMS server is running
     *  - port: the port of the UMS server. A value of 0 (the default) 
     *    means that UMS_PORT (ie. 9666) is used.
     *
     *  Note:
     *  the UMS server (ie. the 'umsd' program) must already be running on 
     *  the (local or remote) host. umsd can be found in directory ubit/ums
     */
    
    ~MessageService();
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    /** see browseUMServers().*/
    struct BrowseReply {
      BrowseReply(MessageEvent&);
      int  flags, errorCode;
      long interfaceIndex;
      String serviceName, serviceType, replyDomain;
      int  status;
      bool isRemoved() {return status == false;}
      bool isAdded()   {return status == true;}
    };
    
    bool browseUMServers(UCall& callback);
    /**< browse available UMSservers on a local net by using ZeroConf/Rendezvous.
     * Exemple:
     <pre><tt>
     // o is an instance of Obj, a an instance of Arg (callback methods,
     // such as foo(), can have 0 to 2 args)
     ums->browseUMServers( ucall(o, a, &Obj::foo) );
     ....
     void Obj::foo(Event& e, Arg a) {
     MessageService::BrowseReply r(e);
     cerr << "browse: " << r.serviceName << endl;
     if (r.isAdded())
     ....;    // this UMS server has been added
     else ...;
     }
     </tt></pre>
     */
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    /** see resolveUMServer().*/
    struct ResolveReply {
      ResolveReply(MessageEvent&);
      int  flags, errorCode;
      long interfaceIndex;
      String fullname, hosttarget; //, txtRecord;
      int  port;
    };
    
    bool resolveUMServer(const String& name, UCall& callback);
    /**< resolves a UMS server name into a IP address by using ZeroConf/Rendezvous.
     * Args:
     * - 'name' : the UMS server name to resolve
     * - 'callback' is fired when the address is resolved and is then 
     *    automatically destroyed (except if it is pointed by a uptr<>).
     *
     * Exemple:
     <pre><tt>
     // o is an instance of Obj, a an instance of Arg (callback methods,
     // such as foo(), can have 0 to 2 args)
     ums->resolveUMServer(name, ucall(o, a, &Obj::foo));
     ....
     void Obj::foo(Event& e, Arg a) {
     MessageService::ResolveReply r(e);
     // r.hosttarget = IP address / r.port = the port (9666 by default)
     cerr << "resolve: " << r.hosttarget << " " << r.port << endl;
     }
     </tt></pre>
     */
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    bool browseUMSNeighbors(UCall& callback);
    /**< browse the neigbors of this UMS.
     *  use MessageService::ResolveReply in the callback function.
     *  flags indicates the position and is one of 't', 'b', 'l', 'r'.
     */
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    //bool tactos(const char* data);
    // tactile output stream.
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    bool pressMouse(int event_flow, int button_id);
    bool releaseMouse(int event_flow, int button_id);
    bool moveMouse(int event_flow, int x, int y, bool absolute_coords);
    /**< control the pointer(s) of the display managed by this UMS server.
     * args:
     * - event_flow = 0 identifies the native X event flow
     * - event_flow > 0 are alternate mouse event flows
     * - button_mask is one of Event::MButton1, MButton2, MButton3...
     * - x and y are screen coordinates if 'absolute_coords' is true
     *   and relative to the previous location of the mouse otherwise.
     * notes:
     * - press and release events MUST be balanced
     * - only ONE mouse button at a time (button_mask should not be ORed)
     */
    
    bool pressKey(int event_flow, int keycode);
    bool releaseKey(int event_flow, int keycode);
    /**< sends key events on the display managed by this UMS server.
     * see: pressMouse().
     */
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    
    bool sendMouseClick(const char* target, int x, int y, int button_id, int modifiers);
    bool sendMousePress(const char* target, int x, int y, int button_id, int modifiers);
    bool sendMouseRelease(const char* target, int x, int y, int button_id, int modifiers);
    /**< sends an event to a window located on the display managed by this UMS server.
     * in contrast with pressMouse() etc. these functions do not control
     * the pointers, they just send events to a given window.
     *
     * Args:
     * - target = see sendMessage()
     * - x and y are relative to the target window
     * - 'button_mask' is one of Event::MButton1, MButton2, MButton3...
     * Notes:
     * - press and release events MUST be balanced.
     * - only ONE mouse button at a time (button_mask should not be ORed)
     */
    
    bool sendMessage(const char* target, const String& message);
    bool sendMessage(const char* target, const char* message);
    /**< sends a message to a target window located on the display managed by this UMS server.
     * window = name or X ID of the window, can be one of:
     * - name-without-blanks
     * - quoted 'name' (can contain blanks)
     * - decimal ID (1234)
     * - hexadecimal ID (0x1234a)
     */
    
    bool sendRequest(int ums_request, const String& data);
    bool sendRequest(int ums_request, const char* data = null);
    ///< sends a request to the UMS server (see UMSrequest).
    
    // - - - impl. - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    MessageService(const String& host, int port, const char* client_name);
    ///< [impl].
    
    virtual void inputCallback();
    ///< [impl].
    
  protected:
    uptr<UCall> browse_call, neighbor_call; 
  };
  
}
#endif
