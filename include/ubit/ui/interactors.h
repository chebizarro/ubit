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

#include <memory>

#include <ubit/core/string.h>
#include <ubit/ui/textedit.h>
#include <ubit/ui/box.h>

namespace ubit {
  
  /** Separator gadget: horizontal or vertical separator for menus and boxes.
   */
  class Separator: public Box {
  public:
    UCLASS(Separator)
    static Style* createStyle();
    
    Separator();
    /**< create a new separator.
     * the orientation of the separator depends on the orientation of its parent box
     */
  };
  
  
  inline Separator& usepar() {return *new Separator();}
  ///< create a new separator.

  
  /** Label widget: can display any combination of text, images, and other widgets.
   *
   * Default look and feel (can be changed by adding appropriate Attribute(s)):
   * - horizontal layout (see attribute Orientation)
   * - font and foreground color inherited from parents (see Font and Color)
   * - transparent backgound (see UBackgound and Alpha)
   * - automatically resized when children (text, images, and other widgets) are 
   *   changed or resized. Add a Size attribute to change this behavior or to 
   *   impose a given size. 
   * - Note that geometry also depend on parents, and, more specifically, on their
   *   uhflex()/uvflex() attributes.
   *
   * As other widgets, a label can contain any combination of text, images and  
   * other widgets and its look can be changed by adding appropriate UAttrs
   * (for specifying a specific Background, Alpha, Padding, Border, etc.)
   *   
   *  Example:
   *  <pre>  
   *     String& s = "ON";
   *     Label& mylabel = ulabel(uima("myimage.jpg") + "Current state: " + s)
   *  </prep>
   *  Creates a label that contains an image followed by "Current state: " and the
   *  content of string 's'. The label will be updated whenever 's' content is changed.
   *
   *  ulabel(...) is a shortcut for *new Label(...)
   */
  class Label: public Box {
  public:
    UCLASS(Label)
    static Style* createStyle();

    Label(Args arglist = Args::none) : Box(arglist) {}
    ///< create a new label.
    
    Label(int nbchars, Args arglist = Args::none);
    ///< create a new label with a size of nbchars.    
  };
  
  
  inline Label& ulabel(Args arglist = Args::none) 
  {return *new Label(arglist);}
  ///< shortcut function that return *new Label(arglist).

  inline Label& ulabel(int nbchars, Args arglist = Args::none) 
  {return *new Label(nbchars, arglist);}
  ///< shortcut function that return *new Label(nbchars, arglist).

  
  /** Textfield widget: single line editor.
   * A textfield is (roughly) similar to a Label except that it can edit text.
   *
   * Default look and feel (can be changed by adding appropriate Attribute()):
   *  - horizontal layout (can't be changed)
   *  - Font and foreground Color inherited from parents
   *  - white Background
   *  - NOT resized when children (text, images, other widgets) are changed or resized:
   *    a textfield keeps its initial size (calculated to fit its initial chidren).
   *    Add a Size attribute to change this behavior or to impose a given size.
   *  - Note that geometry also depend on parents, and, more specifically, on their 
   *    uhflex()/uvflex() attributes.
   *
   * As other widgets, a textfield can contain any combination of text, images and  
   * other widgets and its look can be changed by adding appropriate Attribute(s)
   * (for specifying a specific Background, Alpha, Padding, Border, etc.)
   *
   * Example:
   *  <pre>
   *      String& s = ustr("World!");
   *      TextField& tf = utextfield(UPix::doc + "Hello " + UPix::ray + s)
   *  </pre>
   * Creates a textfield that contains an image followed by "Hello ", another image
   * and the content of string 's'. The textfield will be updated when the content
   * of 's' is changed.
   *
   *  utextfield(...) is a shortcut for *new TextField(...)
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
   *    TextField& tf = utextfield("Hi Folks!"
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
  class TextField: public Box {
  public:
    UCLASS(TextField)
    static Style* createStyle();

    TextField(Args arglist = Args::none);
    ///< creates a widget for editing a line of text (@see also shortcut utextfield()).
    
    TextField(int nbchars, Args arglist = Args::none);
    /**< creates a widget for editing one line of text (@see also shortcut utextfield()).
     * the textbox will be large enough to display nbchars.
     */
    
    virtual bool isEditable() const;
    
    virtual TextField& setEditable(bool state = true);
    
