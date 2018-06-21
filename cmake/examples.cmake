add_executable(example1
    examples/example1.cpp
)

ubit_add_include_dir(example1)

ubit_add_libraries(example1)

add_executable(example2
    examples/example2.cpp
)

ubit_add_include_dir(example2)

ubit_add_libraries(example2)


##example3_SOURCES = example3.cpp
##sedit4_SOURCES	= sedit4.cpp
##events_SOURCES	= events.cpp
##tables_SOURCES	= tables.cpp
##LayoutAndText_SOURCES = LayoutAndText.cpp
##Lists_SOURCES	= Lists.cpp
##Tree_SOURCES	= Tree.cpp
##Text1_SOURCES	= Text1.cpp
##Text2_SOURCES	= Text2.cpp
##glcanvas_SOURCES = glcanvas.cpp
