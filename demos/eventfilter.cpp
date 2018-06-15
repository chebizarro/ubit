
void filterCB(UEvent& e, UBox*) {
  if (e.mouseEvent()) {
    UMouseEvent* me = e.mouseEvent();
    
    if (e.getCond() == UOn::mmove)
      cerr << "MouseEvent MOVE ";
    else if (e.getCond() == UOn::mpress)
      cerr << "MouseEvent PRESS ";
    else if (e.getCond() == UOn::mrelease)
      cerr << "MouseEvent RELEASE ";
    else 
      cerr << "MouseEvent MISC "; 
    
    cerr  << "btn " << me->getButton()   
      << " state " << me->getState() 
      << " x " << me->getX() << " y " << me->getY()
      << " flow " << me->getFlowID() 
      << endl;
  }
  
  else if (e.toWheelEvent()) {
    UWheelEvent* we = e.wheelEvent();
    cerr << "WheelEvent ";
    cerr  << "wheel " << we->getWheelDelta()   
      << " state " << we->getState() 
      << " flow " << we->getFlowID() 
      << endl;
  }
  
  else if (e.toKeyEvent()) {
    UKeyEvent* ke = e.keyEvent();
    if (e.getCond() == UOn::ktype)
      cerr << "KeyEvent TYPE ";
    else if (e.getCond() == UOn::kpress)
      cerr << "KeyEvent PRESS ";
    else if (e.getCond() == UOn::krelease)
      cerr << "KeyEvent RELEASE ";
    else 
      cerr << "KeyEvent MISC "; 
    
    cerr  << "char '" << char(ke->getKeyChar()) <<"'"
      << " code " << ke->getKeyCode()
      << " state " << ke->getState() 
      << " flow " << ke->getFlowID() 
      << endl;
  }
}


