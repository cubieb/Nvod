#include "RefsPlayerCookie.h"

/* Foundation */
#include "BaseType.h"
#include "SystemInclude.h"

/* Entity */
#include "Entities.h"

using namespace std;
using namespace std::chrono;
using namespace std::placeholders;

/**********************class TmssPlayerCookie**********************/
RefsPlayerCookie::RefsPlayerCookie(shared_ptr<GlobalCfgEntity> globalCfg,
    shared_ptr<RefsEntity> refs)
{
	shared_ptr<TsEntity> ts = refs->GetTs().lock();
    this->globalCfg = make_shared<GlobalCfgEntity>(*globalCfg);
    this->ts = make_shared<TsEntity>(*ts);
    this->refs = make_shared<RefsEntity>(*refs);    
    
    /* PosterViewEntity */
    auto cmpposter = [](shared_ptr<PosterEntity> left, shared_ptr<PosterViewEntity> right)->bool
    {
        return *left->GetPosterId() == *right->GetPosterId();
    };
    auto cmprefs = [](shared_ptr<RefsEntity> left, shared_ptr<RefsEntity> right)->bool
    {
        return (*left->GetTsId() == *right->GetTsId() && *left->GetServiceId() == *right->GetServiceId());
    };
    for (auto iter = ts->GetRefses().begin(); iter != ts->GetRefses().end();  ++iter)
    {
        shared_ptr<RefsEntity> refs = *iter;
        list<shared_ptr<RefsEventEntity>> refsEvents = refs->GetRefsEvents();
        for (auto iter = refsEvents.begin(); iter != refsEvents.end(); ++iter)
        {
            shared_ptr<RefsEventEntity> refsEvent = *iter;
            list<shared_ptr<PosterEntity>> posters = refsEvent->GetPosters();
            for (auto iter = posters.begin(); iter != posters.end(); ++iter)
            {
                shared_ptr<PosterViewEntity> posterView;

                auto result1 = find_if(posterViews.begin(), posterViews.end(), bind(cmpposter, *iter, _1));
                if (result1 == posterViews.end())
                {
                    posterView = make_shared<PosterViewEntity>((*iter)->GetId(), *(*iter)->GetPosterId(),
                        (*iter)->GetRemotePath()->c_str(), (*iter)->GetLocalPath()->c_str());
                    posterViews.push_back(posterView);
                }
                else
                {
                    posterView = *result1;
                }

                shared_ptr<RefsEntity> myRefs;

                list<shared_ptr<RefsEntity>> refses = posterView->GetRefses();
                auto result2 = find_if(refses.begin(), refses.end(), bind(cmprefs, refs, _1));
                if (result2 == refses.end())
                {
                    myRefs = make_shared<RefsEntity>(*refs);
                    posterView->Bind(myRefs);
                }
                else
                {
                    myRefs = *result2;
                }

                myRefs->Bind(make_shared<RefsEventEntity>(*refsEvent));
            }
        }
    }
    

    dataPipeRuntimeInfo = make_shared<RefsRuntimeInfoEntity>();
}

RefsPlayerCookie::~RefsPlayerCookie()
{}

shared_ptr<GlobalCfgEntity> RefsPlayerCookie::GetGlobalCfg() const
{
    return globalCfg;
}

shared_ptr<TsEntity> RefsPlayerCookie::GetTs() const
{
    return ts;
}

shared_ptr<RefsEntity> RefsPlayerCookie::GetRefs() const
{
    return refs;
}

list<shared_ptr<PosterViewEntity>>& RefsPlayerCookie::GetPosterViews()
{
    return posterViews;
}

shared_ptr<RefsRuntimeInfoEntity> RefsPlayerCookie::GetDataPipeRuntimeInfo() const
{
    return dataPipeRuntimeInfo;
}

/**********************class RefsRuntimeInfoEntity**********************/
RefsRuntimeInfoEntity::RefsRuntimeInfoEntity()
    : pmtInterval(Duration::zero()), posterInterval(Duration::zero()),
    pmtContinuityCounter(0), posterContinuityCounter(0)
{}

RefsRuntimeInfoEntity::~RefsRuntimeInfoEntity()
{}

ContinuityCounter RefsRuntimeInfoEntity::GetNewPmtContinuityCounter()
{
    pmtContinuityCounter = pmtContinuityCounter & 0xF;
    return pmtContinuityCounter++;
}

ContinuityCounter RefsRuntimeInfoEntity::GetNewPosterContinuityCounter()
{
    posterContinuityCounter = posterContinuityCounter & 0xF;
    return posterContinuityCounter++;
}

Duration RefsRuntimeInfoEntity::GetPmtInterval()
{
    return pmtInterval;
}

void RefsRuntimeInfoEntity::SetPmtInterval(Duration pmtInterval)
{
    this->pmtInterval = pmtInterval;
}

Duration RefsRuntimeInfoEntity::GetPosterInterval()
{
    return posterInterval;
}

void RefsRuntimeInfoEntity::SetPosterInterval(Duration posterInterval)
{
    this->posterInterval = posterInterval;
}