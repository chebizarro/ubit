/*
 * fitts.cpp - elc
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <time.h>
//#include <cstdio>
//#include <cstdlib>
//#include <unistd.h>
//#include <fcntl.h>
//#include <sys/stat.h>
//#include <sys/utsname.h>
//#include <sys/file.h>
//#include <sys/termios.h>
//#include <sys/ioctl.h>
#include <ubit/ubit.hpp>
using namespace std;
using namespace ubit;

/* ==================================================== ===== ======= */

struct Params {
  Params();
  enum {Fitts, Schmidt, Guiard} protocol;
  int D, W, T;
  bool show_trace;
  UWidth frame_width;
  UHeight frame_height;
  //URect target;
  //UEllipse starting_point;
};

/* ==================================================== ===== ======= */

struct Point {
  int x, y;
  UTime time;
  Point(int _x, int _y, UTime _time): x(_x), y(_y), time(_time) {}
};

typedef std::list<Point> Points;

class Gesture : public ULine {
public:
  Gesture(const Params&);
  
  void begin();
  void addPoint(const UMouseEvent&);
  void end();
  
  virtual void draw(const UGraph&) const;
  //virtual void getBounds(URect&) const;  should be redefined

private:
  const Params& params;
  bool is_init;
  UTime time0;
  Points points;
  void save();
};

/* ==================================================== ===== ======= */

class Expe : public UFrame {
public:
  
  Expe();
  
private:
  enum State {START_PANEL, READY, DRAWING, COMPLETED} state;
  Gesture gesture;
  Params params;
  uptr<UButton> start_btn;
  uptr<ULabel> completed_lbl;
  
  void startCB();               // START_PANEL->READY on start_btn(action)
  void pressCB(UMouseEvent&);   // READY->DRAWING on frame(mpress) 
  void dragCB(UMouseEvent&);    // DRAWING->DRAWING on frame(mdrag)
  void releaseCB(UMouseEvent&); // DRAWING->COMPLETED on frame(mrelease)
  void keyCB(UKeyEvent&);
  void spaceKeyCB();            // COMPLETED->START_PANEL on space(pressed)
  void repaintCB(UPaintEvent&);
  void resizeCB(UPaintEvent&);  
};

/* ==================================================== [Elc] ======= */
/* ==================================================== ===== ======= */

int main(int argc, char** argv) {
  UAppli appli(argc, argv);
  
  Expe expe;
  appli.add(expe);
  expe.show();

  return appli.start();
}

Params::Params() {
  D = 800;
  W = 10;
  T = 0;
  show_trace = true;
  frame_width = 1200;
  frame_height = 800;
}

/* ==================================================== ===== ======= */
  
Expe::Expe() : state(START_PANEL), gesture(params) {
  addAttr(UOn::mpress / ucall(this, &Expe::pressCB)
          + UOn::mdrag / ucall(this, &Expe::dragCB)
          + UOn::mrelease / ucall(this, &Expe::releaseCB)
          + UOn::paint / ucall(this, &Expe::repaintCB)
          + UOn::resize / ucall(this, &Expe::resizeCB)
          + UOn::kpress / ucall(this, &Expe::keyCB)
          + params.frame_width + params.frame_height
          + uhmargin(0,0) + uvmargin(0,0)
          );
  
  start_btn = ubutton("  Start Trial  ");
  start_btn->addAttr(upos(50.,25.).setPercentMode(true,true) 
                     + UFont::bold + UFont::xx_large 
                     + UBorder::etchedIn
                     + UOn::action / ucall(this, &Expe::startCB)
                     );
  
  completed_lbl = ulabel("Trial completed, press SPACE bar to continue");
  completed_lbl->addAttr(upos(50.,25.).setPercentMode(true,true) 
                         + UFont::xx_large);
  completed_lbl->show(false);
  
  add(*start_btn + *completed_lbl);
}

/* ==================================================== ===== ======= */

void Expe::startCB() {
  if (state == START_PANEL) {
    state = READY;
    start_btn->show(false);
    repaint();
  }
}

