#include <iostream>
#include <ubit/ubit.hpp>
#include "zoom.hpp"

int main(int argc, char** argv) {
  UAppli app(argc, argv);
  
  ZoomDemo zlib("zlib.xml");
  UFrame frame(usize(550, 400) + zlib);
  
  frame.show();
  app.start();
}
