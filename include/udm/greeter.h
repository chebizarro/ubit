/*
 *  greeter.h 
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
 
#ifndef UDM_GREETER_H_
#define UDM_GREETER_H_

#include <udm/session.h>

namespace ubit {

class Greeter {

public:
	
	void set_file_descriptors(int to_fd, int from_fd);

	void stop();

	void set_pam_services(const std::string pam_service, const std::string autologin_pam_service);

	void set_allow_guest(bool allow_guest);

	void clear_hints();

	void set_hint(const std::string name, const std::string value);

	void idle();

	void reset();

	bool get_guest_authenticated();

	Session* take_authentication_session();

	bool get_start_session();

	bool get_resettable();

	const std::string get_active_username();


};

}

#endif // UDM_GREETER_H_
