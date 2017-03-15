#ifndef _UtController_h_
#define _UtController_h_

#include <cppunit/extensions/HelperMacros.h>

/* Foundation */
#include "SystemInclude.h"

CxxBeginNameSpace(UnitTest)

/**********************Converter**********************/
class UtController : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(UtController);
    CPPUNIT_TEST(TestAddMovie);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();

protected:
    void TestAddMovie();
};

CxxEndNameSpace
#endif