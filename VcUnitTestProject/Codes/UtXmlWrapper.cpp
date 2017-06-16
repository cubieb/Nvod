#include "UtXmlWrapper.h"

/* Cpp Unit */
#include <cppunit/extensions/HelperMacros.h>

/* Foundation */
#include "SystemInclude.h"
#include "Debug.h"

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
    list<const char*> functions =
    {
        "TmssPlayer::TheMovieThreadMain",
        "TmssPlayer::Play",
        //"TmssPlayer::HandlePcrSection",
    };
    DebugFlag& debugFlag = DebugFlag::GetInstance();
    for (list<const char*>::iterator iter = functions.begin(); iter != functions.end(); ++iter)
    {
        debugFlag.SetState(*iter, true);
    }

    for (list<const char*>::iterator iter = functions.begin(); iter != functions.end(); ++iter)
    {
        debugFlag.SetState(*iter, false);
    }
}

/* protected function */
void UtXmlWrapper::TestNvodWebUrlWrapper()
{
    CPPUNIT_ASSERT_THROW(NvodWebUrlWrapperInterface::CreateInstance("aBc123DeF.xml"), runtime_error);

    const char* xmlPath = ".\\..\\InputFile\\UtXmlWrapper.NvodWebUrl.xml";
    shared_ptr<NvodWebUrlWrapperInterface> nvodWebUrlWrapper(NvodWebUrlWrapperInterface::CreateInstance(xmlPath));

    const char* serviceConfigUrl = "http://192.168.3.140:8080/nvodsocket!createtsxml.shtml";
    const char* eventConfigUrl = "http://192.168.3.140:8080/nvodsocket!movieparamxml.shtml";
    CPPUNIT_ASSERT(strcmp(nvodWebUrlWrapper->GetServiceConfigUrl(), serviceConfigUrl) == 0);
    CPPUNIT_ASSERT(strcmp(nvodWebUrlWrapper->GetEventConfigUrl(), eventConfigUrl) == 0);
}

void UtXmlWrapper::TestStaticConfigWrapper()
{
    const char* xmlPath = ".\\..\\InputFile\\UtXmlWrapper.StaticConfig.xml";
    shared_ptr<StaticConfigWrapperInterface> staticConfigWrapper(StaticConfigWrapperInterface::CreateInstance(xmlPath));
    shared_ptr<GlobalCfgEntity> globalCfg = staticConfigWrapper->GetGlobalCfgEntity();
    CPPUNIT_ASSERT(globalCfg->GetPatInterval() == milliseconds(100));
    CPPUNIT_ASSERT(globalCfg->GetPmtInterval() == milliseconds(101));
    CPPUNIT_ASSERT(globalCfg->GetPosterInterval() == milliseconds(102));

    list<shared_ptr<TsEntity>>& tses = staticConfigWrapper->GetTsEntities();
    CPPUNIT_ASSERT(tses.size() == 1);
    TsEntity& ts = **tses.begin();
    CPPUNIT_ASSERT(ts.GetSrcAddr()->s_addr == inet_addr("192.168.3.60"));
    CPPUNIT_ASSERT(ts.GetDstAddr().sin_addr.s_addr == inet_addr("127.0.0.1"));
    CPPUNIT_ASSERT(ts.GetDstAddr().sin_port == htons(1234));

    CPPUNIT_ASSERT(ts.GetTsId() == 1);
    CPPUNIT_ASSERT(ts.GetRefses().size() == 1);
    RefsEntity& refs = **ts.GetRefses().begin();
    CPPUNIT_ASSERT(refs.GetServiceId() == 1);
    CPPUNIT_ASSERT(refs.GetTs()->GetTsId() == 1);

    PstsEntity& psts = *refs.GetPsts();
    CPPUNIT_ASSERT(psts.GetServiceId() == 2);
    CPPUNIT_ASSERT(psts.GetPmtPid() == 201);
    CPPUNIT_ASSERT(psts.GetPosterPid() == 202);

    list<shared_ptr<TmssEntity>>& tmsses = ts.GetTmsses();
    CPPUNIT_ASSERT(tmsses.size() == 2);
    auto tmss = tmsses.begin();
    CPPUNIT_ASSERT((*tmss)->GetServiceId() == 3);
    CPPUNIT_ASSERT((*tmss)->GetPmtPid() == 301);
    CPPUNIT_ASSERT((*tmss)->GetPcrPid() == 302);
    CPPUNIT_ASSERT((*tmss)->GetAudioPid() == 303);
    CPPUNIT_ASSERT((*tmss)->GetVideoPid() == 304);

    ++tmss;
    CPPUNIT_ASSERT((*tmss)->GetServiceId() == 4);
    CPPUNIT_ASSERT((*tmss)->GetPmtPid() == 401);
    CPPUNIT_ASSERT((*tmss)->GetPcrPid() == 402);
    CPPUNIT_ASSERT((*tmss)->GetAudioPid() == 403);
    CPPUNIT_ASSERT((*tmss)->GetVideoPid() == 404);
}

