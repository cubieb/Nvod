#ifndef _TimeShiftedService_h_
#define _TimeShiftedService_h_

#include "SystemInclude.h"
#include "Type.h"
#include "ContainerBase.h"
#include "TsSvcId.h"

class TimeShiftedServiceEvent
{
public:
    TimeShiftedServiceEvent(EventId refsEventId, EventId tmssEventId, PosterId posterId,
                            TimePoint startTimePoint, Seconds duration);

    EventId GetRefsEventId()
    {}

    TimePoint GetStartTimePoint()
    {
        return startTimePoint;
    }

private:
    EventId     refsEventId;  /* Reference Service Event Id */
    EventId     tmssEventId;  /* Time Shifted Service Event Id */
    PosterId    posterId;
    TimePoint   startTimePoint;
    Seconds     duration;
};

class TimeShiftedService: public ContainerBase
{
public:        
    typedef Iterator<const char *>::MyIter      iterator;
    typedef ConstIterator<const char *>::MyIter const_iterator;

    /*  
    Parameter:
        tmssTsId: Time Shifted Service Ts Id
        tmssId  : Time Shifted Service Id
        refsTsId: Reference Service Ts Id
        refsId  : Reference Service Id
    */
    TimeShiftedService(TsId tmssTsId, ServiceId tmssId, TsId refsTsId, ServiceId refsId)
        : tmssTsId(tmssTsId), tmssId(tmssId), refsTsId(refsTsId), refsId(refsId)
    {}
    
    iterator Begin();
    iterator End();
    void PushBack(TimeShiftedServiceEvent *tmsEvent);
    void Remove(EventId tmssEventId);

private:
	TsId		tmssTsId; /* Time Shifted Service Ts Id */
	ServiceId	tmssId;   /* Time Shifted Service Id */ 
    TsId        refsTsId; /* Reference Service Ts Id */
    ServiceId   refsId;   /* Reference Service Id */

    std::vector<TimeShiftedServiceEvent> events;
};

class TimeShiftedServices: public ContainerBaseTemplate<std::list<TimeShiftedService*>>
{
public:
    typedef Iterator<TimeShiftedService>::MyIter      iterator;
    typedef ConstIterator<TimeShiftedService>::MyIter const_iterator;

    TimeShiftedServices() {};
    virtual ~TimeShiftedServices() {};
    virtual void Add(TimeShiftedService *tmss) = 0;

    virtual iterator Begin() = 0;
    virtual iterator End() = 0;
    virtual iterator Find(TsId tmssTsId, ServiceId tmssId) = 0;    
    virtual iterator Remove(TsId tmssTsId, ServiceId tmssId) = 0;

    //static function
    // ContainerBase function.
    static NodePtr GetNextNodePtr(NodePtr ptr)
    {   // return reference to successor pointer in node
        ++ptr.myIter;
        return ptr;
    }
    // ContainerBase function.
    static reference GetValue(NodePtr ptr)
    {
        return ((reference)*ptr.myIter);
    }

    static TimeShiftedServices * CreateInstance();
};

#endif