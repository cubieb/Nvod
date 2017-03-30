#include "UtXmlWrapper.h"

/* Cpp Unit */
#include <cppunit/extensions/HelperMacros.h>

/* Foundation */
#include "SystemInclude.h"

/* Entity */
#include "Entities.h"

/* Functions */
#include "XmlWrapperInterface.h"
#include "PlayerInterface.h"

using namespace std;

CxxBeginNameSpace(UnitTest)
/**********************UtXmlWrapper**********************/
CPPUNIT_TEST_SUITE_REGISTRATION(UtXmlWrapper);

/* public function */
void UtXmlWrapper::setUp()
{
}

/* protected function */
void UtXmlWrapper::TestStaticConfigWrapper()
{
    //StaticConfigWrapperInterface& staticConfigWrapper = StaticConfigWrapperInterface::GetInstance();
    //staticConfigWrapper.Read(".\\..\\InputFile\\TempStaticConfig.xml");
    //list<shared_ptr<TsEntity>>& tses = staticConfigWrapper.GetTsEntities();

    //DynamicConfigWrapperInterface& dynamicConfigWrapper = DynamicConfigWrapperInterface::GetInstance();
    //dynamicConfigWrapper.Read(".\\..\\InputFile\\TempDynamicConfig.xml", tses);

    //shared_ptr<GlobalCfgEntity> globalCfg = staticConfigWrapper.GetGlobalCfgEntity();

    //list<shared_ptr<PlayerInterface>> patPlayers;
    //list<shared_ptr<PlayerInterface>> refsPlayers;    
    //list<shared_ptr<PlayerInterface>> tmssPlayers;

    //for (auto iterTs = tses.begin(); iterTs != tses.end(); ++iterTs)
    //{
    //    shared_ptr<PlayerInterface> patPlayer(CreatePlayerInterface("PatPlayer", globalCfg, *iterTs));
    //    patPlayer->Start();
    //    patPlayers.push_back(patPlayer);

    //    list<shared_ptr<RefsEntity>>& refses = (*iterTs)->GetRefses();
    //    for (auto iterRefs = refses.begin(); iterRefs != refses.end(); ++iterRefs)
    //    {
    //        shared_ptr<PlayerInterface> refsPlayer(CreatePlayerInterface("RefsPlayer", globalCfg, *iterRefs));
    //        refsPlayer->Start();

    //        refsPlayers.push_back(refsPlayer);
    //    }

    //    list<shared_ptr<TmssEntity>>& tmsses = (*iterTs)->GetTmsses();
    //    for (auto iterTmss = tmsses.begin(); iterTmss != tmsses.end(); ++iterTmss)
    //    {
    //        shared_ptr<PlayerInterface> tmssPlayer(CreatePlayerInterface("TmssPlayer", *iterTmss));
    //        tmssPlayer->Start();
    //        tmssPlayers.push_back(tmssPlayer);
    //    }
    //}
	
    //this_thread::sleep_for(chrono::seconds(3600*24));
}
CxxEndNameSpace
