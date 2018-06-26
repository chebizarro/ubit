/*************************************************************************
 *
 *  UDispGDK.hpp : Native Layer: platform dependent implementation
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2008 Eric Lecolinet / ENST Paris / www.enst.fr/~elc/ubit
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

#include <ubit/ubit_features.h>
#if UBIT_WITH_GDK

#ifndef UDispGDK_hpp
#define	UDispGDK_hpp 1

#include <gdk/gdk.h>
#include <ubit/ugl.hpp>
#include <ubit/udisp.hpp>
#include <ubit/nat/uhardwinGDK.hpp>
namespace ubit {

#define UCursorImpl GdkCursor

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/** GDK Atoms.
*/
struct UAtomsGDK {
  GdkAtom PRIMARY_SELECTION, SECONDARY_SELECTION,
  WM_PROTOCOLS, WM_DELETE_WINDOW, WM_TAKE_FOCUS,
  UMS_WINDOW, UMS_MESSAGE;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/** GDK Display.
*/
class UDispGDK : public UDisp {
public:
  UDispGDK(const UStr&);
  virtual ~UDispGDK();
   
  virtual void   setPointerPos(const UPoint& screen_pos);
  virtual UPoint getPointerPos() const;
  virtual int    getPointerState() const;
  virtual bool   grabPointer(const UCursor*);
  virtual void   ungrabPointer();
  virtual bool   pickWindow(int& x_in_win, int& y_in_win, UHardWin* window,
                            UCursor* cursor = null, UCall* callback = null);
  
  virtual unsigned long createColorPixel(const URgba&);  // for 2D_GRAPHICS
  virtual UCursorImpl*  createCursorImpl(int curtype);
  virtual void          deleteCursorImpl(UCursorImpl*);
  virtual UHardWinGDK*  createWinImpl(UWin*);

  GdkDisplay*  getSysDisp()     const {return sys_disp;}
  GdkScreen*   getSysScreen()   const {return sys_screen;}
  GdkVisual*   getSysVisual()   const {return sys_visual;}
  GdkWindow*   getSysRoot()     const;
  GdkWindow*   getSysFrame()    const;
  GdkColormap* getSysColormap() const {return sys_cmap;}

  const UAtomsGDK& getAtoms() const {return atoms;}
  /**< returns useful atoms.
   * - UMS_WINDOW: a X window that has this property will receive UMS events.
   * - UMS_MESSAGE: the property for exchanging messages between applications.
   * see also: umsproto.hpp and the UMS server (available in directory ubit/ums).
   */
  
protected:
  friend class UAppli;
  friend class UAppliImpl;
  friend class UGraph;
  friend class UHardWin;
  friend class UHardWinGDK;
  
  GdkDisplay* sys_disp;
  GdkScreen* sys_screen;
  GdkVisual* sys_visual;
  GdkColormap* sys_cmap;
  UAtomsGDK atoms;

  virtual void startAppli();
  virtual void quitAppli();
  virtual void startLoop(bool main_loop);   
  virtual void quitLoop(bool main_loop); 
  void dispatchEvent(GdkEvent*);
  static void eventHandler(GdkEvent*, gpointer appli);
};


}
#endif
#endif // UBIT_WITH_GDK

