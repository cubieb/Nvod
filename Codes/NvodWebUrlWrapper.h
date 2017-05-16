#ifndef _NvodWebUrlWrapper_h_
#define _NvodWebUrlWrapper_h_

#include "XmlWrapperInterface.h"

/* Foundation */
#include "BaseType.h"
#include "SystemInclude.h"

class TsEntity;

/**********************class NvodWebUrlWrapper**********************/
class NvodWebUrlWrapper : public NvodWebUrlWrapperInterface
{
public:
    NvodWebUrlWrapper(const char *xmlPath);
    ~NvodWebUrlWrapper();

    const char* GetServiceConfigUrl() const;
    const char* GetEventConfigUrl() const;

private:
    std::string serviceConfigUrl;
    std::string eventConfigUrl;
};

#endif