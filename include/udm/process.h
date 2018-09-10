/*
 *  process.h 
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
 
#ifndef UDM_PROCESS_H_
#define UDM_PROCESS_H_

class Process {

public:

	void set_log_file(const std::string path, bool log_stdout, LogMode log_mode);

	void set_clear_environment(bool clear_environment);

	bool get_clear_environment();

	void set_env(const std::string name, const std::string value);

	const std::string get_env(const std::string name);

	void set_command(const std::string command);

	const std::string get_command();

	bool start(bool block);

	bool get_is_running();

	Pid get_pid();

	void signal(int signum);

	void stop();

	int get_exit_status();
	

};


#endif // UDM_PROCESS_H_
