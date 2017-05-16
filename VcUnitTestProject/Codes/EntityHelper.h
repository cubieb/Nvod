#ifndef _EntityHelper_h_
#define _EntityHelper_h_

#include <cppunit/extensions/HelperMacros.h>

/* Type */
#include "BaseType.h"

/* Foundation */
#include "SystemInclude.h"

/* Entity */
#include "Entities.h"

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