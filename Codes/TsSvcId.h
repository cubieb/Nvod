#ifndef _TsSvcId_h_
#define _TsSvcId_h_

#include "SystemInclude.h"
#include <odb/core.hxx>

#include "Type.h"

#pragma db value
class TsSvcId
{
public:
    TsSvcId(): tsId(0), serviceId(0)
    {}
    TsSvcId(const TsSvcId& right): tsId(right.tsId), serviceId(right.serviceId)
    {}
    TsSvcId(TsId tsId, ServiceId serviceId): tsId(tsId), serviceId(serviceId)
    {}
    
    TsSvcId& operator=(const TsSvcId& right)
    {
        tsId =  right.tsId;
        serviceId = right.serviceId;
        return *this;
    }

    TsId GetTsId() const {return tsId;}
    void SetTsId(TsId tsId) {this->tsId = tsId;}

    ServiceId GetServiceId() const {return serviceId;}
    void SetServiceId(ServiceId serviceId) {this->serviceId = serviceId;}
    
    int Compare(const TsSvcId &right) const
    {
        if (tsId < right.tsId)
            return -1;
        if (tsId > right.tsId)
            return 1;
                
        if (serviceId < right.serviceId)
            return -1;
        if (serviceId > right.serviceId)
            return 1;

        return 0;
    }

	/* the following function is provided just for debug */
    void Put(std::ostream& os) const
	{
		os << "{tsId = " << tsId
			<< ", tsId = " << serviceId << "}";
	}

private:
    #pragma db column("TsId") get(GetTsId) set(SetTsId)
    TsId      tsId;
    #pragma db column("SvcId") get(GetServiceId) set(SetServiceId)
    ServiceId serviceId;
};

inline bool operator==(const TsSvcId &left, const TsSvcId &right)
{
    return (left.Compare(right) == 0);
}

inline bool operator!=(const TsSvcId &left, const TsSvcId &right)
{
    return (left.Compare(right) != 0);
}

inline bool operator<(const TsSvcId &left, const TsSvcId &right)
{
    return (left.Compare(right) < 0);
}

inline bool operator>(const TsSvcId &left, const TsSvcId &right)
{
    return (left.Compare(right) > 0);
}

inline bool operator<=(const TsSvcId &left, const TsSvcId &right)
{
    return (left.Compare(right) <= 0);
}

inline bool operator>=(const TsSvcId &left, const TsSvcId &right)
{
    return (left.Compare(right) >= 0);
}

inline std::ostream& operator << (std::ostream& os, const TsSvcId& value) 
{ 
    value.Put(os); 
    return os; 
}

#endif