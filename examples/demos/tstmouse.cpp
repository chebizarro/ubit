#include <ubit/ubit.hpp>
using namespace std;
using namespace ubit;

//class EE extends Exception {}

class TT : public UFrame {
  UPopmenu popup;
  
  void message(UMouseEvent& e, const char* msg) {
    cerr << msg << " " <<e.getX() << " " << e.getY()
    << " source "<< e.getSource() << "\n"<< endl;
  }
    
public:
  TT() {
    //setPreferredSize(new Dimension(400, 300));
    add
    (usize(400, 300)
     + UOn::mpress / ucall(this, "mouse pressed", &TT::message)
     + UOn::mpress / ucall(&popup, &UPopmenu::open)
     + UOn::mrelease / ucall(this, "mouse released", &TT::message)
     + UOn::mmove / ucall(this, "mouse moved", &TT::message)
     + UOn::mdrag / ucall(this, "mouse dragged", &TT::message)
     );
    
    add(popup);
    
    //popup.setPreferredSize(new Dimension(40, 40));
    popup.add
    (usize(40, 40)
     + UOn::mpress / ucall(this, "popup mouse pressed", &TT::message)
     + UOn::mrelease / ucall(this, "popup mouse released", &TT::message)
     + UOn::mmove / ucall(this, "popup mouse moved", &TT::message)
     + UOn::mdrag / ucall(this, "popup mouse dragged", &TT::message)
     );
    
    
    show(true);
  }
  

};
  
  
int main(int argc, char** argv) {
  UAppli a(argc, argv);
  TT tt;
  a.start();
}
  
  