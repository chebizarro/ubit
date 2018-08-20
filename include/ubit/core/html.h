/*
 *  html.h: HTML parser and renderer
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

#ifndef UBIT_CORE_HTML_H_
#define UBIT_CORE_HTML_H_

#include <memory>

#include <ubit/udefs.hpp>
#include <ubit/core/xmlgrammar.h>
#include <ubit/core/xmlparser.h>

namespace ubit {
  
  /** HTML Grammar.
   */ 
  class HtmlGrammar : public XmlGrammar {
  public:
    HtmlGrammar();
  };

  /** HTML Parser.
   * this parser should be used instead of XmlParser for parsing HTML text.
   * It automatically:
   * - adds the HtmlGrammar grammar to the grammar list 
   * - sets the parser in Permissive and CollapseSpaces modes.
   * @see class XmlParser for details.
   */ 
  class HtmlParser : public XmlParser {
  public:
    HtmlParser();
  };
  
  /** HTML Attribute.
   */ 
  struct HtmlAttribute : public Attribute {
    UCLASSDEF("uhtmlattribute", HtmlAttribute, null)
    
    virtual void setValue(const String& val) {setValueImpl(pvalue, val);}
    virtual bool getValue(String& val) const {return getValueImpl(pvalue, val);}
    
    static void setValueImpl(std::unique_ptr<String>& pvalue, const String& value);
    static bool getValueImpl(const std::unique_ptr<String>& pvalue, String& value);

    std::unique_ptr<String> pvalue;
  };
  

  struct HtmlStyle : public AttributeList {
    UCLASSDEF("style", HtmlStyle, new HtmlStyle)
    void initNode(Document* doc, Element* parent);    
    void setValue(const String& val) {HtmlAttribute::setValueImpl(pvalue, val);}
    bool getValue(String& val) const {return HtmlAttribute::getValueImpl(pvalue, val);}
  private:
    std::unique_ptr<String> pvalue;  
  };
  
  struct HtmlClass : public HtmlAttribute {
    UCLASSDEF("class", HtmlClass, new HtmlClass)
    void initNode(Document* doc, Element* parent);
  };

  struct HtmlId : public HtmlAttribute {
    UCLASSDEF("id", HtmlId, new HtmlId)
    void initNode(Document* doc, Element* parent);
  };
  
  struct HtmlRel : public HtmlAttribute {
    UCLASSDEF("rel", HtmlRel, new HtmlRel)
  };
  
  struct HtmlType : public HtmlAttribute {
    UCLASSDEF("type", HtmlType, new HtmlType)
  };
  
  struct HtmlAlt : public HtmlAttribute {
    UCLASSDEF("alt", HtmlAlt, new HtmlAlt)
  };
  
  struct HtmlName : public HtmlAttribute {
    UCLASSDEF("name", HtmlName, new HtmlName)
  };
  
  struct HtmlSrc : public HtmlAttribute {
    UCLASSDEF("src", HtmlSrc, new HtmlSrc)
  };
  
  struct HtmlHref : public HtmlAttribute {
    UCLASSDEF("href", HtmlHref, new HtmlHref)
  };
  
  struct HtmlColor : public Color {
    UCLASSDEF("color", HtmlColor, new HtmlColor)
    void setValue(const String& value);    
    bool getValue(String& val) const {return HtmlAttribute::getValueImpl(pvalue, val);}
  private:
    std::unique_ptr<String> pvalue;
  };
  
  struct HtmlBgcolor : public Background {
    UCLASSDEF("bgcolor", HtmlBgcolor, new HtmlBgcolor)
    void setValue(const String&);
    bool getValue(String& val) const {return HtmlAttribute::getValueImpl(pvalue, val);}
  private:
    std::unique_ptr<String> pvalue;
  }; 
  
  struct HtmlBorder : public Border {
    UCLASSDEF("border", HtmlBorder, new HtmlBorder)
    void setValue(const String& v);
    bool getValue(String& val) const {return HtmlAttribute::getValueImpl(pvalue, val);}
  private:
    std::unique_ptr<String> pvalue;
  };
  
  struct HtmlFontFace : public Font {
    UCLASSDEF("face", HtmlFontFace, new HtmlFontFace)
    void setValue(const String&);    
    bool getValue(String& val) const {return HtmlAttribute::getValueImpl(pvalue, val);}
  private:
    std::unique_ptr<String> pvalue;
  };
  
  struct HtmlFontSize : public Font {
    UCLASSDEF("size", HtmlFontSize, new HtmlFontSize)    
    void setValue(const String&);
    bool getValue(String& val) const {return HtmlAttribute::getValueImpl(pvalue, val);}
  private:
    std::unique_ptr<String> pvalue;
  };
  
  
  struct HtmlWidth : public HtmlAttribute {
    UCLASSDEF("width", HtmlWidth, new HtmlWidth)
    //void setValue(const String&);
    //bool getValue(String& val) const {return HtmlAttribute::getValueImpl(pvalue,val);}
  private:
    void initNode(Document*, Element* parent);
  };
  
  struct HtmlHeight : public HtmlAttribute {
    UCLASSDEF("height", HtmlHeight, new HtmlHeight)
    //void setValue(const String&);
    //bool getValue(String& val) const {return HtmlAttribute::getValueImpl(pvalue,val);}
  private:
    void initNode(Document*, Element* parent);
  };
  
  struct HtmlAlign : public HAlign {
    UCLASSDEF("align", HtmlAlign, new HtmlAlign)
    void setValue(const String&);
    bool getValue(String& val) const {return HtmlAttribute::getValueImpl(pvalue, val);}
  private:    
    std::unique_ptr<String> pvalue;
  };
  
  struct HtmlValign : public VAlign {
    UCLASSDEF("valign", HtmlValign, new HtmlValign)
    void setValue(const String&);
    bool getValue(String& val) const {return HtmlAttribute::getValueImpl(pvalue, val);}
  private:    
    std::unique_ptr<String> pvalue;
  };

  struct HtmlColspan : public HtmlAttribute {
    UCLASSDEF("colspan", HtmlColspan, new HtmlColspan)
    void initNode(Document* doc, Element* parent);
    void setValue(const String&);
  };
  
  struct HtmlRowspan : public HtmlAttribute {
    UCLASSDEF("rowspan", HtmlRowspan, new HtmlRowspan)
    void initNode(Document* doc, Element* parent);
    void setValue(const String&);
  };
    
  struct HtmlCellspacing : public HtmlAttribute {
    UCLASSDEF("cellspacing", HtmlCellspacing, new HtmlCellspacing)
    void setValue(const String&);
  };
    
  struct HtmlCellpadding : public HtmlAttribute {
    UCLASSDEF("cellpadding", HtmlCellpadding, new HtmlCellpadding)
    void setValue(const String&);
  };  
    
  struct HtmlNowrap : public HtmlAttribute {
    UCLASSDEF("nowrap", HtmlNowrap, new HtmlNowrap)
    void setValue(const String&);
  };
  
}
#endif // UBIT_CORE_HTML_H_




