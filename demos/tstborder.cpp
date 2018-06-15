/* ***********************************************************************
 *
 *  udemos.cpp: a program that launches various Ubit demos
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

#include <iostream>
#include <cmath>
#include <ubit/ubit.hpp>
#include <ubit/ufinder.hpp>
#include "gltriangle.hpp"
#include "edi.hpp"
#include "ima.hpp"
#include "draw.hpp"
#include "zoom.hpp"
#include "zoomlib.hpp"
#include "toolglass.hpp"
#include "piemenus.hpp"
#include "magiclens.hpp"
using namespace std;
using namespace ubit;

struct DemoBox : public UBox {
  DemoBox(class Demos*, const char* name, UBox* demo, float scale, bool mini);
  DemoBox& operator=(DemoBox& d);

  UStr title;
  UButton minmax_button;
  UBox content;
  UScale scale;
  UPos pos;
  uptr<UBox> pdemo;
};

class Demos : public UFrame {
public:
  static const int DEMOS_PER_LINE = 3;

  Demos(UArgs = UArgs::none);
  
  void addDemo(const char* name, UBox* demo);
  void maximize(DemoBox*);
  void minimize(DemoBox*);

private:
  UBox mainbox;
  UBox demo_pane;
  int demo_count;
  DemoBox maxbox;
};


void foo(UPaintEvent& e, int i) {
  UGraph g(e);
  
  g.setColor(UColor::green);
  //g.fillEllipse(50, 50, 10, 10);
  g.fillEllipse(50, 50, 120, 120);
  //g.drawEllipse(50, 50, 120, 120);
  //g.setColor(UColor::red);
  //g.drawArc(10, 10,  10, 10, 270, 90);
  
  g.setColor(UColor::blue);
  g.fillRoundRect(10, 10, 100, 100, 10, 10);
  //g.drawRoundRect(10, 10, 100, 100, 10, 10);

  /*
   float x = 10, y = 10;
   float w = 100, h = 100;
   float x2 = x + w;
   float y2 = y + h;
   float cw = 10, ch = 10;   
  g.setColor(UColor::blue);
  g.drawLine(x+cw, y, x2-cw, y);     // top line
  g.drawLine(x, y+cw , x, y2-ch);    // left line
  g.drawLine(x+cw, y2, x2-cw, y2);   // bottom line
  g.drawLine(x2, y+ch, x2, y2-ch);   // right line
  
  float cw2 = cw*2, ch2 = ch*2;
  g.setColor(UColor::red);
  g.drawArc(x, y, cw2, ch2, 90, 90);             // top left arc
  g.drawArc(x2-cw2, y, cw2, ch2, 0, 90);         // top right arc
  g.drawArc(x, y2-ch2, cw2, ch2, 180, 90);       // bottom left arc
  g.drawArc(x2-cw2, y2-ch2, cw2, ch2, 270, 90);  // top right arc
   */
}


