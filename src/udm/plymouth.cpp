/*
 * plymouth.cpp
 * 
 * Copyright 2018 Chris Daley <chebizarro@gmail.com>
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
 * 
 */
#include <string>
#include "udm/plymouth.h"

namespace ubit {
namespace plymouth {

static bool have_pinged = false;
static bool have_checked_active_vt = false;

static bool is_running = false;
static bool is_active = false;
static bool has_active_vt_ = false;

static bool run_command(std::string command, int* exit_status) {
	std::string command_line = "pymouth " + command;

	*exit_status = system(command.c_str());

	return (*exit_status > 0);
}

static bool command_returns_true(std::string command) {
	
	return true;
}

bool get_is_running() {
	if (!have_pinged) {
		have_pinged = true;
		is_running = command_returns_true("--ping");
		is_active = is_running;
	}
	return is_running;
}

bool get_is_active() {
	return get_is_running() && is_active;
}

bool has_active_vt() {
	if (!have_checked_active_vt) {
		have_checked_active_vt = true;
		has_active_vt_ = command_returns_true("--has-active-vt");
	}
	return has_active_vt_;
}

void deactivate() {
	is_active = false;
	run_command ("deactivate", nullptr);
}

void quit(bool retain_splash) {
	have_pinged = true;
	is_running = false;
	
	if (retain_splash) {
		run_command("quit --retain-splash", nullptr);
	} else {
		run_command("quit", nullptr);
	}
}

}
}
