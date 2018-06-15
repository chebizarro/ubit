/*************************************************************************
 *
 *  UHardwinGDK.hpp : Native Layer: platform dependent implementation
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

#ifndef UHardwinGDK_hpp
#define	UHardwinGDK_hpp 1

#include <gdk/gdk.h>
#include <ubit/udisp.hpp>
#include <ubit/nat/uhardwin.hpp>
namespace ubit {

class UDispGDK;

/** GDK Hard Window.
  */
class UHardWinGDK : public UHardWin {
public:
  UHardWinGDK(class UDispGDK*, UWin*);
  virtual ~UHardWinGDK();  
  
  UDispGDK*   getDispGDK() const {return (UDispGDK*)disp;}
  GdkWindow*  getSysWin() const {return sys_win;}
  GdkDisplay* getSysDisp() const;
  GdkScreen*  getSysScreen() const;
  
  virtual void realize(WinType, float w, float h);
  virtual bool isRealized() const {return sys_win != null;}
  
  virtual void show(bool);
  virtual void toBack();
  virtual void toFront();
  
  virtual UPoint getScreenPos() const;
  ///< returns window position relatively to the screen.
    
  virtual UPoint getPos() const;  
  ///< returns window position relatively to its parent window.

  virtual void setPos(const UPoint&);
  ///< changes window position relatively to its parent window.
  
  virtual UDimension getSize() const;
  ///< returns window size.
  
  virtual void setSize(const UDimension&);
  ///< changes window size.
  
  virtual UStr getTitle() const;
  ///< returns window title.
  
  virtual void setTitle(const UStr&);
  ///< changes window title.
  
  virtual UStr getIconTitle() const;
  ///< returns icon title.
  
  virtual void setIconTitle(const UStr&);  
  ///< changes icon title.
  
  virtual void setCursor(const UCursor*);
  ///< changes window cursor (can be null).
  
  virtual void setClassProperty(const UStr& instance_name, const UStr& class_name);
  ///< changes the WM_CLASS property.
  
protected:
  friend class UDispGDK;
  friend class UGraph;
  friend class UGraphCtxGDK;
  GdkWindow* sys_win;
};

}
#endif
#endif
