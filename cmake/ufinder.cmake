add_executable(ufinder
    demos/ufinder.cpp
)

target_include_directories(ufinder
    PUBLIC src
)

target_link_libraries(ufinder
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
