/*
 *  node.cpp:Base Class for all objects than can be added to the scene graph
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2008 | Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
 *
 * ***********************************************************************
 * COPYRIGHT NOTICE :
 * THIS PROGRAM IS DISTRIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE
 * IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION;
 * EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 * SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
 * ***********************************************************************/

#include <ubit/ubit_features.h>
#include <iostream>
#include <ubit/core/node.h>
#include <ubit/uon.hpp>
#include <ubit/ucall.hpp>
#include <ubit/ustr.hpp>
#include <ubit/ubox.hpp>
#include <ubit/uview.hpp>
#include <ubit/core/event.h>
#include <ubit/uupdate.hpp>
#include <ubit/uappli.hpp>
#include <ubit/core/uappliImpl.hpp>
using namespace std;
namespace ubit {



Node::Node() {}  // tout est fait dans UObject

const String& Node::getNodeName() const {static String nn("#node"); return nn;}

String Node::getNodeValue() const {return "";}

Node& Node::setAutoUpdate(bool state) {
  omodes.DONT_AUTO_UPDATE = !state;
  return *this;
}

/* ATTENTION:
* les appels aux fcts vituelles ne marchent pas normalement dans les
* destructeurs (l'appel est fait avec la classe du destructeur,
* pas avec la classe effective de this)
*
* ==> appeler destructs() DANS LE DESTRUCTEUR de la CLASSE de l'objet
*     pour que destructs() soit appele avec ce type     
*     adequate pour qu'il soit applique avec le type effectif de l'objet
* ==> CECI IMPOSE DE DEFINIR DES DESTRUCTEURS DANS LES SOUS-CLASSES
*/
void Node::destructs() {
  if (Application::isExiting()) return;

  // superclasses' destructs() methods wont be called
  if (omodes.IS_DESTRUCTED) return;
  
  // this object is now being destructed
  omodes.IS_DESTRUCTING = true;
  
  /* 12Feb07: callback supprime dans les Node, plus que dans les Element
  * doit etre avant sinon ne fera rien car CBs deja enleves
  * if (hasCallback(UMode::DESTRUCT_CB)) {
  *  Event e; e.setTarget(this); fire(e, UOn::destruct);
  * }
  */
  
  removeAllAttrs();   // detruire les attributs
  
  if (pbegin() != pend()) removeFromParents(false);
  // faudrait aussi s'occuper des liens croises (par les ucall) !!!
  
  // must be at the end (callbacks wont be called otherwise)
  omodes.IS_DESTRUCTED = true;
}


void Node::_addAttr(const Child& c) {       // @@@@ 12fev07 !!!
  // ATTENTION: pas d'appele de callback ni de addingTo()  !!!!!!!
  // => ne marchera pas si prop incluse dans une proplist car addingTo
  // n'est pas execute
  
  //ex: attributes().push_back(c);  
  _attributes.push_back(c);  
  // !!voir aussi Element:  addImpl(attributes(), c, aend(), true, null);
}

void Node::addChangeCall(UCall& b) {
  // addBModes(UMode::CHANGE_CB);        // @@@@ !!!! enleve 12fev07 !!!
  Child c(&b, UOn::change);
  _addAttr(c);
}


Node& Node::removeAllAttrs(bool autodel) {  // @@@@ a revoir % Element::removeAllAttrs !!!
  while (true) {
    if (_abegin() == _aend()) break;

    Node* child_b = *_abegin();
    if (!child_b) return *this; // securite
      
    _attributes.erase(_abegin()); // detacher child de ce parent  
    
    // NB: ptr_count<0 means that the object has already been destructed
    if (child_b->ptr_count == 0 && child_b->omodes.IS_DYNAMIC) delete child_b;
    
    // call callbacks....    
  }
  return *this;
}

/* ==================================================== [Elc] ======= */
// ATT avec les parents: les objets UCONST peuvent avoir bcp de parents
// (idealement il ne faudrait pas faire ces liens)

bool Node::hasSceneGraphParent() const {
  for (UParentIter p = pbegin(); p != pend(); ++p) {
    // WINLIST elements must not be taken into account !
    if ((*p)->getDisplayType() != Element::WINLIST) return true;
  }
  return false;
}


Element* Node::getParent(int pos) const {
  if (pos == -1) {  // find the last one
    //return (_parents.last() ? **(_parents.last()) : null);
    if (pbegin() == pend()) return null;
    else return *_parents.back();
  }
  else if (pos >= 0) {
    int count = 0;
    for (UParentIter p = pbegin(); p != pend(); ++p) {
      //if (count == pos) return l->getChild()->getParent();
      if (count == pos) return *p;
      count++;
    }
  }
  return null;  // index < -1 or to large
}


int Node::getBoxParents(std::vector<Box*>& parvect) const {
  int count = 0;
  //parvect.clear(); //faux 7dec05 !!
  
  for (UParentIter p = pbegin(); p != pend(); ++p) {
    Element* parent = *p;
    if (parent->toBox()) {
      parvect.push_back(parent->toBox());
      count++;
    }
    else count += parent->getBoxParents(parvect);
  }
  return count;
}


Box* Node::getParent(const View* _view) const {
  if (!_view) return null;
  std::vector<Box*> parvect;
  int count = getBoxParents(parvect);
  
  for (int k = 0; k < count; k++) {
    View* v = parvect[k]->getViewContaining(*_view);
    if (v) return parvect[k];
  }
  return null;
}

Box* Node::getParent(const UInputEvent& e) const {
  return getParent(e.getView());
}

View* Node::getParentView(const View* _view) const {
  if (!_view) return null;
  std::vector<Box*> parvect;
  int count = getBoxParents(parvect);
  
  for (int k = 0; k < count; k++) {
    View* v = parvect[k]->getViewContaining(*_view);
    if (v) return v;
  }
  return null;
}

View* Node::getParentView(const UInputEvent& e) const {
  return getParentView(e.getView());
}

/* ==================================================== [Elc] ======= */

void Node::removeFromParents(bool update_parents) {
  // nb: on enleve le 1er jusuq'a ce qu'il n'y en ait plus
  // - correct pour multi-occurences:
  // - l'ordre n'a pas d'importance puisqu'on les enleve tous (si un 
  //   child est inclus plusieurs fois dans un meme parent on ne sait pas 
  //   dans quel ordre parent->remove(child)  enleve ces occurences)
  
  while (true) {
    if (pbegin() == pend()) break;

    Element* parent = *pbegin();
    if (!parent) return;  // securite
    bool update = false;
    
    ChildIter c = parent->attributes().find(*this);
    if (c != parent->aend()) {
      if (!parent->omodes.DONT_AUTO_UPDATE && (*c)->isDisplayable()) update = true;
      parent->removeImpl1(c, Element::REMOVE_FROM_PARENTS, parent->attributes());
    }
    else {
      c = parent->children().find(*this);
      if (c != parent->cend()) {
        if (!parent->omodes.DONT_AUTO_UPDATE && (*c)->isDisplayable()) update = true;
        parent->removeImpl1(c, Element::REMOVE_FROM_PARENTS, parent->children());
      }
      else {
        // cas d'erreur: child pointe vers parent alors que parent ne pointe
        // pas vers child
        // => enlever directement ce lien de la liste des parents et le detruire
        error("Node::removeFromParents",
              "[internal error] This object was not found in the parent child/attribute lists it was supposed to be removed from");
        _parents.erase(pbegin());
      }
    }
    
    if (update) {
      Box* box = parent->toBox();
      if (box && box->getView()) box->update(Update::LAYOUT | Update::PAINT | Update::ADD_REMOVE);
      // et pour les groups .....
    }
  } // endwhile
}

// returns true if argument is a direct parent (case indirect = false)
// or an ancestor (case indirect = true) of 'this' object

bool Node::isChildOf(const Element& possible_ancestor, bool indirect) const {
  // NB: bug avec scrollpanes car les scrollbars sont dans border ce qui fait
  // echouer isChildOf 

  for (UParentIter p = pbegin(); p != pend(); ++p) {
    Element* par = *p;
    // par == null or this if no other parent (UFrame or similar)
    if (par && par != this) {
      if (par == &possible_ancestor) return true; 
      else if (indirect) {
        if (par->isChildOf(possible_ancestor, true)) return true;
      }
    }
  }
  return false; // not found
}

/* ==================================================== [Elc] ======= */

