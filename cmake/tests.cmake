enable_testing()

add_executable(uappli_tests
	tests/test_uappli.cpp
)

target_link_libraries(uappli_tests
	PUBLIC gtest_main
	PUBLIC gmock_main
	PUBLIC gmock
)

ubit_add_include_dir(uappli_tests)

ubit_add_libraries(uappli_tests)

add_test(NAME appli_test COMMAND uappli_tests)


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
