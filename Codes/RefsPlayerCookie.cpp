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
	this->refs->SetPsts(make_shared<PstsEntity>(*refs->GetPsts()));
    
	this->ts->Bind(this->refs);
	
	list<shared_ptr<RefsEventEntity>> refsEvents = refs->GetRefsEvents();
	for (auto iter = refsEvents.begin(); iter != refsEvents.end(); ++iter)
	{
		shared_ptr<RefsEventEntity> refsEvent = *iter;
		list<shared_ptr<PosterEntity>> posters = refsEvent->GetPosters();

		for (auto iter = posters.begin(); iter != posters.end(); ++iter)
		{
			shared_ptr<PosterEntity> poster = *iter;
			auto cmp = [poster](shared_ptr<PosterViewEntity> right)->bool
			{
				return *poster->GetPosterId() == right->posterId;
			};
			auto result = find_if(posterViews.begin(), posterViews.end(), cmp);
			
			shared_ptr<PosterViewEntity> posterView;
			if (result == posterViews.end())
			{
				posterView = make_shared<PosterViewEntity>();
				posterView->id = poster->GetId();
				posterView->posterId = *poster->GetPosterId();
				posterView->remotePath = *poster->GetRemotePath(); 
				posterView->localPath = *poster->GetLocalPath();
				posterViews.push_back(posterView);
			}
			else
			{
				posterView = *result;
			}

			posterView->refsEventIds.push_back(*(*iter)->GetPosterId());
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