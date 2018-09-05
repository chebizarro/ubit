/*
 *  pdatecontext.h: [implementation] stack context
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

#ifndef _uupdatecontext_hpp_
#define	_uupdatecontext_hpp_ 1

#include <ubit/udefs.hpp>
#include <ubit/draw/style.h>
#include <ubit/draw/ufontImpl.hpp>
#include <ubit/ui/view.h>
#include <vector>

namespace ubit {
  
  // ====[internal implementation]=========================================
  // NOTE: this header is part of the Ubit intrinsics and subject to change
  
  /* [impl] UpdateContext = a subcontext (not the first layer of the context cstack)
   * and a base for WindowUpdateContext (the first layer of the cstack)
   */
  class UpdateContext {
  public:  
    UpdateContext(const UpdateContext& parent_ctx, Element*, View*, UViewUpdateImpl*);
    ///< creates a subcontext (not the first layer of the context stack)
    
    Element* getObj() {return obj;}
    ///< returns the object (a Element, a Element or a Box) that is related to this context.
    
    Box* getBoxParent() {return view->getBoxParent();}
    /*< returns the direct or *indirect* box parent of getObj().
     * 'boxes' are Box objects or objects that derive from Box.
     */  
    
    View* getView() {return view;}
    ///< returns the view related to this context.
    
    View* getWinView() {return view->getWinView();}
    ///< returns the window view that contains getView().
    
    Display* getDisp() const {return view->getDisp();}  
    // const necessaire pour certains appels
    
    Graph* getGraph() {return graph;}

    void rescale();
    ///< rescales the coordinates according to the current scale.
    
    void addFlagdef(const UFlagdef*);
    const UFlagdef* getFlagdef(const Flag&) const;
    const UFlagdef* getFlagdef(const Flag*) const;
    const UPropdef* getPropdef(const Flag&) const;
    const UPropdef* getPropdef(const Flag*) const;
    
    // cette implementation est dangereuse car dans certains cas win_ctx est undef
    const UpdateContext* parent_ctx;
    WindowUpdateContext* win_ctx;  // not const because of the flags vector
    unsigned short flag_count;   // number of valid flagdefs for this stack level
    
    Element* obj;
    View* view;
    UViewUpdateImpl* view_impl;
    const Style* obj_style;
    LocalProperties local;
    Position* pos;                // Position ou U3dpos, Position peut etre proportionnelle
    FontDescription fontdesc;
    bool boxIsHFlex, boxIsVFlex; // true if the object is Horiz or Vert Flexible
    char valign, halign; 
    float xyscale;            // current scale
    float vspacing, hspacing;
    TextEdit* edit;
    const Color *color, *bgcolor;
    const Cursor *cursor;
    Graph* graph;

  protected:
    friend class ViewFind;
    UpdateContext() {}  // pour ViewFind
    UpdateContext(View* win_view); // for WindowUpdateContext
  };
  
    /* [impl] WindowUpdateContext = the first layer of the context cstack
   */
  class WindowUpdateContext : public UpdateContext {
  public:
    WindowUpdateContext(View* winview, Graph*);
    /**< creates the first layer of the context stack
     * !Warning: 'win_view' must be a valid (NOT null) window view!
     */
        
  private:
    friend class UpdateContext;
    std::vector<const UFlagdef*> flags;
  };
  
    
  // cette implementation est dangereuse car dans certains cas win_ctx est undef
  //inline View*  UpdateContext::getWinView() {return win_ctx->view;}
  //inline Display*  UpdateContext::getDisp() const {return win_ctx->view->getDisp();}
  //inline Graph* UpdateContext::getGraph() {return win_ctx->graph;}
  
}
#endif
