add_executable(xmlparser
    demos/xmlparser.cpp
    demos/zoom.cpp
    demos/zoom.hpp
)

target_include_directories(xmlparser
    PUBLIC src
)

target_link_libraries(xmlparser
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
