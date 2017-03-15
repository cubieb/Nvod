#ifndef _PatPlayerCookie_h_
#define _PatPlayerCookie_h_

/* Foundation */
#include "BaseType.h"
#include "SystemInclude.h"

using std::list;
using std::shared_ptr;
using std::weak_ptr;
using std::string;

class GlobalCfgEntity;
class TsEntity;
class RefsEntity;
class TmssEntity;
class PatViewEntity;
class PatRuntimeInfoEntity;

/**********************class PatPlayerCookie**********************/
class PatPlayerCookie
{
public:
	PatPlayerCookie(shared_ptr<GlobalCfgEntity> globalCfg, shared_ptr<TsEntity> ts);
    ~PatPlayerCookie();

	shared_ptr<GlobalCfgEntity> GetGlobalCfg() const;
    shared_ptr<TsEntity> GetTs() const;

    ContinuityCounter GetNewPatContinuityCounter();

private:
    shared_ptr<GlobalCfgEntity> globalCfg;
    shared_ptr<TsEntity> ts;
    
    ContinuityCounter patContinuityCounter;
};

#endif