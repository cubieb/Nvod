#ifndef _StaticConfigWrapper_h_
#define _StaticConfigWrapper_h_

#include "XmlWrapperInterface.h"

/* Foundation */
#include "BaseType.h"
#include "SystemInclude.h"

class GlobalCfgEntity;
class TsEntity;

using std::list;
using std::shared_ptr; 

/**********************class StaticConfigWrapper**********************/
class StaticConfigWrapper : public StaticConfigWrapperInterface
{
public:
    StaticConfigWrapper() {};
    ~StaticConfigWrapper() {};

    void Read(const char *xmlPath);

    shared_ptr<GlobalCfgEntity> GetGlobalCfgEntity();
    list<shared_ptr<TsEntity>>& GetTsEntities();

private:
    shared_ptr<GlobalCfgEntity> globalCfgEntity;
    list<shared_ptr<TsEntity>> tsEntities; /* refses, tmsses, pstses */
};

#endif