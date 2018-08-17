/*
 *  finder.h: UFinder element
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

#ifndef _ufinder_hpp_
#define _ufinder_hpp_
#include <ubit/ucolor.hpp>
#include <ubit/ubackground.hpp>
#include <ubit/umenu.hpp>
#include <ubit/udoc.hpp>
#include <ubit/uicon.hpp>

namespace ubit {

 class UFinderDir;
 class UFinderHost;
 class UFinderFullwin;
 class UFinderControls;
 class UFinderCom;

/** UFinder Listener.
*/
struct UFinderListener {
  virtual ~UFinderListener() {}
  virtual void fileRequest(const String& pathname) {}
  virtual void docLoaded(const String& pathname, Document*) {}
  virtual void docShown(const String& pathname, Document*) {}
  virtual void dirLoaded(const String& pathname) {}
  virtual void dirShown(const String& pathname) {}
};

/** file finder.
*/
class UFinder : public Box {
public:
  UCLASS(UFinder)

  struct Options : Element {
    friend class UFinder;
    Options();
    USize  clone_frame_size;
    Background default_background;
    Color default_color;
    bool show_icon_images;
  };

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  UFinder(const String& pathname = ".");
  virtual ~UFinder();
    
  virtual void open(const String& pathname);
  ///< opens a directory or a file (the second variant normalizes pathname).

  virtual void openParent();
  virtual void openEntry();  
  virtual void nextEntry();
  virtual void previousEntry();
  ///< navigates in the finder.

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  virtual bool browseHosts();
  virtual UFinderHost* addHost(const String& hostname);
  virtual UFinderHost* addHostCB(const String* hostname);
  virtual void removeHost(UFinderHost*);
  virtual void addHosts(const std::vector<String*>& hostnames);
  virtual void addHosts(const char* hostnames[]);

  virtual void createClone(const String& hostname);
  ///< creates a synchronized clone on hostname.

  // virtual void compactEvents(bool = true);
  // compact events to avoid delays.

  virtual void setTracking(bool);
  virtual void setTracking(bool doc, bool icons);
  ///< tracking mode updates the value while the scrollbar is being dragged (default is true).

  virtual UFinderListener* getListener();
  virtual void setListener(UFinderListener*);
  /**< gets/set the current listener.
    * the finder creates a default listener that opens documents
    * when hyperlinks are clicked (see UFinderListener)
    * limitation: only one listener in this version!
    */
    
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  Options& getOptions() {return opts;}
  ///< returns the options of the UFinder.
  
  UDocbox*  getDocbox() {return pdocbox;}
  UIconbox* getIconbox() {return piconbox;}

  UDocbox* getCurrentBox();
  ///< returns docbox or iconbox depending on which is currently shown.
  
  UIcon* getSelectedIcon();
  ///< return the icon box that is currently shown (if any, null otherwise).
  
  Document* getDoc() {return pdocument;}
  ///< return the document box that is currently shown (if any, null otherwise).

  virtual void zoom(float zoom = 1.);
  virtual void zoomIn();
  virtual void zoomOut();
  virtual void setZoomQuantum(float);
    
  UCtlmenu& getContextMenu() {return *ctlmenu;}
  ///< returns the Contextual menu of the Finder.
  
  virtual void openContextMenuIn(Box&);
  virtual UCtlmenu& createContextMenu();
 
  void showSideBar(bool state);
  ///< shows/hides the side bar (located on the left side).

  void showAlert(const String& msg);
  void hideAlert();
  ///< shows/hides an alert message.
  
  //void showDocGlass(bool state);
  // shows/hides the document glass.
  //Box* getGlass() {return docglass;}
  //returns the glass box that is on top of the document box.
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
protected:
  // documents.
  void openFBox(UFilebox&);
  void openMsg(MessageEvent&);
  virtual void openImpl(const String& path, int path_mode, int path_type);
  virtual int openFile(const String& path, int path_type);
  virtual int openDir(const String& path, int path_type);
  virtual int openDir(UFinderDir*);
  virtual void showFile(const String& path, Document*);
  virtual void showDir(const String& path);
  virtual void showPreview(UIcon*);
  virtual void showPreviewRequest(UIcon*);
  virtual void showIconPreviews();  
  
  // dirs and icons.
  virtual void addToHistory(UFinderDir*);
  virtual void removeFromHistory(UFinderDir*);
  virtual UFinderDir* findDir(const String& path);
  virtual void showDirInfo(UFinderDir*);
  virtual void removeIconbox(UFinderDir*, bool upd);
  virtual void iconSelectCB(UIconbox*);
  virtual void iconActionCB(UIconbox*);
  virtual void linkActionCB(UInputEvent&, Document*);

  // hosts and clones.
  virtual void browseCB(MessageEvent&);
  bool isBrowsing() const;
  virtual void createCloneRequest(MessageEvent&);
  virtual UFrame* createCloneFrame(const String& title);
  
  virtual void initOptbox();
  void showSelectedIcon();
  void showDirBox();
  void showDocBox();
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
  friend class UFinderDir;
  friend class UFinderHost;
  friend class UFinderCom;  
  static UPix& doc_pix;
  enum {NoMode, DirMode, DocMode} mode;
  bool is_tracking, open_in_fullwin;
  Options opts;
  UBar toolbar;
  Box mainbox, optbox, folderlist, hostlist, filelist;
  uptr<Box> optbox_btn, filelist_btn;
  uptr<UDialog> ask_dialog;
  uptr<String> ask_dialog_msg;
  UFinderListener* listener;
  uptr<UAlertbox> alertbox;
  uptr<UCtlmenu> ctlmenu;
  MessageService *local_ums, *remote_ums;

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // current selection
  uptr<Document> pdocument;
  uptr<UDocbox> pdocbox;
  uptr<UIconbox> piconbox;
  //uptr<Box> docglass;
  uptr<UFinderDir>last_direntry;
  UIcon* last_preview_request;
  UIcon* last_preview;
  ChildIter previews_current, previews_end;
  uptr<Timer> preview_timer;
 };

}
#endif
