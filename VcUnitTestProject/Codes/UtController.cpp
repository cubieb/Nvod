#include "UtController.h"

/* Cpp Unit */
#include <cppunit/extensions/HelperMacros.h>

/* Foundation */
#include "SystemInclude.h"

/* Functions */
#include "ControllerInterface.h"
#include "XmlWrapperInterface.h"

using namespace std;

CxxBeginNameSpace(UnitTest)

/**********************UtController**********************/
CPPUNIT_TEST_SUITE_REGISTRATION(UtController);

/* public function */
void UtController::setUp()
{
}

/* protected function */
void UtController::TestStart()
{
    //ControllerInterface& controller = ControllerInterface::GetInstance();
    //controller.Start(".\\..\\InputFile\\UtController.NvodWebUrl.xml");
    //this_thread::sleep_for(chrono::seconds(3600 * 24));
}

CxxEndNameSpace