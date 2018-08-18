/*
 *  styleparser.hpp
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

#ifndef UBIT_DRAW_STYLEPARSER_H_
#define UBIT_DRAW_STYLEPARSER_H_

#include <vector> 
#include <fstream>
#include <ubit/core/node.h>
#include <ubit/ustr.hpp>

namespace ubit {
  
  typedef char Char;
  
  /** Style Parser.
   * Abstract class: methods addStyle() and addProp() must be
   * implemented by subclasses.
   */
  class StyleParser {
  public:
    
    struct StyleMaker {
      std::vector<String*> selectors;
      unsigned int count;
      
      StyleMaker();
      virtual ~StyleMaker();
      virtual void begin() {}
      virtual void create() {}
      virtual void addProp(const String& name, const String& value) {}
      virtual void end(bool ok) {}
    };
    
        
    StyleParser();
    virtual ~StyleParser();
    
    virtual int parseImpl(StyleMaker&, const String& buffer);
    ///< parses the buffer by using this StyleMaker.
    
    int getStatus() {return stat;}
    ///< returns the parsing status.
    
    UErrorHandler* getErrorHandler() {return perrhandler;}
    ///< returns the current error handler.
    
    void setErrorHandler(UErrorHandler* eh) {perrhandler = eh;}
    ///< changes the error handler (Application default handler used if argument is null).
    
  protected:
    bool readStyleDef(StyleMaker&);
    ///< reads one style definition (its selectors + its properties).
    
    bool readStyleSelectors(StyleMaker&);
    ///< reads the selector(s) of the style definition.
    
    bool readStyleProps(StyleMaker&);
    ///< reads the properties of the style definition (the {} part after the selector).
    
    void skipSpaces();
    bool readName(String& name);
    bool readValue(String& value);
    bool readNameValuePair(String& name, String& value);
    
    virtual void error(const char* msg, const Char* line);
    virtual void error(const char* msg_start, const String& name,
                       const char* msg_end, const Char* line);
    virtual void unexpected(const char* msg, const Char* line);
    
    bool permissive;
    int stat;
    const Char *text_buffer, *p;
    unique_ptr<UErrorHandler> perrhandler;
  };
  
}
#endif // UBIT_DRAW_STYLEPARSER_H_

