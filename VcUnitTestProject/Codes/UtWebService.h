#ifndef _UtWebService_h_
#define _UtWebService_h_

#include <cppunit/extensions/HelperMacros.h>

/* Foundation */
#include "SystemInclude.h"

CxxBeginNameSpace(UnitTest)

/**********************UtXmlWrapper**********************/
class UtWebService : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(UtWebService);
    CPPUNIT_TEST(TestDownloadConfigXml);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();

protected:
    void TestDownloadConfigXml();
};

CxxEndNameSpace
#endif /* _UtXmlWrapper_h_ */