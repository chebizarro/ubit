/*
*
*  utreebox.cpp
 *  Ubit GUI Toolkit - Version 6
 *  (C) 2009 | Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
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

#ifndef _utreebox_hpp_
#define	_utreebox_hpp_  1

#include <ubit/ulistbox.hpp>

namespace ubit {

/** Tree widget.
  * @see also: Treenode.
*/
class Treebox : public ListBox {
public:
  UCLASS(Treebox)
  
  Treebox(const Args& = Args::none);
  ///< creates a new tree widget (@see also shortcut utreebox()).

  virtual ~Treebox();
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  virtual Treenode* getSelectedNode() const;
  ///< returns the selected node; null if there is no selected node.
  
  virtual void setSelectedNode(Treenode&);
  ///< selects this node.
  
  virtual int getSelectedIndex() const;
  ///< returns the index of the selected node; -1 if there is no selected node.
  
  virtual Treebox& setSelectedIndex(int n);
  ///< selects the Nth node; selects the last node if n = -1.
  
private:
    friend class Treenode;
};

Treebox& utreebox(const Args& = Args::none);
///< shortcut that returns *new Treebox().

/** Node in a Tree widget.
* @see also: Treebox.
*/
class Treenode : public Box {
public:
  UCLASS(Treenode)

  Treenode(const Args& label = Args::none);
  ///< creates a new node in a Treebox (@see also shortcut utreenode()).

  Treenode(const Args& label, const Args& subnode_list);
  ///< creates a new node in a Treebox (@see also shortcut utreenode()).
 
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
  virtual void addNode(Treenode&);
  virtual void removeNode(Treenode&);

  virtual void expand(bool = true);
  virtual void collapse() {expand(false);}
  ///< expand/collapse the treenode.

  //virtual void setOffset(UHmargin&);

  Box& label() {return *plabel;}
  const Box& label() const {return *plabel;}
  ///< the box that contains the label of the treenode.
  
  Box& subnodes() {return *psubnodes;}
  const Box& subnodes() const {return *psubnodes;}
  ///< the box that contains the subnodes of the treenode.
  
private:
  friend class Treebox;
  uptr<Box> plabel, psubnodes;
  uptr<Box> pexpander;
  void constructs(const Args& label, const Args& subnode_list);
};

Treenode& utreenode(const Args& label = Args::none);
///< shortcut that returns *new Treenode().

Treenode& utreenode(const Args& label, const Args& subnode_list);
///< shortcut that returns *new Treenode().

} 
#endif

