/* **********************************************************************
*
*  zoomImpl.hpp: semantic zooming
 *  Ubit Demos - Version 6
 *  (C) 2009 Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
*
* ***********************************************************************
* COPYRIGHT NOTICE :
* THIS PROGRAM IS DISTRIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE
* IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
* YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER THE TERMS OF THE GNU
* GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION;
* EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
* SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
* ************************************************************************/

#ifndef _zoomImpl_hpp_
#define _zoomImpl_hpp_ 1
#include <ubit/ubit.hpp>
#include <ubit/uhtml.hpp>
using namespace ubit;


struct ZBox : public UBox {
public:
  UCLASS(ZBox)
  ZBox();
  static UStyle* createStyle();
  virtual void initNode(UDoc*);
  void setGeom(float _x, float _y, float _w, float _h);
  void pack(ZBox* parent_cell);
  static void packImpl(UElem* obj, ZBox* parent_cell);
protected:
  UPos pos;
  USize size;
};

struct ZMore : public UElem {
  UCLASS(ZMore)
  virtual void initNode(UDoc*);
  UInscale inscale;
};

struct ZLess : public UElem {
  UCLASS(ZLess)
  virtual void initNode(UDoc*);
  UInscale inscale;
};

struct ZLeaf : public UElem {
  UCLASS(ZLeaf)
  virtual void initNode(UDoc*);
  UInscale inscale;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct Val : public UHtmlAttribute {
  UCLASS(Val)
};

struct Geom : public UHtmlAttribute {
  UCLASS(Geom)
  virtual void initNode(UDoc*, UElem* parent);
};

struct Shape: public UBorder {
  UCLASS(Shape)
  virtual void initNode(UDoc*, UElem* parent);
  void setValue(const UStr& val) {UHtmlAttribute::setValueImpl(pvalue, val);}
  const UStr* getValue() const {return pvalue;}
private:
  uptr<UStr> pvalue;
  std::vector<UPoint> points;
  virtual void paint(UGraph&, const UUpdateContext&, const URect&) const;  
};

#endif
