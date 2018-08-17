/*
 *  finderImpl.h: ufinder implementation (see ufinder.hpp)
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

#ifndef _ufinderImpl_hpp_
#define _ufinderImpl_hpp_
#include <pthread.h>
#include <ubit/uwin.hpp>
#include <ubit/uicon.hpp>
#include <ubit/ufont.hpp>
#include <ubit/ulistbox.hpp>

namespace ubit {
  
  /* [Impl] A directory in the UFinder.
   */ 
  class UFinderDir : public UItem {
    friend class UFinder;
    String fpath, fname;
    uptr<UIconbox> iconbox;
    UPopmenu popmenu;
    bool keep_open;
    int path_type;
    
  public:
    UFinderDir(class UFinder*, const String& path, int path_type);
    ~UFinderDir();
    
    const String& getDir() const;
    class UIconbox* getIconbox() const;
    
    void setDir(const String& dirname);
    void setIconbox(class UIconbox*);
    
    void emph(bool state);
    void setKeepOpen(bool st) {keep_open = st;}
    bool isKeepOpen() {return keep_open;}
  };
  
  /* [Impl] A host in the UFinder.
   */ 
  class UFinderHost : public Box {   // UVbox
  public:
    typedef void (UFinderHost::*HostMethod)();
    
    UFinderHost(class UFinder*, const String& hostname);
    void resolve(HostMethod);
    void createClone();
    void deleteClone();
    void addXhost();
    void removeXhost();
    void calibrate();
    void putFile();
    
  protected:
    friend class UFinder;
    class UFinder& fd;
    String hostname, address;
    Box *clone_btn, *xhost_btn, *put_btn, *get_btn;
    Window *clone_win;
    
    void resolveCB(MessageEvent&, HostMethod);
    void createCloneImpl();
    void addXhostImpl();
    void calibrateImpl();
    void putFileImpl();
  };
  
  /* [Impl] UFinder communication (remote file access by using ssh, etc)
   */ 
  class UFinderCom {
  public:
    UFinderCom(UFinder*, const String& path, int path_type);
    
  protected:
    enum {INVALID_URL=-2};
    static void loadDoc(UFinderCom*);  // pthread_create() callback
    void showDoc();
    
    pthread_t thread_id;
    UFinder* fd;
    String path, cachepath;
    int type, mode;
    int stat;
  };
  
}
#endif
