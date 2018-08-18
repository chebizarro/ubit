/*
 *  filebox.h: File Box Element
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

#ifndef _ufilebox_hpp_
#define	_ufilebox_hpp_ 1

#include <ubit/ubox.hpp>
#include <ubit/uchoice.hpp>

namespace ubit {

/* File chooser.
* A FileChooser can be included into a Dialog or a Menu as follows:
* examples:
* <pre>
*   class Demo {
*   public:
*     void openFile(Event& e) {...}   // the Event& parameter is optional
*     ...
*   };
*
*   Demo* d = new Demo();
*
*   // NB: ufilebox(...) is a shortcut for *new FileChooser(...)
*   Box& fbox = ufilebox( utitle("Open File")
*                          + UOn::action / ucall(this, &Demo::openFile)
*                         );
*   Dialog& fdialog = udialog(fbox)
*   // and/or:
*   Menu& fmenu = umenu(fbox)
* </pre>
*
* this->openFile() is called when a file is double clicked or when
* the OK button of the filebox is clicked.
*
* @see: UCall for more info on callback methods/functions and their parameters
*/
class FileChooser : public Box {
public:
  UCLASS(FileChooser)

  FileChooser(const Args& arglist = Args::none);
  /**< create a new file box (@see also shortcut ufilebox()).
    * The arglist is typically used to add a callback function.
    * see class FileChooser for an example.
    */
    
  virtual ~FileChooser();

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  const String& getDir() const {return *fdir2;}
  ///< returns the directory of the file (without the file name).

  const String& getName() const {return *fname;}
  ///< returns the name of the file (without the directory).

  const String& getPath() const {return *fpath;}
  String& path() {return *fpath;}
  ///< returns the full pathname: getDir()/getName().

  const String& getFilter() const {return *ffilter;}
  ///< returns the extension filter.

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  virtual void setDir(const String&);
  ///< note that setDir and setFilter reload the directory.

  virtual void setName(const String&);

  virtual void setFilter(const String&);
  ///< syntax: "C++ Files (*.cpp; *.hpp)" or "*.cc; *.hh" or "*.*" 

  virtual void showList(bool);
  virtual void showHiddenFiles(bool);

  virtual void rescan();
  ///< rescans the directory and updates the display.

  void setAutoClose(bool stat) {autoclose = stat;}
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
protected:
  String fspec;
  unique_ptr<String> fname, fdir2, ffilter, fpath;
  //Scrollpane* scrollpane;
  Box *mainbox;
  Box *show_list, *show_hidden_files;
  RadioSelect new_sel;
  bool autoclose;
  
  virtual void cancelBehavior(InputEvent&);
  virtual void okBehavior(InputEvent&);
  virtual void selectBehavior(Event&, String* pathname);
  virtual void setDirImpl(String*);
  virtual void changeDirImpl(String*);
  /* sets the directory in a relative way.
    * - goes to the parent directory if arg is ".."
    * - goes to subdirectory "xxx" if arg is "xxx"
    */
};

FileChooser& ufilebox(const Args& arglist = Args::none);
///< shortcut that returns *new FileChooser().

}
#endif



