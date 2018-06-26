
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
  float h = HEIGHT;
  UArgs args = usize(100, 25) + ualpha(0.5);

  //- - - - - - pixel/mm positionning
  UArgs args1 = args + UBackground::orange;

  canvas.add(ubox(args1 + upos(50|UPX, h) + "x=50px"));
  canvas.add(ubox(args1 + upos(50|UPX|UPos::RIGHT, h) + "50px|RIGHT"));
  h+=HEIGHT;
  
  canvas.add(ubox(args1 + upos(50|UPT, h) + "x=50pt"));
  canvas.add(ubox(args1 + upos(50|UPT|UPos::RIGHT, h) + "50pt|RIGHT"));
  h+=HEIGHT;
  
  canvas.add(ubox(upos(50.|UMM, h) + args1 + "x=50mm"));
  h+=HEIGHT;
  
  //- - - - - - font positionning
  UArgs args2 = args + UBackground::red;

  canvas.add(ubox(upos(10|UEX, h) + args2 + "x=10ex"));
  canvas.add(ubox(upos(10|UEX|UPos::RIGHT, h) + args2 + "10ex|RIGHT"));
  h+=HEIGHT;
  canvas.add(ubox(upos(10|UEM, h) + args2 + "x=10em"));
  canvas.add(ubox(upos(10|UEM|UPos::RIGHT, h) + args2 + "10em|RIGHT"));
  h+=HEIGHT;
  
  canvas.add(uvbox(upos(10|UEX, h) + args2 + UFont::small + "x=10ex" + "x_small"));
  h+=HEIGHT;
  canvas.add(uvbox(upos(10|UEM, h) + args2 + UFont::small + "x=10em" + "x_small"));
  h+=HEIGHT;
    
  //- - - - - - percentage 
  UArgs args3a = args + UBackground::lightblue;

  canvas.add(ubox(args3a + upos(50.0|UPERCENT, h) + "x=50%"));
  h+=HEIGHT;
  canvas.add(ubox(args3a + upos(25.0|UPERCENT, h) + "x=25%"));
  canvas.add(ubox(args3a + upos(25.0|UPERCENT|UPos::RIGHT, h) + "25%|RIGHT"));
  h+=HEIGHT;
  
  //- - - - - - width corrected percent 
  UArgs args3b = args + UBackground::navy;

  canvas.add(ubox(args3b + upos(0|UPERCENT_CTR, h) + "0%|CTR"));
  canvas.add(ubox(args3b + upos(50|UPERCENT_CTR, h) + "50%|CTR"));
  canvas.add(ubox(args3b + upos(100|UPERCENT_CTR, h) + "100%|CTR"));
  h+=HEIGHT;
  
  //- - - - - - size 

  UArgs args4a = ualpha(0.5) + UBackground::green;

  canvas.add(uvbox(args4a + upos(25|UPX, h) + usize(50|UMM, 50|UPX) 
                   + "x=25" + "w=50mm" +  "h=50px"));
  
  canvas.add(uvbox(args4a + upos(50|UPERCENT_CTR, h) + usize(100|UPX, 50|UPX) 
                   + "x=50%|CTR" + "w=100px" + "h=50px"));
  
  canvas.add(uvbox(args4a + upos(25|UPos::RIGHT, h) + usize(20|UEX, 50|UPX) 
                   + "x=25|RIGHT"+ "w=50ex" + "h=50px"));
  h+=HEIGHT;
  h+=HEIGHT;
  
  UArgs args4b = ualpha(0.5) + UBackground::yellow;
  canvas.add(uvbox(args4b + upos(50|UPERCENT_CTR, h) + usize(50|UPERCENT, h) 
                   + "x=50%|CTR" + "w=50%"));
  h+=HEIGHT;
  h+=HEIGHT;
  
  //- - - - - - 
    
  canvas.addAttr(usize(WIDTH*20, h) + UBackground::white);
  canvas.addAttr(UOn::paint / ucall(this, &Demo::paintGrid));
  
  //- - - - - - 

  UArgs bg1 = ualpha(0.75) + UBackground::teal;
  UArgs bg2 = ualpha(0.75) + UBackground::orange;

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
  
  body.add("in utable");
  body.add(utable(utrow(ubox(bg1 + usize(200|UPX, 25) + "w=200px ubox")
                        + ubox(bg2 + usize(100|UPX, 25) + "w=100px ubox")
                        )));
  body.add(utable(utrow(uflowbox(bg1 + usize(200|UPX, UAUTO) + "w=200px uflowbox")
                        + uflowbox(bg2 + usize(100|UPX, UAUTO) + "w=100px uflowbox")
                        )));  
  body.add(utable(utrow(utcell(bg1 + usize(200|UPX, UAUTO) + "w=200px utcell")
                        + utcell(bg2 + usize(100|UPX, UAUTO) + "w=100px utcell")
                        )));
  add(UOrient::vertical + canvas + body);
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
  appli.getDisp()->setPixelPerMM(1440./330.);

  Demo demo;
  UFrame frame(demo);
  
  frame.show();
  appli.start();
}

