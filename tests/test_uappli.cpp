#include <ubit/uappli.hpp>
#include <gtest/gtest.h>

using namespace ubit;

TEST(UAppliTest, NewAppli) {
	auto argc = 1;
	char *argv = (char*)"testapp";
	UAppli appli(argc, &argv);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
