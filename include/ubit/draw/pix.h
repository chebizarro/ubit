/*
 *  pix.h: X11 pixmaps
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


#ifndef _upix_hpp_
#define	_upix_hpp_ 1

#include <ubit/uima.hpp>

namespace ubit {

/** Pixmap Image.
  * Pixmaps are a special kind of images that allows for faster rendition when
  * X11 is used as a graphics engine (see Application). 
  * 
  * Pixmaps also allows for background transparency (image shaping) for XPM
  * files and data.
  *
  * @see class Image for details.
 */
class UPix: public Image {
public:
  UCLASS(UPix)
  
  /** Predefined Pixmap Images (small icons).
   * some of these pixmaps were originally designed by Ludvig A. Norin
   * for the FVWM window manager. Please refer to file upixmaps.hpp
   * for notice (more pixmaps are also available in this file).
   * NB: most of these pixmaps are shaped (= have a "transparent" background)
   */
  static UPix
  metal, velin, folder, ofolder, doc, diskette, plus, minus, 
  zoom, undo, bigFolder, bigDoc, bigText, bigIma, bigLeft, bigRight, bigUp, bigDown, 
  hslider, vslider, ray, question, link, stop;
    
  UPix(const char* filename = null, bool load_now = false);
  ///< creates a new UPix image from an image file; @see Image and the upix() shortcut.
  
  UPix(const String& filename, bool load_now = false);
  ///< creates a new UPix image from an image file; @see Image and the upix() shortcut.

  UPix(const char** xpm_data, bool load_now = false);
  ///< creates a new UPix image from XPM data; @see Image and the upix() shortcut.

  UPix(const char** xpm_data, UConst);

  virtual ~UPix();

  virtual bool isPix() const {return true;}
  ///< returns true as this Image is a UPix.
  
  // - - - impl - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
  virtual void cleanCache();
  virtual void getSize(UpdateContext&, Dimension&) const;
  virtual void paint(Graph&, UpdateContext&, const Rectangle&) const;
  UHardPix* getOrCreatePix(Display*, float xyscale) const;  // not virtual!
private:
#if WITH_2D_GRAPHICS
  std::vector<UHardPix*>& getNatPixs() const {return natpixs;}
  mutable std::vector<UHardPix*> natpixs;
#endif
#endif
};


inline UPix& upix(const String& filename)  {return *new UPix(filename);}
///< shortcut function that creates a new UPix image.

inline UPix& upix(const char* filename)  {return *new UPix(filename);}
///< shortcut function that creates a new UPix image.

inline UPix& upix(const char** xpm_data) {return *new UPix(xpm_data);}
///< shortcut function that creates a new UPix image.

}
#endif
