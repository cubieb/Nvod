#ifndef _XmlWrapperInterface_h_
#define _XmlWrapperInterface_h_

/* Foundation */
#include "SystemInclude.h"

using std::list;
using std::shared_ptr;

class GlobalCfgEntity;
class TsEntity;

class StaticConfigWrapperInterface
{
public:
    StaticConfigWrapperInterface() {}
    virtual ~StaticConfigWrapperInterface() {}
    static StaticConfigWrapperInterface& GetInstance();

    virtual void Read(const char *xmlPath) = 0;
    virtual shared_ptr<GlobalCfgEntity> GetGlobalCfgEntity() = 0;
    virtual list<shared_ptr<TsEntity>>& GetTsEntities() = 0;
};

class DynamicConfigWrapperInterface
{
public:
    DynamicConfigWrapperInterface() {}
    virtual ~DynamicConfigWrapperInterface() {}
    static DynamicConfigWrapperInterface& GetInstance();

    virtual void Read(const char *xmlPath, list<shared_ptr<TsEntity>>& tsEntities) = 0;
};

#endif