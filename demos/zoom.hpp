/* ***********************************************************************
 *
 *  zoomdemo.hpp (part of udemo)
 *  A simple example of semantic zooming
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

#ifndef _zoomdemo_hpp_
#define _zoomdemo_hpp_ 1
#include <ubit/ubit.hpp>
using namespace ubit;

class ZoomDemo : public UBox {
public:
  ZoomDemo(const UStr& filename = "");
  bool read(const UStr& filename);

private:
  UZoompane zpane;             // working area that will be zoomed and paned 
  class UXmlParser& parser;     // for parsing XML map files 
  UStr& createDefaultMap();
};

#endif
