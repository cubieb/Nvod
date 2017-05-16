#ifndef _UtDownloaderFtp_h_
#define _UtDownloaderFtp_h_

#include <cppunit/extensions/HelperMacros.h>

/* Foundation */
#include "SystemInclude.h"

class FtpResource;
CxxBeginNameSpace(UnitTest)

/**********************UtDownloaderFtp**********************/
class UtDownloaderFtp : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(UtDownloaderFtp);
    CPPUNIT_TEST(TestDownload);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();

protected:
    void TestDownload();
    
private:
    std::list<std::shared_ptr<FtpResource>> ftpResources;
};

CxxEndNameSpace
#endif