#include <ubit/ubit.hpp>
#include <iostream>
using namespace std;
using namespace ubit;


class SpeedList : public UFrame {
public:
  enum DispMode {NONE, STATIC, DYNAMIC};
  enum ItemMode {UNIFORM, LAST, D2L, L2D};
  
  SpeedList(const char* item_strings[], int nb_items_per_view);
  void createItems(const char* item_strings[]);
  void setDispMode(DispMode);
  void setItemFGMode(ItemMode);
  void setItemBGMode(ItemMode);

  void testCB1(int& i) {cerr << "val int : " << ++(i) << endl;}
  int testint;
  
  void testCB2(UColor& c) {cerr << "val color : " << &c << endl;}
  UColor testcol;
  
private:
  int item_count_perview;
  DispMode disp_mode;
  ItemMode fg_mode, bg_mode;
  float min_fg, max_fg, min_bg, max_bg;  // entre 0. et 100.
  unsigned long clear_timeout;
  UDialog settings;
  UListbox list;
  UScrollpane spane;
  //UHeight spane_height;
  USize spane_size;
  UCtlmenu cmenu;
  UTimer timer;
  
  void clearList();
  void highlightList(bool up2down = true);  
  void timerCB(UTimerEvent&);
  void setColorCB(UEvent&, float* val);
  void keyPressCB(UKeyEvent& e);
  void mousePressCB(UMouseEvent&);
  void wheelCB(UWheelEvent&);
  void resizeFrameCB(UResizeEvent&);
  void paintListCB(UPaintEvent&);
};


