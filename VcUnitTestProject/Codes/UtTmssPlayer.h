#ifndef _UtTmssPlayer_h_
#define _UtTmssPlayer_h_

#include <cppunit/extensions/HelperMacros.h>

/* Foundation */
#include "SystemInclude.h"

class EntityHelper;
CxxBeginNameSpace(UnitTest)

/**********************UtTmssPlayer**********************/
class UtTmssPlayer : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(UtTmssPlayer);
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