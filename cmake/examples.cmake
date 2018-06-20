add_executable(example1
    examples/example1.cpp
)

target_include_directories(example1
    PUBLIC src
)

target_link_libraries(example1
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

add_executable(example2
    examples/example2.cpp
)

target_include_directories(example2
    PUBLIC src
)

target_link_libraries(example2
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
