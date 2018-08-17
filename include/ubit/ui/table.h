/*
 *  table.hpp
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

#ifndef _utable_hpp_
#define	_utable_hpp_ 1

#include <vector>
#include <ubit/ubox.hpp>
#include <ubit/uboxes.hpp>

namespace ubit {
  
  /** UTable = class for creating (simplified) HTML like tables.
   * See: UTrow, UTcell
   */
  class UTable: public Box {
  public:
    UCLASS(UTable)
    
    UTable(Args a = Args::none): Box(a) {}
    ///< creates a new table widget (@see also shortcut utable()).
    
    static UStyle* createStyle();
  };
  
  inline UTable& utable(Args a = Args::none) {return *new UTable(a);}
  ///< shortcut that returns *new UTable().
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** Table Row.
   *  NOTE: MUST be a child of a UTable object
   *  - See also: UTable, UTcell
   */
  class UTrow: public Element {
  public:
    UCLASS(UTrow)
    
    UTrow(Args a = Args::none): Element(a) {}
    ///< creates a new table row (@see also shortcut utrow()).
    
    static UStyle* createStyle();
  };
  
  inline UTrow& utrow(Args a = Args::none) {return *new UTrow(a);}
  ///< shortcut that returns *new UTrow().
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** Table Cell.
   *  NOTE: MUST be a child of a UTrow object
   *  - See also: UTable, UTrow
   */
  class UTcell: public UFlowbox {
  public:
    UCLASS(UTcell)
    
    UTcell(Args a = Args::none);
    ///< creates a new table cell (@see also shortcut utcell()).
    
    UTcell(short colspan, Args a = Args::none);
    ///< creates a new table cell (@see also shortcut utcell()).
    
    UTcell(short colspan, short rowspan, Args a = Args::none);
    ///< creates a new table cell (@see also shortcut utcell()).
    
    static UStyle* createStyle();
    
    void  setColspan(short);
    void  setRowspan(short);
    short getColspan() {return colspan;}
    short getRowspan() {return rowspan;}
    
  private:
    short colspan, rowspan;
  };
  

  inline UTcell& utcell(Args a = Args::none) {return *new UTcell(a);}
  ///< shortcut that returns *new UTcell().
  
  inline UTcell& utcell(short colspan, Args a = Args::none) {return *new UTcell(colspan,a);}
  ///< shortcut that returns *new UTcell().
  
  inline UTcell& utcell(short colspan, short rowspan, Args a = Args::none) {return *new UTcell(colspan,rowspan,a);}
  ///< shortcut that returns *new UTcell().
  
  
  
  struct UViewCell {
    UViewCell();
    float d, min_d, max_d, spec_d, percent;
    int rowspan, colspan;
  };
  
  class UTableView: public View {
  public:
    static ViewStyle style;  // renderer
    virtual ViewStyle* getViewStyle() {return &style;}
    
    UTableView(Box*, View* parview, UHardwinImpl*);
    virtual ~UTableView();
    
    static View* createView(Box*, View* parview, UHardwinImpl*);
    virtual UTableView* toTableView() {return this;}
    
#ifndef NO_DOC
    std::vector<UViewCell> cols, lines;
    int lcur, ccur; 
    int ccount, lcount;
    virtual bool doLayout(UpdateContext&, class UViewLayout&);
    static void tableDoLayout(class UTableLayoutImpl&, UpdateContext& ctx, 
                              Element& grp, UViewLayout&);
    static void rowDoLayout(View* row_view, class UTableLayoutImpl&, 
                            UpdateContext& parctx, Element& grp, UViewLayout&);
#endif
  };

}
#endif
