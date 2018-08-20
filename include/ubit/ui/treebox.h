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

#include <memory>

#include <ubit/ui/listbox.h>

namespace ubit {

/** Tree widget.
  * @see also: TreeNode.
*/
class TreeBox : public ListBox {
public:
  UCLASS(TreeBox)
  
  TreeBox(const Args& = Args::none);
  ///< creates a new tree widget (@see also shortcut utreebox()).

  virtual ~TreeBox();
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  virtual TreeNode* getSelectedNode() const;
  ///< returns the selected node; null if there is no selected node.
  
  virtual void setSelectedNode(TreeNode&);
  ///< selects this node.
  
  virtual int getSelectedIndex() const;
  ///< returns the index of the selected node; -1 if there is no selected node.
  
  virtual TreeBox& setSelectedIndex(int n);
  ///< selects the Nth node; selects the last node if n = -1.
  
private:
    friend class TreeNode;
};

TreeBox& utreebox(const Args& = Args::none);
///< shortcut that returns *new TreeBox().

/** Node in a Tree widget.
* @see also: TreeBox.
*/
class TreeNode : public Box {
public:
  UCLASS(TreeNode)

  TreeNode(const Args& label = Args::none);
  ///< creates a new node in a TreeBox (@see also shortcut utreenode()).

  TreeNode(const Args& label, const Args& subnode_list);
  ///< creates a new node in a TreeBox (@see also shortcut utreenode()).
 
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
  virtual void addNode(TreeNode&);
  virtual void removeNode(TreeNode&);

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
  friend class TreeBox;
  std::unique_ptr<Box> plabel, psubnodes;
  std::unique_ptr<Box> pexpander;
  void constructs(const Args& label, const Args& subnode_list);
};

TreeNode& utreenode(const Args& label = Args::none);
///< shortcut that returns *new TreeNode().

TreeNode& utreenode(const Args& label, const Args& subnode_list);
///< shortcut that returns *new TreeNode().

} 
#endif

