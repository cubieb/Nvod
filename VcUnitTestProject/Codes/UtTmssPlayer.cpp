#include "UtTmssPlayer.h"

/* Cpp Unit */
#include <cppunit/extensions/HelperMacros.h>

/* Foundation */
#include "SystemInclude.h"
#include "Debug.h"

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
    //list<const char*> functions =
    //{
    //    "TmssPlayer::TheMovieThreadMain",
    //    "TmssPlayer::Play",
    //    //"TmssPlayer::HandlePcrSection",
    //};
    //DebugFlag& debugFlag = DebugFlag::GetInstance();
    //for (list<const char*>::iterator iter = functions.begin(); iter != functions.end(); ++iter)
    //{
    //    debugFlag.SetState(*iter, true);
    //}

    //shared_ptr<GlobalCfgEntity> globalCfg = entityHelper->GetGlobalCfgEntity();

    //shared_ptr<RefsEntity> refs = entityHelper->GetRefsEntity();
    //shared_ptr<PlayerInterface> refsPlayer(CreatePlayerInterface("RefsPlayer", globalCfg, refs));
    //refsPlayer->Start();

    //shared_ptr<TsEntity> ts = entityHelper->GetTsEntity();
    //shared_ptr<PlayerInterface> patPlayer(CreatePlayerInterface("PatPlayer", globalCfg, ts));
    //patPlayer->Start();

    //list<shared_ptr<PlayerInterface>> tmssPlayers;
    //list<shared_ptr<TmssEntity>>& tmsses = ts->GetTmsses();
    //for (auto iter = tmsses.begin(); iter != tmsses.end(); ++iter)
    //{
    //    shared_ptr<PlayerInterface> tmssPlayer(CreatePlayerInterface("TmssPlayer", *iter));
    //    tmssPlayer->Start();
    //    tmssPlayers.push_back(tmssPlayer);
    //}

    //this_thread::sleep_for(chrono::seconds(3600 * 24));

    //for (list<const char*>::iterator iter = functions.begin(); iter != functions.end(); ++iter)
    //{
    //    debugFlag.SetState(*iter, false);
    //}
}
CxxEndNameSpace
