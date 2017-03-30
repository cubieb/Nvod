#ifndef _UtXmlWrapper_h_
#define _UtXmlWrapper_h_

#include <cppunit/extensions/HelperMacros.h>

/* Foundation */
#include "SystemInclude.h"

CxxBeginNameSpace(UnitTest)

/**********************UtXmlWrapper**********************/
class UtXmlWrapper : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(UtXmlWrapper);
    CPPUNIT_TEST(TestStaticConfigWrapper);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();

protected:
    void TestStaticConfigWrapper();
};

CxxEndNameSpace
#endif /* _UtXmlWrapper_h_ */