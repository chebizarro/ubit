/*
 *  display-manager.h: Display Manager for UDM 
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
 
#ifndef UDM_DISPLAY_MANAGER_H_
#define UDM_DISPLAY_MANAGER_H_

#include <list>
#include <udm/seat.h>
#include <rxcpp/rx.hpp>
using namespace rxcpp;
using namespace rxcpp::subjects;

namespace ubit {

class DisplayManager {

public:

	DisplayManager();

	~DisplayManager();

	bool add_seat(Seat* seat);

	std::list<Seat> get_seats() { return seats; }

	Seat* get_seat(const std::string name);

	void start();

	void stop();
	
	observable<std::shared_ptr<DisplayManager>> stopped; 

	observable<std::shared_ptr<Seat>> seat_removed; 
	

private:

	subject<std::shared_ptr<DisplayManager>> stopped_subject_; 
	subscriber<std::shared_ptr<DisplayManager>> subscriber_;

	subject<std::shared_ptr<Seat>> seat_removed_subject_; 
	subscriber<std::shared_ptr<Seat>> seat_subscriber_;

	std::list<Seat> seats;
	
	bool stopping_;
	
	bool stopped_;

};

}

#endif // UDM_DISPLAY_MANAGER_H_
