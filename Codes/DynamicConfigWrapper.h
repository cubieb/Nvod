#ifndef _DynamicConfigWrapper_h_
#define _DynamicConfigWrapper_h_

#include "XmlWrapperInterface.h"

/* Foundation */
#include "BaseType.h"
#include "SystemInclude.h"

class TsEntity;

/**********************class DynamicEventWrapper**********************/
class DynamicConfigWrapper : public DynamicConfigWrapperInterface
{
public:
    DynamicConfigWrapper() {}
    ~DynamicConfigWrapper() {}

    void Read(const char *xmlPath, list<shared_ptr<TsEntity>>& tsEntities);

private:
};

#endif