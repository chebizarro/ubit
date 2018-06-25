add_library(ums STATIC
	src/ums/umsd.cpp
	src/ums/umserver.cpp
	src/ums/flow.cpp
	src/ums/source.cpp
	src/ums/events.hpp
	src/ums/macevents.hpp
	src/ums/request.cpp
	src/ums/calib.cpp
	src/ums/remoteserver.cpp
	src/ums/zeroconf.cpp
	src/ums/posixloop.cpp
	src/ums/macloop.cpp
	src/ums/macevents.cpp
	src/ums/xevent.cpp
	src/ums/events.cpp
	src/ums/xwin.cpp
	src/ums/umserver.hpp
	src/ums/flow.hpp
	src/ums/source.hpp
	src/ums/calib.hpp
	src/ums/remoteserver.hpp
	src/ums/zeroconf.hpp
)

ubit_add_include_dir(ums)

ubit_add_libraries(ums)
