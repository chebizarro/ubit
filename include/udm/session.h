/*
 *  session.h: Session interface for UDM 
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
 
#ifndef UDM_SESSION_H_
#define UDM_SESSION_H_

#include <udm/session-config.h>
#include <udm/log-file.h>
#include <udm/display-server.h>

namespace ubit {

enum class SessionType { local, remote };

class Session {

public:

	void set_config(SessionConfig* config);

	SessionConfig* get_config();

	const std::string get_type();

	void set_pam_service(const std::string pam_service);

	void set_username(const std::string username);

	void set_do_authenticate(bool do_authenticate);

	void set_is_interactive(bool is_interactive);

	void set_is_guest(bool is_guest);

	bool get_is_guest();

	void set_log_file(const std::string filename, LogMode log_mode);

	void set_display_server(DisplayServer* display_server);

	DisplayServer* get_display_server();

	void set_tty(const std::string tty);

	void set_xdisplay(const std::string xdisplay);

	//void set_x_authority(XAuthority* authority, bool use_system_location);

	void set_remote_host_name(const std::string remote_host_name);

	void set_env(const std::string name, const std::string value);

	const std::string get_env(const std::string name);

	void unset_env(const std::string name);

	void set_argv(std::string *argv);

	bool start();

	bool get_is_started();

	const std::string get_username();

	const std::string get_login1_id();

	const std::string get_console_kit_cookie();

	void respond(struct pam_response* response);

	void respond_error(int error);

	int get_messages_length();

	const struct pam_message* get_messages();

	bool get_is_authenticated();

	int get_authentication_result();

	const std::string get_authentication_result_string();

	void run();

	bool get_is_run();

	void lock();

	void unlock();

	void activate();

	void stop();

	bool get_is_stopping();

};

}

#endif // UDM_SESSION_H_
