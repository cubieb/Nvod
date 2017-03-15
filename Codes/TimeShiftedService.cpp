#include "SystemInclude.h"

#include "NvodService.h"
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

const std::list<std::shared_ptr<TimeShiftedServiceEvent>>& TimeShiftedService::GetEvents() const
{
    return events;
}

std::list<std::shared_ptr<TimeShiftedServiceEvent>>& TimeShiftedService::GetEvents()
{
    return events;
}

void TimeShiftedService::SetEvents(const std::list<std::shared_ptr<TimeShiftedServiceEvent>>& events)
{
    this->events = events;
}

void TimeShiftedService::BindEvent(std::shared_ptr<TimeShiftedServiceEvent> evnt)
{
    evnt->BindTimeShiftedService(shared_from_this());
    events.push_back(evnt);
}

void TimeShiftedService::UnbindEvent(std::shared_ptr<TimeShiftedServiceEvent> evnt)
{
    evnt->UnbindTimeShiftedService();
    auto cmp = [evnt](shared_ptr<TimeShiftedServiceEvent> i)->bool
    {
        return (i->GetIndex() == evnt->GetIndex());
    };
    events.remove_if(cmp);
}

void TimeShiftedService::Put(std::ostream& os) const
{
    size_t width = (size_t)os.width();

	os << string(width, ' ')
        << "tmssId      = " << tmssId << endl
		<< "description = " << description << endl
		<< "events      = " << endl;
    os.width(width + 4);
	for (auto i = events.cbegin(); i != events.cend(); ++i)
	{
		os << *i;
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

std::weak_ptr<TimeShiftedService> TimeShiftedServiceEvent::GetTimeShiftedService() const
{
    return tmss;
}

void TimeShiftedServiceEvent::SetTimeShiftedService(std::weak_ptr<TimeShiftedService> tmss)
{
    if (!this->tmss.expired())
    {
        this->tmss.lock()->UnbindEvent(shared_from_this());
    }

    if (!tmss.expired())
    {
        tmss.lock()->BindEvent(shared_from_this());
    }
}

std::shared_ptr<ReferenceServiceEvent> TimeShiftedServiceEvent::GetRefsEvent() const
{
    return refsEvent;
}

void TimeShiftedServiceEvent::SetRefsEvent(std::shared_ptr<ReferenceServiceEvent> refsEvent)
{
    if (this->refsEvent != nullptr)
    {
        this->refsEvent->UnbindTmssEvent(shared_from_this());
    }

    if (refsEvent != nullptr)
    {
        refsEvent->BindTmssEvent(shared_from_this());
    }
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

/* private functions */
void TimeShiftedServiceEvent::BindTimeShiftedService(std::shared_ptr<TimeShiftedService> tmss)
{
    this->tmss = tmss;
}

void TimeShiftedServiceEvent::UnbindTimeShiftedService()
{
    tmss.reset();
}

void TimeShiftedServiceEvent::BindRefsEvent(std::shared_ptr<ReferenceServiceEvent> refsEvent)
{
    this->refsEvent = refsEvent;
}

void TimeShiftedServiceEvent::UnbindRefsEvent()
{
    refsEvent.reset();
}