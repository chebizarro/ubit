/*
 *  uima.h: images
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


#ifndef _uima_hpp_
#define	_uima_hpp_ 1

#include <ubit/udata.hpp>

namespace ubit {

/** Image.
  *
  * Depending on the constructor, one can create: an empty image, an image that
  * is loaded from a JPEG, GIF or XPM file (see below) or an image that uses
  * XPM data compiled in the program code.
  * 
  * Loading a file image.
  * By default, the constructor does not load the file immediately but first time
  * the Image is displayed on the screen (but an optional arg changes this behavior).
  * The load() and read() functions can also be used to (re)load a file.
  *
  * Filename and formats:
  * - XPM, GIF and JPEG formats are supported *IF* the corresponding libraries
  *   where linked to the Ubit library at ompiltaion time
  * - the filename must have the appropriate suffix (xpm, gif, jpg, jpeg)
  * - the filename is prefixed by Application::getImaPath() if it does not start by / or .
  *   this prefix is empty by default. It can be changed by Application::setImaPath()
  *
  * Loading compiled XPM data 
  * XPM data is compiled in the program code. It is not duplicated and must not be
  * freed. As for files, the data is loaded when the image appers for the fisrt time.
  * 
  * Alpha channel and Background Transparency
  * - background transparency (image shaping) is supported by the UPix subclass.
  * - alpha channel is only supported in OpenGL mode (when available, see Application).
  *
  * @see: the UPix subclass.
  */
class Image: public Data {
public:
  UCLASS(Image)

  Image(int width, int height);
  /**< creates an empty image.
  */
     
  Image(const char* filename = null, bool load_now = false);
  /**< creates a new image from an image file.
  * By default, the image file is NOT loaded immediately but the first time
  * the Image is displayed on the screen. 'load_now' = true, forces the file 
  * to be loaded immediately
  * @see: shortcut uima(const char* filename) and class Image for more details.
  */
    
  Image(const String& filename, bool load_now = false);
  /**< creates a new image from an image file.
  * By default, the image file is NOT loaded immediately but the first time
  * the Image is displayed on the screen. 'load_now' = true, forces the file 
  * to be loaded immediately
  * @see: shortcut uima(const String& filename) and class Image for more details.
  */
  
  Image(const char** xpm_data, bool load_now = false);
  /**< creates a new image from XPM data (beware that XPM data is NOT duplicated!).
  * By default, the data is NOT loaded immediately but the first time
  * the Image is displayed on the screen. 'load_now' = true, forces the data 
  * to be loaded immediately  
  *
  * !NOTE that the XPM data is not duplicated and should NEVER be freed!
  *
  * @see: shortcut uima(const char** xpm_data) and class Image for more details.
  */
  
  Image(const char** xpm_data, UConst);

  virtual ~Image();

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  Image& operator=(const Image& ima2);
  /**< copies the content of ima2 to this image.
    * bewaee that no action is performed if ima2 has not yet been loaded (@see isLoaded()).
    */
  
  void resize(int width, int height) { resize(width, height, false, false);}
  /**< resizes the image data to the specified size.
    * @see: resize(int width, int height, bool preserve_ratio, bool dont_magnify);
    */

  void resize(int max_width, int max_height, bool preserve_ratio, bool dont_magnify);
  /**< resizes this image data according to the specified constraints.
    * the new size will be inferior or egal to the one specified according to:
    * - preserves the aspect ratio if 'preserve_ratio' is true (either the actual 
    *   width or height may be smaller than the requested one)
    * - dont magnify images that are smaller than the requested size if 
    *   'dont_magnify' is true
    */

  bool rescale(float scale) {return rescale(scale, scale);}
  ///< rescales this image.
    
  bool rescale(float xscale, float yscale);
  ///< rescales this image.
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  virtual int read(const String& filename, int max_width = 0, int max_height =0);
  /**< loads an image file.
    * calls resize(max_width,max_height,true,true) if max_width and max_height are >0.
    * returns the reading status (@see UFilestat).
    * note: use Image::set() instead of read() to postone loading.
    */
    
