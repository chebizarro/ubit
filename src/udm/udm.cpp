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

#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <regex>
#include <fstream>
#include <csignal>

#include <args.hxx>
#include <rxcpp/rx.hpp>
namespace Rx {
	using namespace rxcpp;
	using namespace rxcpp::schedulers;
	using namespace rxcpp::util;
	using namespace rxcpp::sources;
}
using namespace Rx;

#include <udm/configuration.h>
#include <udm/display-manager.h>
#include <udm/login1.h>
#include <udm/plymouth.h>
using namespace ubit;

static std::string find_program_in_path(std::string pname) {
	auto env = std::string(getenv("PATH"));
	std::vector<uint8_t> path(env.begin(), env.end());
	
	auto findpath = observable<>::from<std::vector<uint8_t>>(path);
	
	std::string result;
	
	auto strings = findpath.
		map([](std::vector<uint8_t> v){
			std::string s(v.begin(), v.end());
			std::regex delim(R"/(:)/");
			std::sregex_token_iterator cursor(s.begin(), s.end(), delim, -1);
			std::sregex_token_iterator end;
			std::vector<std::string> splits(cursor, end);
			return iterate(std::move(splits));
		}).
		concat().
        subscribe(
			[&result, pname](std::string t) {
				struct stat buffer;
				std::string fname = t + "/" + pname;
				if (stat(fname.c_str(), &buffer) == 0) {
					result = fname;
				}
			});
	
	return result;
}

int main(int argc, char *argv[]) {

	std::signal(SIGPIPE, SIG_IGN);
	std::signal(SIGHUP, SIG_IGN);

	
	args::ArgumentParser argumentParser("Ubit Display Manager");
	args::ValueFlag<std::string> configValue(argumentParser, "file", "Use configuration file", {'c', "config"});
	args::Flag debugFlag(argumentParser, "debug", "Print debugging messages", {"debug"});
	args::HelpFlag helpFlag(argumentParser, "help", "Display this help menu", {'h', "help"});
	args::ValueFlag<std::string> pidValue(argumentParser, "file", "PID file", {"pid-file"});
	args::Flag testFlag(argumentParser, "test_mode", "Run UDM in Test mode", {"test"});
	args::Flag versionFlag(argumentParser, "version", "Print UDM version", {'v'});

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
	
	if (!args::get(testFlag) && getuid() != 0) {
		std::cerr << "Ubit Display Manager must be run as the root user" << std::endl;
		exit(1);
	}

	if (getenv("DISPLAY") && getuid() != 0) {
		auto path = find_program_in_path("Xephyr");
		if (path.empty()) {
			std::cerr << "Xephyr is required to run Ubit Display Manager inside an X Server" << std::endl;
			exit(1);
		}
	}

	std::string pid_path = pidValue ? args::get(pidValue) : "/var/run/udm.pid";
	std::ofstream pid_file(pid_path);
	if (pid_file) {
		pid_file << getpid() << std::endl;
		pid_file.close();
	}

	std::shared_ptr<Configuration> config = std::make_shared<Configuration>(args::get(configValue));
	
	
	
	DisplayManager displayManager;

	if (config.get<bool>("UDM", "dbus-service") {
		
		
	} else {
		
	}


	displayManager.start();

	displayManager.stopped.subscribe(
		[](std::shared_ptr<DisplayManager> dm) {
			std::cout << "Display Manager stopped" << std::endl;
			// exit the run loop
		}
	);

	displayManager.seat_removed.subscribe(
		[&displayManager](std::shared_ptr<Seat> seat) {
			std::cout << "Seat removed" << std::endl;
			displayManager.stop();
		}
	);
		
	Login1Service login1;
	
	if (login1.connect()) {
		
	} else {
		
	}
		
	auto mainthreadid = std::this_thread::get_id();	
	schedulers::run_loop rl;	
	auto mainthread = observe_on_run_loop(rl);


	auto threads = observe_on_event_loop();
	
	return 0;
}

