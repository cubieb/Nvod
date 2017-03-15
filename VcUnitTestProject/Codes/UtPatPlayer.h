#ifndef _UtPatPlayer_h_
#define _UtPatPlayer_h_

#include <cppunit/extensions/HelperMacros.h>

/* Foundation */
#include "SystemInclude.h"

class EntityHelper;
CxxBeginNameSpace(UnitTest)

/**********************UtTmssPlayer**********************/
class UtPatPlayer : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(UtPatPlayer);
    CPPUNIT_TEST(TestStart);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();

protected:
    void TestStart();

private:
    std::shared_ptr<EntityHelper> entityHelper;
};

CxxEndNameSpace
#endif /* _UtTmssPlayer_h_ */