
#include <iostream>
#include <ubit/ubit.hpp>
using namespace std;
using namespace ubit;


struct Demo : public UBox {
  static const int WIDTH = 25, HEIGHT = 25;
  Demo();
  void paintGrid(UPaintEvent& e);
  UBox canvas;
  UVbox body;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Demo::Demo() {      // various ways of positioning widgets by using UPos
  
  //- - - - - - 

  UArgs bg1 = ualpha(0.75) + UBackground::teal;
  UArgs bg2 = ualpha(0.75) + UBackground::orange;

  /*
  body.add("in uhbox");
  body.add(uhbox(ubox(bg1 + usize(200|UPX, 25) + "200px uhbox")
                 + ubox(bg2 + usize(100|UPX, 25) + "100px uhbox")
                 ));  
  body.add(uhbox(ubox(bg1 + usize(75|UPERCENT, 25) + "75% ubox")
                    + ubox(bg2 + usize(25|UPERCENT, 25) + "25% ubox")
                    ));

  body.add(uhbox(uflowbox(bg1 + usize(200|UPX, 25) + "200px uflowbox")
                 + uflowbox(bg2 + usize(100|UPX, 25) + "100px uflowbox")
                 ));
  body.add(uhbox(uflowbox(bg1 + usize(75|UPERCENT, 25) + "75% uflowbox")
                 + uflowbox(bg2 + usize(25|UPERCENT, 25) + "25% uflowbox")
                 ));
  
  body.add("in uflowbox");
  body.add(uflowbox(ubox(bg1 + usize(200|UPX, 25) + "200px ubox")
                 + ubox(bg2 + usize(100|UPX, 25) + "100px ubox")
                 ));
  body.add(uflowbox(ubox(bg1 + usize(75|UPERCENT, 25) + "75% ubox")
                    + ubox(bg2 + usize(25|UPERCENT, 25) + "25% ubox")
                    ));
  */
  
  /*
  body.add("in utable");
  body.add(utable(utrow(ubox(bg1 + usize(200|UPX, 25) + "w=200px ubox")
                        + ubox(bg2 + usize(100|UPX, 25) + "w=100px ubox")
                        )));
  */
  
  body.add(utable(utrow(uflowbox(bg1 + usize(200|UPX, UAUTO) + "w=200px uflowbox")
                        + uflowbox(bg2 + usize(100|UPX, UAUTO) + "w=100px uflowbox")
                        )));  

 
 
 /*
  body.add(utable(utrow(utcell(bg1 + usize(200|UPX, UAUTO) + "w=200px utcell")
                        + utcell(bg2 + usize(100|UPX, UAUTO) + "w=100px utcell")
                        )));
  */
  add(UOrient::vertical + usize(400, UAUTO) + canvas + body);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Demo::paintGrid(UPaintEvent& e) {
  UGraph g(e);
  UView* v = e.getView();
  float view_w = v->getWidth();
  float view_h = v->getHeight();
  
  g.setColor(UColor::navy);
  for (int x=0; x < view_w; x+= WIDTH)  g.drawLine(x, 0, x, view_h);
  for (int y=0; y < view_h; y+= HEIGHT) g.drawLine(0, y, view_w, y);
  
  int no = 0;
  for (int x=0; x < view_w; x+= WIDTH) {
    UStr s;
    s.setInt(no);
    g.drawString(s, x+5, 5);
    no++;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int main(int argc, char** argv) {
  UAppli appli(argc, argv);
  
  Demo demo;
  UFrame frame(demo);
  
  frame.show();
  appli.start();
}

