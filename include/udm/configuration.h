/*
 *  configuration.h
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

#ifndef UBIT_CONFIGURATION_H_
#define UBIT_CONFIGURATION_H_

#include <list>
#include <string>

namespace ubit {

class Configuration {

public:

	Configuration(std::string config_path);

	template <class T>
	void set(const std::string section, const std::string key, const T value);

	template <class T>
	T get(const std::string section, const std::string key);



	//bool load_from_file(const std::string path, GList* *messages, GError* *error);

	//bool load_from_standard_locations(const std::string path, GList* *messages);

	const std::string get_directory();

	std::list<std::string> get_groups();

	std::list<std::string> get_keys(const std::string group_name);

	bool has_key(const std::string section, const std::string key);

	//GList* get_sources();

	const std::string get_source(const std::string section, const std::string key);



	void set_string_list(const std::string section, const std::string key, const std::list<std::string> value, size_t length);

	std::list<std::string> get_string_list(const std::string section, const std::string key);


private:
	
	std::string directory_;

};

}
#endif // UBIT_CONFIGURATION_H_
