#include <ubit/core/application.h>
#include <gtest/gtest.h>

using namespace ubit;

TEST(ApplicationTest, NewApplication) {
	auto argc = 1;
	char *argv = (char*)"testapp";
	Application app(argc, &argv);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
