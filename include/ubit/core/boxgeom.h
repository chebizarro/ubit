/*
 *  boxgeom.h: attributes for controlling Box geometry
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

#ifndef uboxgeom_hpp
#define	uboxgeom_hpp 1

#include <ubit/uattr.hpp>
#include <ubit/ulength.hpp>
#include <ubit/ugeom.hpp>

namespace ubit {
  
  /** Widget scale.
   * The Scale attribute specifies the scaling factor of widgets (Box subclasses)
   * and their direct and indirect children in the instance graph. 
   *
   * If a child widget has also a Scale attribute, this scale spec. is multiplied
   * with the scale spec. of the parent (and so on, recursively)
   */
  class Scale : public Attribute {
  public:
    UCLASS(Scale)
    
    Scale(float value = 1.);
    ///< creates a new scale attribute; see also shortcut uscale().
    
    Scale(const Float&);
    ///< creates a new scale attribute; see also shortcut uscale().
    
    operator float() const {return value;}
    ///< type conversion: returns scale value.
    
    Scale& operator=(float v)         {set(v); return *this;}
    Scale& operator=(const Float& v) {set(v); return *this;}
    Scale& operator=(const Scale& v) {set(v.value); return *this;}
    
    bool operator==(float v) const         {return v == value;}
    bool operator==(const Float& v) const {return v == value;}
    bool operator==(const Scale& v) const {return value == v.value;}
    
    virtual void set(float);
    virtual void mult(float);
    virtual void div(float);
    virtual void update();
    
  private:
    float value;
    virtual void putProp(UpdateContext*, Element&);
  };
  
  inline Scale& uscale(float v = 1.)    {return *new Scale(v);}
  ///< shortcut that creates a new Scale.
  
  inline Scale& uscale(const Float& v) {return *new Scale(v);}
  ///< shortcut that creates a new Scale.
  
  
  // ==================================================== Ubit Toolkit =========  
  /** Widget position.
   * The UPos Attribute specifies the position of a widget (Box subclasses) relatively
   * to its parent widget (note that, currently, UPos don't work with Window widgets)
   *
   * Most widgets do not need to have a UPos attribute: their position is then 
   * calculated automatically by their parent widgets (see Halign, Valign).
   * 
   * Depending of its x and y Length parameters, a UPos attribute works as follows:
   * - it is ignored if either 'x' or 'y' is set to UAUTO or UIGNORE
   * - the widget is floating otherwise and 'x' and 'y' are relative to its parent origin
   *
   * 'x' and 'y' units can either be:
   * - UPX (pixel), UIN (inch), UCM (centimeter), UMM (millimeter), UPT (point), UPC (pica)
   * - UEM (current font size), UEX (x-height of a font)
   * - UPERCENT (percentage of the parent size in range [0f,100f])
   * - UPERCENT_CTR (same as UPERCENT except that x,y then correspond to the widget center)
   *
   * Exemple: 
   * - UPos(100|UPX, 25|UPERCENT) specifies a width of 100 pixels and a height equals
   *   to 25% of the parent's height
   */
  class UPos : public Attribute, public PosSpec {
  public:
    UCLASS(UPos)

    static const Length::Modes TOP, LEFT, RIGHT, BOTTOM;
      
    UPos() : PosSpec(0,0) {}
    ///< creates a new position attribute with x=0 and y=0.
      
    UPos(Length x, Length y) : PosSpec(x, y) {}
    ///< creates a new position attribute; see also shortcut upos(Length x, Length y).
           
    UPos(const Point& p) : PosSpec(p.x, p.y) {}
    ///< creates a new position attribute; see also shortcut upos().
    
    UPos(const UPos& p) : PosSpec(p.x, p.y) {}
    ///< creates a new position attribute.
    
    //UPos(const String& xSpec, const String& ySpec);
    //creates a new position attribute.

    virtual ~UPos() {destructs();}
      
    const Length& getX() const {return x;}
    const Length& getY() const {return y;}

    UPos& setX(const Length& _x) {return set(_x, y);}
    UPos& setY(const Length& _y) {return set(x, _y);}
    
    virtual UPos& set(const Length& x, const Length& y);
    UPos& operator=(const UPos& p)   {return set(p.x, p.y);}
    UPos& operator=(const Point& p) {return set(p.x, p.y);}

    virtual bool equals(const Length& x, const Length& y) const;
    bool operator==(const UPos& p) const {return equals(p.x, p.y);}
    bool operator==(const Point& p) const {return equals(p.x, p.y);}

    bool isFloating() const;
    /**< returns true if this UPos makes the widget to be floating.
     * this UPos is ignored, and thus, the widget nt floating, if either 'x' or 'y'
     * has been set to UAUTO or UIGNORE
     */
    
    virtual bool is3Dpos() const {return false;}
    ///< returns true if this is a U3Dpos.

    virtual void update();

   private:
    friend class View;
    friend class Box;
    friend class Window;
    virtual void putProp(UpdateContext*, Element&);
    virtual void addingTo(Child&, Element& parent);
    virtual void removingFrom(Child&, Element& parent);
    //NB: removingFrom() requires UPos to have a destructor.    
  };
    
  inline UPos& upos(const Length& x, const Length& y) {return *new UPos(x, y);}
  ///< shortcut that creates a new UPos.
  
  inline UPos& upos(const Point& p) {return *new UPos(p);}
  ///< shortcut that creates a new UPos.

  
  /** Position Control: interactive control of the position of a widget.
   * when dragged interactivelly, a handle widget (typically a button) having  
   * this attribute changes the UPos of another widget (typically a palette) 
   * that contains this handle.
   */
  class UPosControl : public Attribute {
  public:
    UCLASS(UPosControl)
    
    UPosControl(UPos* = null);
    ~UPosControl();
    
    UPosControl& setModel(UPos*);
    UPosControl& changeCursor(bool state);
    UPosControl& freezeX(bool state);
    UPosControl& freezeY(bool state);
    
    // - - - impl - - - - -- - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
  protected:
    static const int MARGIN = 5;
    Point box_pos0, pt_in_pane0;
    bool change_cursor, freeze_x, freeze_y;
    View* moved_view;
    uptr<UCall> callbacks;
    uptr<UPos> posAttr;
    
    virtual void putProp(UpdateContext*, Element&) {}
    virtual void addingTo(Child&, Element& parent);
    virtual void removingFrom(Child&, Element& parent);
    // NOTE that removingFrom() requires a destructor to be defined.
    
    virtual void mouseCB(MouseEvent&);
    virtual void pressCB(MouseEvent&);
    virtual void releaseCB(MouseEvent&);
    virtual void dragCB(MouseEvent&);
#endif
  };
  
  /** Makes it possible to change the UPos of a Magic Lens interactively.
   * PBM: aucun effet si on modifie pos directement par programme !!
   */
  class UMagicLensControl : public UPosControl {
  public:
    UCLASS(UMagicLensControl)
    UMagicLensControl(UPos* = null, Scrollpane* = null);
    UMagicLensControl& setModel(UPos*, Scrollpane*);
  private:
    uptr<Scrollpane> pane;
    virtual void dragCB(MouseEvent&);
  };
  
  
  /** Widget Size.
   * The USize attribute specifies the size of a widget (a node deriving from Box).
   *
   * USize specifies the preferred size of the widget and its behavior. The actual
   * widget size may depend on other factors such as its scale (see Scale) and 
   * layout constraints (see uhflex(), uvflex())
   *
   * The type of the width and the height of the USize is Length, which makes it
   * to specify a unit (length are in pixels if no unit is specified)
   *
   * A length of -1 means that the size of the initial content of the widget
   * should be used to compute the widget size (in this direction)
   *
   * UNRESIZABLE prevents uhflex() and uvflex() attributes to resize the widget
   *
   * Note: A USize object can be shared by several widgets. 
   */
  class USize : public Attribute, public SizeSpec {
  public:
    UCLASS(USize)
    
    static const Length INITIAL;
    static const Length::Modes UNRESIZABLE;
    
    USize() : SizeSpec(UIGNORE,UIGNORE) {}
    ///< creates a new size attribute with width=UAUTO and height=UAUTO.
    
    USize(Length width, Length height = UIGNORE) : SizeSpec(width, height) {}
    ///< creates a new size attribute, @see also the usize() shortcut.
    
    USize(const Dimension& d) : SizeSpec(d.width, d.height) {}
    ///< creates a new size attribute.

    USize(const USize& s) : SizeSpec(s) {}
    ///< creates a new size attribute.
    
    //UPos(const String& widthSpec, const String& heightSpec);
    //creates a new position attribute.

    const Length& getWidth()  const {return width;}
    const Length& getHeight() const {return height;}
    
    USize& setWidth(const Length& _w)  {return set(_w, height);}
    USize& setHeight(const Length& _h) {return set(width, _h);}
    
    virtual USize& set(const Length& width, const Length& height);
    USize& operator=(const USize& s) {return set(s.width, s.height);}
    USize& operator=(const Dimension& d) {return set(d.width, d.height);}
    
    virtual bool equals(const Length& w, const Length& h) const;
    bool operator==(const USize& s) const {return equals(s.width, s.height);}
    bool operator==(const Dimension& d) const {return equals(d.width, d.height);}

    virtual void update();
    
  private:
    virtual void putProp(UpdateContext*, Element&);  
    friend class UViewLayoutImpl;
  };
  
  inline USize& usize() {return *new USize();}
  ///< shortcut function that returns *new USize().

  inline USize& usize(const Length& w, const Length& h =UIGNORE) {return *new USize(w,h);}
  ///< shortcut function that returns *new USize(w, h).
  
  inline USize& usize(const Dimension& d) {return *new USize(d);}
  ///< shortcut function that returns *new USize(d).
  
  
  /** Size Control: interactive control of the size of a widget.
   * when dragged interactivelly, a handle widget (typically a button) having  
   * this attribute changes the USize of another widget (typically a palette) 
   * that contains this handle.
   */
  class USizeControl : public Attribute {
  public:
    UCLASS(USizeControl)
    
    USizeControl(USize* = null);    
    ~USizeControl();
    
    USizeControl& setModel(USize*);
    USizeControl& freezeWidth(bool state);
    USizeControl& freezeHeight(bool state);
    
    // - - - impl - - - - -- - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
    virtual void putProp(UpdateContext*, Element&) {}
    virtual void addingTo(Child&, Element& parent);
    virtual void removingFrom(Child&, Element& parent);
    // NOTE that removingFrom() requires a destructor to be defined.
  protected:
    float w0, h0;
    float xev0, yev0;
    bool freeze_w, freeze_h;
    uptr<UCall> callbacks;
    uptr<USize> psize;
    virtual void mouseCB(MouseEvent&);
    virtual void pressCB(MouseEvent&);
    virtual void releaseCB(MouseEvent&);
    virtual void dragCB(MouseEvent&);
#endif
  };
  
  
  /** Widget padding.
   */
  class UPadding : public Attribute {
  public:
    UCLASS(UPadding)
    
    UPadding() : val(UIGNORE,UIGNORE) {}
    UPadding(Length width, Length height) : val(width, height) {}
        
    UPadding& operator=(const UPadding&);
    
    UPadding& setWidth(Length);
    ///< sets top and bottom padding.
    
    UPadding& setHeight(Length);
    ///< sets left and right padding.

    UPadding& setTop(Length);
    UPadding& setRight(Length);
    UPadding& setBottom(Length);
    UPadding& setLeft(Length);
    
    Length getTop()   const {return val.top;}
    Length getRight() const {return val.right;}
    Length getBottom()const {return val.bottom;}
    Length getLeft()  const {return val.left;}
    
    virtual void update();
  private:
    PaddingSpec val;
    virtual void putProp(UpdateContext*, Element&);
  };
  
  
  inline UPadding& upadding() {return *new UPadding();}
  ///< shortcut function that returns *new UPadding().
  
  inline UPadding& upadding(Length horiz, Length vert) {return *new UPadding(horiz,vert);}
  ///< shortcut function that returns *new UPadding(horiz,vert).

  
  /** Widget Orientation.
   * specifies the orientation of box(es) that contain this brick.
   * Possible values: Orientation::vertical or Orientation::horizontal
   */
  class Orientation : public Attribute {
  public:
    UCLASS(Orientation)
    
    static Orientation horizontal, vertical;
    
    Orientation();
    ///< creates a new Orientation object; @see also shortcut Orientation().
    
    Orientation(const Orientation&);
    ///< creates a new Orientation object; @see also shortcut Orientation().
    
    Orientation& operator=(const Orientation& v);
    bool operator==(const Orientation& v) const {return value == v.value;}
    
    virtual void update();
    virtual void putProp(UpdateContext*, Element&);
    
    // - impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
    enum {HORIZONTAL=0, VERTICAL=1, INHERIT=2}; //dont change values
    void addingTo(Child&, Element& parent);
    void removingFrom(Child&, Element& parent);
  private:
    char value;
    Orientation(char value, UConst);
#endif
  };
  
  inline Orientation& Orientation(const Orientation& val) {return *new Orientation(val);}
  ///< shortcut function that creates a new Orientation object.
  
  
  /** Box horizontal layout.
   * specify the horizontal layout of the children that *follow* this brick.
   * Possible values: Halign::left, ::right, ::center, ::flex.
   * Shortcut functions uleft(), uright(), uhcenter(), uhflex() can be used
   * to improve source code readability.
   *
   * Example:
   * <pre>
   *    uhbox( uleft() + ch1 + ch2 + uvflex() + ch3 + ch4 + uright() + ch5 )
   * </pre>
   * uleft() specifies the layout of ch1 and ch2, uhflex() the layout of
   * ch3 and ch4 and uright() the layout of ch5.
   */
  class Halign : public Attribute {
  public:
    UCLASS(Halign)

    static Halign left, right, flex, center;
    
    Halign();
    ///< creates a new Halign; @see also shortcuts: uhalign(), uleft(), uright(), uhcenter(), uhflex(). 
    
    Halign(const Halign&);
    ///< creates a new Halign; @see also shortcuts: uhalign(), uleft(), uright(), uhcenter(), uhflex(). 
    
    Halign& operator=(const Halign&);
    bool operator==(const Halign& v) const {return value == v.value;}
    
    virtual void update();
    virtual void putProp(UpdateContext*, Element&);
    
    // - impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    enum {INHERIT, LEFT, RIGHT, FLEX, CENTER};
  private:
    char value;
    Halign(char value, UConst);
  };
  
  
  inline Halign& uhalign(const Halign& val) {return *new Halign(val);}
  ///< shortcut function that creates a new Halign object.
  
  inline Halign& uleft() {return Halign::left;}
  /**< child widgets following uleft() are laid out sequentially from the left side of their parent.
   * uleft have no effect on children width (in contrast with uhflex()).
   * uleft and uright have the same effect if uhflex() appears in the same child list
   * @see Halign.
   */
  
  inline Halign& uright() {return Halign::right;}
  /**< child widgets following uright() are laid out sequentially so that the last child reaches the right side of their parent.
   * uright have no effect on children width (in contrast with uhflex()).
   * uleft and uright have the same effect if uhflex() appears in the same child list
   * @see Halign.
   */
  
  inline Halign& uhcenter() {return Halign::center;}
  /**< child widgets following uhcenter() are horizontally centered in their parent.
   * uhcenter has no effect on children width (in contrast with uhflex()).
   * uhcenter has the same effect as uleft if uhflex() appears in the same child list
   * @see Halign.
   */
  
  inline Halign& uhflex() {return Halign::flex;}
  /**< child widgets following uhflex() have a "flexible" horizontal layout.
   * these children will occupy the remaining space and will be resized
   * (in the horizontal direction) when their parents are resized.
   * These rules dont apply for children that are NOT resizable.
   * @see Halign.
   */
  
  
  /** Box vertical layout.
   * specify the vertical layout of the children that follow this brick.
   * Possible values: Halign::top, ::bottom, ::center, ::flex.
   * Shortcut functions utop(), ubottom(), uvcenter(), uvflex() can be used
   * to improve readability (they return the corresponding value).
   *
   * Behaviors: same behaviors as Halign (but in the vertical direction).
   */
  class Valign : public Attribute {
  public:
    UCLASS(Valign)

    static Valign top, bottom, flex, center;
    
    Valign();
    ///< creates a new Valign; @see also shortcuts: uvalign(), utop(), ubottom(), uvcenter(), uvflex(). 
    
    Valign(const Valign&);
    ///< creates a new Valign; @see also shortcuts: uvalign(), utop(), ubottom(), uvcenter(), uvflex(). 
    
    Valign& operator=(const Valign& v);
    bool operator==(const Valign& v) const {return value == v.value;}
    
    virtual void update();
    virtual void putProp(UpdateContext*, Element&);
    
    // - impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    enum {INHERIT, TOP, BOTTOM, FLEX, CENTER};
  private:
    char value;
    Valign(char value, UConst);
  };
  
  inline Valign& uvalign(const Valign& val) {return *new Valign(val);}
  ///< shortcut function that creates a new Valign object.
  
  inline Valign& utop() {return Valign::top;}
  /**< child widgets following utop() are laid out sequentially from the top of their parent.
   * uleft have no effect on children width (in contrast with uhflex()).
   * uleft and uright have the same effect if uhflex() appears in the same child list
   * @see Valign.
   */
  
  inline Valign& ubottom() {return Valign::bottom;}
  /**< child widgets following ubottom() are laid out sequentially so that the last child reaches the bottom of their parent.
   * uright have no effect on children width (in contrast with uhflex()).
   * uleft and uright have the same effect if uhflex() appears in the same child list
   * @see Valign.
   */
  
  inline Valign& uvcenter() {return Valign::center;}
  /**< child widgets following uvcenter() are vertically centered in their parent.
   * uhcenter has no effect on children width (in contrast with uhflex()).
   * uhcenter has the same effect as uleft if uhflex() appears in the same child list
   * @see Valign.
   */
  
  inline Valign& uvflex() {return Valign::flex;}
  /**< child widgets following uvflex() have a "flexible" vertical layout.
   * these children will occupy the remaining space and will be resized
   * (in the horizontal direction) when their parents are resized.
   * These rules dont apply for children that are NOT resizable.
   * @see Valign.
   */
  
  
  /** Box vertical spacing.
   */
  class VSpacing : public Attribute {
  public:
    UCLASS(VSpacing)

    static VSpacing none;
    
    VSpacing(float = 0);
    
    operator float() const {return value;}
    ///< type conversion: returns value.
    
    VSpacing& operator=(float);
    VSpacing& operator=(const VSpacing&);
    
    bool operator==(float v) const {return value == v;}  
    bool operator==(const VSpacing& v) const {return value == v.value;}
    
    virtual void update();
    
    // - impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    enum {INHERIT = -1};
    virtual void putProp(UpdateContext*, Element&);
  private:
    float value;
  };
  
  inline VSpacing& uvspacing(float val = 0) {return *new VSpacing(val);}
  ///< shortcut function that returns a new VSpacing.
  
  /** Box horizontal spacing.
  */
  class HSpacing : public Attribute {
  public:
    UCLASS(HSpacing)
    
    static HSpacing none;
    
    HSpacing(float = 0);
    
    operator float() const {return value;}
    ///< type conversion: returns value.
    
    HSpacing& operator=(float);
    HSpacing& operator=(const HSpacing&);
    
    bool operator==(float v) const {return value == v;}  
    bool operator==(const HSpacing& v) const {return value == v.value;}
    
    virtual void update();
    
    // - impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    enum {INHERIT = -1};
    virtual void putProp(UpdateContext*, Element&);
  private:
    float value;
  };
  
  inline HSpacing& uhspacing(float val) {return *new HSpacing(val);}
  ///< shortcut function that returns a new HSpacing.
  
}  
#endif
