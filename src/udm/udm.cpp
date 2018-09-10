/*
 * udm.cpp
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

#include <args.hxx>
#include <rxcpp/rx.hpp>
namespace Rx {
	using namespace rxcpp;
	using namespace rxcpp::schedulers;
}
using namespace Rx;

#include <udm/configuration.h>
#include <udm/display-manager.h>
#include <udm/plymouth.h>
using namespace ubit;

static void start_display_manager() {
	
}

int main(int argc, char *argv[]) {
	
	args::ArgumentParser argumentParser("Ubit Display Manager");
	args::Flag configFlag(argumentParser, "config", "Use configuration file", {'c'});
	args::Flag debugFlag(argumentParser, "debug", "Print debugging messages", {"debug"});
	args::HelpFlag helpFlag(argumentParser, "help", "Display this help menu", {'h', "help"});
	args::Flag versionFlag(argumentParser, "version", "Print version", {'v'});

	try {
		argumentParser.ParseCLI(argc, argv);
	} catch (const args::Help&) {
		std::cout << argumentParser;
		exit(0);
	} catch (const args::ParseError& e) {
		std::cerr << e.what() << std::endl;
		std::cerr << argumentParser;
		exit(1);
	} catch (const args::ValidationError& e) {
		std::cerr << e.what() << std::endl;
		std::cerr << argumentParser;
		exit(2);
	}

	if (args::get(versionFlag)) {
		std::cout << "UDM Version 8" << std::endl;
		exit(0);
	}

	Configuration config;
	DisplayManager displayManager;

	start_display_manager();

	
	auto mainthreadid = std::this_thread::get_id();	
	schedulers::run_loop rl;	
	auto mainthread = observe_on_run_loop(rl);


	auto threads = observe_on_event_loop();
	

}

