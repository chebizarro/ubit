add_executable(wspace
    demos/3Dworkspace.cpp
    demos/draw.cpp
    demos/zoom.cpp
    demos/draw.hpp
    demos/zoom.hpp
)

ubit_add_include_dir(wspace)

ubit_add_libraries(wspace)
