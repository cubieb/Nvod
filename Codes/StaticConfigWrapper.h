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
    StaticConfigWrapper(const char *xmlPath);
    ~StaticConfigWrapper();    

    shared_ptr<GlobalCfgEntity> GetGlobalCfgEntity();
    list<shared_ptr<TsEntity>>& GetTsEntities();

private:
    void Read(const char *xmlPath);

private:
    shared_ptr<GlobalCfgEntity> globalCfgEntity;
    list<shared_ptr<TsEntity>> tsEntities; /* refses, tmsses, pstses */
};

#endif