enable_testing()

add_executable(apptests
	tests/test_application.cpp
)

target_link_libraries(apptests
	PUBLIC gtest_main
	PUBLIC gmock_main
	PUBLIC gmock
)

ubit_add_include_dir(apptests)

ubit_add_libraries(apptests)

add_test(NAME app_test COMMAND apptests)


add_executable(ubittests
	tests/test_uon.cpp
	tests/test_uzoom.cpp
)

target_link_libraries(ubittests
	PUBLIC gtest_main
	PUBLIC gmock_main
	PUBLIC gmock
)

ubit_add_include_dir(ubittests)

ubit_add_libraries(ubittests)

add_test(NAME ubit_test COMMAND ubittests)
