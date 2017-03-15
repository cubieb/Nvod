#ifndef _RefsPlayerCookie_h_
#define _RefsPlayerCookie_h_

/* Foundation */
#include "BaseType.h"
#include "SystemInclude.h"

using std::list;
using std::shared_ptr;
using std::weak_ptr;
using std::string;

class GlobalCfgEntity;
class TsEntity;
class PosterViewEntity;
class RefsEntity;

class RefsRuntimeInfoEntity;

/**********************class RefsPlayerCookie**********************/
class RefsPlayerCookie
{
public:
    RefsPlayerCookie(shared_ptr<GlobalCfgEntity> globalCfg, shared_ptr<RefsEntity> refs);
    ~RefsPlayerCookie();

    shared_ptr<GlobalCfgEntity> GetGlobalCfg() const;
    shared_ptr<TsEntity> GetTs() const;
    shared_ptr<RefsEntity> GetRefs() const;
    list<shared_ptr<PosterViewEntity>>& GetPosterViews();
    shared_ptr<RefsRuntimeInfoEntity> GetDataPipeRuntimeInfo() const;

private:
    shared_ptr<GlobalCfgEntity> globalCfg;
    shared_ptr<TsEntity> ts;
    shared_ptr<RefsEntity> refs;
    list<shared_ptr<PosterViewEntity>> posterViews;
    shared_ptr<RefsRuntimeInfoEntity> dataPipeRuntimeInfo;
};

/**********************class RefsRuntimeInfoEntity**********************/
class RefsRuntimeInfoEntity
{
public:
    RefsRuntimeInfoEntity();
    ~RefsRuntimeInfoEntity();

    ContinuityCounter GetNewPmtContinuityCounter();
    ContinuityCounter GetNewPosterContinuityCounter();

    Duration GetPmtInterval();
    void SetPmtInterval(Duration pmtInterval);

    Duration GetPosterInterval();
    void SetPosterInterval(Duration posterInterval);

private:
    ContinuityCounter pmtContinuityCounter;
    ContinuityCounter posterContinuityCounter;

    Duration pmtInterval;
    Duration posterInterval;
};

#endif