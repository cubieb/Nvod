#ifndef _EntityHelper_h_
#define _EntityHelper_h_

#include <cppunit/extensions/HelperMacros.h>

/* Foundation */
#include "BaseType.h"
#include "SystemInclude.h"

class GlobalCfgEntity;
class TsEntity;
class RefsEntity;
class TmssEntity;
class DataPipeTsEntity;

/**********************EntityHelper**********************/
class EntityHelper
{
public:
    EntityHelper();
    ~EntityHelper();

    std::shared_ptr<GlobalCfgEntity> GetGlobalCfgEntity();
    std::shared_ptr<TsEntity> GetTsEntity();
    std::shared_ptr<RefsEntity> GetRefsEntity();

private:
    std::shared_ptr<GlobalCfgEntity> globalCfgEntity;
    std::shared_ptr<TsEntity> tsEntity;
    std::shared_ptr<RefsEntity> refsEntity;
};

#endif