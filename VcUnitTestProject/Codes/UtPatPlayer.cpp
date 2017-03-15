#include "UtPatPlayer.h"

/* Cpp Unit */
#include <cppunit/extensions/HelperMacros.h>

/* Foundation */
#include "SystemInclude.h"

/* Entity */
#include "Entities.h"

/* Functions */
#include "PlayerInterface.h"

/* local head files */
#include "EntityHelper.h"

using namespace std;

CxxBeginNameSpace(UnitTest)
/**********************UtTmssPlayer**********************/
CPPUNIT_TEST_SUITE_REGISTRATION(UtPatPlayer);

/* public function */
void UtPatPlayer::setUp()
{
    entityHelper = make_shared<EntityHelper>();
}

/* protected function */
void UtPatPlayer::TestStart()
{
    //shared_ptr<GlobalCfgEntity> globalCfg = entityHelper->GetGlobalCfgEntity();
    //shared_ptr<TsEntity> ts = entityHelper->GetTsEntity();

    //shared_ptr<PlayerInterface> player(CreatePlayerInterface("PatPlayer", globalCfg, ts));
    //player->Start();

    //while (player->IsRunning())
    //{
    //    this_thread::sleep_for(chrono::seconds(1));
    //}
}
CxxEndNameSpace
