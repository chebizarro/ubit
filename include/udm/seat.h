/*
 *  seat.h
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
 
#ifndef UDM_SEAT_H_
#define UDM_SEAT_H_

#include <list>
#include <memory>
#include <unordered_map>

#include <udm/session.h>
#include <udm/greeter-session.h>

namespace ubit {

class Seat {

public:

	void set_name(const std::string name);

	void set_property(const std::string name, const std::string value);

	const std::string get_string_property(const std::string name);

	std::string *get_string_list_property(const std::string name);

	bool get_boolean_property(const std::string name);

	int get_integer_property(const std::string name);

	const std::string get_name();

	void set_supports_multi_session(bool supports_multi_session);

	void set_share_display_server(bool share_display_server);

	bool start();

	std::list<Session> get_sessions();

	void set_active_session(std::shared_ptr<Session> session);

	std::shared_ptr<Session> get_active_session();

	std::shared_ptr<Session> get_next_session();

	void set_externally_activated_session(std::shared_ptr<Session> session);

	std::shared_ptr<Session> get_expected_active_session();

	std::shared_ptr<Session> find_session_by_login1_id(const std::string login1_session_id);

	bool get_can_switch();

	bool get_allow_guest();

	bool get_greeter_allow_guest();

	bool switch_to_greeter();

	bool switch_to_user(const std::string username, const std::string session_name);

	bool switch_to_guest(const std::string session_name);

	bool lock(const std::string username);

	void stop();

	bool get_is_stopping();

private:

	std::string name_;

	std::unordered_map<std::string, std::string> properties_;
	
	bool supports_multi_session_;
	
	bool share_display_server_;
	
	std::list<DisplayServer> display_servers_;
	
	std::list<Session> sessions_;
	
	std::shared_ptr<Session> active_session_;
	
	std::shared_ptr<Session> next_session_;
	
	bool started_;
	
	bool stopping_;
	
	bool stopped_;
	
	std::shared_ptr<GreeterSession> replacement_greeter_;

};

}

#endif // UDM_SEAT_H_