class Item : public UItem {
public:
  Item(UArgs);
  void setFG(float value);  // entre 0. et 100.
  void setBG(float value);
private:
  UColor fg, bg;
  UBackground background;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int main(int argc, char** argv) {
  extern const char* item_set[];
  UAppli app(argc, argv);
  SpeedList sp(item_set, 20);
  app.start();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Item::Item(UArgs args) : UItem(args) {
  fg.setRgbaI(0, 0, 0);
  bg.setRgbaI(255, 255, 255);
  background.setColor(bg);
  addAttr(fg + background);
}

void Item::setFG(float value) {
  unsigned int col = 255 * value / 100.;
  fg.setRgbaI(col, col, col);
}

void Item::setBG(float value) {
  int col = 255 * value / 100.;
  bg.setRgbaI(col, col, 255);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

SpeedList::SpeedList(const char* item_strings[], int nb_items_per_view) : 
item_count_perview(nb_items_per_view),
//from_item(-1),  // -1 means all items
disp_mode(NONE),
fg_mode(UNIFORM), bg_mode(UNIFORM),
min_fg(0), max_fg(50),
min_bg(50), max_bg(100),
clear_timeout(1000),
spane(true, false)
{
  timer.onAction(ucall(this, &SpeedList::timerCB));
  
  list.addAttr(upadding(0,0));
   // necessaire pouer ajuster la taille au nombre d'items
  list.addAttr(UOn::paint / ucall(this, &SpeedList::paintListCB));
  createItems(item_strings);

  //taille approximative raejustee plus tard dans paintListCB()
  //spane_height = nb_items_per_view* 15;
  spane_size.setHeight(nb_items_per_view* 15);
  spane.setAttr(spane_size);
  spane.add(list);
  
  // !!!BUG ::: UOn::kpress effectif que si UOn::mpress !!!!!
  list.catchEvents(UOn::kpress / ucall(this, &SpeedList::keyPressCB));
  list.catchEvents(UOn::mpress / ucall(this, &SpeedList::mousePressCB));
  spane.catchEvents(UOn::wheel / ucall(this, &SpeedList::wheelCB));
  
  UScrollAction* sa = new UScrollAction(spane);
  cmenu.item(2).add(UPix::bigUp + sa);
  cmenu.item(6).add(UPix::bigDown + sa);
  add(cmenu);
  
  add(spane);
  addAttr(UOn::resize / ucall(this, &SpeedList::resizeFrameCB));

  // - - - settings dialog
  
  UBox& fg_modes = uradiobox
  (ubutton("none" + ucall(this, UNIFORM, &SpeedList::setItemFGMode))
   + ubutton("last" + ucall(this, LAST, &SpeedList::setItemFGMode))
   + ubutton("A->B" + ucall(this, L2D, &SpeedList::setItemFGMode))
   + ubutton("B->A" + ucall(this, D2L, &SpeedList::setItemFGMode))
   ).select(0);
  fg_mode = UNIFORM;

  UBox& bg_modes = uradiobox
  (ubutton("none" + ucall(this, UNIFORM, &SpeedList::setItemBGMode))
   + ubutton("last" + ucall(this, LAST, &SpeedList::setItemBGMode))
   + ubutton("A->B" + ucall(this, L2D, &SpeedList::setItemBGMode))
   + ubutton("B->A" + ucall(this, D2L, &SpeedList::setItemBGMode))
   ).select(2);
  bg_mode = L2D;

  UBox& viewport_modes = uradiobox
  (ubutton("None" + ucall(this, NONE, &SpeedList::setDispMode))
   + ubutton("Static" + ucall(this, STATIC, &SpeedList::setDispMode))
   + ubutton("Dynamic" + ucall(this, DYNAMIC, &SpeedList::setDispMode)) 
   ).select(0);
  disp_mode = NONE;

  settings.add
  (utitle("Settings")
   + upadding(20,20) + uvspacing(6)
   + UOn::close / ushow(settings, true)  // reopen if "close" button is pressed

   + ulabel(UFont::bold + "Text")
   + uhbox("Type: " + fg_modes)
   + uhbox("A: dark "
           + uhslider(ucall(this,&max_fg, &SpeedList::setColorCB)).setValue(max_fg)
           + " bright")
   + uhbox("B: dark " 
           + uhslider(ucall(this,&min_fg, &SpeedList::setColorCB)).setValue(min_fg)
           + " bright")
 
   + " " 
   + ulabel(UFont::bold + "Background")
   + uhbox("Type: " + bg_modes)
   + uhbox("A: dark " 
           + uhslider(ucall(this,&max_bg, &SpeedList::setColorCB)).setValue(max_bg)
           + " bright")
   + uhbox("B: dark "
           + uhslider(ucall(this,&min_bg, &SpeedList::setColorCB)).setValue(min_bg)
           + " bright")
   
   + " " 
   + uhbox(ulabel(UFont::bold + "Mode") + viewport_modes)
   
   + ubutton("Test1" + ucall(this, testint, &SpeedList::testCB1))
   + ubutton("Test2" + ucall(this, testcol, &SpeedList::testCB2))
   );

  show();
  settings.show();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SpeedList::createItems(const char* item_strings[]) {
  list.removeAll();
  int no = 0;
  
  for (int k = 0; item_strings[k] != null; ++k, ++no) {
    if (no == item_count_perview) no = 0;
    list.add(new Item(item_strings[k]));
   }
  
  // completer pour avoir un nombre total d'items multiple de item_count_perview
  for (; no < item_count_perview; ++no) {
    list.add(new Item(""));
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SpeedList::setItemFGMode(ItemMode m) {
  fg_mode = m;
  highlightList();
}

void SpeedList::setItemBGMode(ItemMode m) {
  bg_mode = m;
  highlightList();
}

void SpeedList::setColorCB(UEvent& e, float* val) {
  USlider* s = dynamic_cast<USlider*>(e.getSource());
  if (!s) return;
  *val = s->getValue();
  highlightList();
}

void SpeedList::timerCB(UTimerEvent&) {
  //cerr << "timeout " << endl;
  clearList();
  //from_item = -1;
}

void SpeedList::setDispMode(DispMode m) {
  disp_mode = m;
  //from_item = -1;
  if (disp_mode == STATIC) highlightList();
  else clearList();
}

void SpeedList::clearList() {
  //cerr <<"clearList" << endl;
  for (UChildIter c = list.cbegin(); c != list.cend(); ++c) {
    Item* item = dynamic_cast<Item*>(*c);
    if (!item) continue;
    item->setFG(0);
    item->setBG(100);
  }
  list.repaint();
}

void SpeedList::highlightList(bool up2down) {
  //cerr <<"highlightList" << endl;
  if (disp_mode == NONE) {
    clearList();
    return;
  }
  
  int first, last;
  if (disp_mode == STATIC) {
    first = 0;
    last = list.children().size();
  }
  else {
    // the no of the first item visible in the viewport
    first = (list.children().size()-item_count_perview) * spane.getYScroll()/100.;
    last = first + item_count_perview;
  }
  
  int k = 0, no = 0;
  
  for (UChildIter c = list.cbegin(); c != list.cend(); ++c, ++k) {
    Item* item = dynamic_cast<Item*>(*c);
    if (!item) continue;
    
    if (k < first || k >= last) {
      item->setFG(0);
      item->setBG(100);
      continue;
    }

    if (no == item_count_perview) {
      no = 0;
    }
    
    float rank = float(no+1) / item_count_perview;
    
    switch(fg_mode) {
      case UNIFORM:
        item->setFG(0);
        break;
      case LAST:
        if (no == item_count_perview-1) item->setFG(50); 
        else item->setFG(0);
        break;
      case D2L:
        if (up2down) item->setFG(min_fg + (max_fg-min_fg) * rank);
        else item->setFG(max_fg - (max_fg-min_fg) * rank);
      break;
        case L2D:
        if (up2down) item->setFG(max_fg - (max_fg-min_fg) * rank);
        else item->setFG(min_fg + (max_fg-min_fg) * rank);
        break;
    }
    
    switch(bg_mode) {
      case UNIFORM:
        item->setBG(100);
        break;
      case LAST:
        if (no == item_count_perview-1) item->setBG(50); 
        else item->setBG(100);
        break;
      case D2L:
        if (up2down) item->setBG(min_bg + (max_bg-min_bg) * rank);
        else item->setBG(max_bg - (max_bg-min_bg) * rank);
        break;
      case L2D:
        if (up2down) item->setBG(max_bg - (max_bg-min_bg) * rank);
        else item->setBG(min_bg + (max_bg-min_bg) * rank);
        break;
    }
    
    no++;
  }
  
  list.repaint();
  if (disp_mode == DYNAMIC) timer.start(clear_timeout, 1);
}


/*
void SpeedList::setColorCB(UEvent& e, float* val) {
  UElement* box = e.getSource();
  UStr s;
  if (box) s = box->retrieveText();
  *val = s.toFloat();
  list.repaint();
}
*/
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SpeedList::wheelCB(UWheelEvent& e)  {
  float h_list = list.getView()->getHeight();
  float h_spane = spane.getView()->getHeight();
  int delta = e.getWheelDelta();

  // !!!BUG :: serait pas in verse ???
  // !!!PBM :: sur les bords (pass un temps fou a reffaicher inutilement !!
  
  if (delta > 0)  // vers le haut
    spane.setYScroll(spane.getYScroll() - h_spane/(h_list-h_spane)*10.);
  else  // vers le bas
    spane.setYScroll(spane.getYScroll() + h_spane/(h_list-h_spane)*10.);
  
  static unsigned long last_time = 0;
  unsigned long time = UAppli::getTime();
  //cerr << "time " << time << " lasttime " << last_time << endl;

  if (disp_mode == DYNAMIC) {
    if (time - last_time > clear_timeout) {
      highlightList(delta < 0);  // start
      //static int gest = 0;
      //cerr << "New gesture " << ++gest << " deltaT " << (time - last_time)/1000. << endl;
    }
    timer.start(clear_timeout, 1);  // reculer timeout
  }
  last_time = time;    
}
  
void SpeedList::mousePressCB(UMouseEvent& e) {
  //cerr << "mousePressCB " << endl;
  //return;
  
  if (e.getButton() == e.LeftButton || e.getButton() == e.RightButton) {
    cmenu.open(e);   // open the menu
    if (disp_mode == DYNAMIC) highlightList(true);
  }
  else
    e.propagate();  // propagate to children
}

void SpeedList::keyPressCB(UKeyEvent& e) {
  long key =  e.getKeyCode();
  float h_list = list.getView()->getHeight();
  float h_spane = spane.getView()->getHeight();
  //cerr << "keyPressCB " << key << endl;

  if (key == UKey::Up) {
    spane.setYScroll(spane.getYScroll() - h_spane/(h_list-h_spane)*100.);
    if (disp_mode == DYNAMIC) highlightList(false);
  }
  else if (key == UKey::Down) {
    spane.setYScroll(spane.getYScroll() + h_spane/(h_list-h_spane)*100.);
    if (disp_mode == DYNAMIC) highlightList(true);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SpeedList::resizeFrameCB(UResizeEvent& e) {
  // adjust the size of the spane so that it display the right number of items
  // NB: frame_h doit etre un int a cause d'une erreur d'arrondi
  static int frame_h = 0;
  UView* v = e.getView();
  float h = v->getHeight();
  if (frame_h != h) {
    frame_h = h;
    adjustSize();
    frame_h = v->getHeight();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SpeedList::paintListCB(UPaintEvent& e) {
  UView* v = e.getView();
  
  // adjust the size of the spane so that it contains exactly item_count_perview items
  static bool init = false;
  if (!init) {
    init = true;
    //spane_height = v->getHeight() / (list.children().size() / float(item_count_perview));
    spane_size.setHeight(v->getHeight() / (list.children().size() / float(item_count_perview)));
  }
  
  /*
   UGraph g(e);
   g.setColor(UColor::orange);
   g.drawLine(0, 0, v->getWidth(), v->getHeight());
   */
  
  /*
   e.beginGL();
   glBegin(GL_QUADS);			          // Draw A Quad
   glColor3f(1., 0., 0.);            // Set The Color To Red
   glVertex3f(-1., 1., 0.);          // Top Left
   glColor3f(0., 1., 0.);            // Set The Color To Green
   glVertex3f(1., 1., 0.);           // Top Right
   glColor3f(0., 0., 1.);            // Set The Color To Blue	
   glVertex3f( 1., -1., 0.);         // Bottom Right
   glColor3f(1, 1., 0.);             // Set The Color To Yellow	
   glVertex3f(-1., -1., 0.);         // Bottom Left
   glEnd();							            // Done Drawing The Quad
   e.endGL();  // !! DO NOT FORGET e.endGL() !!
   */
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

const char* item_set[] = {
  "Dworkspace",
  "3Dworkspace.cpp",
  "3Dworkspace.o",
  "3dblocks",
  "3dblocks.cpp",
  "Curv.java",
  "Makefile",
  "Makefile.am",
  "Makefile.in",
  "TT.java",
  "asciify",
  "asciify.cpp",
  "bovary.txt",
  "build",
  "d.xcodeproj",
  "draw.cpp",
  "draw.hpp",
  "draw.o",
  "edi.cpp",
  "edi.hpp",
  "edi.o",
  "eventfilter.cpp",
  "ex.html",
  "fitts.cpp",
  "frisco.gif",
  "frisco.gif.A",
  "frisco2.gif",
  "glcanvas",
  "glcanvas.bak",
  "glcanvas.cpp",
  "glcanvas.o",
  "gltriangle.cpp",
  "gltriangle.hpp",
  "gltriangle.o",
  "ima.cpp",
  "ima.hpp",
  "ima.o",
  "inner.cpp",
  "list.cpp",
  "magiclens.cpp",
  "magiclens.hpp",
  "magiclens.o",
  "makefile.umake",
  "map.css",
  "map.xml",
  "paris-metro-sav.jpg",
  "paris-metro.cpp",
  "paris-metro.jpg",
  "path.cpp",
  "piemenus.cpp",
  "piemenus.hpp",
  "piemenus.o",
  "pos.html",
  "resources.cpp",
  "table.cpp",
  "toolglass.cpp",
  "toolglass.hpp",
  "toolglass.o"
  "tstmouse.cpp",
  "tstpos",
  "tstpos.cpp",
  "tstpos.dSYM",
  "tstsize",
  "tstsize.cpp",
  "tt.cpp",
  "udemos",
  "udemos.bak",
  "udemos.cpp",
  "udemos.o",
  "ufinder",
  "ufinder.cpp",
  "ufinder.o",
  "unasciify",
  "unasciify.cpp",
  "xhtml",
  "xmlparser",
  "xmlparser.cpp",
  "xmlparser.o",
  "zoom.cpp",
  "zoom.hpp",
  "zoom.o",
  "zoomImpl.hpp",
  "zoomlib.cpp",
  "zoomlib.hpp",
  "zoomlib.o",
  null
};

