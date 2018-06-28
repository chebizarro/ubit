add_executable(xmlparser
    examples/demos/xmlparser.cpp
    examples/demos/zoom.cpp
    examples/demos/zoom.hpp
)

ubit_add_include_dir(xmlparser)

ubit_add_libraries(xmlparser)
