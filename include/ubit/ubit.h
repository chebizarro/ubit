/*
 *  ubit.h: includes all common Ubit headers 
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


#ifndef _ubit_hpp_
#define	_ubit_hpp_ 1

// udefs.hpp includes <ubit/ubit_features.h> that defines defines crucial compilation options
// and is generated by 'configure' or 'umake' (which are in the Ubit base directory).
#include <ubit/udefs.hpp>
#include <ubit/core/object.h>
#include <ubit/core/node.h>
#include <ubit/core/length.h>
#include <ubit/ugeom.hpp>
#include <ubit/core/number.h>

#include <ubit/core/attribute.h>
#include <ubit/uboxgeom.hpp>
#include <ubit/ui/uborder.h>
#include <ubit/ui/background.h>
#include <ubit/draw/color.h>
#include <ubit/ui/cursor.h>
#include <ubit/ui/textedit.h>
#include <ubit/draw/font.h>

#include <ubit/uon.hpp>
#include <ubit/ucall.hpp>

#include <ubit/core/data.h>
#include <ubit/core/string.h>
#include <ubit/usymbol.hpp>
#include <ubit/draw/image.h>
#include <ubit/upix.hpp>

#include <ubit/core/element.h>
#include <ubit/ui/box.h>
#include <ubit/uboxes.hpp>
#include <ubit/ui/window.h>
#include <ubit/udialogs.hpp>
#include <ubit/ui/menu.h>
#include <ubit/ui/control-menu.h>
#include <ubit/ui/interactors.h>
#include <ubit/ui/scrollpane.h>
#include <ubit/ui/scrollbar.h>
#include <ubit/ui/slider.h>
#include <ubit/ui/table.h>
#include <ubit/ui/choice.h>
#include <ubit/ui/filechooser.h>
#include <ubit/ui/listbox.h>
#include <ubit/ui/palette.h>
#include <ubit/ui/treebox.h>

#include <ubit/ui/zoom-pane.h>
#include <ubit/ui/glcanvas.h>
#include <ubit/ui/3d.h>

#include <ubit/core/application.h>
#include <ubit/ui/display.h>
#include <ubit/core/event.h>
#include <ubit/ukey.hpp>
#include <ubit/ui/view.h>
#include <ubit/ui/fontmetrics.h>
#include <ubit/ui/update.h>
#include <ubit/ui/updatecontext.h>
#include <ubit/draw/graph.h>
#include <ubit/core/config.h>
#include <ubit/draw/style.h>
#include <ubit/ui/timer.h>
#include <ubit/net/source.h>
#include <ubit/ui/eventflow.h>
#include <ubit/ui/selection.h>
#include <ubit/core/file.h>

#endif

