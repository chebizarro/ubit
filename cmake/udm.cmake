#project(UDM)

add_executable(udm
	src/udm/udm.cpp
	src/udm/plymouth.cpp
	src/udm/display-manager.cpp
)

ubit_add_include_dir(udm)

#target_link_libraries(udm ${CONAN_LIBS})

target_link_libraries(udm 
	PRIVATE -lpthread
)

#add_executable(udmtests)
