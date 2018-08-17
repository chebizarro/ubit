/*
 *  attribute.h: Base class for the attributes of the Scene Graph
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

#ifndef UBIT_CORE_ATTRIBUTE_H_
#define	UBIT_CORE_ATTRIBUTE_H_

#include <ubit/unumber.hpp>

namespace ubit {
  
  /**
   * Base class for attributes.
   *
   * attributes specify the layout, the geometry, the color, the font...
   * of elements that derive from ubit::Box (some attributes also apply
   * on ubit::Element elements, as explained in the doc of these properties).                                              
   *
   * they can be added to the attribute list of one or several containers by using
   * ubit::Element::addAttr(). They can also be inserted in the *beginning* of their child
   * list, before any viewable child (that is to say a child that derive from 
   * ubit::Element or ubit::Data). Certains properties (mainly ubit::Font and ubit::Color)
   * can be inserted anywhere in the child list and take effect of the following children.
   *
   * attributes update their parents automatically when they are modified. All
   * parents that share the same property are thus automatically synchronized.
   *
   * attributes can have callback functions that are fired when their value is
   * changed (see an example below). Property parents can also be notified when
   * attribute values are modified by using the UOn::propChange condition (see below).
   *
   * Example:
   * <pre>
   *    class Demo {
   *    public:
   *       void colorCB(Color* c);
   *       void boxCB(Event& e);
   *       ....
   *    };
   *
   *    Demo* d = new Demo();
   *
   *    Color& color = ucolor(255, 0, 0);  // shorcut for: *new Color(...)
   *
   *    // d->colorCB(&color) will be called when the color is changed
   *    color.onChange(ucall(d, &color, &Demo::colorCB));
   * 
   *    Box& btn = ubutton("Click Me");    // shorcut for: *new Button(...)
   *
   *    btn.addAttr(color);                 // adds color to the attribute list
   *
   *    // the content of Color::blue (which is a predefined value) is copied
   *    // into the content of the color object (note that color is not a pointer
   *    // but a reference)
   *
   *    color = Color::blue;               // fires colorCB
   * </pre>
   *
   * Alternatively, we could insert color at the beginning of the child list:
   * <pre>
   *    Box& btn = ubutton(color + "Click Me");
   *    // btn.addAttr(color); useless: color is in the child list!
   * </pre>
   *
   * Finally, 
   * <pre>
   *    Box& btn = ubutton(color + "Click Me" 
   *                        + UOn::propChange / ucall(d, &Demo::boxCB));
   * </pre>
   *
   * will call d->boxCB(event) when the value of any property of btn is changed.
   * In this case, color is the only property, and boxCB is fired when color
   * is modified. The Event& argument of boxCB() makes it possible to retrieve
   * the color property as follows:
   * 
   * <pre>
   *    void Demo::boxCB(Event& e) {
   *      Color* c = null;
   *      if (e.getTarget()) c = dynamic_cast<Color*>(e.getTarget());
   *      ....
   *   }
   * </pre>
   * Note that we could have use the same technique to retrieve the color property
   * in colorCB(), instead of giving color as an argument to this function
   *
   */
  class Attribute: public Node {
  public:
    
    Attribute();
    Attribute(UConst);
    virtual ~Attribute();
         
    virtual int getDisplayType() const {return ATTRIBUTE;}    
    virtual int getNodeType() const {return ATTRIBUTE_NODE;}
    
    virtual const String& getNodeName() const {return getClassName();}
    ///< returns the name of this attribute (calls getName()).

    virtual String getNodeValue() const;
    ///< returns the value of this attribute (calls getValue()).
        
    virtual const String& getName() const {return getClassName();}
    ///< returns the name of this attribute.
    
    virtual bool getValue(String& value) const;
    /**< gets the value of this attribute.
     * return false if this attribute has no value. 
     * Stores it in argument 'value' otherwise, 
     */
    
    virtual void setValue(const String& value) {}
    /**< changes the value of this attribute
     * does nothing if not applicable for a given subclass.
     */
    
    virtual void initNode(Document*, Element* parent) {}
    ///< called when the document is created.

      
    virtual Attribute& onChange(UCall&);
    /**< adds a callback that is fired when the value of the property is modified.
     * @see an example in the Attribute doc.
     */
        
    virtual void update() {}
    ///< updates parents graphics.
    
    virtual void changed(bool update = true);
    /**< [impl] called when object's content is changed.
     * This function updates graphics (if 'update' is true) then fires 
     * parents' UOn::propChange callbacks
     */
    
    virtual void putProp(UpdateContext*, Element&) {}
    ///< [impl] changes corresponding value in the UpdateContext
    
  protected:
    friend class Box;
  };
}
#endif // UBIT_CORE_ATTRIBUTE_H_
