#ifndef _UtSocketServer_h_
#define _UtSocketServer_h_

#include <cppunit/extensions/HelperMacros.h>

/* Foundation */
#include "SystemInclude.h"

class FtpResource;
CxxBeginNameSpace(UnitTest)

/**********************UtSocketServer**********************/
class UtSocketServer : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(UtSocketServer);
    CPPUNIT_TEST(TestStart);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();

protected:
    void TestStart();
};

CxxEndNameSpace
#endif