  virtual int read(const char* filename, int max_width = 0, int max_height =0);
  /**< loads an image file.
    * calls resize(max_width,max_height,true,true) if max_width and max_height are >0.
    * returns the reading status (@see UFilestat).
    * note: use Image::set() instead of read() to postone loading.
    */
  
  virtual int loadFromData(const char** xpm_data);
  /**< loads XPM data.
    * the XPM data is not duplicated and should not be destroyed.
    * note: use Image::set() instead of read() to postone loading.
    */
    
  virtual int loadNow();
  /**< loads the image file that was specified by set() or by the constructor.
    * returns the reading status (@see UFilestat).
    */
    
  bool isLoaded() const {return stat > 0;}
  /**< returns true if the image has been loaded.
    * Note that images that have not yet been displayed on the screen are not
    * loaded except if stated explicitely (see the Image::Uima, load(), read())
    */
  
  virtual void set(const String& filename);
  virtual void set(const char* filename);
  virtual void set(const char** xpm_data);
  /**< specifies the file name.
    * the file is not loaded immediately but when the image appears for
    * the first time. Call load() to load the file immediately.
    */

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  int getWidth()  const;
  ///< returns the image width (0 if the image is not loaded).
  
  int getHeight() const;
  ///< returns the image height (0 if the image is not loaded).

  int getBpp() const;
  ///< returns the number of bits per pixel (0 if the image is not loaded).
  
  int getTransparency() const;
  ///< 0 (opaque), 1(bitmask) or >1(alpha channel).
  
  int getStatus() const {return stat;}
  ///< returns the loading status of this image (see UFilestat)

  virtual bool isPix() const {return false;}
  ///< returns true if this Image is in fact a UPix (that derives from Image).
  
  virtual void update(); 
  ///< update parents' views.

  // - - - impl - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  int realize(int max_w = 0, int max_h = 0,
              Display* disp = null, bool force_reload = true) const;
  /**< 
   * [impl] allocates physical resources and loads the image in memory.
   * this function is automatically called when this image is displayed
   * for the first time: there no need to call it in client code.
   * details:
   * - returns the loading status  (see UFilestat)
   * - the 'disp' arg. can be the Application
   * - the 'force_reload' arg. specifies if the file must be reloaded:
   *   #- if false, previously loaded files are not reloaded
   *   #- if true files are always reloaded
   */
   
  bool isRealized() const;
  ///< [impl] returns true if the image has been sucesfully loaded and initialized.

  std::list<UHardIma*>& getNatImas() const {return natimas;}
  ///< [impl] returns internal implementation.

  static void getFullPath(String& fullpath, const char* filename);
  /**< gets the full image pathname.
    * Application::getImaPath() (the default image pathname) is prefixed
    * to image filenames that don't start with / or .
    */

#ifndef NO_DOC
  friend class Box;
  friend class UHardIma;
  friend class UPix;

  virtual void setImpl(const char* fname);
  virtual void setImpl(const char** xpm_data);
  virtual void setImpl(int width, int height);
  virtual void cleanCache();
  virtual void getSize(UpdateContext&, Dimension&) const;
  virtual void paint(Graph&, UpdateContext&, const Rectangle&) const;

  UHardIma* getOrCreateIma(Display*, float xyscale) const;  // not virtual!
  UHardIma* findImaInCache(Display*, float xyscale) const;  // not virtual!
  UHardIma* addImaInCache(Display*, float xyscale) const;   // not virtual!
  
protected:
  mutable std::list<UHardIma*> natimas;
  char *name;
  const char **data;
  mutable signed char stat;  // can have negative values
  enum Mode {EMPTY, CREATE, READ_FROM_FILE, READ_FROM_DATA};
  mutable char mode;
  bool show_unknown_ima;
#endif
};

inline Image& uima(const String& filename)  {return *new Image(filename);}
///< shortcut function that creates a new Image image.

inline Image& uima(const char* filename)  {return *new Image(filename);}
///< shortcut function that creates a new Image image.

inline Image& uima(const char** xpm_data) {return *new Image(xpm_data);}
///< shortcut function that creates a new Image image.

}
#endif
