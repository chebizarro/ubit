#include "tstclass.hpp"

AClass& a = *new AClass;
BClass b;

int main() {
  A& _a = a(1,2);
  B& _b = b(3);

  _a.print();
  _b.print();
}

