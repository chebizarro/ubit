/*
 *  boxes.hpp : misc containers.
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


#ifndef _uboxes_hpp_
#define	_uboxes_hpp_ 1

#include <ubit/ubox.hpp>

namespace ubit {
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** UFlowBox = Box with a Flow Layout (similar to an HTML page).
   *  Base class for creating text areas or hypermedia gadgets.
   *
   *  Geometry: the width of a UFlowbox object does do not change when its content
   *  is modified. This behavior differs from those of most other boxes.
   *  It is generally well suited for displaying/editing text (UTexfield and 
   *  TextArea enforce this behavior)
   *
   *  Implementation note: a UFlowbox is an Box with UFlowview renderer.
   */
  class UFlowbox: public Box {
  public:
    UCLASS(UFlowbox)

    UFlowbox(Args args = Args::none);
    /**< create a new flowbox; @see also shortcut: uflowbox().
     * a flowbox displays strings and other objects as a continous 2D flow
     */
    
    static Style* createStyle();
  };
  
  inline UFlowbox& uflowbox(Args args = Args::none) {return *new UFlowbox(args);}
  ///< shortcut that creates a new UFlowBox.
  
   
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** UBar = horizontal toolbar or status bar.
   *  This class is similar to UHbox but with a specific ("toolbar like") decoration.
   *  Buttons included in this object are displayed in a specific
   *  way (their border does not appear, etc.)
   *
   *  See class Box for details.
   *  See also: class UMenubar for creating menu bars
   */
  class UBar: public Box {
  public:
    UCLASS(UBar)

    UBar(Args args = Args::none): Box(args) {}
    ///< creates a new horizontal bar; @see also shortcut: ubar().
    
    static Style* createStyle();
  };
  
  inline UBar& ubar(Args args = Args::none) {return *new UBar(args);}
  ///< shortcut function that creates a new UBar.
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** Status bar.
   *  This class is similar to UHbox but with a specific ("statusbar like") 
   *  decoration.
   *  Buttons included in this object are also displayed in a specific
   *  way (their border won't appear, etc.)
   *
   *  See class Box for details.
   */
  class UStatusbar: public Box {
  public:
    UCLASS(UStatusbar)

    UStatusbar(Args args = Args::none): Box(args) {}
    ///< create a new status bar; @see also shortcut: ustatusbar().
    
    static Style* createStyle();
  };
  
  inline UStatusbar& ustatusbar(Args args = Args::none) {return *new UStatusbar(args);}
  ///< shortcut that creates a new UStatusbar.
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** Card box (or Tabbed Pane).
   */
  class UCardbox : public Box {
  public:
    UCLASS(UCardbox)

    UCardbox(Args args = Args::none);
    ///< creates a new card box; @see also shortcut: ucardbox().
    
    virtual ~UCardbox();
    
    static  Style* createStyle();
    
    virtual UCardbox& addCard(Box& card);
    /**< adds a superimposed card (without a tab).
     * @see also: addTab()
     */
    
    virtual UCardbox& addTab(Args tab_content, Box& card);
    /**< adds a superimposed card with an associated tab.
     * tab_content can be a string, an icon and a string, or any other valid +arglist.
     * @see also: addCard()
     */
    
    virtual Box* getCard(int index) const;
    ///< returns the card at this index.
    
    virtual int getCardIndex(Box& card) const;
    ///< returns the index of this card (-1 if not found).
    
    virtual Box* getSelectedCard() const;
    ///< returns the card that is currently selected (null if none).
    
    virtual int getSelectedIndex() const;
    ///< returns the index of the card that is currently selected (-1 if none).
    
    virtual void setSelectedCard(Box& card);
    ///< selects this card.
    
    virtual void setSelectedIndex(int index);
    ///< selects the card at this index (the last card if n = -1).
    
    Choice& choice();
    ///< returns the Choice object that controls the selection of cards and tabs.
    
    ListBox& tablist() {return *ptabs;}
    ///< returns the tab list.
    
#ifndef NO_DOC
  protected:
    uptr<ListBox> ptabs;
    virtual void setSelectedImpl();
#endif
  };
  
  inline UCardbox& ucardbox(Args args = Args::none) {return *new UCardbox(args);}
  ///< shortcut that creates a new UCardbox.
  
    
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** document box gadget.
   *¨ a box that has a titlebar and a zoomable+scrollable working area.
   *  when a docbox is iconfied the working area is hidden but the titlebar
   * remains visible.
   */
  class UDocbox : public Box {
  public:
    UCLASS(UDocbox)

    UDocbox(Args args = Args::none);
    virtual ~UDocbox();
    
    virtual Box& titlebar()           {return *ptitlebar;}
    virtual Scrollpane& scrollpane()  {return *pspane;}
    virtual Box& content()            {return *pcontent;}
    virtual Scale& scale()            {return *pscale;}
    virtual void iconify(bool);
    virtual bool isIconified() const;
    virtual void zoom(float value = 1.);
    virtual void zoomIn();
    virtual void zoomOut();
    virtual void setZoomQuantum(float);
    
#ifndef NO_DOC
  protected:
    uptr<Box> ptitlebar, pcontent;
    uptr<Scrollpane> pspane;
    uptr<Scale> pscale;
    float zoom_quantum;
#endif
  };
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** Alertbox gadget
   */
  class AlertBox : public Box {
  public:
    UCLASS(AlertBox)

    AlertBox(Args args = Args::none);
    
    static Style* createStyle();
    
    // NB: show doit etre redefini, sinon show("abcd") est pris comme show(bool)
    // a cause des conversions implicites (stupides) du C++
    virtual void show(bool b) {Box::show(b);}
    virtual void show(const char* msg) {showMsg(msg);}
    virtual void show(const String& msg) {showMsg(msg);}
    
    virtual void showMsg(const char* msg);
    virtual void showMsg(const String& msg);
    ///< shows the alert box with this message.
    
  private:
    Element message;
  };
  
}
#endif
