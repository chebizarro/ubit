#include <ubit/uzoom.hpp>
#include "mock_ubox.hpp"
#include <gtest/gtest.h>

using namespace ubit;

TEST(UZoommenuTest, NewZoommenu) {

	MockUBox mock1;
	MockUBox mock2;

	UZoommenu testmenu(mock1, mock2);

}
