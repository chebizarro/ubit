/*
 *  zoom.cpp: semantic zooming
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
#include <iostream>
#include <ubit/ubit_features.h>
#include <ubit/ui/zoom-pane.h>
#include <ubit/core/boxgeom.h>
#include <ubit/ui/updatecontext.h>
#include <ubit/draw/color.h>
#include <ubit/draw/pix.h>

using namespace std;

namespace ubit {



bool UInscale::verifies(const UpdateContext& ctx, const Element&) const {
  return (ctx.xyscale >= smin && ctx.xyscale <= smax);
}


ZoomMenu::ZoomMenu(Box& zoomed_box, Box& panned_box) :
pzoom_action(new ZoomAction(zoomed_box)),  // for centred zooming
ppan_action(new PanAction(panned_box)),    // for panning
event_mask(Modifier::RightButton | Modifier::ControlDown)
{
  itemRadius() = 0;

  // items 0 and 4 zoom the zoomed_box
  item(0).setAttr(*pzoom_action);
  item(0).add(Font::xx_large + Font::bold + Color::red + "  Z+ ");
  
  item(4).setAttr(*pzoom_action);
  item(4).add(Font::xx_large + Font::bold + Color::red + "  Z-  ");
  
  // items 2 and 6 pan the zoomed_box
  item(2).setAttr(*ppan_action);
  item(2).add(UPix::bigUp);
  
  item(6).setAttr(*ppan_action);
  item(6).add(UPix::bigDown);
}

void ZoomMenu::openMenuOn(int _event_mask) {
  event_mask = _event_mask;
}

// opens menu on event_mask on event_box and propagates events to children otherwise
void ZoomMenu::openMenuCB(MouseEvent& e) {
  if ((e.getButton() & event_mask) || (e.getModifiers() & event_mask))
    open(e);   // open the menu
  else
    e.propagate();  // propagate to children
}


ZoomPane::ZoomPane(Args a) : 
pviewport(new Box(a)),
pmenu(new ZoomMenu(*pviewport, *pviewport)),  // creates pos and scale
ppos(pviewport->obtainAttr<Position>()),
pscale(pviewport->obtainAttr<Scale>())
{
  add(*pviewport);
  catchEvents(UOn::mpress / ucall(pmenu(), &ZoomMenu::openMenuCB));  
}

void ZoomPane::openMenuOn(int event_mask) {
  pmenu->openMenuOn(event_mask);
}

}

