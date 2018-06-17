#pragma once

#include <cppunit/extensions/HelperMacros.h>

class UonTest : public CPPUNIT_NS::TestCase {
  CPPUNIT_TEST_SUITE( UonTest );
  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp();
  void tearDown();

  void testConstructor();
};
