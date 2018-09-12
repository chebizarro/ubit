/*
 *  login1.h 
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
 
#ifndef UDM_LOGIN1_H_
#define UDM_LOGIN1_H_

#include <sdbus-c++/sdbus-c++.h>

namespace ubit {

class Login1Seat {

public:
	Login1Seat(std::shared_ptr<sdbus::IObjectProxy> connection, std::string id, std::string path):
		connection_(connection),
		id_(id),
		path_(path) { }

private:

	std::string id_;

	std::string path_;
	
	int signal_id_;
	
	bool can_graphical_;
	bool can_multi_session_;
	
	std::shared_ptr<sdbus::IObjectProxy> connection_;


};

class Login1Service {

public:
	
	Login1Service();
	
	bool connect();


private:
	
	void add_seat(const std::string id, const std::string path);
	
	std::shared_ptr<sdbus::IObjectProxy> connection_;
	
	std::vector<Login1Seat> seats_;
	
	bool connected_;
	
};
}

#endif // UDM_LOGIN1_H_
