/*
 *  session-config.h 
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
 
#ifndef UDM_SESSION_CONFIG_H_
#define UDM_SESSION_CONFIG_H_

#include <vector>

namespace ubit {

class SessionConfig {

public:

	SessionConfig(const std::string filename, const std::string default_session_type);

	const std::string get_command();

	const std::string get_session_type();

	std::vector<std::string> get_desktop_names();

	bool get_allow_greeter();

};

}

#endif // UDM_SESSION_CONFIG__H_
