add_executable(xmlparser
    demos/xmlparser.cpp
    demos/zoom.cpp
    demos/zoom.hpp
)

ubit_add_include_dir(xmlparser)

ubit_add_libraries(xmlparser)
