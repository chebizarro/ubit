add_executable(table
    demos/table.cpp
    demos/viewer.cpp
    demos/viewer.hpp
)

target_include_directories(table
    PUBLIC src
)

target_link_libraries(table
    PRIVATE ubit
	PUBLIC -lGL
	PUBLIC -lX11
	PUBLIC -lpthread
	PUBLIC -lglut
	PUBLIC -lGLU
	PUBLIC -lXmu
	PUBLIC -lfreetype
	PUBLIC -lftgl
)
