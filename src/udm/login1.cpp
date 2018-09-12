/*
 *  login1.cpp 
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

#include <udm/login1.h>

namespace ubit {


Login1Service::Login1Service(): connected_(false) { }

void on_connected(sdbus::Signal& signal) {
	std::cout << "dbus connected" << std::endl;
}

void Login1Service::add_seat(const std::string id, const std::string path) {
	Login1Seat seat(connection_, id, path);
	
	
	
	
	seats_.push_back(seat);
}
	
bool Login1Service::connect() {
	if (connected_)
		return true;
	
	connection_ = sdbus::createObjectProxy("org.freedesktop.login1", "/org/freedesktop/login1");
	connection_->registerSignalHandler("org.freedesktop.login1.Manager", "SeatNew", &on_connected);
	connection_->finishRegistration();
	
	
	
	auto method = connection_->createMethodCall("org.freedesktop.login1.Manager", "ListSeats");
	auto reply = connection_->callMethod(method);
	std::vector<sdbus::Struct<std::string, sdbus::ObjectPath>> result;	
	reply >> result;

	for (auto st : result)
		add_seat(std::get<0>(st), std::get<1>(st));
	
	connected_ = true;
	return true;
}
	
}
