/*
 *  user.h: User Account 
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
 
#ifndef UDM_USER_H_
#define UDM_USER_H_


namespace ubit {

class User {
	
public:
	
	std::string get_name(std::string username);
	
	uid_t get_uid();
	
	gid_t get_gid();
	
	std::string get_home_directory();
	
	std::string get_shell();
	
	std:string get_xsession();
	
	void set_xsession(std::string session);
	
	std::string get_language();
	
	void set_language(std::string language);
	
};

}

#endif // UDM_USER_H_
