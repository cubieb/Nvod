#include "SystemInclude.h"

#include "ReferenceService.h"
using namespace std;

/**********************class ReferenceService**********************/
ReferenceService::ReferenceService(): refsId()
{}

ReferenceService::ReferenceService(TsId refsTsId, ServiceId refsId, const char *description)
	: refsId(refsTsId, refsId), description(description)
{}

ReferenceService::ReferenceService(const ReferenceService &refs)        
	: refsId(refs.refsId), description(refs.description)
{}

ReferenceService::~ReferenceService()
{}

void ReferenceService::BindEvent(RefsEventPtrType evnt)
{
	events.push_back(evnt);
	evnt.lock()->SetReferenceService(shared_from_this());
}

void ReferenceService::Put(ostream& os) const
{
    size_t width = (size_t)os.width();
	os << string(width, ' ')
	    << "refsId      = " << refsId << endl
		<< "description = " << description << endl
		<< "events      = " << endl;
    os.width(width + 4);
	for (EventsType::const_iterator i = events.cbegin(); i != events.cend(); ++i)
	{
		os << *(i->lock());
	}
    os.width(width);
}

/**********************class ReferenceServiceEvent**********************/
ReferenceServiceEvent::ReferenceServiceEvent()
{}

ReferenceServiceEvent::ReferenceServiceEvent(TableIndex idx, EventId eventId, 
											 TimePoint startTimePoint, Seconds duration)
	: idx(idx), eventId(eventId), startTimePoint(startTimePoint), duration(duration)
{}

ReferenceServiceEvent::~ReferenceServiceEvent()
{}

void ReferenceServiceEvent::BindMovie(MoviePtrType movie)
{    
    shared_ptr<Movie> moviePtr = movie.lock();
	movies.push_back(movie.lock());
	Movie::RefsEventsType events = moviePtr->GetRefsEvents();
	events.push_back(shared_from_this());
    moviePtr->SetRefsEvents(events);
}

void ReferenceServiceEvent::BindReferenceService(RefsPtrType refs)
{
    refs->BindEvent(shared_from_this());
}

void ReferenceServiceEvent::Put(ostream& os) const
{	
    size_t width = (size_t)os.width();
	os << string(width, ' ') << "{idx = " << idx << ", eventId = " << eventId << "}" << endl;
    os.width(width + 4);
	for (MoviesType::const_iterator i = movies.cbegin(); i != movies.cend(); ++i)
	{
		os << *(i->lock());
	}
    os.width(width);
}

/**********************class Movie**********************/
Movie::Movie()
{}

Movie::Movie(MovieId movieId, const char *description)
    : movieId(movieId), description(description)
{} 

void Movie::BindRefsEvent(RefsEventPtrType refsEvent)
{
    refsEvent->BindMovie(shared_from_this());
}

void Movie::Put(ostream& os) const
{
    size_t width = (size_t)os.width();
	os << string(width, ' ') << "{movieId = " << movieId << "}" << endl;
    os.width(width);
}