#include <ubit/uzoom.hpp>
#include "mock_ubox.hpp"
#include <gtest/gtest.h>

using namespace ubit;

TEST(UZoommenuTest, NewZoommenu) {

	MockUBox mock1;
	MockUBox mock2;

	EXPECT_CALL(mock1, toBox()).WillRepeatedly(testing::Return(&mock1));
	EXPECT_CALL(mock2, toBox()).WillRepeatedly(testing::Return(&mock2));

	UZoommenu testmenu(mock1, mock2);

}
