/*
 *  edit.cpp: Text Edition Attribute
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

#ifndef _uedit_hpp_
#define	_uedit_hpp_ 1

#include <ubit/uattr.hpp>

namespace ubit {
  
  /** Text editing and caret controller.
   * exemple: 
   * <pre>
   *   String& str = ustr("abcd");
   *   Box& box = ubox(uedit() + str);
   * </pre>
   * makes'str' editable inside container 'box'
   */
  class UEdit : public Attribute {
  public:
    UCLASS(UEdit)
    
    UEdit();
    ///< creates a new UEdit property; @see also shortcut uedit().
    
    virtual ~UEdit(); // NOTE: removingFrom() requires a destructor.
    
    
    bool isEditable() const;
    UEdit& setEditable(bool = true);
    ///< get/set text editing mode
    
    bool isCaretVisible() const;
    UEdit& setCaretVisible(bool = true);
    ///< get/set caret visibility.
    
    Color* getCaretColor() const;
    UEdit& setCaretColor(Color*);
    /**< get/set the color used to render the caret.
     * a value of null means that the caret has the same color as the text 
     */
    
    long getCaretPos() const;
    /**< returns the caret position.
     * NOTE that \n chars are not visible but count for 1 char!
     */
    
    UEdit& setCaretPos(long pos);
    /**< changes the caret position.
     * NOTE that \n chars are not visible but count for 1 char!
     */
    
    long getCaretPos(Element& container) const;
    UEdit& setCaretPos(long pos, Element& container);
    /**< get/sets the caret position of a shared UEdit.
     * when the UEdit is shared by several containers (a strange idea
     * actually) it is necessary to specify which container we are
     * talking about
     */
    
    
    String* getCaretStr() const;
    String* getCaretStr(int& pos_in_str) const;
    /**< returns the String that contains the caret and the position of the caret in this string.
     * details:
     * - returns null if the caret is not set
     * - 'pos_in_str' is relative to the string that contains the caret
     * - Note that a container (Box or subclass) can contain several String
     *   and that a UEdit may (rarely) be included in several containers.
     */
    
    UEdit& setCaretStr(String* str);
    UEdit& setCaretStr(String* str, int pos_in_str);
    /**< sets the String that contains the caret and the position of the caret in this string.
     * details:
     * - 'pos_in_str' is relative to 'str', the string that will contain 
     *   the caret. 'pos_in_str' = 0 corresponds to the beginning of the 
     *   string; 'pos_in_str' = -1 to the end of the string.
     * - the 'str' must (obvioulsy) be in the same container as the UEdit
     * - Note that a container (Box or subclass) can contain several String.
     *   and that a UEdit may be included in several containers.
     */
    
    static String* getPreviousStr(String* from, Element& container);
    static String* getNextStr(String* from, Element& container);
    /**< [unstable] returns the string before/after 'from' in this 'container'.
     * returns null if no previous/next string.
     * [unstable] this fct is not defined at the right place!
     */
    
    static String* getParagraphFirstStr(String* from, int& pos, Element& container);
    static String* getParagraphLastStr(String* from, int& pos, Element& container);
    /**< [unstable] returns the first/last string in the same paragraph than 'from'.
     * returns the beginning/ending string contained in the same line 
     * than 'from' in this parent.
     * returns 'from' itself if it is the beginning/ending string.
     * [unstable] this fct is not defined at the right place!
     */
    
    // - - - impl - - - - -- - - - - - - - - - - - - - - - - - - - - - - -
    
    Selection* getSelection(UInputEvent& e, String& sel_text);
    void deleteSelection(Selection* sel, String& sel_text, Element& container);
    void deletePreviousChar(Element& container, View* view);
    void deleteChar(Element& container, View* view);
    bool nextChar(Element& container, View* view);
    bool previousChar(Element& container, View* view);
    
    void beginningOfLine(Element& container, View* view);
    void endOfLine(Element& container, View* view);
    void previousLine(Element& container, View* view);
    void nextLine(Element& container, View* view);
    ///< [impl] moves the caret in the expected way.
    
    virtual void update();
    
#ifndef NO_DOC
    virtual void putProp(UpdateContext*, Element&);
    virtual void addingTo(Child&, Element& parent);
    virtual void removingFrom(Child&, Element& parent);
    // NOTE: removingFrom() requires a destructor.
    
    void setCaretStr(String*, int pos_in_str, bool update, View* view);
    
    // prototypes for non-warped text (View)
    virtual float getXpos(UpdateContext&, const Rectangle& r) const;
    virtual void  paint(Graph&, UpdateContext&, const Rectangle &r) const;
    
    // prototypes for warped text (UFlowview)
    virtual float getXpos(UpdateContext&, const Rectangle&, int offset, int cellen) const;
    virtual void  paint(Graph&, UpdateContext&, const Rectangle&, int offset, int cellen) const;
    
  private:
    uptr<UCall>  calls;        // callback object
    uptr<UCall>  calls2;        // callback object
    uptr<Color> caret_color;  // specific color for displaying the caret (if any)
    uptr<String>   caret_str;    // the string that contains the caret (if any)
    long caret_pos;             // the position of the caret in 'caret_str'
    bool is_editable, is_visible;
    mutable bool repainted;
    virtual void inputCB(UInputEvent&);
    virtual void kpressed(UKeyEvent&);
    virtual void mpressed(UMouseEvent&);
    virtual void mreleased(UMouseEvent&);
    virtual void callbacks2(UMouseEvent&);
#endif
  };
  
  inline UEdit& uedit() {return *new UEdit();}
  ///< creator shortcut that returns *new UEdit().
  
}
#endif