int main(int argc, char* argv[]) 
{  
  UAppli appli(argc, argv);
  
  //UColor& fg = UColor::black;
  //UColor& bg = UColor::white;
  UColor& fg = UColor::darkgrey;
  UColor& bg = UColor::lightgrey;

  float aw = 10, ah = 10;
  
  UButton& bb = ubutton("mybutton");
  UFrame f(
           uvcenter() + uhcenter() + upadding(30, 40)
           //+ UBackground::white
           
           + uhbox("1:"+  bb 
                   + ubox(+ "2:" + bb) 
                   + ubox(+ "3:" + bb) 
                   // FAUX + bb 
                   // FAUX + uelement(bb)
                   )
           + " " 
           + " " 
           + uhbox(uhcenter() //+ UBackground::yellow
                   + ubutton("default")
                   + " "
                   + ubutton(UBorder::line + "line")
                   + " "
                   + ubutton(UBorder::shadowIn  + "shadowIn")
                   + " "
                   + ubutton(UBorder::shadowOut + "shadowOut")
                   + " "
                   + ubutton(UBorder::etchedIn  + "etchedIn")
                   + " "
                   + ubutton(UBorder::etchedOut + "etchedOut")
                   + " "
                   )
           + " " 
           + " " 
           + uhbox(uhcenter()   
                   + ubutton(*new URoundBorder(+UBorder::LINE, fg,bg,1,1,aw,ah)
                             + "line+")
                   + " "
                   + ubutton(*new URoundBorder(-UBorder::SHADOW, fg,bg,1,1,aw,ah)
                             + "shadowIn")
                   + " "
                   + ubutton(*new URoundBorder(+UBorder::SHADOW, fg,bg,1,1,aw,ah)
                             + "shadowOut")
                   + " "
                   + ubutton(*new URoundBorder(-UBorder::ETCHED, fg,bg,2,2,aw,ah)
                             + "etchedIn")
                   + " "
                   + ubutton(*new URoundBorder(+UBorder::ETCHED, fg,bg,2,2,aw,ah)
                             + "etchedOut")
                   + " "
                   )
           + " " 
           + " " 
           + uhbox(uhcenter() + usize(200,200) + UBackground::white
                   + UOn::paint/ucall(1, foo)
                   )
           );           
  
  f.show();
  
  // start the event loop
  return appli.start();
}

/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Demos::Demos(UArgs args) : 
UFrame(args), 
demo_count(0), 
maxbox(this, "", new UBox, 1., false)
{
  // demo_pane is a panel that will contain all the demos arranged as a matrix
  demo_pane.addAttr(UOrient::vertical);
  mainbox.add(demo_pane + maxbox);
  maxbox.show(false);
  
  UBox* box = &ubutton(UBackground::metal + "kkkkk");
  
  //addDemo("GLTriangle", new GLTriangle);
  addDemo("KKK", box);
  
  add(UOrient::horizontal + uvflex() 
      //+ uleft() + commands 
      + uhflex() + mainbox
      );
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Demos::addDemo(const char* name, UBox* demo) 
{
  static UBox* last_row = null;

  UBox* demobox = new DemoBox(this, name, demo, 0.6, true);
   
  if (demo_count % DEMOS_PER_LINE == 0) {
    last_row = &ubox(UOrient::horizontal);
    demo_pane.add(*last_row);
  }
  demo_count++;
  last_row->add(demobox);
}

void Demos::maximize(DemoBox* d) {
  maxbox.title = d->title;
  
  d->pdemo->add("1111");
  d->content.add("2222");

  maxbox.content.add(*d->pdemo);
  
  demo_pane.show(false);
  maxbox.show(true);

  cerr << "MAX : " << d << ":"<<endl;

  UStr sdem = d->pdemo->retrieveText();
  UStr scont = d->content.retrieveText();
  cerr << "sdem : " << sdem << endl<<endl;
  cerr << "scont : " << scont << endl<<endl;
  
}

void Demos::minimize(DemoBox* d) {
  maxbox.show(false);
  maxbox.content.removeAll();
  demo_pane.show(true);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

DemoBox::DemoBox(Demos* _demos, const char* _title, UBox* _demo, float _scale, bool mini) 
: pos(0,0), pdemo(_demo)
{
  title = _title;
  
  addAttr(UOrient::vertical + UBorder::shadowIn + usize(250, 200) + upadding(10,10));
  
  if (mini) {
    minmax_button.add(uhcenter() + title + " " + USymbol::square
                      + ucall(_demos, this, &Demos::maximize));
    content.add(_demo);  
  }
  else
    minmax_button.add(uhcenter() + title + " " 
                      + ugroup(UFont::small + USymbol::square 
                               + "  (click to minimize)")
                      + ucall(_demos, this, &Demos::minimize));

  content.addAttr((scale = _scale) + UBackground::orange);
  add(UOrient::vertical + utop() + minmax_button + uvflex() + content);  
}



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -





