/* ***********************************************************************
 *
 *  zoom.cpp (part of udemo)
 *  A simple example of semantic zooming
 *  Ubit GUI Toolkit - Version 6
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
 
#include <iostream>
#include <cstdio>
#include <ubit/ubit.hpp>
#include <ubit/uhtml.hpp>
#include "zoomImpl.hpp"
#include "zoom.hpp"
using namespace std;
using namespace ubit;

static const float Z0 = 0.3;
static const float Z1 = 0.7;
static const float Z2 = 1.5;
static const float Z3 = 4.0;
static const float Z4 = 5.0;
static const float Z5 = 6.0;

struct MapGrammar : public UXmlGrammar {  
  MapGrammar();
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

ZoomDemo::ZoomDemo(const UStr& filename) 
// UHtmlParser uses a predefined HMTL grammar and parses code in Permissive
// and CollapseSpaces modes.
: parser(*new UHtmlParser)
{
  // MapGrammar defines a set of additional tags that are used in the example 
  // XML file returned by createDefaultMap()
  parser.addGrammar(*new MapGrammar);
  
  zpane.addAttr(UBackground::white);
  zpane.viewportPos().set(175, 125);

  // the menu will be opend when the left or right mouse buttons are pressed
  // (by default it is only opend when the right button is pressed)
  zpane.openMenuOn(UMouseEvent::LeftButton | UMouseEvent::RightButton);

  add(UOrient::vertical + uhflex() 
      + uvflex()
      + zpane
      + ubottom()
      + ulabel(uhcenter() + UColor::navy + UFont::large
               + "Press the mouse in the canvas to open the Control menu")
      );
  
  read(filename);
}  

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool ZoomDemo::read(const UStr& filename) {
  UXmlDocument* doc = null;
  
  if (filename.empty())
    doc = parser.readBuffer("ZoomMap", createDefaultMap());
  else 
    doc = parser.read(filename);
  
  if (!doc) {
    cerr << "can't parse file '" << filename << "'"<< endl;
    return false;
  }
  
  zpane.viewport().add(doc);
  ZBox::packImpl(doc, null);
  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UStyle* ZBox::createStyle() {
  UStyle* s = UBox::createStyle();
  s->orient = UOrient::HORIZONTAL;
  s->halign = UHalign::CENTER;
  s->valign = UValign::CENTER;
  s->local.border = &UBorder::line;
  return s;
}

ZBox::ZBox() : pos(0,0) {
  addAttr(pos);
  addAttr(size);
}

void ZBox::setGeom(float _x, float _y, float _w, float _h) {
  pos.set(_x, _y);
  size.set(_w, _h);
}

void ZBox::initNode(UDoc*) {}

void ZBox::pack(ZBox* parent_cell) {
  if (parent_cell) {
    pos.setX( pos.getX().val * parent_cell->size.getWidth().val / 100.);
    pos.setY( pos.getY().val * parent_cell->size.getHeight().val / 100.);
    size.setWidth( size.getWidth().val  * parent_cell->size.getWidth().val / 100.);
    size.setHeight( size.getHeight().val * parent_cell->size.getHeight().val / 100.);
  }
  packImpl(this, this);
}

void ZBox::packImpl(UElem* obj, ZBox* parent_cell) {
  for (UChildIter i = obj->cbegin(); i != obj->cend(); ++i) {
    UElem* c = (*i)->toElem();
    if (c) {
      ZBox* zc = dynamic_cast<ZBox*>(c);
      if (zc) zc->pack(parent_cell);
      else packImpl(c, parent_cell);
    }
  }
} 
  
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
static float getZoomLevel(const UStr& s) {
  if (s.empty()) return 0.;
  if (isdigit(s[0]) || s[0]=='.') return s.toFloat();

  if (s[0]=='#') {    // symbolical zoom level
    switch (s[1]) {
    case '0': return Z0;
    case '1': return Z1;
    case '2': return Z2;
    case '3': return Z3;
    case '4': return Z4;
    case '5': return Z5;
    }
  }
  cerr << "wrong zoom value: " << s << endl;
  return 0.;
}

static float FONDU = 0.3;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ZMore::initNode(UDoc*) {
  UStr val;
  getAttrValue(val, "val");
  float z = getZoomLevel(val);
  inscale.setMin(z);
  inscale.setMax(999999);

  for (UParentIter p = pbegin(); p != pend(); ++p) {
    p.parent().getChild().setCond(inscale);
  }
}

void ZLess::initNode(UDoc*) {
  UStr val;
  getAttrValue(val, "val");
  float z = getZoomLevel(val);
  inscale.setMin(0);
  inscale.setMax(z + FONDU);
  if (z != 0) addAttr(uscale(1/z));

  for (UParentIter p = pbegin(); p != pend(); ++p) {
    p.parent().getChild().setCond(inscale);
  }
}

void ZLeaf::initNode(UDoc*) {
  UStr val; 
  getAttrValue(val, "val");
  float z = getZoomLevel(val);
  inscale.setMin(z);
  inscale.setMax(999999);
  if (z != 0) addAttr(uscale(1/z));
  
  for (UParentIter p = pbegin(); p != pend(); ++p) {
    p.parent().getChild().setCond(inscale);
  }
}

void Geom::initNode(UDoc*, UElem* parent) {
  UStr val; 
  getValue(val);
  ZBox* cell = dynamic_cast<ZBox*>(parent);  
  if (!val.empty() && cell) {
    float x, y, w, h;
    sscanf(val.c_str(), "%f %f %f %f", &x, &y, &w, &h);
    cell->setGeom(x, y, w, h);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Shape::initNode(UDoc*, UElem* parent) {
  const UStr* val = getValue();  
  if (!val || val->empty()) return;

  const char* s = val->c_str();
  char *p = null;
  while (true) {
    UPoint pt;
    pt.x = strtod(s, &p);
    if (!p || p == s) return;
    s = p;
    pt.y = strtod(s, &p);
    points.push_back(pt);
    if (!p || p == s) return;
    s = p;
  }
}

void Shape::paint(UGraph& g, const UUpdateContext& props, const URect&r) const {
  const UColor* col = props.color;
  g.setColor(*col);

  // ici il faudrait faire une transformation affine
  std::vector<UPoint> pts(points.size());
  for (unsigned int k = 0; k < points.size(); k++) {
    pts[k].x = r.x + points[k].x * (r.width-1) / 100.;
    pts[k].y = r.x + points[k].y * (r.height-1) / 100.;
  }
  g.drawPolygon(pts);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct campus : public ZBox {
  UCLASS(campus)
};

struct building : public ZBox {
  UCLASS(building)
};

struct room : public ZBox {
  UCLASS(room)
};

struct classroom : public ZBox {
  UCLASS(classroom)
};

struct amphi : public ZBox {
  UCLASS(amphi)
};

struct elevator : public ZBox {
  UCLASS(elevator)
};

struct stairs : public ZBox {
  UCLASS(stairs)
};

struct restroom : public ZBox {
  UCLASS(restroom)
};

struct lab : public ZBox {
  UCLASS(lab)
};

struct hall : public ZBox {
  UCLASS(hall)
};

struct cafeteria : public ZBox {
  UCLASS(cafeteria)
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// MapGrammar defines a set of additional tags that are used in the example 
// XML file returned by createDefaultMap()

MapGrammar::MapGrammar() {  
  //addElementClass(hbox::Class());
  //addElementClass(vbox::Class());

  addElementClass(ZBox::Class());
  addElementClass(ZMore::Class());
  addElementClass(ZLess::Class());
  addElementClass(ZLeaf::Class());
  addAttrClass(Geom::Class());
  addAttrClass(Val::Class());
  addAttrClass(Shape::Class());

  addElementClass(campus::Class());
  addElementClass(building::Class());
  addElementClass(room::Class());
  addElementClass(classroom::Class());
  addElementClass(amphi::Class());
  addElementClass(elevator::Class());
  addElementClass(stairs::Class());
  addElementClass(restroom::Class());
  addElementClass(lab::Class());
  addElementClass(hall::Class());
  addElementClass(cafeteria::Class());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UStr& ZoomDemo::createDefaultMap() {
  return *new UStr
  (
"<xml> \
\
<head>\
<!-- <link href='map.css' rel='stylesheet' type='text/css'> -->\
<style>\
em        {color: red; font-weight: bold;}\
campus    {color: navy; font-size: 16pt; font-weight: bold;}\
building  {background-color: #e4e4e4;}\
building[id=toto]  {background-color: orange;}\
room      {color: navy; background-color: grey; font-weight: medium;}\
lab       {color: blue; background-color: #ddddff;}\
amphi     {color: blue;}\
classroom {color: white;  background-color: #88aa88;}\
restroom  {color: white;  background-color: blue;}\
zbox      {color: black;}\
stairs    {color: red; background-color: yellow;}\
elevator  {color: black; background-color: pink;}\
cafeteria {color: white; background-color: orange;}\
</style>\
</head>\
\
<body>\
<campus geom='0 0 300 200' id='Campus' border='none'\
shape='0 0  100 0  100 50  80 70  80 100  10 100  10 30  0 30'>\
<zmore val='#1'>\
\
<building geom='0 0 100 30' id='1'>\
<zmore val='#2'>\
\
<classroom geom='0 0 20 50' id='1.1'>\
<zless val='#3'> Classroom 1.1 </zless>\
<zleaf val='#3'> <img src='../images/madeleine.gif'/> </zleaf>\
</classroom>\
\
<classroom geom='20 0 20 50' id='1.2'>\
<zless val='#3'> Classroom 1.2 </zless>\
<zleaf val='#3'> <img src='../images/mosquee.gif'/> </zleaf>\
</classroom>\
\
<lab geom='40 0 30 50' id='1.3'>\
<zless val='#3'> InfoLab </zless>\
<zleaf val='#3'> <img src='../images/louvre.gif'/> </zleaf>\
</lab>\
\
<classroom geom='70 0 10 50' id='1.4'>\
<zless val='#3'> Classr 1.4 </zless>\
<zleaf val='#3'> <img src='../images/mosquee.gif'/> </zleaf>\
</classroom>\
\
<restroom geom='80 0 10 50' id='1.5'>\
<zless val='#3'> <i> Restroom </i> </zless>\
<zleaf val='#3'> <img src='../images/cluny.gif'/> </zleaf>\
</restroom>\
\
<stairs geom='90 0 10 50' id='1.6a'>\
<zless val='#3'> <b> Staircase </b> </zless>\
<zleaf val='#3'> <img src='../images/forum-des-halles.gif'/> </zleaf>\
</stairs>\
\
<!-- ............ -->\
\
<stairs geom='0 50 10 50' id='1.6b'>\
<zless val='#3'> <b> Staircase </b> </zless> \
<zleaf val='#3'> <img src='../images/bhv.gif'/> </zleaf> \
</stairs> \
\
<lab geom='15 70 10 30' id='1.7'> \
<zless val='#3'> Lab 1.7 </zless> \
<zleaf val='#3'> <img src='../images/tour-eiffel.gif'/> </zleaf> \
</lab> \
\
<lab geom='25 70 10 30' id='1.8'> \
<zless val='#3'> Lab 1.8 </zless> \
<zleaf val='#3'> <img src='../images/bon-marche.gif'/> </zleaf> \
</lab> \
\
<lab geom='35 70 10 30' id='1.9'> \
<zless val='#3'> Lab 1.9 </zless> \
<zleaf val='#3'> <img src='../images/sainte-chapelle.gif'/> </zleaf> \
</lab> \
\
<room geom='45 70 5 30' id='1.10'> \
<zless val='#3'> R 1.10 </zless> \
<zleaf val='#3'> <img src='../images/saint-germain.gif'/> </zleaf> \
</room> \
\
<room geom='50 70 5 30' id='1.11'>  \
<zless val='#3'> R 1.11 </zless> \
<zleaf val='#3'> <img src='../images/sacre-coeur.gif'/> </zleaf> \
</room> \
\
<room geom='55 70 5 30' id='1.12'>  \
<zless val='#3'> R 1.12 </zless> \
<zleaf val='#3'> <img src='../images/sacre-coeur.gif'/> </zleaf> \
</room> \
\
<room geom='60 70 5 30' id='1.13'>  \
<zless val='#3'> R 1.13 </zless> \
<zleaf val='#3'> <img src='../images/sacre-coeur.gif'/> </zleaf> \
</room> \
\
<classroom geom='65 70 15 30' id='1.14'>  \
<zless val='#3'> Classroom 1.14  </zless> \
<zleaf val='#3'> <img src='../images/orsay.gif'/> </zleaf> \
</classroom> \
\
<classroom geom='87 70 13 30' id='1.15'>  \
<zless val='#3'> Classroom 1.15  </zless> \
<zleaf val='#3'> <img src='../images/orsay.gif'/> </zleaf> \
</classroom> \
\
<elevator geom='95 40 5 30' id='1.16'> \
<zless val='#3'> <b> Elevator </b> </zless> \
<zleaf val='#3'> <img src='../images/printemps.gif'/> </zleaf> \
</elevator> \
\
</zmore> \
\
<!-- ....attention les objets sont affiches dans l ordre ..... --> \
\
<zless val='#2'> <em>Building 1</em> </zless> \
\
</building> \
\
<!-- ...................................................... --> \
\
<building geom='15 40 60 25' id='2'> \
<zmore val='#2'> \
\
<classroom geom='0 0 20 50' id='2.1'> \
<zless val='#3'> Classroom 2.1  </zless> \
<zleaf val='#3'> <a>'aaaa'</a> </zleaf> \
</classroom> \
\
<stairs geom='30 0 10 50' id='2.2'>  \
<zless val='#3'> Stairs  </zless> \
<zleaf val='#3'> <a>'cc cccc'</a> </zleaf> \
</stairs> \
\
<classroom geom='40 0 30 50' id='2.3'> \
<zless val='#3'> Classroom 2.3  </zless> \
<zleaf val='#3'> <a>'dd ddd ddd'</a> </zleaf> \
</classroom> \
\
<classroom geom='70 0 30 50' id='2.4'> \
<zless val='#3'> Classroom 2.4 </zless> \
<zleaf val='#3'> <a>'ee ee'</a> </zleaf> \
</classroom> \
\
<!-- ............ --> \
\
<classroom geom='0 50 20 50' id='2.5'> \
<zless val='#3'> Classroom 2.5 </zless> \
<zleaf val='#3'> <a>'zzzz'</a> </zleaf> \
</classroom> \
\
<restroom geom='30 70 20 30' id='2.6'> \
<zless val='#3'> Restroom </zless> \
<zleaf val='#3'> <a>'uuuu'</a> </zleaf> \
</restroom> \
\
<room geom='50 70 10 30' id='2.7'> \
<zless val='#3'> Room 2.7  </zless> \
<zleaf val='#3'> <img src='../images/orsay.gif'/> </zleaf> \
</room> \
\
<room geom='60 70 10 30' id='2.8'>  \
<zless val='#3'> Room 2.8 </zless> \
<zleaf val='#3'> <img src='../images/printemps.gif'/> </zleaf> \
</room> \
\
<stairs geom='70 70 20 30' id='2.9'>  \
<zless val='#3'> Staircase  </zless> \
<zleaf val='#3'> <img src='../images/tour-eiffel.gif'/> </zleaf> \
</stairs> \
\
</zmore> \
\
<!-- ............ --> \
<zless val='#2'> <em>Building 2</em> </zless> \
\
</building>  \
\
<!-- ...................................................... --> \
\
<building geom='10 75 60 25' id='3'> \
<zmore val='#2'> \
\
<cafeteria geom='0 0 40 100' id='3.1'> \
<zless val='#3'> Cafeteria </zless> \
<zleaf val='#3'> <a>'aaaa'</a> </zleaf> \
</cafeteria> \
\
<room geom='30 0 20 50' id='3.2'>  \
<zless val='#3'> Room 3.2  </zless> \
<zleaf val='#3'> <a>'cc cccc'</a> </zleaf> \
</room> \
\
<lab geom='50 0 30 50' id='3.3'> \
<zless val='#3'> Lab 3.3 </zless> \
<zleaf val='#3'> <img src='../images/forum-des-halles.gif'/> </zleaf> \
</lab> \
\
<restroom geom='80 0 10 50' id='3.4'> \
<zless val='#3'>restroom</zless> \
<zleaf val='#3'> <a>'dd ddd ddd'</a> </zleaf> \
</restroom> \
\
<stairs geom='90 0 10 50' id='3.4'> \
<zless val='#3'>Stairs</zless> \
<zleaf val='#3'> <a>'dd ddd ddd'</a> </zleaf> \
</stairs> \
\
<!-- ............ --> \
\
<room geom='50 70 20 30' id='3.5'> \
<zless val='#3'> Room 3.5 </zless> \
<zleaf val='#3'> <a>'vvv vv'</a> </zleaf> \
</room> \
\
<room geom='70 70 20 30' id='3.6'>  \
<zless val='#3'> Room 3.6 </zless> \
<zleaf val='#3'> <a>'ttttt'</a> </zleaf> \
</room> \
\
<elevator geom='90 70 10 30' id='3.7'>  \
<zless val='#3'> Elevator </zless> \
<zleaf val='#3'> <a>'elevator'</a> </zleaf> \
</elevator> \
\
</zmore> \
\
<!-- ............ --> \
<zless val='#2'> <em>Building 3</em> </zless> \
</building> \
</zmore> \
\
<!-- ............ --> \
<zless val='#1' style='color:orange'> <b>Campus</b> </zless> \
\
</campus> \
\
</body> \
</xml>");
}

