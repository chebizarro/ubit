/*
 *  css.hpp
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

#ifndef UBIT_DRAW_CSS_H_
#define UBIT_DRAW_CSS_H_

#include <map>
#include <ubit/udefs.hpp>
#include <ubit/ustyle.hpp>
#include <ubit/ustyleparser.hpp>
#include <ubit/udom.hpp>

namespace ubit {
  
  /** [impl] CSS parser for XML/HTML documents.
   * this stylesheet parser is used implicitely when reading XmlDocument objects.
   */
  class CssParser : public StyleParser {
  public:
    int parse(const String& buffer, class XmlDocument*);
    int parseAttr(const String& buffer, class XmlDocument*, AttributeList*);
  };
  
  
  /** [impl] precompiled style properties.
   * base for CssProperties.
   */
  class StyleProperties {
  public:
    typedef void (*AddPropFunc)(XmlDocument*, AttributeList*, const String&);
    ~StyleProperties();
    
    AddPropFunc findAddPropFunc(const String& prop_name);
    void defProp(const char* prop_name, AddPropFunc);
    void defProp(const String& prop_name, AddPropFunc);
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // useful fcts.
    
    static bool parseUrl(const String&, String& url, String& remain);
     
  private:
    struct Comp {
      bool operator()(const String*s1, const String*s2) const {return s1->compare(*s2)<0;}
    };
    typedef std::map<const String*, AddPropFunc, Comp> PropMap;
    PropMap prop_map;
  };
  
    /** [impl] precompiled CSS properties.
   * to be completed...
   */
  class CssProperties : public StyleProperties {
  public:
    CssProperties();
    
    static void create_font_family(XmlDocument*, AttributeList*, const String& v);
    static void create_font_size(XmlDocument*, AttributeList*, const String& v);
    static void create_font_weight(XmlDocument*,AttributeList*, const String& v);
    static void create_font_style(XmlDocument*, AttributeList*, const String& v);
    
    static void create_color(XmlDocument*, AttributeList*, const String& v);
    static void create_background_color(XmlDocument*, AttributeList*, const String& v);
    static void create_background_image(XmlDocument*, AttributeList*, const String& v);
    static void create_background(XmlDocument*, AttributeList*, const String& v);
    
    static void create_width(XmlDocument*, AttributeList*, const String& v);
    static void create_height(XmlDocument*, AttributeList*, const String& v);
    static void create_text_align(XmlDocument*, AttributeList*, const String& v);
    static void create_vertical_align(XmlDocument*, AttributeList*, const String& v);

    static void create_padding_top(XmlDocument*, AttributeList*, const String& v);
    static void create_padding_bottom(XmlDocument*, AttributeList*, const String& v);
    static void create_padding_left(XmlDocument*, AttributeList*, const String& v);
    static void create_padding_right(XmlDocument*, AttributeList*, const String& v);
    
    static void create_border(XmlDocument*, AttributeList*, const String& v);
  };
  
  /**
   * [impl] precompiled HTML Elements Styles.
   * to be completed...
   */
  class UCssStyles {
  public:
    static UStyle* create_body_style();
    static UStyle* create_div_style();
    static UStyle* create_p_style();
    
    static UStyle* create_ul_style();
    static UStyle* create_ol_style();
    static UStyle* create_li_style();
    
    static UStyle* create_pre_style();
    static UStyle* create_blockquote_style();
    static UStyle* create_center_style();
    
    static UStyle* create_h1_style();
    static UStyle* create_h2_style();
    static UStyle* create_h3_style();
    static UStyle* create_h4_style();
    static UStyle* create_h5_style();
    static UStyle* create_h6_style();
    
    static UStyle* create_table_style();
    static UStyle* create_tr_style();
    static UStyle* create_td_style();
    static UStyle* create_th_style();
    
    static UStyle* create_span_style();
    static UStyle* create_b_style();
    static UStyle* create_i_style();
    static UStyle* create_em_style();
    static UStyle* create_u_style();
    static UStyle* create_font_style();
    
    static UStyle* create_a_style();
    static UStyle* create_img_style();
    static UStyle* create_br_style();
  };
  
}
#endif // UBIT_DRAW_CSS_H_


