/*
 *  finder.h: Finder element
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
#include <ubit/ui/background.h>
#include <ubit/ui/menu.h>
#include <ubit/core/doc.h>
#include <ubit/uicon.hpp>

namespace ubit {

 class UFinderDir;
 class FinderHost;
 class UFinderFullwin;
 class UFinderControls;
 class UFinderCom;

/** Finder Listener.
*/
struct FinderListener {
  virtual ~FinderListener() {}
  virtual void fileRequest(const String& pathname) {}
  virtual void docLoaded(const String& pathname, Document*) {}
  virtual void docShown(const String& pathname, Document*) {}
  virtual void dirLoaded(const String& pathname) {}
  virtual void dirShown(const String& pathname) {}
};

/** file finder.
*/
class Finder : public Box {
public:
  UCLASS(Finder)

  struct Options : Element {
    friend class Finder;
    Options();
    USize  clone_frame_size;
    Background default_background;
    Color default_color;
    bool show_icon_images;
  };

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  Finder(const String& pathname = ".");
  virtual ~Finder();
    
  virtual void open(const String& pathname);
  ///< opens a directory or a file (the second variant normalizes pathname).

  virtual void openParent();
  virtual void openEntry();  
  virtual void nextEntry();
  virtual void previousEntry();
  ///< navigates in the finder.

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  virtual bool browseHosts();
  virtual FinderHost* addHost(const String& hostname);
  virtual FinderHost* addHostCB(const String* hostname);
  virtual void removeHost(FinderHost*);
  virtual void addHosts(const std::vector<String*>& hostnames);
  virtual void addHosts(const char* hostnames[]);

  virtual void createClone(const String& hostname);
  ///< creates a synchronized clone on hostname.

  // virtual void compactEvents(bool = true);
  // compact events to avoid delays.

  virtual void setTracking(bool);
  virtual void setTracking(bool doc, bool icons);
  ///< tracking mode updates the value while the scrollbar is being dragged (default is true).

  virtual FinderListener* getListener();
  virtual void setListener(FinderListener*);
  /**< gets/set the current listener.
    * the finder creates a default listener that opens documents
    * when hyperlinks are clicked (see FinderListener)
    * limitation: only one listener in this version!
    */
    
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  Options& getOptions() {return opts;}
  ///< returns the options of the Finder.
  
  DocumentBox*  getDocbox() {return pdocbox;}
  IconBox* getIconbox() {return piconbox;}

  DocumentBox* getCurrentBox();
  ///< returns docbox or iconbox depending on which is currently shown.
  
  Icon* getSelectedIcon();
  ///< return the icon box that is currently shown (if any, null otherwise).
  
  Document* getDoc() {return pdocument;}
  ///< return the document box that is currently shown (if any, null otherwise).

  virtual void zoom(float zoom = 1.);
  virtual void zoomIn();
  virtual void zoomOut();
  virtual void setZoomQuantum(float);
    
  ControlMenu& getContextMenu() {return *ctlmenu;}
  ///< returns the Contextual menu of the Finder.
  
  virtual void openContextMenuIn(Box&);
  virtual ControlMenu& createContextMenu();
 
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
  void openFBox(FileChooser&);
  void openMsg(MessageEvent&);
  virtual void openImpl(const String& path, int path_mode, int path_type);
  virtual int openFile(const String& path, int path_type);
  virtual int openDir(const String& path, int path_type);
  virtual int openDir(UFinderDir*);
  virtual void showFile(const String& path, Document*);
  virtual void showDir(const String& path);
  virtual void showPreview(Icon*);
  virtual void showPreviewRequest(Icon*);
  virtual void showIconPreviews();  
  
  // dirs and icons.
  virtual void addToHistory(UFinderDir*);
  virtual void removeFromHistory(UFinderDir*);
  virtual UFinderDir* findDir(const String& path);
  virtual void showDirInfo(UFinderDir*);
  virtual void removeIconbox(UFinderDir*, bool upd);
  virtual void iconSelectCB(IconBox*);
  virtual void iconActionCB(IconBox*);
  virtual void linkActionCB(InputEvent&, Document*);

  // hosts and clones.
  virtual void browseCB(MessageEvent&);
  bool isBrowsing() const;
  virtual void createCloneRequest(MessageEvent&);
  virtual Frame* createCloneFrame(const String& title);
  
  virtual void initOptbox();
  void showSelectedIcon();
  void showDirBox();
  void showDocBox();
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
  friend class UFinderDir;
  friend class FinderHost;
  friend class UFinderCom;  
  static UPix& doc_pix;
  enum {NoMode, DirMode, DocMode} mode;
  bool is_tracking, open_in_fullwin;
  Options opts;
  UBar toolbar;
  Box mainbox, optbox, folderlist, hostlist, filelist;
  unique_ptr<Box> optbox_btn, filelist_btn;
  unique_ptr<Dialog> ask_dialog;
  unique_ptr<String> ask_dialog_msg;
  FinderListener* listener;
  unique_ptr<AlertBox> alertbox;
  unique_ptr<ControlMenu> ctlmenu;
  MessageService *local_ums, *remote_ums;

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // current selection
  unique_ptr<Document> pdocument;
  unique_ptr<DocumentBox> pdocbox;
  unique_ptr<IconBox> piconbox;
  //unique_ptr<Box> docglass;
  unique_ptr<UFinderDir>last_direntry;
  Icon* last_preview_request;
  Icon* last_preview;
  ChildIter previews_current, previews_end;
  unique_ptr<Timer> preview_timer;
 };

}
#endif
