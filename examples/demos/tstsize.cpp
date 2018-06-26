
#include <iostream>
using namespace std;

int main() {
  struct utfloat {float val; char type; char mode; short count;};

  struct width_mode {unsigned char is_resizable:1, is_adjustable:1;} ;

width_mode tt;
  utfloat f;
  
  cerr << sizeof(utfloat) << " " << sizeof(f) << endl;

cerr << sizeof(width_mode) << " " << sizeof(tt) << endl;
}
