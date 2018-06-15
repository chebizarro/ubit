#include <iostream>
#include <ubit/ubit.hpp>
using namespace std;
using namespace ubit;

/* ==================================================== ======== ======= */

class Demo1 : public UBox {
public:
  Demo1();
private:
  UListbox list;
  UStr text, index;

  // the callback functions are member functions in this example
  void actionCB(UEvent&);
  void changeCB(UEvent&);

  // will change the index according to its argument
  void setIndex(UStr* index_string);
};

/* ==================================================== ======== ======= */

Demo1::Demo1() {
  list.add
  (// actionCB is called when an item is activated (clicked)
   // this is the default callback: UOn::action/ can be omitted
   UOn::action / ucall(this, &Demo1::actionCB)
   
   // changeCB is called when the selected item is changed
   // (not called when the selected item is selected again)
   + UOn::change / ucall(this, &Demo1::changeCB)

   // a Label is not an interactor (it can't be armed)
   // it can be included in the List but wont be selectable
   + ulabel(UFont::italic + "Simple List")
   
   // any interactor (= an object that can be armed) could be used here
   + uitem("One")
   + uitem("Two")
   + uitem("Three")
   + uitem("Four")
   + uitem("Five")
   );

  UBox& controls = uvbox
    (UBorder::etchedIn + upadding(3,3)
     + "Content"
     + utextfield(text).setEditable(false)
     + "Index"
     + utextfield(index + ucall(this, &index, &Demo1::setIndex))
     );
  
  add(UBorder::etchedOut
      + uhflex()
      + uhbox(uhflex() + list + " " + controls)
      );
}

/* ==================================================== ======== ======= */

void Demo1::actionCB(UEvent& e) {
  // getSource(): the listbox, getAux(): the selected item (or null)
  UBox* list = e.getSource()->toBox();
  UBox* item = e.getAux() ? e.getAux()->toBox() : null;
 
  cout << "actionCB: "
  << "source: " << list << " " << list->getClassName()
  << " / aux: " << item << " " << (item ? item->getClassName() : "None")
  << endl;
}

/* ==================================================== ======== ======= */

void Demo1::changeCB(UEvent& e) {
  // getSource(): the listbox, getAux(): the selected item (or null)
  UListbox* list = (UListbox*)(e.getSource());
  UBox* item = e.getAux() ? e.getAux()->toBox() : null;

  cout << "changeCB: "
  << "source: " << list << " " << list->getClassName()
  << " / aux: " << item << " " << (item ? item->getClassName() : "None")
  << endl;

  if (item) {
    // NB: item could also be retrieved by: list.choice().getSelectedItem()
    text = item->retrieveText();

    // !! index est decale !!!
    // NB: source == *list 
    index.setInt(list->choice().getSelectedIndex());
  }
  else {
    text = "";
    index = "";
  }
}

void Demo1::setIndex(UStr* index_str) {
  UInt i = *index_str;  // converts the UStr to an integer
  list.choice().setSelectedIndex(i);
}

/* ==================================================== ======== ======= */

class Demo2 : public UBox {
public:
  Demo2();
private:
  UListbox list;
  UBox box;
};

// the callback functions are non member functions in this example
void listCB(UEvent&, const char* msg);
void radioCB(UEvent&, const char* msg);
void itemCB(UEvent&, const char* msg);

// ================================================================

Demo2::Demo2() {
  list.add
  (UOn::action / ucall("action", listCB)
   + UOn::change / ucall("change", listCB)

   // a Label is not an interactor (it can't be armed)
   // it can be included in the List but wont be selectable
   + ulabel(UFont::italic  + "Composite List")

   // any interactor (= an object that can be armed) could be used here
   + uitem(UPix::folder + "aaaa"
           // the item can have its own callbacks
           + UOn::select   / ucall("aaa On::select", itemCB)
           + UOn::deselect / ucall("aaa On::deselect", itemCB)
           + UOn::action   / ucall("aaa On::action ", itemCB)
           ) 
   + uitem("bbbb" + UPix::zoom + uflatbutton(" b2 ") + UPix::ray)
   + uitem("cccc" + USymbol::right + USymbol::right + USymbol::right)
   + uitem("dddd")
   + ubutton("eeee")
   + ucheckbox("ffff")
   );

  // ================================================================
  
  URadioSelect& sel = uradioSelect();
  
  // printMsg is called when the selected item is changed
  // (not called when the selected item is selected again)
  sel.onChange(ucall("radioSelect(onChange)", radioCB));
  
  box.add
    (UBorder::etchedIn + UOrient::horizontal + uhflex() + uhflex()
     + uvbox(ulabel(UFont::italic  + "RadioSelect")
             + uitem("xxxx" + sel
                     // the item can have its own callbacks
                     + UOn::select   / ucall("xxx On::select", itemCB)
                     + UOn::deselect / ucall("xxx On::deselect", itemCB)
                     + UOn::action   / ucall("xxx On::action", itemCB)
                     )
             + uitem("yyyy" + sel)
             + utextfield("tttt ttt tt" + sel)
             )
     + ubutton("zzzz" + sel)
     );
  
  add(upadding(10,10) + UBorder::etchedOut
      + uhbox(list + " " + box)
      + uvbox(UBackground::white + UBorder::etchedOut
              + " List boxes can contain any interactor "
              + " List items can contain any brick ")
      );
}

/* ==================================================== ======== ======= */

void listCB(UEvent& e, const char* msg) {
  // getSource(): the listbox, getAux(): the selected item (or null)
  UBox* list = e.getSource()->toBox();
  UBox* item = e.getAux() ? e.getAux()->toBox() : null;
  
  cout << "listCB: "
  << "source: " << list << " " << list->getClassName()
  << " / aux: " << item << " " << (item ? item->getClassName() : "None")
  << endl;
  
}

void radioCB(UEvent& e, const char* msg) {
  UBox* item = e.getAux() ? e.getAux()->toBox() : null;
  cout << msg << " / target: " << item << " " << (item ? item->getClassName() : "None") << endl; 
}

void itemCB(UEvent& e, const char* msg) {
  UBox* list = e.getSource()->toBox();
  cout << msg << " / source: " << list << " " << list->getClassName() << endl;
}

/* ==================================================== ======== ======= */

int main(int argc, char* argv[]) {
  // create the application context.
  UAppli appli(argc, argv);

  // create the main frame, add it to the appli and show it
  UFrame frame;
  frame.add(*new Demo1() + *new Demo2());
  
  appli.add(frame);
  frame.show();
  return appli.start();
}


