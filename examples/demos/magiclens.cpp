/* ***********************************************************************
 *
 *  magiclens.cpp : magic lens demo
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
 * ***********************************************************************/

#include <iostream>
#include <ubit/uxpm.hpp>
#include <ubit/uflag.hpp>
#include "magiclens.hpp"
using namespace std;
using namespace ubit;

// NOTE that flags MUST be declared as consts
const UFlag SiteFlag, MuseumFlag, StoreFlag, StationFlag;
const UFlag ImageFlag, DetailFlag;
extern MapElem *station_data, *store_data, *museum_data, *site_data;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class Note : public UPalette {
public:
  Note(const char* title);
  UColor text_color;
};


class Portal : public UPalette {
public:
  Portal(const char* title, UBox& scene);
};


class Magiclens : public UPalette {
public:
  Magiclens(const char* title, UBox& scene);
private:
  UScrollpane scrollpane;
  UFlagdef show_topic, show_images, show_details;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

MagicLensDemo::MagicLensDemo() 
{
  UBox& toolbar = ubar
    (uitem(upix(UXpm::colors) + "Lens"  + ucall(this, 2, &MagicLensDemo::addLens))
     + uitem(upix(UXpm::windows)+ "Portal"+ ucall(this, 1, &MagicLensDemo::addLens))
     + uitem(upix(UXpm::edit)   + "Notes" + ucall(this, 0, &MagicLensDemo::addLens))
     + uitem(upix(UXpm::windows) + "Overview" + umenu(uscale(0.33) + scene_container))
     + uhflex() + ubox("")
     + uright()
     + ubox("Controls" 
             + UOn::select / upix(UXpm::rightarrow)
             + UOn::deselect / upix(UXpm::leftarrow)
             + UOn::select / ushow(controls, false)
             + UOn::deselect / ushow(controls, true)
             )
     );
  
  // layers
  
  UElem& stores_layer =
  makeElems(store_data,
            StoreFlag,
            UFont::bold 
            + ualpha(0.35) + UBackground::blue
            + UOn::enter / UBackground::black
            + UColor::black + UOn::enter / UColor::red
            + UBorder::none
            );
  
  UElem& museums_layer =
  makeElems(museum_data,
            MuseumFlag,
            UFont::bold + UBorder::none
            + UColor::red + UOn::enter / UColor::white
            + UBackground::none + UOn::enter / UBackground::black
            );
  
  UElem& sites_layer =
  makeElems(site_data,
            SiteFlag,
            UFont::bold + ualpha(0.75)
            + UColor::navy + UOn::enter/UColor::yellow
            + UBackground::none + UOn::enter/UBackground::teal
            + UBorder::none
            );
  
  UElem& stations_layer =
  makeElems(station_data,
            StationFlag,
            UFont::bold
            + UColor::white
            + UBackground::orange + UOn::enter / UBackground::red
            );
  
  // scene
  UFlagdef& fdef1 = uflagdef();
  UFlagdef& fdef2 = uflagdef(ImageFlag);  // default: shows images
  UFlagdef& fdef3 = uflagdef();

  scene.add(scene_scale
            // GLOBAL Flag Defs (!= from the internal lens flags)
            + fdef1 + fdef2 + fdef3
            // background layer = the physical map
            + uima("paris-metro.gif")
            // flagged layers
            + stores_layer + museums_layer + sites_layer + stations_layer
            );
  
  controls
  .add(UOrient::vertical + upadding(5,UIGNORE) 
       + utop()
       + ulabel(UFont::bold + "Show")
       + ulistbox(uitem("Nothing" + UOn::select / uset(fdef1, UFlag::none))
                  + uitem("Museums"        + UOn::select / uset(fdef1, MuseumFlag))
                  + uitem("Sites"          + UOn::select / uset(fdef1, SiteFlag))
                  + uitem("Dept. Stores"   + UOn::select / uset(fdef1, StoreFlag))
                  + uitem("Train Stations" + UOn::select / uset(fdef1, StationFlag)))
       + " "
       + ulabel(UFont::bold + "With")
       + uvbox(ucheckbox("Images"
                         + UOn::select     / uset(fdef2, ImageFlag)
                         + UOn::deselect   / uset(fdef2, UFlag::none))
               + ucheckbox("Details"
                           + UOn::select   / uset(fdef3, DetailFlag)
                           + UOn::deselect / uset(fdef3, UFlag::none)))
       + " "
       + ulabel(UFont::bold + "Lenses")
       + uvbox(ubutton("Lens" + ucall(this, 2, &MagicLensDemo::addLens))
               + ubutton("Portal" + ucall(this, 1, &MagicLensDemo::addLens))
               + ubutton("Notes" + ucall(this, 0, &MagicLensDemo::addLens)))
       );
  
  //controls.show(false);
  
  // NOTE: scene_container avoids to create a loop in the instance graph
  // when lenses are added
  scene_container.add(uleft() + utop() + upadding(0,0)
                          + uhspacing(0) + uvspacing(0) + scene);
  scene_scroll.add(scene_container);

  UBox& working_area = uhbox
    (uvflex()
     // 'scene_scroll' is flexible in vert and horiz directions
     + uhflex() + scene_scroll 
     // 'controls'is flexible in vert direction only
     + uright() + controls
     );

  // add to MainWin

  this->add
    (uhflex()
     // flexible in horiz direction only
     + utop() + toolbar
     // 'working_area':flexible in vert and horiz directions
     + uvflex() + working_area
    );
  
  // set scrollbars in the middle of the scrollpane
  scene_scroll.getHScrollbar()->setValue(50.);
  scene_scroll.getVScrollbar()->setValue(50.);  
}

/* ==================================================== ======== ======= */

class ZElem : public UButton  {
public:
  UPos pos;
  ZElem(MagicLensDemo* lm, const UFlag& flag, MapElem& mel, const UArgs& args);
};


ZElem::ZElem(MagicLensDemo* lm, const UFlag& flag, MapElem& mel, const UArgs& args)
: pos(0,0){
  USymbol* sym = null;
  const char* p = strchr(mel.title, '$');
  if (p) {
    switch(p[1]) {
    case '<': sym = &USymbol::left ; break;
    case '>': sym = &USymbol::right ; break;
    case '^': sym = &USymbol::up ; break;
    case 'v': sym = &USymbol::down ; break;
    }
  }
  
  UElem* t = null;
  if (!p) t = &uhbox(mel.title).ignoreEvents();
  else {
    int ldeb = p - mel.title + 1;
    char* deb = new char[ldeb];
    strncpy(deb, mel.title, ldeb-1); deb[ldeb-1] = 0;
    
    t = &uhbox(ustr(deb) + sym + ustr(p+2)).ignoreEvents();
    delete [] deb;
  }
  
  pos.set(mel.x, mel.y);

  add
  (pos
   + UOrient::vertical + args + t
   + ImageFlag / uima(mel.image)
   + DetailFlag / uelem(UFont::small + mel.details)
   );
  
}

/* ==================================================== ======== ======= */

UElem& MagicLensDemo::makeElems(MapElem tab[], const UFlag& flag, const UArgs& args)
{
  UElem* e = new UElem();  
  for (int k = 0; tab[k].name != null; k++) {
    e->add(flag / new ZElem(this, flag, tab[k], args));
  }
  return *e;
}

/* ==================================================== ======== ======= */

void MagicLensDemo::addLens(int lenstype) {
  UPalette* lens = null;
  switch (lenstype) {
  case 0:
    lens = new Note("Note");
    break;
  case 1:
    lens = new Portal("Portal", scene);
    break;
  case 2:
    lens = new Magiclens("Magic Lens", scene);
    break;
  }
  
  static UPoint p(150,150);
  p.x += 15;
  p.y += 15;
  lens->pos() = p;
  
  // NOTE: the Lens is added to scene_container, not scene itself, because this would
  //  make a loop in the instance graph (as the Lens also points to the scene)
  scene_container.add(lens);
}

/* ==================================================== ======== ======= */
/*
void Lens::deleteLens() {
  // second arg == true ==> will also destroy this object (except if shared)
  //map.scene_container.remove(this, true);
  this->show(false);
  delete this;
}
*/

Note::Note(const char* _title) {
  title().add(_title);
  titleBar().addAttr(ualpha(0.6) + UBackground::navy + UColor::white
                     + UFont::bold + upadding(3,UIGNORE));
  text_color = UColor::red;
  
  content().add(UOrient::vertical
                + utextarea(usize(125, 100) + ualpha(0.50) + UBackground::wheat 
                            + text_color + UFont::bold 
                            + "you can write your notes here")
                + uhbox(UFont::x_small
                        + ubutton(UBackground::black + "  "  
                                  + uassign(text_color, UColor::black))
                        + ubutton(UBackground::navy  + "  "  
                                  + uassign(text_color, UColor::navy))
                        + ubutton(UBackground::red   + "  "  
                                  + uassign(text_color, UColor::red))
                        + ubutton(UBackground::green + "  "  
                                  + uassign(text_color, UColor::green)))
                );
}


Portal::Portal(const char* _title, UBox& scene) {
  title().add(_title);
  titleBar().addAttr(ualpha(0.6) + UBackground::navy + UColor::white
                     + UFont::bold + upadding(3,UIGNORE));
  title().add(uscrollpane(usize(180, 150) + scene));
}


Magiclens::Magiclens(const char* _title, UBox& scene) {
  title().add(_title);
  titleBar().addAttr(ualpha(0.6) + UBackground::navy + UColor::white
                     + UFont::bold + upadding(3,UIGNORE));

  setPosControlModel(&(new UMagicLensControl)->setModel(&pos(), &scrollpane));
  
  show_topic.set(SiteFlag);
  show_images.set(ImageFlag);
  show_details.clear();

  scrollpane.add(usize(200, 170) + upadding(0,0)
                 // NOTE: LOCAL defs of the Lens (!= from the global defs of the scene)
                 + show_topic + show_images + show_details + scene);
  scrollpane.getVScrollbar()->show(false);
  scrollpane.getHScrollbar()->show(false);

  content()
  .add(scrollpane
       + ulistbox(UOrient::horizontal + ualpha(0.5) + UBorder::none
                  + UFont::bold + UColor::navy
                  + uitem("Museums" + UOn::select / uset(show_topic,MuseumFlag))
                  + uitem("Sites" + UOn::select / uset(show_topic,SiteFlag))
                  + uitem("Stores" + UOn::select / uset(show_topic,StoreFlag))
                  + uitem("Stations" + UOn::select / uset(show_topic,StationFlag))
                  )
       );
}

/* ==================================================== ======== ======= */

MapElem _Stores[] = {
  {
    "Printemps",
    "Printemps",
    "64 bd Haussmann, 9e",
    "printemps.gif",
    314, 200
  },
  {
    "Galeries Lafayette",
    "Gal. Lafayette",
    "40 bd Haussmann, 9e",
    "galeries-lafayettes.gif",
    410, 195
  },
  {
    "Bon Marche",
    "Bon Marche",
    "24 rue de Sevres, 7e",
    "bon-marche.gif",
    300, 438
  },
  {
    "Forum des Halles",
    "Forum des Halles",
    "M. Halles, 1e",
    "forum-des-halles.gif",
    490, 305
  },
  {
    "Samaritaine",
    "Samaritaine",
    "rue de la Monnaie, 1e",
    "samaritaine.gif",
    405, 350
  },
  {
    "BHV",
    "BHV",
    "140 rue du Bac, 7e",
    "bhv.gif",
    500, 355
  },
  {null, null}
};

MapElem* store_data = _Stores;

/* ==================================================== ======== ======= */

MapElem _Museums[] = {
  {
    "Musee du Louvre",
    "v Musee du Louvre",
    "rue de Rivoli, 1e",
    "louvre.gif",
    417, 327
  },
  {
    "Musee d'Orsay",
    "^ Musee d'Orsay",
    "7 quai Anatole France, 7e",
    "orsay.gif",
    362, 395,
  },
  {
    "Centre Pompidou",
    "v Centre Pompidou",
    "19 rue Beaubourg, 4e",
    "pompidou.gif",
    525, 323,
  },
  {
    "Musee Picasso",
    "< Musee Picasso",
    "5 rue Thorigny, 3e",
    "picasso.gif",
    578, 360
  },
  {
    "Musee Rodin",
    "Musee Rodin =>",
    "77 rue de Varenne, 7e",
    "rodin.gif",
    245, 384,
  },
  {
    "Thermes de Cluny",
    "< Thermes de Cluny",
    "6 place Painleve, 5e",
    "cluny.gif",
    470, 470
  },
  {null, null}
};  

MapElem* museum_data = _Museums;

/* ==================================================== ======== ======= */

MapElem _Sites[] = {
  {
    "Tour Eiffel",
    "< Tour Eiffel",
    null,
    "tour-eiffel.gif",
    219, 415
  },
  {
    "Notre Dame",
    "^ Notre Dame",
    null,
    "notre-dame.gif",
    478, 407
  },
  {
    "Sainte Chapelle",
    "Sainte Chapelle",
    null,
    "sainte-chapelle.gif",
    417, 400
  },
  {
    "Sacre Coeur",
    "^ Sacre Coeur",
    null,
    "sacre-coeur.gif",
    460, 100
  },
  {
    "Mosquee de Paris",
    "< Mosquee de Paris",
    null,
    "mosquee.gif",
    549, 530
  },
  {
    "Madeleine",
    "< Madeleine",
    null,
    "madeleine.gif",
    365, 290
  },
  {null, null}
};  

MapElem* site_data = _Sites;

/* ==================================================== ======== ======= */

MapElem _Stations[] = {
  {
    "Gare Austerlitz",
    "Gare Austerlitz",
    null,
    null,
    589, 490
  },
  {
    "Gare de l'Est",
    "Gare de l'Est",
    null,
    null,
    555, 200
  },
  {
    "Gare St Lazare",
    "Gare St Lazare",
    null,
    null,
    364, 210
  },
  {
    "Gare de Lyon",
    "Gare de Lyon",
    null,
    null,
    655, 476
  },
  {
    "Gare Montparnasse",
    "Gare Montparnasse",
    null,
    null,
    369, 519
  },
  {
    "Gare du Nord",
    "Gare du Nord",
    null,
    null,
    553, 155
  },
  {null, null}
};  

MapElem* station_data = _Stations;
