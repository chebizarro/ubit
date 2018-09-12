#project(UDM)

add_executable(udm
	src/udm/udm.cpp
	src/udm/plymouth.cpp
	src/udm/display-manager.cpp
	src/udm/login1.cpp
)

ubit_add_include_dir(udm)

target_link_libraries(udm
	${CONAN_LIBS}
	-lpthread
	-lsystemd
)

#add_executable(udmtests)
