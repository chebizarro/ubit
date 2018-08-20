/*
 *  Selection.hpp
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


#ifndef _Selection_hpp_
#define	_Selection_hpp_

#include <memory>

#include <ubit/udefs.hpp>

namespace ubit {


/** text selection.
*/
class Selection {
public:
  Selection(const Color* color, const Color* bgcolor, const Font*);
  ~Selection();
  
  void copyText(String&);
  void cutText(String&);
  void deleteText();
  void clear();

  Color& color()   {return *pcolor;}
  Color& bgcolor() {return *pbgcolor;}
  Font&  font()    {return *pfont;}

  const Element* getObj() {return in_obj;}

protected:
  void setObj(Element* obj);
  void start(MouseEvent&);
  void extend(MouseEvent&);
  bool complete(MouseEvent&);
  void keyPress(KeyEvent&);
  bool beingSelected() {return being_selected;}
  void perform(String* copy, bool del);

private:
  friend class TextEdit;
  friend class String;
  friend class EventFlow;
  
  bool being_selected;  
  bool rec_sel;  //< recursive selection (the default).
  Element* in_obj;
  
  static const ChildIter null_link;
  // reordered in the left to right order
  ChildIter fromLink, toLink, pressLink, endPressLink, oldLink;
  long  fromPos, toPos, pressPos, oldPos;
  std::unique_ptr <Color> pcolor;
  std::unique_ptr <Color> pbgcolor;
  std::unique_ptr <Font>  pfont;

  void update(UDataContext*);
  void paint(long refreshFromPos, long refreshToPos);
  void paintImpl(Element* obj, bool state, long refreshFromPos, long refreshToPos);
};
}
#endif

