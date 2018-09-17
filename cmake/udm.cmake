#project(UDM)

add_executable(udm
	src/udm/configuration.cpp
	src/udm/display-manager.cpp
	src/udm/login1.cpp
	src/udm/plymouth.cpp
	src/udm/udm.cpp
)

ubit_add_include_dir(udm)

set_property(TARGET udm PROPERTY CXX_STANDARD 17)

target_link_libraries(udm
	${CONAN_LIBS}
	-lpthread
	-lsystemd
)

#add_executable(udmtests)
