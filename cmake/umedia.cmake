add_executable(umedia
    demos/umedia.cpp
    demos/umedia.hpp
)

target_include_directories(umedia
    PUBLIC src
)

target_link_libraries(umedia
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
