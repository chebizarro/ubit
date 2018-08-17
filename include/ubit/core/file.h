/*
 *  file.hpp
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

#ifndef _ufile_hpp_
#define	_ufile_hpp_ 1

#include <vector>
#include <ubit/core/node.h>

namespace ubit {
  
    /** File status returned by functions that open files.
   */
  struct UFileStat {
    enum {
      UnknownType   = -5,
      MiscError     = -4,
      NoMemory      = -3,
      InvalidData   = -2,
      CannotOpen    = -1,
      NotOpened     =  0,
      Opened        =  1
    };
  };
  
  /** synonym of UFilestat.
   */
  typedef UFileStat UFilestat;
  
    /** file mode.
   */
  class UFileMode {
  public:
    UFileMode(const String& pathname);
    
    bool isValid() const;
    bool isDir()  const;
    bool isFile() const;
    bool isLink() const;
    bool isExec() const;
    
    static const int DIR, FILE, LINK; ///< modes
    int getMode() const {return mode;}
    
  protected:
    UFileMode() {}
    int mode;  //mode_t mode;
  };
  
    /** file info.
   */
  class UFileInfo : public UFileMode {
  public:
    UFileInfo(const String& path);
    ///< sets fileinfo data for this path and stores path as a name.
    
    UFileInfo(const char* fullpath, const char* fname);  
    ///< sets fileinfo data for fullpath and stores fname as a name.
    
    void setPath(const String& path);
    ///< resets fileinfo data for this path.
    
    const String* getFileName() const {return pname;}
    ///< returns the file name (without the path).
    
    unsigned long getSize() const {return size;}
    //< returns the size of the file.
    
    unsigned long getModTime() const {return modtime;}
    //< returns the time when file data was last modified.
    
    Image& getIconImage() const;
    /**< return an image that characterizes this file type.
     * the returned image is shared and can't be deleted.
     */
    
    Image& getMiniIconImage() const;
    /**< return an mini image that characterizes this file type.
     * the returned image is shared and can't be deleted.
     */
    
    void getInfoString(String& str) const;
    ///< copies infos in str.
    
    enum PrefixType {LOCAL, SSH, HTTP, FTP};  
    static int parsePrefix(const String& path);
    ///< returns the PrefixType.
    
  protected:
    friend class UFileDir;
    friend class UFilebox;
    unsigned long size;
    unsigned long modtime;
    uptr<String> pname;
    void stat(const char* path);
  };
  
  /** file info container.
   */
  typedef std::vector<UFileInfo*> UFileInfos;
  
    /** File Directory.
   */
  class UFileDir {
  public:
    UFileDir();
    UFileDir(const String& path);
    virtual ~UFileDir();
    
    void read(const String& path);
    /**< reads this directory and gets file entries.
     * @see read(const String& path, const String& prefix, const String& filter, bool with_dot_files)
     */
    
    void read(const String& path, const String& prefix, const String& filter, bool with_dot_files);
    /**< reads this directory and gets file entries.
     * Args:
     * - prefix: get files that start witrh this prefix
     * - filter: get files with this filter.
     *   filter syntax: "C++ Files (*.cpp; *.hpp)" or "*.cc; *.hh" or "*.*" 
     * _ with_hidden_files: get files starting with a dot.
     */
    
    void readRemote(const String& path, const String& prefix, const String& filter, bool with_dot_files);
    /**< reads a remote directory and gets file entries.
     * same as read() but SSH is used to acces a remote directory
     */
    
    const String& getPath() const;
    /**< returns the full pathname of the directory
     * note that the path arg. given to the constr, read(), readRemote()
     * is expanded, so that getPath() may return a different string.
     */
    
    const UFileInfos& getFileInfos() const {return file_infos;}
    ///< returns the (filtered) file entries of the directory.
    
    static void expandDirPath(String& dirpath);
    ///< expands the directory path (if there are path symbols in the dirpath).
    
  private:
    friend class UIconbox;
    UFileInfo dir_info;
    UFileInfos file_infos;
    std::vector<String*> filters;
    static bool compareEntries(const UFileInfo*, const UFileInfo*);  
    static void parseFilter(std::vector<String*>& filters, const String& filter);
  };
  
    /** file cache for SSH.
   */
  class UFileCache {
  public:
    static void createCache();
    static void cleanCache();
    
    static const String& getCachePath();
    
    static String* createFullPath(const String& path);
    //  expands ~, ssh:, http:, ftp:.
    
    static const String* getOrCreateFullPath(const String& path);
    // same as createFullPath() but fullpath may be shared with path
    
    static bool getCachePath(const String& url, int path_type,
                             String& server, String& path, String& cachepath);
  protected:
    static String cache;
  };
  
}
#endif
