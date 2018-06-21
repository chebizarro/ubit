#find_package(GTest REQUIRED)

include_directories(
	PRIVATE src
#	PUBLIC ${GTEST_INCLUDE_DIRS}
)

add_executable(ubittests
	tests/test_uappli.cpp
	tests/test_uon.cpp
	tests/test_uzoom.cpp
)

target_link_libraries(ubittests
	PUBLIC gtest_main
	PUBLIC gmock_main
	PUBLIC gmock
)

ubit_add_libraries(ubittests)

add_test(NAME ubit_test COMMAND ubittests)
