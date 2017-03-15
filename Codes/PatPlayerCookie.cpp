#include "PatPlayerCookie.h"

/* Foundation */
#include "BaseType.h"
#include "SystemInclude.h"

/* Entity */
#include "Entities.h"

using namespace std;
using namespace std::chrono;
using namespace std::placeholders;

/**********************class PatPlayerCookie**********************/
PatPlayerCookie::PatPlayerCookie(shared_ptr<GlobalCfgEntity> globalCfg, shared_ptr<TsEntity> ts)
{
    this->globalCfg = make_shared<GlobalCfgEntity>(*globalCfg);

    this->ts = make_shared<TsEntity>(*ts);
    list<shared_ptr<RefsEntity>>& refses = ts->GetRefses();
    for (auto iter = refses.begin(); iter != refses.end(); ++iter)
	{
		this->ts->Bind(make_shared<RefsEntity>(**iter));
	}

    list<shared_ptr<TmssEntity>>& tmsses  = ts->GetTmsses();
    for (auto iter = tmsses.begin(); iter != tmsses.end(); ++iter)
    {
        this->ts->Bind(make_shared<TmssEntity>(**iter));
    }
	
    patContinuityCounter = 0;
}

PatPlayerCookie::~PatPlayerCookie()
{}

shared_ptr<GlobalCfgEntity> PatPlayerCookie::GetGlobalCfg() const
{
    return globalCfg;
}

shared_ptr<TsEntity> PatPlayerCookie::GetTs() const
{
    return ts;
}

ContinuityCounter PatPlayerCookie::GetNewPatContinuityCounter()
{
    patContinuityCounter = patContinuityCounter & 0xF;
    return patContinuityCounter++;
}