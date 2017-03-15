#include "UtRefsPlayer.h"

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
/**********************UtRefsPlayer**********************/
CPPUNIT_TEST_SUITE_REGISTRATION(UtRefsPlayer);

/* public function */
void UtRefsPlayer::setUp()
{
    entityHelper = make_shared<EntityHelper>();
}

/* protected function */
void UtRefsPlayer::TestStart()
{
	//shared_ptr<GlobalCfgEntity> globalCfg = entityHelper->GetGlobalCfgEntity();
	//shared_ptr<RefsEntity> refs = entityHelper->GetRefsEntity();

	//shared_ptr<PlayerInterface> player(CreatePlayerInterface("RefsPlayer", globalCfg, refs));
	//player->Start();

	//while (player->IsRunning())
	//{
	//	this_thread::sleep_for(chrono::seconds(1));
	//}
}
CxxEndNameSpace
