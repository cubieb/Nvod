#include "SystemInclude.h"

#include "TimeShiftedService.h"
using namespace std;

/**********************class TimeShiftedService**********************/
TimeShiftedService::TimeShiftedService(): tmssId()
{}

/*  
Parameter:
tmssTsId: Time Shifted Service Ts Id
tmssId  : Time Shifted Service Id
*/
TimeShiftedService::TimeShiftedService(TsId tmssTsId, ServiceId tmssId, const char *description)
	: tmssId(tmssTsId, tmssId), description(description)
{}

TimeShiftedService::TimeShiftedService(const TimeShiftedService &tmss)        
	: tmssId(tmss.tmssId), description(tmss.description)
{}

TimeShiftedService::~TimeShiftedService()
{}


void TimeShiftedService::BindEvent(TmssEventPtrType evnt)
{
    TmssEventSharedPtrType eventPtr = GetSharedPtr(evnt);
    eventPtr->SetTimeShiftedService(shared_from_this());
}

void TimeShiftedService::UnbindEvent(TmssEventPtrType evnt)
{
    TmssEventSharedPtrType eventPtr = GetSharedPtr(evnt);
    eventPtr->SetTimeShiftedService(nullptr);
}

void TimeShiftedService::Put(std::ostream& os) const
{
    size_t width = (size_t)os.width();

	os << string(width, ' ')
        << "tmssId      = " << tmssId << endl
		<< "description = " << description << endl
		<< "events      = " << endl;
    os.width(width + 4);
	for (TmssEventsType::const_iterator i = events.cbegin(); i != events.cend(); ++i)
	{
		os << *(i->lock());
	}
    os.width(width);
}

/**********************class TimeShiftedServiceEvent**********************/
TimeShiftedServiceEvent::TimeShiftedServiceEvent()
{}

TimeShiftedServiceEvent::TimeShiftedServiceEvent(TableIndex idx, EventId eventId, PosterId posterId, 
												 TimePoint startTimePoint, Seconds duration)
	: idx(idx), eventId(eventId), posterId(posterId), 
	  startTimePoint(startTimePoint), duration(duration)
{}

TimeShiftedServiceEvent::~TimeShiftedServiceEvent()
{}

void TimeShiftedServiceEvent::SetTimeShiftedService(TmssPtrType tmss)
{
    TmssSharedPtrType tmssPtr;

    /* step 1: unbind event and current tmss */
    tmssPtr = GetSharedPtr(this->tmss);
    if (tmssPtr != nullptr)
    {
        TimeShiftedService::TmssEventsType events = tmssPtr->GetEvents();
        events.remove_if(CompareTmssEventIndex(idx));
        tmssPtr->SetEvents(events);
    }

    /* step 2.1: add event into tmss */
    tmssPtr = GetSharedPtr(tmss);
    //refs could be null, when user delete the event.
    if (tmssPtr != nullptr) 
    {
        TimeShiftedService::TmssEventsType events = tmssPtr->GetEvents();
        events.push_back(shared_from_this());
        tmssPtr->SetEvents(events);
    }

    /* step 2.2: set tmss member variable. */
    this->tmss = tmssPtr;
}

void TimeShiftedServiceEvent::Put(std::ostream& os) const
{
    size_t width = (size_t)os.width();

	os << string(width, ' ') << "{idx = " << idx 
        << ", eventId = " << eventId 
        << ", posterId = " << posterId
        << "}" << endl;
    os.width(width);
}