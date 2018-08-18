/*
 *  conf.hpp : configuration of the Application
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

#ifndef _uconf_hpp_
#define	_uconf_hpp_ 1

#include <ubit/udefs.hpp>
#include <ubit/core/string.h>
#include <ubit/draw/font.h>

namespace ubit {


/** specification and retreiving of non standard options on the command line.
 * example:
 * - command line:  myappli --input data.txt --val 5 --no-reco
 * - note: --no-reco is the negation or --reco
 *
 * Source code for parsing these options:
 * <pre>
 *   char* input_file  = null;
 *   int   value = 1;
 *   bool reco_mode = false;
 *
 *   Option options[] = {
 *     {"in","put", Option::Arg(input_file)}, // "in" is mandatory, "put" is optionnal
 *     {"value","", Option::Arg(value)},      // "value" is mandatory, no optionnal part
 *     {"reco","",  Option::Arg(reco_mode)},  
 *     {null}  // must be NULL terminated !!
 *   };
 *
 *   // changes the values of variables 'input_file', 'value' and 'reco_mode'
 *   // if the corresponding options are specified in the command line
 *
 *   UConf conf(argc, argv, options);
 *   Application appli(conf);
 *   ...
 * </pre>
 */
class Option {
public:
  const char *begname, *endname;
  class UOptionArg* arg;

  static class UOptionArg* Arg(bool& val);
  static class UOptionArg* Arg(int& val);
  static class UOptionArg* Arg(float& val);
  static class UOptionArg* Arg(String& val);
  static class UOptionArg* Arg(char*& val);
  static class UOptionArg* Arg(const char*& val);
  static void parseOptions(int& argc, char** argv, Option* options);
};

/**
 * configuration of the Application.
 */
class UConf {
public:  
  UConf();
  virtual ~UConf();
  
  void useGL(bool = true);
  /**< specifies whether OpenGL is used for rendering graphics.
    * OpenGL rendering is used by default. This can be changed by calling this
    * function BEFORE the Application is created or by launching the application with
    * the -no-gl option
    */
  
  void useFreeType(bool = true);
  /**< specifies whether FreeType is used for rendering fonts.
    * the current implementation of FreeType requires OpenGL. 
    * This function should not be called after the creation of the Application.
    */
  
  void setDepthBuffer(int size);
  /**< specifies if a depth buffer should be used and the desired size.
    * typical values are 0 (no depth buffer) or 16. The size that is actually
    * used is implementation dependent (e.g. GLUT uses a default value if 'size' > 0)
    */
  
  void setStencilBuffer(int size);
  /**< specifies if a stencil buffer should be used and the desired size.
    * The size that is actually used is implementation dependent.
    */
  
  void setBpp(int bpp);
  /**< specifies the prefered number of Bits Per Pixels.
    * default is 24 bits, a value of 0 means "use default". In any case, the requested
    *  BPP is a hint: the closest available BPP will be used
    */
  
  void setLocale(const String& s);
  ///< changes the locale [default = "" meaning: use default locale].

  void setScale(float v);
  ///< changes the scaling factor of the whole application [default = 1].

  void setVerbose(int v);
  ///< set verbose mode [default = false].

  void setDefaultBackground(const Background&);
  ///< changes the default background.
    
  void setTransparentScrollbars(bool);
  
  virtual void printHelp();
  ///< prints Ubit help on cout.
  
  
  bool isUsingGL() const {return is_using_gl;}  
  bool isUsingFreeType() const {return is_using_freetype;}
  bool isVerbose() const {return verbosity != 0;}
  int  getVerbosity() const {return verbosity;}
  
  const String& getLocale() const {return locale;}
  ///< current locale.
  
  float getScale() const {return scale;}
  ///< global scale of the application.
  
  int getMouseSelectButton() const {return mouse_select_button;}
  ///< No of the standard button [default = left btn].
  
  int getMouseMenuButton() const {return mouse_menu_button;}
  ///< No of the button that opens contextual menus [default = right btn or Ctrl-btn on the Mac].
  
  int getMouseAltButton() const {return mouse_alt_button;}
  /**< No of the alternate button [default = middle btn or Alt-btn on the Mac].
    * for pasting_text, DND and scroll_to_pointed_position in scrollbars.
    */ 
  
