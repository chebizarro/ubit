find_package(GTest REQUIRED)

include_directories(
	PRIVATE src
	PUBLIC ${GTEST_INCLUDE_DIRS}
)

add_executable(ubittests
	tests/test_uon.cpp
)

target_link_libraries(ubittests
    PRIVATE ubit
	PUBLIC ${GTEST_LIBRARIES}
	PUBLIC -lGL
	PUBLIC -lX11
	PUBLIC -lpthread
	PUBLIC -lglut
	PUBLIC -lGLU
	PUBLIC -lXmu
	PUBLIC -lfreetype
	PUBLIC -lftgl
)