void Expe::pressCB(UMouseEvent& e) {
  if (state == READY) {
    state = DRAWING;
    gesture.begin();
    gesture.addPoint(e);
    repaint();
  }
}

void Expe::dragCB(UMouseEvent& e) {
  if (state == DRAWING) {
    //state = DRAWING; idem
    gesture.addPoint(e);
    repaint();
  }
}

void Expe::releaseCB(UMouseEvent& e) {
  if (state == DRAWING) {
    state = COMPLETED;
    gesture.addPoint(e);
    repaint();
    completed_lbl->show(true);
    gesture.end();
  }
}

void Expe::keyCB(UKeyEvent& e) {
  if (e.getKeyChar() == ' ') spaceKeyCB();
}
    
void Expe::spaceKeyCB() {
  if (state == COMPLETED) {
    state = START_PANEL;
    repaint();
    completed_lbl->show(false);
    start_btn->show(true);
  }
}

//NOTE: on pourrait eviter de reafficher tous les points a chaque fois !!!!
void Expe::repaintCB(UPaintEvent& e) {
  if (state != START_PANEL) {
    UDimension size = e.getView()->getSize();
    int delta = params.W/2;

    UGraph g(e);
    //g.setColor(UColor::red);

    // ! attention aux parites !
    int x = size.width/2 - params.D/2;
    g.drawLine(x-delta, 0, x-delta, size.height);
    g.drawLine(x+delta, 0, x+delta, size.height);

    x = size.width/2 + params.D/2;
    g.drawLine(x-delta, 0, x-delta, size.height);
    g.drawLine(x+delta, 0, x+delta, size.height);
        
    if (params.show_trace) {
      //g.setColor(UColor::black);
      g.draw(gesture);
    }
  }
}

void Expe::resizeCB(UPaintEvent& e) {
}

/* ==================================================== ===== ======= */
/*
 ofstream fout;
 ifstream in(html_doc);
 if (!in) ...;
 if (fout.is_open()) ...;
 */

Gesture::Gesture(const Params& p) : params(p), is_init(false) {}

void Gesture::begin() {
  if (is_init) {
    cerr << "Gesture::begin(): end() was not called" << endl;
    return;
  }
  is_init = true;
  time0 = UAppli::getTime();
}

void Gesture::end() {
  if (!is_init) {
    cerr << "Gesture::end(): begin() was not called" << endl;
    return;
  }
  save();    // save the points of the gesture in a file
  points.clear();     // clear the point list
  is_init = false;
}

void Gesture::addPoint(const UMouseEvent& e) {
  if (!is_init) {
    cerr << "Gesture::addPoint(): begin() was not called" << endl;
    return;
  }
  points.push_back(Point(e.getX(), e.getY(), UAppli::getTime() - time0));
}

void Gesture::draw(const UGraph& g) const {
  Points::const_iterator prev = points.begin();
  Points::const_iterator p = points.begin();
  ++p;
  for (; p != points.end(); ++p) {
    g.drawLine((*prev).x, (*prev).y, (*p).x, (*p).y);
    prev = p;
  }
}

void Gesture::save() {
  char datebuf[50];
  {  // get date to create the file name
    time_t mtime;
    time (&mtime);
    struct tm* timebuf = ::localtime(&mtime);
    *datebuf = 0;
    ::strftime(datebuf, sizeof(datebuf), "%d-%m-%y-%Hh%Mm%S", timebuf);
  }
  
  UStr base = "Fitts";
  //UStr fname = target_name.basename(false) & "-" & datebuf & ".log";
  UStr fname = base & "-" & datebuf & ".log";
  
  ofstream fout(fname.c_str());
  if (!fout) {
    //alert(UStr("Can't open logfile: ") & fname);
    cerr << "Gesture::save: could not open log file: " << fname << endl;
    return;
  }
  
  // write header
  fout << "#Log:" <<base << " params: x y etc. file: " << fname << endl;

  // write point data
  for (Points::const_iterator p = points.begin(); p != points.end(); ++p) {
    fout << (*p).time << '\t' << (*p).x << '\t' << (*p).y << '\t' << endl;
  }
  fout.close();
}

/* ==================================================== ===== ======= */
