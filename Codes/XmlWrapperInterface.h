#ifndef _XmlWrapperInterface_h_
#define _XmlWrapperInterface_h_

/* Foundation */
#include "SystemInclude.h"

using std::list;
using std::shared_ptr;

class GlobalCfgEntity;
class TsEntity;

class NvodWebUrlWrapperInterface
{
public:
    NvodWebUrlWrapperInterface() {}
    virtual ~NvodWebUrlWrapperInterface() {}
    static NvodWebUrlWrapperInterface* CreateInstance(const char *xmlPath);

    virtual const char* GetServiceConfigUrl() const = 0;
    virtual const char* GetEventConfigUrl() const = 0;
};

class StaticConfigWrapperInterface
{
public:
    StaticConfigWrapperInterface() {}
    virtual ~StaticConfigWrapperInterface() {}
    static StaticConfigWrapperInterface* CreateInstance(const char *xmlPath);

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