/*
 *  appliImpl.hpp
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

#ifndef _uappliImpl_hpp_
#define	_uappliImpl_hpp_ 1

#include <vector>
#include <sys/time.h>  // fd_set
#include <ubit/uappli.hpp>
#include <ubit/core/uclassImpl.hpp>
#include <ubit/uwin.hpp>
#include <ubit/uupdate.hpp>
#include <ubit/ustyle.hpp>
#include <ubit/umessage.hpp>
#include <ubit/utimer.hpp>

namespace ubit {
  
  class UpdateRequest {
  public:
    Box* obj;
    Update upd;
    UpdateRequest(Box* _obj, const Update& _upd, bool remove_paint) 
    : obj(_obj), upd(_upd) {if (remove_paint) upd.modes &= ~Update::PAINT;}
  };
  
  
  class UAppliImpl {
  public:
    UAppliImpl();
    
    bool isTerminated() const {return is_terminated;}
    
    bool hasPendingRequest() {return request_mask != 0;}  
    ///< true if there is a delete, update or paint request 
    
    void processPendingRequests();
    ///< process all requests (process update then delete then paint requests).
    
    void addDeleteRequest(UObject*);
    void addDeleteRequest(View*);
    void processDeleteRequests();
    
    void addUpdateRequest(Box*, const Update&);
    void removeUpdateRequests(Box*);
    void processUpdateRequests();
    bool isProcessingUpdateRequests() const {return is_processing_update_requests;}
    bool isProcessingLayoutUpdateRequests() const {return is_processing_layout_update_requests;}
    
    int  startModalWinLoop(Window&);
    void addModalWin(Window&);
    void removeModalWin(Window&);
    void setModalStatus(int);
    
    void resetSources(Element* sources, fd_set& read_set, int& maxfd);
    void cleanSources(Element* sources);
    void fireSources(Element* sources, fd_set& read_set);
    
  //private:
    friend class Application;
    friend class Display;
    friend class View;
    friend class EventFlow;
    friend class Timer;
    friend class USource;
    friend class MessageService;
    
    typedef std::vector<UpdateRequest> UpdateRequests;
    typedef std::vector<UObject*> DeletedObjects;
    typedef std::vector<View*> DeletedViews;
    
    Application* appli;        // only ONE Application object should be created
    Display* disp;
    String *app_name;
    bool is_terminated;   // true if the Application has been terminated
    unique_ptr<UErrorHandler> error_handler;
    UFrame* main_frame;   // the main frame of the Application
    // Note: the display list must be static to avoid seg faults if the Application
    // is distroyed before the widgets by the client program 
    UDispList displist;
    UFlowList flowlist;    // list of event flows
    StyleSheet stylesheet;
    String imapath;
    Element* sources;
    UTimerImpl timer_impl;
    class UWinList *modalwins;         // modal windows
    MessagePortMap* messmap;    // the message port of the Application
    unsigned long app_motion_lag, nat_motion_lag;
    
    int main_status;       // status of the event loop of the Application
    int modal_status;      // status of the inner loop the current modal dialog
    bool mainloop_running, subloop_running;
    
    enum {DELETE_REQUEST = 1<<0, UPDATE_REQUEST = 1<<1, PAINT_REQUEST = 1<<2};
    int request_mask;
    bool is_processing_update_requests, is_processing_layout_update_requests;  
    UpdateRequests update_list;    // boxes and wins that will be updated
    DeletedObjects del_obj_list;   // objects that will be deleted
    DeletedViews   del_view_list;  // views that will be deleted
  };
  
}
#endif


