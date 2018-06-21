add_executable(table
    demos/table.cpp
    demos/viewer.cpp
    demos/viewer.hpp
)

ubit_add_include_dir(table)

ubit_add_libraries(table)
