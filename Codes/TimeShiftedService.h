#ifndef _TimeShiftedService_h_
#define _TimeShiftedService_h_

#include "SystemInclude.h"
#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>

#include "Type.h"
#include "TsSvcId.h"

#pragma db namespace() pointer(std::shared_ptr)

class TimeShiftedService;
class TimeShiftedServiceEvent;

/**********************class TimeShiftedService**********************/
#pragma db object table("TimeShiftedService")
class TimeShiftedService 
{
public:
    //typedef odb::lazy_weak_ptr<TimeShiftedServiceEvent> EventPtrType;
    typedef std::weak_ptr<TimeShiftedServiceEvent> TmssEventPtrType;
    typedef std::vector<TmssEventPtrType> EventsType;
    TimeShiftedService();

    /*  
    Parameter:
        tmssTsId: Time Shifted Service Ts Id
        tmssId  : Time Shifted Service Id
    */
    TimeShiftedService(TsId tmssTsId, ServiceId tmssId, const char *description);
    TimeShiftedService(const TimeShiftedService &tmss);
    ~TimeShiftedService();

    TsSvcId GetTmssId() const {return tmssId;}
    void SetTmssId(TsSvcId tmssId) {this->tmssId = tmssId;}

    std::string GetDescription() const {return description;}
    void SetDescription(const std::string& description) {this->description = description;}

    EventsType GetEvents() const {return events;}
    void SetEvents(const EventsType& events) {this->events = events;}

    /* the following function is provided just for debug */
    void Put(std::ostream& os) const;

private:    
    #pragma db id column("TmssId") get(GetTmssId) set(SetTmssId)
	TsSvcId	   tmssId; /* Time Shifted Service Ts Id + Time Shifted Service Id */

    #pragma db column("Description") get(GetDescription) set(SetDescription)
    std::string description;
    
    #pragma db value_not_null inverse(tmss) get(GetEvents) set(SetEvents)
    EventsType events;
};

inline std::ostream& operator << (std::ostream& os, const TimeShiftedService& value) 
{ 
    value.Put(os); 
    return os; 
}

/**********************class TimeShiftedServiceEvent**********************/
#pragma db object table("TimeShiftedServiceEvent")
class TimeShiftedServiceEvent
{
public:
    typedef std::shared_ptr<TimeShiftedService> TmssPtrType;

    TimeShiftedServiceEvent();
    TimeShiftedServiceEvent(TableIndex idx, EventId eventId, PosterId posterId, 
		TimePoint startTimePoint, Seconds duration);
	~TimeShiftedServiceEvent();

	TableIndex GetIndex() const {return idx;}
	void SetIndex(TableIndex idx) {this->idx = idx;}

    EventId GetEventId() const {return eventId;}
    void SetEventId(EventId eventId) {this->eventId = eventId;}

    PosterId GetPosterId() const {return posterId;}
    void SetPosterId(PosterId posterId) {this->posterId = posterId;}

    TimePoint GetStartTimePoint() const {return startTimePoint;}
    void SetStartTimePoint(TimePoint startTimePoint) {this->startTimePoint = startTimePoint;}
    
    Seconds GetDuration() const {return duration;}
    void SetDuration(Seconds duration) {this->duration = duration;}

    TmssPtrType GetTimeShiftedService() const {return tmss;}
    void SetTimeShiftedService(const TmssPtrType& tmss) {this->tmss = tmss;}
    
    /* the following function is provided just for debug */
    void Put(std::ostream& os) const;

private:
	#pragma db id column("Idx") get(GetIndex) set(SetIndex)
	TableIndex idx;

    #pragma db column("EventId") get(GetEventId) set(SetEventId)
    EventId eventId; 
    #pragma db column("PosterId") get(GetPosterId) set(SetPosterId)
    PosterId posterId;
    #pragma db column("StartTimePoint") get(GetStartTimePoint) set(SetStartTimePoint)
    TimePoint startTimePoint; 
    #pragma db column("Duration") get(GetDuration) set(SetDuration) 
    Seconds duration; 
    
    #pragma db not_null column("Tmss") get(GetTimeShiftedService) set(SetTimeShiftedService) 
    TmssPtrType tmss;
};

inline std::ostream& operator << (std::ostream& os, const TimeShiftedServiceEvent& value) 
{ 
    value.Put(os); 
    return os; 
}

#endif