  // - - - misc options - - - - - - - - - - - - - - - - - - - - - - - - - - -  -

  bool 
    xsync, usync,
    ///< enables synchronous graphics rendering for X11 and Ubit [default: false].
    
    menu_grab,
    ///< enables mouse grabs when opening menus [default: true].
        
    postpone_delete,
    ///< delete operations of objects that derive from Node are postponed until the main loop returns [default: true].

    soft_dbf,
    ///< enables soft double buffering when X11 is used for rendering graphics [default: true].
  
    soft_wins, soft_menus,
    ///< windows/menus are rendered as "soft windows" (i.e. simulated)  [default: false].
    
    transp_scrollbars,
    ///< scrollbars are transparent and superimposed [default: false].
    
    tele_pointers,
    ///< telepointers are shown when the appli is displayed on several X servers.

    generic_textsel;
    ///< any object can select text [default: false].
        
  int click_radius;  // (pas unsigned, les dist ne le sont jamais)
  ///< UOn::mclick is not generated if the mouse if moved more than this distance.

  unsigned long multi_click_delay;   
  ///< max delay (in ms) between 2 mouse clicks to generate a multiple click.
  
  unsigned long auto_repeat_delay;
  ///< delay (in ms) before autoRepeat becomes active (see Element::setAutoRepeat()).

  unsigned long open_tip_delay;
  ///< delay (in ms) before a UTip is opened.

  unsigned long open_submenu_delay; 
  ///< delay (in ms) before a cascaded Menu is opened.

  // - - - option parsing  - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  virtual void parseOptions(int& argc, char** argv, Option* options);
  /**< retrieves options from the command line.
   * this function parses the command line (ie. argc, argv) to retrieve 
   * options that are specified in the 'options' argument which is a NULL
   * terminated table of Option elements: @see Option for details.
   * Note: parseOptions() is called by Application::Application() if its 'options'
   * argument is not null.
   */
  
  virtual void readAttributes(const char* pathname);
  /**< retrieves attributes from a configuration file.
   * 'pathname' specifies a file that has a CSS syntax which can start with
   * a ~ (eg. ~/.uremote). For instance, if this file exists and contains:
   * <pre>
   *   local {
   *     color: red;
   *     sizes: 8,10,12;
   *   }
   *   remote {
   *     color: blue;
   *     sizes: 18,20,24;
   *   }
   * </pre>
   * these attributes can be retrieved as follows in the program:
   * <pre>
   *   AttributeList* alist = Application::conf.getAttributes("local");
   *   if (alist) {
   *      String* color = alist->getAttr("color");
   *      String* sizes = alist->getAttr("sizes");
   *   }
   *   etc...
   *</pre>
   */
  
  AttributeList* getAttributes(const char* category);
  ///< see readAttributes().
  
private:
  friend class Display;
  friend class Application;
  friend class UAppliImpl;
  friend class UpdateContext;
  friend class EventFlow;
  friend class Element;
  friend class Box;
  friend class Window;
  friend class TextEdit;
  friend class Image;
  friend class UPix;
  friend class Graph;
  friend class Font;
  friend class FontDescription;
  friend class UHardFont;
  friend class FileChooser;
  friend class GLCanvas;
  friend class U3Dcanvas;
  virtual void parseUbitOptions(int& argc, char** argv);
  
protected:
  bool freeze_options;
  const char* const windowing_toolkit;
  bool is_using_gl, is_using_freetype;
  int bpp;
  int depth_size, stencil_size;
  int verbosity; // verbose mode.
  float scale;   // scaling factor for rendering the application [default = 1,0].
  int filebox_width, filebox_height, filebox_line_count;   // 400, 230, 10. !!!@@@ A VIRER !!!
  const int& mouse_select_button;  
  const int& mouse_menu_button; 
  const int& mouse_alt_button; 
  String locale;   // locale of the application [default = "" : the environment locale]
  String display;         // name of the X11 display [default = "": the local X server is used)
  Font* default_font;  // the default font of the application [default = Font::sans_serif].
  UPix* unknow_image;   // the image that appears instead of an unknown image (when applicable].
  Font*  selection_font;    // font of the text selection (can be null)
  Color* selection_color;   // text color of the text selection (can be null)
  Color* selection_bgcolor; // background color of the text selection (can be null)
  Background& default_background;  // the default background of the application.
};

}
#endif
