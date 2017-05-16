#include "UtWebService.h"

/* Cpp Unit */
#include <cppunit/extensions/HelperMacros.h>

/* Foundation */
#include "SystemInclude.h"
#include "Debug.h"

/* Entity */
#include "Entities.h"

/* Functions */
#include "WebServiceInterface.h"

using namespace std;

CxxBeginNameSpace(UnitTest)
#define HasUtWebService

/**********************UtXmlWrapper**********************/
CPPUNIT_TEST_SUITE_REGISTRATION(UtWebService);

/* public function */
void UtWebService::setUp()
{}

/* protected function */
#ifdef HasUtWebService
void UtWebService::TestDownloadConfigXml()
{
    //const char* staticConfigurl = "http://192.168.3.140:8080/nvodsocket!createtsxml.shtml";
    //const char* staticConfigXmlPath = ".\\..\\InputFile\\UtWebService.StaticConfig.xml";

    //const char* dynamicConfigurl = "http://192.168.3.140:8080/nvodsocket!movieparamxml.shtml";
    //const char* dynamicConfigXmlPath = ".\\..\\InputFile\\UtWebService.DynamicConfig.xml";

    //WebServiceInterface& webService = WebServiceInterface::GetInstance();
    //CPPUNIT_ASSERT(webService.DownloadConfigXml(staticConfigurl, staticConfigXmlPath));
    //CPPUNIT_ASSERT(webService.DownloadConfigXml(dynamicConfigurl, dynamicConfigXmlPath));
}

#else

void UtWebService::TestDownloadConfigXml()
{}

#endif

CxxEndNameSpace