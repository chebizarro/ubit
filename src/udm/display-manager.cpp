/*
 *  display-manager.cpp: Display Manager for UDM 
 *  Ubit GUI Toolkit - Version 8
 *  (C) 2018 Chris Daley
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
#include <memory>

#include <udm/display-manager.h>
#include <udm/plymouth.h>

namespace ubit {

DisplayManager::DisplayManager():
stopped_subscriber_(stopped_subject_.get_subscriber()),
seat_subscriber_(seat_removed_subject_.get_subscriber()) {
	stopped = stopped_subject_.get_observable();
	seat_removed = seat_removed_subject_.get_observable();
}

DisplayManager::~DisplayManager() {
	stopped_subscriber_.on_completed();
	seat_subscriber_.on_completed();
}

void DisplayManager::start() {
	if(plymouth::get_is_active()) {
		plymouth::quit(false);
	}
}

void DisplayManager::stop() {
	stopped_subscriber_.on_next(std::make_shared<DisplayManager>(*this));
}

}