#include <iostream>
#include <string>
using namespace std;

class Class {
public:
  virtual ~Class() {}
  virtual const char* getName() const {return "Class";}
};


class Obj {
public:
  virtual ~Obj() {}
  virtual Class& getClass() const = 0;
  const char* getClassName() const {return getClass().getName();}

  virtual void print() const = 0;
};

// - - - - -  - - - - -  - - - - -  - - - - -  - - - - -  - - - - -  - - - - - 

extern class AClass& a;

class A : public Obj {
public:
  A(int _i = 0) : i(_i), j(0) {}
  A(int _i, int _j) : i(_i), j(_j) {}
  
  virtual Class& getClass() const {return a;}
  virtual void print() const {cout << "A: " << i << " j " << j << endl;}
  
protected:
  int i, j;
};

class AClass : public Class {
public:
  virtual const char* getName() const {return "A";}
  virtual A& operator()(int i = 0) const {return *new A(i);}
  virtual A& operator()(int i, int j) const {return *new A(i,j);}
};


// - - - - -  - - - - -  - - - - -  - - - - -  - - - - -  - - - - -  - - - - - 

extern class BClass b;

class B : public A {
public:
  B(int _i = 0) : A(_i) {}
  
  virtual Class& getClass() const {return b;}
  virtual void print() const {cout << "B: " << i << " x " << x << endl;}

protected:
  float x;
};

class BClass : public Class {
public:
  virtual const char* getName() const {return "B";}
  virtual B& operator()(int i = 0) const {return *new B(i);}
};