void UtXmlWrapper::TestDynamicConfigWrapper()
{
    const char* xmlPath = ".\\..\\InputFile\\UtXmlWrapper.StaticConfig.xml";
    shared_ptr<StaticConfigWrapperInterface> staticConfigWrapper(StaticConfigWrapperInterface::CreateInstance(xmlPath));
    list<shared_ptr<TsEntity>>& tses = staticConfigWrapper->GetTsEntities();

    DynamicConfigWrapperInterface& dynamicConfigWrapper = DynamicConfigWrapperInterface::GetInstance();
    dynamicConfigWrapper.Read(".\\..\\InputFile\\UtXmlWrapper.DynamicConfig.xml", tses);

    TsEntity& ts = **tses.begin();
    RefsEntity& refs = **ts.GetRefses().begin();
    list<shared_ptr<TmssEntity>>& tmsses = ts.GetTmsses();

    CPPUNIT_ASSERT(refs.GetRefsEvents().size() == 1);
    RefsEventEntity& refsEvent = **refs.GetRefsEvents().begin();
    CPPUNIT_ASSERT(refsEvent.GetEventId() == 1);
    CPPUNIT_ASSERT(refsEvent.GetDuration().count() == 86400);

    CPPUNIT_ASSERT(refsEvent.GetPosters().size() == 1);
    PosterEntity& poster = **refsEvent.GetPosters().begin();
    CPPUNIT_ASSERT(poster.GetPosterId() == 1001);
    CPPUNIT_ASSERT(strcmp(poster.GetRemotePath().c_str(), "ftp://127.0.0.1/sample.1001.jpg") == 0);

    CPPUNIT_ASSERT(refsEvent.GetMovies().size() == 1);
    MovieEntity& movie = **refsEvent.GetMovies().begin();
    CPPUNIT_ASSERT(movie.GetMovieId() == 1001);
    CPPUNIT_ASSERT(strcmp(movie.GetRemotePath().c_str(), "ftp://127.0.0.1/sample.1001.ts") == 0);

    CPPUNIT_ASSERT(tmsses.size() == 2);
    auto tmss = tmsses.begin();
    CPPUNIT_ASSERT((*tmss)->GetServiceId() == 3);
    CPPUNIT_ASSERT((*tmss)->GetTmssEvents().size() == 2);
    auto tmssEvent = (*tmss)->GetTmssEvents().begin();
    CPPUNIT_ASSERT((*tmssEvent)->GetEventId() == 301);
    CPPUNIT_ASSERT((*tmssEvent)->GetDuration().count() == 1200);
    std::time_t t = system_clock::to_time_t((*tmssEvent)->GetStartTimePoint());
    //cout << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S");

    ++tmssEvent;
    CPPUNIT_ASSERT((*tmssEvent)->GetEventId() == 302);
    CPPUNIT_ASSERT((*tmssEvent)->GetDuration().count() == 1200);
    t = system_clock::to_time_t((*tmssEvent)->GetStartTimePoint());
    //cout << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S");


    ++tmss;
    CPPUNIT_ASSERT((*tmss)->GetServiceId() == 4);
    tmssEvent = (*tmss)->GetTmssEvents().begin();
    CPPUNIT_ASSERT((*tmssEvent)->GetEventId() == 401);
    CPPUNIT_ASSERT((*tmssEvent)->GetDuration().count() == 1200);
    t = system_clock::to_time_t((*tmssEvent)->GetStartTimePoint());
    //cout << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S");

    ++tmssEvent;
    CPPUNIT_ASSERT((*tmssEvent)->GetEventId() == 402);
    CPPUNIT_ASSERT((*tmssEvent)->GetDuration().count() == 1200);
    t = system_clock::to_time_t((*tmssEvent)->GetStartTimePoint());
    //cout << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S");
}

CxxEndNameSpace
