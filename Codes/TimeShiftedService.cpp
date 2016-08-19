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

void TimeShiftedService::Put(std::ostream& os) const
{
    size_t width = (size_t)os.width();

	os << string(width, ' ')
        << "tmssId      = " << tmssId << endl
		<< "description = " << description << endl
		<< "events      = " << endl;
    os.width(width + 4);
	for (EventsType::const_iterator i = events.cbegin(); i != events.cend(); ++i)
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

void TimeShiftedServiceEvent::Put(std::ostream& os) const
{
    size_t width = (size_t)os.width();

	os << string(width, ' ') << "{idx = " << idx << ", eventId = " << eventId << "}" << endl;
    os.width(width);
}