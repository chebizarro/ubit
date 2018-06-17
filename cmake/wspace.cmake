add_executable(wspace
    demos/3Dworkspace.cpp
    demos/draw.cpp
    demos/zoom.cpp
    demos/draw.hpp
    demos/zoom.hpp
)

target_include_directories(wspace
    PUBLIC src
)

target_link_libraries(wspace
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