  // !!! cf Element::fire() !!!@@@@@              !!!!!!!!!!!!!!!!
  
bool Node::fire(Event& e) const {
  // dont call callback if the object has already been destructed
  if (omodes.IS_DESTRUCTED) return false;
  
  bool fired = false;
  
  UCall* call = null;
  const Condition* n_cond = null;
  
  ChildIter i = _abegin();
  while (i != _aend()) {
    Child& ch = i.child();
    ++i;
    if (ch.cond //&& e.cond
        && (n_cond = ch.cond->matches(e.getCond()))
        && (call = (*ch)->toCall())          
        && !call->omodes.IS_DESTRUCTED) {
      //if (cond != UOn::filterEvent) e.setCond(*n_cond);
      (*call)(e);
      fired = true;
      // return now if the object has been destructed by c->call()
      if (omodes.IS_DESTRUCTED) return fired;
    }
  }
  return fired;
}

/* ==================================================== [Elc] ======= */

Data& Data::onChange(UCall& c) {
  addChangeCall(c);
  return *this;
}

void Data::changed(bool _update) {
  if (_update && !omodes.DONT_AUTO_UPDATE) update();

  if (!omodes.IGNORE_CHANGE_CALLBACKS) {
    if (_abegin() != _aend()) {
      Event e(UOn::change, this);  // UNodeEvent
      fire(e);                 // ??? MANQUE MASK @@@@ !!!!
    }
    // ensuite on lance les callbacks des parents
    fireParents(UOn::dataChange, this); 
  }
}


Child::Child(const char* s) 
: obj(new String(s)), parent(0), cond(0) {}

ChildIter Children::at(int pos) {
  if (pos == -1) {
    if (begin() == end()) return end();
    else {
      ChildIter it = end();
      return --it;
    }
    //ChildIter last = end();
    //for (ChildIter i = begin(); i != end(); ++i) last = i;
    //return last;
  }
  else {
    int count = 0;
    for (ChildIter i = begin(); i != end(); ++i) {
      if (pos == count) return i;
      count++;
    }
  }
  return end();  // not found
}

ChildIter Children::find(const Node& b) {
  for (ChildIter i = begin(); i != end(); ++i) {
    if (*i == &b) return i;
  }
  return end();
}

ChildIter Children::findStr(const String& str, bool ignore_case) {
  for (ChildIter i = begin(); i != end(); ++i) {
    String* s = (*i)->toStr();
    if (s && s->equals(str, ignore_case)) return i;
  }
  return end();
}

ChildIter Children::findBox(const String& str, bool ignore_case) {
  for (ChildIter i = begin(); i != end(); ++i) {
    Box* box = (*i)->toBox();
    if (box && box->children().findStr(str, ignore_case) != box->cend()) 
      return i;
  }
  return end();
}

/* ==================================================== [Elc] ======= */

void Parents::removeFirst(Child* c) {
  for (UParentIter p = begin(); p != end(); ++p) {
    if (&p.parent().getChild() == c) {
      erase(p);
      break;
    }
  }  
}

void Parents::updateAutoParents(const Update& upmode) {
  for (UParentIter p = begin(); p != end(); ++p) {
    Element* grp = *p;
    // box==null --> pas encore initialise
    if (grp && !grp->omodes.DONT_AUTO_UPDATE) grp->update(upmode);
  }
}

void Parents::fireParents(const Condition& cond, Node* child) {
  for (UParentIter p = begin(); p != end(); ++p) {
    Element* grp = *p;                // ???TrueParent??
    // box==null --> pas encore passe par initContext()
    if (grp) {
      Event e2(cond, grp, child);  // UElemEvent
      grp->fire(e2);
      // ATT: les Element doivent etre transparents aux events
      if (!grp->toBox()) grp->_parents.fireParents(cond, child);
    }
  }
}

/*
void Parents::setParentsModes(long bmodes, long cmodes, bool state) {
  for (UParentIter p = begin(); p != end(); ++p) {
    Element* g = *p;
    // g==null --> pas encore passe par initContext()
    if (g) {
      if (state) g->addBModes(bmodes); else g->removeBModes(bmodes);
      if (state) g->addCModes(cmodes); else g->removeCModes(cmodes);
    }
  }
}
*/

void Parents::setParentsViewsModes(int vmodes, bool state) {
  for (UParentIter p = begin(); p != end(); ++p) {
    Box* b = *p ? (*p)->toBox() : null;
    if (b) {
      for (View* v = b->getView(0); v != null; v = v->getNext()) {
        if (state) v->addVModes(vmodes); else v->removeVModes(vmodes);
      }
    }
  }
}

}
