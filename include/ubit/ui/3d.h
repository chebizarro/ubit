/*
 *  3d.cpp: 3D Widgets
 *  Ubit GUI Toolkit - Version 6 - Matthieu Nottale, Eric Lecolinet
 *  (C) 2009 | Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
 *
 * ***********************************************************************
 * COPYRIGHT NOTICE :
 * THIS PROGRAM IS DISTRIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE
 * IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION;
 * EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 * SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
 * ***********************************************************************/

#ifndef _u3d_hpp_
#define	_u3d_hpp_ 1

#include <ubit/uattr.hpp>
#include <ubit/ubox.hpp>
#include <ubit/uctlmenu.hpp>
#include <ubit/uview.hpp>

namespace ubit {
  
  /** [Instable/OpenGL] 3D canvas: makes it possible to display children in 3D space.
   *
   * Requires OpenGL. See U3Dwin, U3Dpos for details.
   *  
   * Note: the displayed view is a 60° wide perspective.
   */
  class U3Dcanvas: public Box {
  public:
    UCLASS(U3Dcanvas)
    
    U3Dcanvas(Args a = Args::none);
    ///< creates a new 3D canvas; @see also shortcut: u3dcanvas().
    
    virtual ~U3Dcanvas();
    
    static Style* createStyle();
    
    float getFovy() const   {return fovy;}
    float getAspect() const {return aspect;}
    float getNear() const   {return near;}
    float getFar() const    {return far;}
    
    void setFovy(float f)   {fovy = f;}
    void setAspect(float a) {aspect = a;}
    void setNear(float n)   {near = n;}
    void setFar(float f)    {far = f;}
    
  private:
    friend class 3DCanvasView;
    float fovy, aspect, near, far;
  };
  
  inline U3Dcanvas& u3dcanvas(Args a = Args::none) {return *new U3Dcanvas(a);}
  ///< shortcut function that creates a new U3Dcanvas.
  
  /* ==================================================== ==== ======= */
  /** [Instable/OpenGL] 3D box: a box that can be orientated in 3D space.
   * 
   * This widget must be included in a U3Dcanvas. Its pos() method returns a reference
   * to a U3Dpos attribute that controls its position [i.e. the x,y,z translation 
   * and the rotations around x,y,z] relatively to the origin of the U3Dcanvas.
   * 
   * This widget can contain any other widgets, which are layed out as usual. 
   * They will appear on top of this U3Dbox, with the same orientation and a position
   * that is relative to its origin in 3D space.
   *
   * Requires OpenGL. @see: U3Dcanvas, U3Dpos.
   */
  class U3Dbox : public Box {
  public:
    UCLASS(U3Dbox)
    
    U3Dbox(Args = Args::none);
    ///< creates a new 3D box; @see also shortcut: u3dbox().
    
    static Style* createStyle();
    
    U3Dpos& pos() {return *ppos;}
    /**< returns a reference to the 3D position of this 3D window.
     * note that the returned object can be shared but must not be deleted by other widgets.
     */
    
    U3Dbox& setTrans(float x, float y, float z);
    U3Dbox& setRot(float x_rot, float y_rot, float z_rot);
    U3Dbox& translate(float delta_x, float delta_y, float delta_z);
    U3Dbox& rotate(float delta_x_rot, float delta_y_rot, float delta_z_rot);
    
  protected:
    uptr<U3Dpos> ppos;
  };
  
  inline U3Dbox& u3dbox(Args a = Args::none) {return *new U3Dbox(a);}
  ///< shortcut function that creates a new U3Dbox.
  
  /* ==================================================== ==== ======= */
  /** [Instable/OpenGL] 3D window: framed U3Dbox with a title. 
   */
  class U3Dwin : public U3Dbox {
  public:
    UCLASS(U3Dwin)
    
    U3Dwin(Args = Args::none);
    ///< creates a new 3D window; @see also shortcut: u3dwin().
    
    static Style* createStyle();
    
  protected:
    virtual Box& createTitleBar(const String& title);
  };
  
  inline U3Dwin& u3dwin(Args a = Args::none) {return *new U3Dwin(a);}
  ///< shortcut function that creates a new U3Dwin.
  
  /* ==================================================== ==== ======= */
  /** [Instable/OpenGL] 3D position.
   *
   * This attribute controls the position of a Box in 3D space [i.e. the x,y,z translation
   * and the rotations around x,y,z] relatively to the origin of its parent.
   *
   * All widgets that are orientated in 3D space (ie. all widgets that have a U3Dpos)
   * must be (indirect or direct) children of a U3Dcanvas.
   *
   * Requires OpenGL. @see: U3Dcanvas, U3Dwin.
   */
  class U3Dpos : public UPos {
  public:
    UCLASS(U3Dpos)
    
    U3Dpos();
    
    U3Dpos(float x, float y, float z);
    ///< creates a new 3D position; @see also shortcut: u3dpos().
    
    float getZ() const {return z;}
    float getXRot() const {return x_rot;}
    float getYRot() const {return y_rot;}
    float getZRot() const {return z_rot;}
    
    U3Dpos& setTrans(float x, float y, float z);
    U3Dpos& setRot(float x_rot, float y_rot, float z_rot, bool update = true);
    U3Dpos& translate(float delta_x, float delta_y, float delta_z);
    U3Dpos& rotate(float delta_x_rot, float delta_y_rot, float delta_z_rot);
    
    virtual bool is3Dpos() const {return true;}
    ///< returns true if this is a U3Dpos.
    
    
    virtual void setImpl(float _x, float _y, bool upd);
    virtual void setImpl(float _x, float _y, float _z, bool upd);
    virtual void update();
    virtual void putProp(UpdateContext*, Element&);
    
  private:
    friend class View;
    friend class 3DCanvasView;
    float z, x_rot, y_rot, z_rot;
  };
  
  inline U3Dpos& u3dpos(float x, float y, float z) {return *new U3Dpos(x, y, z);}
  ///< shortcut function that creates a new U3Dpos.
  
  /* [Impl] U3Dcanvas' view.
   *
   * Mouse and paint coordinates are transformed by 3DCanvasView when passed 
   * to its children to make geometrical transformations transparent for them.
   */
  class 3DCanvasView: public View {
  public:
    static  ViewStyle style;  // renderer
    virtual ViewStyle* getViewStyle() {return &style;}
    
    3DCanvasView(Box* _box, View* _parview, UHardwinImpl* hw) : View(_box, _parview, hw) {}
    
    static View* createView(Box* box, View* parv, UHardwinImpl* hw) {
      return new 3DCanvasView(box, parv, hw);
    }
    
    virtual void doUpdate(UpdateContext&, Rectangle r, Rectangle clip, class UViewUpdate&);
    virtual View* findInChildren(Element*, const Point& winpos, 
                                  const UpdateContext&, ViewFind&);
    
    bool unproject(const U3Dpos* pos3d, const Point& winpos, Point& convpos);
  };
  
}
#endif
