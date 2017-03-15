#include "UtTmssPlayer.h"

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
CPPUNIT_TEST_SUITE_REGISTRATION(UtTmssPlayer);

/* public function */
void UtTmssPlayer::setUp()
{
    entityHelper = make_shared<EntityHelper>();
}

/* protected function */
void UtTmssPlayer::TestStart()
{
	shared_ptr<GlobalCfgEntity> globalCfg = entityHelper->GetGlobalCfgEntity();

	shared_ptr<RefsEntity> refs = entityHelper->GetRefsEntity();
	shared_ptr<PlayerInterface> refsPlayer(CreatePlayerInterface("RefsPlayer", globalCfg, refs));
	refsPlayer->Start();

	shared_ptr<TsEntity> ts = entityHelper->GetTsEntity();
	shared_ptr<PlayerInterface> patPlayer(CreatePlayerInterface("PatPlayer", globalCfg, ts));
	patPlayer->Start();
	
	std::shared_ptr<TmssEntity> tmss = entityHelper->GetTmssEntity();
	shared_ptr<PlayerInterface> tmssPlayer(CreatePlayerInterface("TmssPlayer", tmss));
	tmssPlayer->Start();
	
	this_thread::sleep_for(chrono::seconds(3600*24));
}
CxxEndNameSpace
