add_executable(wspace
    examples/demos/3Dworkspace.cpp
    examples/demos/draw.cpp
    examples/demos/zoom.cpp
    examples/demos/draw.hpp
    examples/demos/zoom.hpp
)

ubit_add_include_dir(wspace)

ubit_add_libraries(wspace)
