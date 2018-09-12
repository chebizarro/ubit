#include <ubit/uappli.hpp>
#include <ubit/uzoom.hpp>
#include "mock_ubox.hpp"
#include <gtest/gtest.h>

using namespace ubit;

TEST(UZoommenuTest, NewZoommenu) {

	auto argc = 1;
	char *argv = (char*)"testapp";
	UAppli appli(argc, &argv);


	MockUBox mock1;
	MockUBox mock2;

	EXPECT_CALL(mock1, toBox()).WillRepeatedly(testing::Return(&mock1));
	EXPECT_CALL(mock2, toBox()).WillRepeatedly(testing::Return(&mock2));

	UZoommenu testmenu(mock1, mock2);
	
	//appli.add(testmenu);
	
	//testmenu.show();

}
