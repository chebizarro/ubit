/*
 *  choice.cpp: item chooser in a list
 *  Ubit GUI Toolkit - Version 8
 *  (C) 2018 Chris Daley
 *  (C) 2009 | Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */

#include <ubit/ubit_features.h>
#include <iostream>
#include <ubit/ubox.hpp>
#include <ubit/ucall.hpp>
#include <ubit/uon.hpp>
#include <ubit/ubox.hpp>
#include <ubit/core/event.h>
#include <ubit/uchoice.hpp>
#include <ubit/uupdate.hpp>
#include <ubit/ueventflow.hpp>
#include <ubit/ucall.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT
  
static UChoice::IsSelectable is_selectable_by_default;
bool UChoice::IsSelectable::operator()(const Box* box) const {return box->isArmable();}
void UChoice::setSelectionRule(IsSelectable& v) {is_selectable = &v;}

UChoice::UChoice() :
  callbacks(ucall(this, &UChoice::mouseCB)),
  container(null),
  last_browsing_group(null),
  sel_items(new Element()),
  is_selectable(&is_selectable_by_default)
  //sel_mode(SINGLE_SELECTION), 
  // complement non standard : norlamement la selection se fait au release
  // ce qui permet d'annuler
  //sel_style(SELECT_ON_ARM|SELECT_ON_DRAG) 
{}

UChoice::~UChoice() {
  destructs();
}

void UChoice::addingTo(Child& c, Element& parent) {
  Attribute::addingTo(c, parent);
  
  container = parent.toBox();
  if (!container) {
    Application::warning("UChoice::addingTo","parent of this UChoice (%p) should be a Box", this);
    return;
  }
  // the parent can select its ARMable children
  container->setBrowsable(true);
  container->observeChildrenEvents(UOn::mpress / *callbacks);
  container->observeChildrenEvents(UOn::mrelease / *callbacks);
  container->observeChildrenEvents(UOn::arm / *callbacks);
  container->observeChildrenEvents(UOn::disarm / *callbacks);
  container->observeChildrenEvents(UOn::select / *callbacks);
  container->observeChildrenEvents(UOn::deselect / *callbacks);
  //NB: multicond: poserait le pbm de la desctruction auto
}