    TextEdit& edit();
    ///< returns the TextEdit attribute that controls the edition of the textfield.
    
  };
  
  inline TextField& utextfield(Args arglist = Args::none) 
  {return *new TextField(arglist);}
  ///< shortcut that returns *new TextField(arglist).
  
  inline TextField& utextfield(int nbchars, Args arglist = Args::none)
  {return *new TextField(nbchars, arglist);}
  ///< shortcut that returns *new TextField(nbchars, arglist).
  
  
  /** Textarea widget: multiple line editor.
   * This widget does the same as TextField except that is can contains as many
   * lines as needed. Children are layed out as a continuous flow: see FlowView.
   */
  class TextArea: public TextField {
  public:
    UCLASS(TextArea)
    static Style* createStyle();

    TextArea(Args arglist = Args::none) : TextField(arglist) {}
    ///< creates a new text area (@see also shortcut utextarea()).    
  };
  
  inline TextArea& utextarea(Args arglist = Args::none) 
  {return *new TextArea(arglist);}
  ///< shortcut thar returns *new TextArea(arglist).
  
  
  /** Button widget.
   * A Button is similar to a Button except that:
   * - it is clickable (see the "callback" section below)
   * - its "look" depends on parent(s): its backgound is transparent if the button
   *   is contained in a Menu, a MenuBar or a UBar and grey otherwise.
   *
   * As other widgets, a button can contain any combination of text, images and  
   * other widgets and its look can be changed by adding appropriate Attribute(s)
   * (for specifying a specific Background, Alpha, Padding, Border, etc.)
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
   *  ubutton(...) is a shortcut for *new Button(...)
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
   * saveFile() could have an optional Event argument (see TextField as an example)
   * or some specific arguments given bu ucall (see UCall)
   *
   * utip(..) adds a tool tip to this button that will show the corresponding message
   * (utip() can in fact be added to any Ubit widget)
   *   
   * IMPLICIT CALLBACKS:
   * special rules applies when a Dialog or a Menu is a *direct* child of a button:
   * these widgets are then automatically opened when the button is (respectively)
   * clicked or pressed.
   *
   * @see also: Item, RadioButton, Checkbox
   */
  class Button: public Box {
  public:
    UCLASS(Button)
    static Style* createStyle();
    enum ButtonType {AUTO, NORMAL, MENU, BAR, TAB};

    Button(Args arglist = Args::none);
    ///< creates a standard push button.
 
    Button(int button_style, Args arglist = Args::none);
    ///< creates a push button wich the specified 'button_style'.
    
  private:
    friend class ButtonStyle;
    unsigned char button_type;
  };
  
  inline Button& ubutton(Args arglist = Args::none) {return *new Button(arglist);}
  ///< creates a button.
  
  Button& uflatbutton(Args arglist = Args::none);
  ///< creates a "flat" button with a thin line border.
  
  Button& utabbutton(Args arglist = Args::none);
  ///< creates a "tab" button (for tabbed panes, @see CardBox).

     
  /** Item Button widget (a kind of button that is used in ListBox(es)).
   *
   *  Same behavior as Button but with a different decoration (no visible
   *  border). UItems are typically used as ListBox or UTrow children.
   *
   *  Properties and callbacks: 
   *  - same default properties, layout and resize behavior as Label.
   *  - same callbacks as Button.
   */
  class Item: public Box {
  public:
    UCLASS(Item)
    static Style* createStyle();

    Item(Args = Args::none);
    ///< creates a new "item" button.
  };
  
  inline Item& uitem(Args a = Args::none) {return *new Item(a);}
  ///< creates a new "item" button.
  
  
  /** Radio Button gadget.
   * See Button.
   */
  class RadioButton: public Button {
  public:
    UCLASS(RadioButton)
    static Style* createStyle();

    RadioButton(Args arglist = Args::none);
    ///< create a radio button.
  };
  
  inline RadioButton& uradiobutton(Args a = Args::none) {return *new RadioButton(a);}
  ///< creates a radio button.
  
  /** Checkbox gadget.
   *
   * in addition to Button's Callbacks :
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
  class Checkbox: public Button {
  public:
    UCLASS(Checkbox)
    static Style* createStyle();

    Checkbox(Args arglist = Args::none);
    ///< creates a checkbox.
    
    Checkbox& select(bool state = true) {return setSelected(state);}
    ///< shortcut for setSelected(bool state = true).
    
    virtual Checkbox& setSelected(bool state = true, bool call_callbacks = true) 
    {Element::setSelected(state, call_callbacks); return *this;}
    /**< specifies whether this object is selected.
     * fire related callbacks if 'call_callbacks' is true
     */
    
    bool isSelected() const {return emodes.IS_SELECTED;}
  };
  
  inline Checkbox& ucheckbox(Args arglist = Args::none) {return *new Checkbox(arglist);}
  ///< creates a checkbox.
  
  
  /** Link Button gadget.
   * used for HTML links (no border and specific properties).
   * See Button.
   */
  class LinkButton: public Button {
  public:
    UCLASS(LinkButton)
    static Style* createStyle();
    
    LinkButton(Args = Args::none);
    
    virtual String getHRef() const;// {String val; getAttrValue(val,"href"); return val;}  // pas tres efficace!
  };
  
  inline LinkButton& ulinkbutton(Args a = Args::none) {return *new LinkButton(a);}
  ///< creates a "link" button (@see LinkButton).
  
  
  /** Spinbox gadget.
   *  Geometry:
   *  - Same geometry as TextField: keeps its initial size
   *  Callbacks
   *  - UOn::action (default callback, can be omitted) is fired when the value is changed
   *  - UOn::change is fired when the value is changed
   */
  class SpinBox : public Box {
  public:
    UCLASS(SpinBox)
    static Style* createStyle();

    SpinBox(Args = Args::none);
    ///< creates a spin box.
    
    SpinBox(Int& value, Args = Args::none);
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
    std::unique_ptr<Int> pvalue;
    std::unique_ptr<String> pstr;
    std::unique_ptr<TextField> ptextfield;
    int increment;
  };
  
  inline SpinBox& uspinbox(Args a = Args::none) {return *new SpinBox(a);}
  ///< creates a spin box.
  
}
#endif

