find_package(GTest REQUIRED)

include_directories(
	PRIVATE src
	PUBLIC ${GTEST_INCLUDE_DIRS}
)

add_executable(ubittests
	tests/test_uappli.cpp
	tests/test_uon.cpp
	tests/test_uzoom.cpp
)

target_link_libraries(ubittests
	PUBLIC ${GTEST_LIBRARIES}
	PUBLIC /usr/lib/libgmock_main.a
	PUBLIC /usr/lib/libgmock.a
)

ubit_add_libraries(ubittests)
