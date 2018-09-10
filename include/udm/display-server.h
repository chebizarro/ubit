/*
 *  display-server.h: Display Server for UDM 
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
 
#ifndef UDM_DISPLAY_SERVER_H_
#define UDM_DISPLAY_SERVER_H_

#include <memory>

#include <udm/session.h>

namespace ubit {

class Session;

class DisplayServer {

public:

	const std::string get_session_type();

	DisplayServer* get_parent();

	bool get_can_share();

	int get_vt();

	bool start();

	bool get_is_ready();

	void connect_session(Session* session);

	void disconnect_session(Session* session);

	void stop();

	bool get_is_stopping();


private:
	bool is_ready;
	bool stopping;
	bool stopped;
};

}
#endif // UDM_DISPLAY_SERVER_H_
