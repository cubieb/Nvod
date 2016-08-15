#ifndef _TsSvcId_h_
#define _TsSvcId_h_

#include "SystemInclude.h"
#include "Type.h"

struct TsSvcId
{
    TsId      tsId;
    ServiceId serviceId;
};

inline bool operator==(const TsSvcId &left, const TsSvcId &right)
{	// test for list equality
    return (left.tsId == right.tsId && left.serviceId == right.serviceId);
}

#endif