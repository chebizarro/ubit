add_executable(table
    examples/demos/table.cpp
    examples/demos/viewer.cpp
    examples/demos/viewer.hpp
)

ubit_add_include_dir(table)

ubit_add_libraries(table)
