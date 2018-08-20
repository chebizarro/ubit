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
#include <memory>

#include <ubit/ui/window.h>
#include <ubit/uicon.hpp>
#include <ubit/draw/font.h>
#include <ubit/ui/listbox.h>

namespace ubit {
  
  /* [Impl] A directory in the Finder.
   */ 
  class UFinderDir : public Item {
    friend class Finder;
    String fpath, fname;
    std::unique_ptr<IconBox> iconbox;
    PopupMenu popmenu;
    bool keep_open;
    int path_type;
    
  public:
    UFinderDir(class Finder*, const String& path, int path_type);
    ~UFinderDir();
    
    const String& getDir() const;
    class IconBox* getIconbox() const;
    
    void setDir(const String& dirname);
    void setIconbox(class IconBox*);
    
    void emph(bool state);
    void setKeepOpen(bool st) {keep_open = st;}
    bool isKeepOpen() {return keep_open;}
  };
  
  /* [Impl] A host in the Finder.
   */ 
  class FinderHost : public Box {   // UVbox
  public:
    typedef void (FinderHost::*HostMethod)();
    
    FinderHost(class Finder*, const String& hostname);
    void resolve(HostMethod);
    void createClone();
    void deleteClone();
    void addXhost();
    void removeXhost();
    void calibrate();
    void putFile();
    
  protected:
    friend class Finder;
    class Finder& fd;
    String hostname, address;
    Box *clone_btn, *xhost_btn, *put_btn, *get_btn;
    Window *clone_win;
    
    void resolveCB(MessageEvent&, HostMethod);
    void createCloneImpl();
    void addXhostImpl();
    void calibrateImpl();
    void putFileImpl();
  };
  
  /* [Impl] Finder communication (remote file access by using ssh, etc)
   */ 
  class UFinderCom {
  public:
    UFinderCom(Finder*, const String& path, int path_type);
    
  protected:
    enum {INVALID_URL=-2};
    static void loadDoc(UFinderCom*);  // pthread_create() callback
    void showDoc();
    
    pthread_t thread_id;
    Finder* fd;
    String path, cachepath;
    int type, mode;
    int stat;
  };
  
}
#endif