//NB: removingFrom() requires a destructor to be defined
void UChoice::removingFrom(Child& c, Element& parent) {
  if (container) {
    // bug si plusieurs
    container->setBrowsable(false);
    // et enlever onChildCalls....
  }
  Attribute::removingFrom(c, parent);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UChoice::update() {
  //if (container) container->update(Update::paint, now);
}

void UChoice::putProp(UpdateContext*, Element&) {
}

Box* UChoice::setSelectedItem(Box* obj) {
  if (obj) return setSelectedItem(*obj);
  else {
    clearSelection();
    return null;
  }
}

void UChoice::mouseCB(UInputEvent& e) {
  const Condition& cond = e.getCond();

  // *** browsing 
  
  if (cond == UOn::mpress) {
    // il faut preserver et restituer le precedent browsing_group
    // (cas ou le listbox est dans un menu, etc.)
    //last_browsing_group = e.getFlow()->getBrowsingGroup();
    //e.getFlow()->setBrowsingGroup(container);
  }
  else if (cond == UOn::mrelease) {
    //e.getFlow()->setBrowsingGroup(last_browsing_group);
  }

  // *** selection

  // on n'est pas cense selectionner le listbox lui-meme
  Box* source = e.getSource() ? e.getSource()->toBox() : null;
  if (!source || source == container) return;
  if (!(*is_selectable)(source)) return;

  // NB: ne marche pas si on fait directement un select sur un item
  // (il faut faire selectItem() sur le choice ou le listbox)
  
  if (cond == UOn::arm) {
    // les objets dans la liste NE DOIVENT PAS etre CAN_SELECT sinon armBehavior()
    // (qui est appele au Disarm) va les deslectionner
    //if (source->isCmode(UMode::CAN_SELECT)) source->setCmodes(UMode::CAN_SELECT,false);

    if (e.toMouseEvent() && e.toMouseEvent()->isBrowsing()) { 
      //if (sel_style & SELECT_ON_DRAG) setSelectedItemImpl(source, &e);
    }
    else {
      //if (sel_style & SELECT_ON_ARM) setSelectedItemImpl(source, &e);
    }
  }
  else if (cond == UOn::disarm) {
    //cerr << "disarm "<< e.getSource() <<endl;
    /*if (sel_style & SELECT_ON_DISARM)*/ setSelectedItemImpl(source, &e);
  }
  else if (cond == UOn::select) {
    //cerr << "select" << e.getSource()<< endl;
    setSelectedItemImpl(source, &e);
  }
  else if (cond == UOn::deselect) {
    //cerr << "deselect" << e.getSource()<< endl;
    // reselectionner obligatoirement les objets deselectionnes
    // par l'utilisateur (qui a clique 2 fois dessus)
    setSelectedItemImpl(source, &e);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// NB: actuellement il n'y a qu'une seule selection mais elle est stockee
// dans un Element de telle sorte qu'il n'y ait pas de probleme si elle
// elle est detruite par ailleurs (elle sera auto enlevee du Element)

Box* UChoice::getSelectedItemImpl() const {
  Node* b = null;
  if (sel_items->cbegin() != sel_items->cend()) b = *sel_items->cbegin();
  return b ? b->toBox() : null;
}

// NB: item can be null
Box* UChoice::setSelectedItemImpl(Box* item, UInputEvent* source_event) {
  if (!container) return null;
  Box* selected_item = getSelectedItemImpl();
  
  // verifier que l'item est bien dans la hierarchie
  if (item && !item->isChildOf(*container, true)) item = null;
    
  if (item == selected_item) {
    // forcer la selection dans le cas ou l'on clique 2 fois de suite
    // sur le meme item
    if (item && !item->isSelected()) item->setSelected(true, true);
    // (par contre on n'appelle pas changeCB() : rien n'a change
    // du point de vue du ListBox
  }
  else {
    // verifier que selected_item est toujours dans la hierarchie
    if (selected_item && selected_item->isChildOf(*container, true))
      selected_item->setSelected(false, true);  // 2nd arg: call item callbacks

    //setSelectedItemImpl(item);
    sel_items->removeAll();
    // item==null when called by clearSelection()
    if (item) sel_items->add(item);
    if (item) item->setSelected(true, true);   // 2nd arg: call item callbacks

    changeCB(source_event);
  }

  // a la difference de change(), action() est appele dans tous les cas
  actionCB(source_event);
  return item;
}


Box* UChoice::setSelectedItem(Box& item) {
  return setSelectedItemImpl(&item, null);
}

Box* UChoice::setSelectedIndex(int index) {
  return setSelectedItemImpl(getSelectableItem(index), null);
}

void UChoice::clearSelection() {
  setSelectedItemImpl(null, null);
}


Box* UChoice::getSelectedItem() const {
  if (!container) return null;

  // att si detruit: verifier que selected_item est toujours dans la hierarchie
  Box* selected_item = getSelectedItemImpl();
  if (selected_item && selected_item->isChildOf(*container, true))
    return selected_item;
  else return null;
}

int UChoice::getSelectedIndex() const {
  Box* selected_item = getSelectedItemImpl();
  if (!container || !selected_item) return -1;
  int count = 0;
  for (ChildIter i = container->cbegin(); i != container->cend(); ++i) {
    Box* box = (*i)->toBox();
    //if (box && box->isArmable()) {
    if (box && (*is_selectable)(box)) {
      if (box == selected_item) return count;
      ++count;
    }
  }
  return -1; // not found
}

//int UChoice::getSelectedIndex(Int& index) const {
//  index = getSelectedIndex();
//  return index.intValue();
//}


Box* UChoice::getSelectableItem(int index) const {
  if (!container) return null;
  int count = 0;
  for (ChildIter i = container->cbegin(); i != container->cend(); ++i) {
    Box* box = (*i)->toBox();
    //if (box && box->isArmable()) {
    if (box && (*is_selectable)(box)) {
      if (count == index) return box;
      ++count;
    }
  }
  return null;
}

Box* UChoice::getSelectableItem(const String& s, bool ignore_case) const {
  ChildIter i = container->children().findBox(s, ignore_case);
  return (i == container->cend()) ? null : dynamic_cast<Box*>(*i);
}

/* ==================================================== [Elc:] ======= */

void UChoice::changed(bool update_now) {
  changeCB(null);
}

void UChoice::changeCB(UInputEvent* source_event) {
  //EventFlow* flow = source_event ? source_event->getFlow() : null;
  //View* hardwv = source_event ? source_event->getWinView() : null;
  // manque la bonne view et les coords
  
  Box* selected_item = getSelectedItemImpl();
  
  //if (update_now && (bmodes & UMode::NO_AUTO_UPDATE) == 0) update();
  if (_abegin() != _aend()) {
    Event e2(UOn::change, this, selected_item);  //UNodeEvent
    fire(e2);
  }
  
  // ensuite on lance les callbacks des parents
  fireParents(UOn::change, this);
}

// a la difference de UOn::change, UOn::action est appele chaque fois
// que l'on clique un item, meme s'il etait deja selectionne

void UChoice::actionCB(UInputEvent* source_event) {
  //EventFlow* flow = source_event ? source_event->getFlow() : null;
  //View* hardwv = source_event ? source_event->getWinView() : null;
  Box* selected_item = getSelectedItemImpl();
  
  // que les callbacks des parents
  //Event e2(UOn::action, hardwv, flow);
  //e2.setTarget(selected_item);
  //fireParents(e2);
  fireParents(UOn::action, selected_item);
}

/* ==================================================== [Elc:] ======= */

URadioSelect::URadioSelect() :
  can_unselect_mode(false),
  pindex(new Int(-1)),
  pselect_callback(ucall(this, &URadioSelect::itemChanged)) {
  pindex->onChange(ucall(this, &URadioSelect::setIndexImpl));
}

URadioSelect::URadioSelect(Int& idx) :
  can_unselect_mode(false),
  pindex(idx),
  pselect_callback(ucall(this, &URadioSelect::itemChanged)) {
  pindex->onChange(ucall(this, &URadioSelect::setIndexImpl));
}

URadioSelect::~URadioSelect() {
  destructs();
}


void URadioSelect::addingTo(Child& c, Element& parent) {
  Attribute::addingTo(c, parent);
  // ajouter handlers au parent
  // si on clique un item deja selectionne, il faut que
  // les callbacks du client soient appeles 2 fois et dans le bon ordre:
  // - d'abord: pour unselect provoque' par click sur item
  // - ensuite: pour select provoque par radioSelect => son callback
  //   doit etre execute *ensuite*. pour garantir l'ordre on utilise
  //   Uon::action qui est tjrs declenche apres Uon::change (meme si
  //   place dans la attrlist plutot que ds la childlist)

  parent.addAttr(UOn::action / *pselect_callback); 
  // rend le parent selectable (indispensable!)
  parent.setSelectable(true);
}

//NB: removingFrom() requires a destructor to be defined

void URadioSelect::removingFrom(Child& c, Element& parent) {
  // enlever les callbacks (2 occurences!)
  // false -> ne PAS detruire les ucall (qui sont partages!)
  parent.removeAttr(*pselect_callback, false);
  parent.setSelectable(false);
  Attribute::removingFrom(c, parent);
}

void URadioSelect::setCanUnselectMode(bool state) {
  can_unselect_mode = state;
} 

bool URadioSelect::isCanUnselectMode() const {
  return can_unselect_mode;
} 

/* ==================================================== [Elc:] ======= */

void URadioSelect::update() {
  updateAutoParents(Update::paint);
}

void URadioSelect::putProp(UpdateContext*, Element&) {}

void URadioSelect::changed(bool update_now) {
  changed((Element*)null);
}

void URadioSelect::changed(Element* target) {
  //if (update_now && (bmodes & UMode::NO_AUTO_UPDATE) == 0) update();
  if (_abegin() != _aend()) {
    Event e(UOn::change, this, target);  //UNodeEvent
    fire(e);
  } 
  /* FAUX: agirait sur les parents du radioSelect !
     Event e2(Event::change.., null, NULL);
     e2.setChildSource(getSelected());
     fireParents(e2, UOn::change);     // !! objet !!
  */
}


void URadioSelect::itemChanged(UInputEvent& e) {
  Element* new_item = e.getSource();
  if (!new_item) return;

  Element* old_item = getSelectedItem();

  // distinguer cas on et cas off;
  // rien a faire dans cas off sauf si c'est celui qui etait selectionne

  if (new_item->isSelected()) {	    // new_item vient d'etre selectionne
    // pas de changement si cet item etait deja selectionne
    if (new_item != old_item) _selectItem(new_item);
  }

  else {			    // new_item vient d'etre deselectionne
    // mise a jour seulement si cet item etait precedemment selectionne
    if (new_item == old_item) {

      // si le USelect impose qu'un item soit toujours selectionne
      // => reselectionner cet objet (cas ou on clique 2 fois sur l'item)
      //    mais sans rappeler les callbacks du USelect
      if (!can_unselect_mode) new_item->setSelected(true, true);

      // sinon, un 2e click deselectionne effectivement l'item
      else _selectItem(null);
    }
  }
}

/* ==================================================== [Elc:] ======= */

Element* URadioSelect::getSelectedItem() const {
  if (pindex->intValue() < 0) return null;
  int k = 0;
  for (UParentIter p = pbegin(); p != pend(); ++p) {
    if (k == pindex->intValue()) return *p;
    k++;
  }
  return null;	    // pas trouve
}


void URadioSelect::_selectItem(Element* selection) {
  int k = 0;
   for (UParentIter p = pbegin(); p != pend(); ++p) {
    if (*p == selection) {
      *pindex = k;
      return;
    }
    k++;
  }
  *pindex = -1; // not found
}

void URadioSelect::setSelectedItem(Element& selection) {
  _selectItem(&selection);
}

void URadioSelect::clearSelection() {
  _selectItem(null);
}


int URadioSelect::getSelectedIndex() const {
  return pindex->intValue();
}

void URadioSelect::setSelectedIndex(int new_index) {
  if (new_index == -1)		// means last item
    *pindex = parents().size() - 1;
  else *pindex = new_index;
}

void URadioSelect::setSelectedIndex(const Int& new_index) {
  if (new_index.intValue() == -1)		// means last item
    *pindex = parents().size() - 1;
  else *pindex = new_index;
}

void URadioSelect::setIndexImpl() {
  int k = 0;
  Element* found = null;

  for (UParentIter p = pbegin(); p != pend(); ++p) {
    Element* obj = *p;
    if (obj) { 
      if (k == pindex->intValue()) {
        if (!obj->isSelected()) obj->setSelected(true, true);
        found = obj;
      }
      else { // k != new_index
        if (obj->isSelected()) obj->setSelected(false, true);
      }
    }
    k++;
  }
  
  changed(found);     // update();  deja fait objet par objet
}

}
