/*
 *  interactors.h: most common interactors
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

#ifndef _uinteractors_hpp_
#define	_uinteractors_hpp_ 1

#include <ubit/ustr.hpp>
#include <ubit/uedit.hpp>
#include <ubit/ubox.hpp>

namespace ubit {
  
  /** Separator gadget: horizontal or vertical separator for menus and boxes.
   */
  class USepar: public Box {
  public:
    UCLASS(USepar)
    static UStyle* createStyle();
    
    USepar();
    /**< create a new separator.
     * the orientation of the separator depends on the orientation of its parent box
     */
  };
  
  
  inline USepar& usepar() {return *new USepar();}
  ///< create a new separator.

  
  /** Label widget: can display any combination of text, images, and other widgets.
   *
   * Default look and feel (can be changed by adding appropriate Attribute(s)):
   * - horizontal layout (see attribute UOrient)
   * - font and foreground color inherited from parents (see Font and Color)
   * - transparent backgound (see UBackgound and UAlpha)
   * - automatically resized when children (text, images, and other widgets) are 
   *   changed or resized. Add a USize attribute to change this behavior or to 
   *   impose a given size. 
   * - Note that geometry also depend on parents, and, more specifically, on their
   *   uhflex()/uvflex() attributes.
   *
   * As other widgets, a label can contain any combination of text, images and  
   * other widgets and its look can be changed by adding appropriate UAttrs
   * (for specifying a specific Background, UAlpha, UPadding, Border, etc.)
   *   
   *  Example:
   *  <pre>  
   *     String& s = "ON";
   *     ULabel& mylabel = ulabel(uima("myimage.jpg") + "Current state: " + s)
   *  </prep>
   *  Creates a label that contains an image followed by "Current state: " and the
   *  content of string 's'. The label will be updated whenever 's' content is changed.
   *
   *  ulabel(...) is a shortcut for *new ULabel(...)
   */
  class ULabel: public Box {
  public:
    UCLASS(ULabel)
    static UStyle* createStyle();

    ULabel(Args arglist = Args::none) : Box(arglist) {}
    ///< create a new label.
    
    ULabel(int nbchars, Args arglist = Args::none);
    ///< create a new label with a size of nbchars.    
  };
  
  
  inline ULabel& ulabel(Args arglist = Args::none) 
  {return *new ULabel(arglist);}
  ///< shortcut function that return *new ULabel(arglist).

  inline ULabel& ulabel(int nbchars, Args arglist = Args::none) 
  {return *new ULabel(nbchars, arglist);}
  ///< shortcut function that return *new ULabel(nbchars, arglist).

  
  /** Textfield widget: single line editor.
   * A textfield is (roughly) similar to a ULabel except that it can edit text.
   *
   * Default look and feel (can be changed by adding appropriate Attribute()):
   *  - horizontal layout (can't be changed)
   *  - Font and foreground Color inherited from parents
   *  - white Background
   *  - NOT resized when children (text, images, other widgets) are changed or resized:
   *    a textfield keeps its initial size (calculated to fit its initial chidren).
   *    Add a USize attribute to change this behavior or to impose a given size.
   *  - Note that geometry also depend on parents, and, more specifically, on their 
   *    uhflex()/uvflex() attributes.
   *
   * As other widgets, a textfield can contain any combination of text, images and  
   * other widgets and its look can be changed by adding appropriate Attribute(s)
   * (for specifying a specific Background, UAlpha, UPadding, Border, etc.)
   *
   * Example:
   *  <pre>
   *      String& s = ustr("World!");
   *      UTextfield& tf = utextfield(UPix::doc + "Hello " + UPix::ray + s)
   *  </pre>
   * Creates a textfield that contains an image followed by "Hello ", another image
   * and the content of string 's'. The textfield will be updated when the content
   * of 's' is changed.
   *
   *  utextfield(...) is a shortcut for *new UTextfield(...)
   *
   * CALLBACKS:
   * Any Box callback can be added to a textfield (see UOn and UCall). However,
   * the most useful callback conditions for this widget are:
   *  - UOn::action (default condition) fires callbacks when the RETURN key is pressed.
   *  - UOn::strChange fires callbacks when the text is changed
   *
   * Example:
   * <pre>
   *    struct Demo {
   *       void enterCB(Event&);
   *       void changeCB(Event&);
   *    };
   * 
   *    Demo* d = new Demo();
   *    UTextfield& tf = utextfield("Hi Folks!"
   *                                + UOn::action / ucall(d, &Demo::enterCB)
   *                                + UOn::strChange / ucall(d, &Demo::changeCB));
   * </pre>
   * enterCB() and changeCB() are instance methods of class Demo. They will 
   * respectivelly be called when ENTER is pressed or when the contained text
   * is modified. Note that 'd' must be given as the first argument of ucall()
   * as these functions are instance methods of 'd'. ucall() also offers many other
   * possibilities, such as passing arguments to instance methods or calling static
   * or non-member functions. See UCall for details.
   *
   * UOn::action can be omitted, because it is the default callback condition for
   * this widget: ucall(...) alone is thus identical to: UOn::action / ucall(...)
   *
   * The enterCB() and changeCB() methods would typically do something like:
   * <pre>
   *    void Demo::enterCB(Event& e) {
   *        if (!e.getSource()) return;
   *        String s = e.getSource()->retrieveText();
   *        char c = e.getChar();
   *        ...
   *    }
   * </pre>
   * e.getSource() returns the address of the object that produced the event (the
   * textfield in this case) and retrieveText() returns the text contained inside
   * The Event argument is optional: ucall() could refer to an instance method
   * that has no such argument.
   */
  class UTextfield: public Box {
  public:
    UCLASS(UTextfield)
    static UStyle* createStyle();

    UTextfield(Args arglist = Args::none);
    ///< creates a widget for editing a line of text (@see also shortcut utextfield()).
    
    UTextfield(int nbchars, Args arglist = Args::none);
    /**< creates a widget for editing one line of text (@see also shortcut utextfield()).
     * the textbox will be large enough to display nbchars.
     */
    
    virtual bool isEditable() const;
    
    virtual UTextfield& setEditable(bool state = true);
    
    TextEdit& edit();
    ///< returns the TextEdit attribute that controls the edition of the textfield.
    
  };
  
  inline UTextfield& utextfield(Args arglist = Args::none) 
  {return *new UTextfield(arglist);}
  ///< shortcut that returns *new UTextfield(arglist).
  
  inline UTextfield& utextfield(int nbchars, Args arglist = Args::none)
  {return *new UTextfield(nbchars, arglist);}
  ///< shortcut that returns *new UTextfield(nbchars, arglist).
  
  
  /** Textarea widget: multiple line editor.
   * This widget does the same as UTextfield except that is can contains as many
   * lines as needed. Children are layed out as a continuous flow: see FlowView.
   */
  class UTextarea: public UTextfield {
  public:
    UCLASS(UTextarea)
    static UStyle* createStyle();

    UTextarea(Args arglist = Args::none) : UTextfield(arglist) {}
    ///< creates a new text area (@see also shortcut utextarea()).    
  };
  
  inline UTextarea& utextarea(Args arglist = Args::none) 
  {return *new UTextarea(arglist);}
  ///< shortcut thar returns *new UTextarea(arglist).
  
  
  /** Button widget.
   * A UButton is similar to a UButton except that:
   * - it is clickable (see the "callback" section below)
   * - its "look" depends on parent(s): its backgound is transparent if the button
   *   is contained in a UMenu, a UMenubar or a UBar and grey otherwise.
   *
   * As other widgets, a button can contain any combination of text, images and  
   * other widgets and its look can be changed by adding appropriate Attribute(s)
   * (for specifying a specific Background, UAlpha, UPadding, Border, etc.)
   *
   * CALLBACKS:
   * Any Box callback can be added to a textfield (see UOn and UCall). However,
   * the most useful callback conditions for this widget are:
   *  - UOn::action (default condition) fires callbacks the button is clicked
   *  - UOn::arm : fires callbacks when the button is pressed
   *  - UOn::disarm : fires callbacks when the button is released
   *
   * Example:
   *  <pre>
   *    struct Demo {
   *        void saveFile();
   *    };
   *
   *    Demo* d = new Demo();
   *    Box& b = ubutton(UPix::diskette + " Save"
   *                      + UOn::action / ucall(d, &Demo::saveFile)
   *                      + utip("Saves the current file"));
   * </pre>
   *  ubutton(...) is a shortcut for *new UButton(...)
   *
   * saveFile() is an instance methods of class Demo. It will called when the button
   * is activated (i.e. clicked). Note that 'd' must be given as the first argument 
   * of ucall() as saveFile() instance methods of 'd'. ucall() also offers many other
   * possibilities, such as passing arguments to instance methods or calling static
   * or non-member functions. See UCall for details.
   *
   * UOn::action can be omitted, because it is the default callback condition for
   * this widget:  ucall(...) alone is thus identical to: UOn::action / ucall(...)
   *
   * saveFile() could have an optional Event argument (see UTextfield as an example)
   * or some specific arguments given bu ucall (see UCall)
   *
   * utip(..) adds a tool tip to this button that will show the corresponding message
   * (utip() can in fact be added to any Ubit widget)
   *   
   * IMPLICIT CALLBACKS:
   * special rules applies when a UDialog or a UMenu is a *direct* child of a button:
   * these widgets are then automatically opened when the button is (respectively)
   * clicked or pressed.
   *
   * @see also: UItem, URadiobutton, UCheckbox
   */
  class UButton: public Box {
  public:
    UCLASS(UButton)
    static UStyle* createStyle();
    enum ButtonType {AUTO, NORMAL, MENU, BAR, TAB};

    UButton(Args arglist = Args::none);
    ///< creates a standard push button.
 
    UButton(int button_style, Args arglist = Args::none);
    ///< creates a push button wich the specified 'button_style'.
    
  private:
    friend class UButtonStyle;
    unsigned char button_type;
  };
  
  inline UButton& ubutton(Args arglist = Args::none) {return *new UButton(arglist);}
  ///< creates a button.
  
  UButton& uflatbutton(Args arglist = Args::none);
  ///< creates a "flat" button with a thin line border.
  
  UButton& utabbutton(Args arglist = Args::none);
  ///< creates a "tab" button (for tabbed panes, @see UCardbox).

     
  /** Item Button widget (a kind of button that is used in Listbox(es)).
   *
   *  Same behavior as UButton but with a different decoration (no visible
   *  border). UItems are typically used as Listbox or UTrow children.
   *
   *  Properties and callbacks: 
   *  - same default properties, layout and resize behavior as ULabel.
   *  - same callbacks as UButton.
   */
  class UItem: public Box {
  public:
    UCLASS(UItem)
    static UStyle* createStyle();

    UItem(Args = Args::none);
    ///< creates a new "item" button.
  };
  
  inline UItem& uitem(Args a = Args::none) {return *new UItem(a);}
  ///< creates a new "item" button.
  
  
  /** Radio Button gadget.
   * See UButton.
   */
  class URadiobutton: public UButton {
  public:
    UCLASS(URadiobutton)
    static UStyle* createStyle();

    URadiobutton(Args arglist = Args::none);
    ///< create a radio button.
  };
  
  inline URadiobutton& uradiobutton(Args a = Args::none) {return *new URadiobutton(a);}
  ///< creates a radio button.
  
  /** Checkbox gadget.
   *
   * in addition to UButton's Callbacks :
   * - UOn::select fire callbacks when the the checkbox is selected
   * - UOn::deselect fires callbacks when the the checkbox is deselected
   *
   * Note:
   *  - UOn::action fires callbacks when the user clicks the checkbox.
   *    => action is performed if the user selects or deselects the checkbox
   *    by clicking on it (but it is not activated if the checkbox is selected
   *    or deselected by a function call or a radio selection behavior)
   *    
   */
  class UCheckbox: public UButton {
  public:
    UCLASS(UCheckbox)
    static UStyle* createStyle();

    UCheckbox(Args arglist = Args::none);
    ///< creates a checkbox.
    
    UCheckbox& select(bool state = true) {return setSelected(state);}
    ///< shortcut for setSelected(bool state = true).
    
    virtual UCheckbox& setSelected(bool state = true, bool call_callbacks = true) 
    {Element::setSelected(state, call_callbacks); return *this;}
    /**< specifies whether this object is selected.
     * fire related callbacks if 'call_callbacks' is true
     */
    
    bool isSelected() const {return emodes.IS_SELECTED;}
  };
  
  inline UCheckbox& ucheckbox(Args arglist = Args::none) {return *new UCheckbox(arglist);}
  ///< creates a checkbox.
  
  
  /** Link Button gadget.
   * used for HTML links (no border and specific properties).
   * See UButton.
   */
  class ULinkbutton: public UButton {
  public:
    UCLASS(ULinkbutton)
    static UStyle* createStyle();
    
    ULinkbutton(Args = Args::none);
    
    virtual String getHRef() const;// {String val; getAttrValue(val,"href"); return val;}  // pas tres efficace!
  };
  
  inline ULinkbutton& ulinkbutton(Args a = Args::none) {return *new ULinkbutton(a);}
  ///< creates a "link" button (@see ULinkbutton).
  
  
  /** Spinbox gadget.
   *  Geometry:
   *  - Same geometry as UTextfield: keeps its initial size
   *  Callbacks
   *  - UOn::action (default callback, can be omitted) is fired when the value is changed
   *  - UOn::change is fired when the value is changed
   */
  class USpinbox : public Box {
  public:
    UCLASS(USpinbox)
    static UStyle* createStyle();

    USpinbox(Args = Args::none);
    ///< creates a spin box.
    
    USpinbox(Int& value, Args = Args::none);
    ///< creates a spin box that is linked to an integer model.
        
    virtual int getValue() const {return *pvalue;}
    ///< returns the current value.
    
    virtual void setValue(int);
    /**< changes the current value.
     * UOn::change and UOn::action callbacks are fired when the value is changed.
     */
    
    Int& value() {return *pvalue;}
    /**< returns the internal value model.
     * UOn::change and UOn::action callbacks are fired when the value is changed.
     * this object can be shared but can't be deleted.
     */
    
    int getIncrement() const {return increment;}
    ///< returns the increment value.
    
    virtual void setIncrement(int);
    ///< changes the increment.
    
    String& str() {return *pstr;}
    /**< return internal string model of the text.
     * this object can be shared but can't be deleted.
     */
    
  protected:
    virtual void constructs(const Args& a);
    virtual void updateValue(int dir);
    virtual void changed();
    
  private:
    uptr<Int> pvalue;
    uptr<String> pstr;
    uptr<UTextfield> ptextfield;
    int increment;
  };
  
  inline USpinbox& uspinbox(Args a = Args::none) {return *new USpinbox(a);}
  ///< creates a spin box.
  
}
#endif

