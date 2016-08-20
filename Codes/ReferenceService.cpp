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
    RefsEventSharedPtrType eventPtr = GetSharedPtr(evnt);
    eventPtr->SetReferenceService(shared_from_this());
}

void ReferenceService::UnbindEvent(RefsEventPtrType evnt)
{
    RefsEventSharedPtrType eventPtr = GetSharedPtr(evnt);
    eventPtr->SetReferenceService(nullptr);
}

void ReferenceService::Put(ostream& os) const
{
    size_t width = (size_t)os.width();
	os << string(width, ' ')
	    << "refsId      = " << refsId << endl
		<< "description = " << description << endl
		<< "events      = " << endl;
    os.width(width + 4);
	for (RefsEventsType::const_iterator i = events.cbegin(); i != events.cend(); ++i)
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

void ReferenceServiceEvent::SetReferenceService(RefsPtrType refs)
{
    RefsSharedPtrType refsPtr;

    /* step 1: unbind event and current refs */
    refsPtr = GetSharedPtr(this->refs);
    if (refsPtr != nullptr)
    {
        ReferenceService::RefsEventsType events = refsPtr->GetEvents();
        events.remove_if(CompareRefsEventIndex(idx));
        refsPtr->SetEvents(events);
    }

    /* step 2.1: add event into refs */
    refsPtr = GetSharedPtr(refs);
    //refs could be null, when user delete the event.
    if (refsPtr != nullptr) 
    {
        ReferenceService::RefsEventsType events = refsPtr->GetEvents();
        events.push_back(shared_from_this());
        refsPtr->SetEvents(events);
    }

    /* step 2.2: set refs member variable. */
    this->refs = refsPtr;
}

void ReferenceServiceEvent::BindMovie(MoviePtrType movie)
{    
    MovieSharedPtrType moviePtr = GetSharedPtr(movie);
    moviePtr->BindRefsEvent(shared_from_this());
}

void ReferenceServiceEvent::UnbindMovie(MoviePtrType movie)
{
    MovieSharedPtrType moviePtr = GetSharedPtr(movie);    
    moviePtr->UnbindRefsEvent(shared_from_this());
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
    RefsSharedEventPtrType envtPtr = GetSharedPtr(refsEvent);
    /* add movie into event */
    ReferenceServiceEvent::MoviesType movies = envtPtr->GetMovies();
    movies.push_back(shared_from_this());
    envtPtr->SetMovies(movies);

    /* add event to into list */
    events.push_back(envtPtr);
}

void Movie::UnbindRefsEvent(RefsEventPtrType refsEvent)
{
    RefsSharedEventPtrType envtPtr = GetSharedPtr(refsEvent);

    /* delete movie from event */
    ReferenceServiceEvent::MoviesType movies = envtPtr->GetMovies();
    movies.remove_if(CompareMovieId(movieId));
    envtPtr->SetMovies(movies);

    /* delete event from movie list */    
    events.remove_if(CompareRefsEventIndex(envtPtr->GetIndex()));
}

void Movie::Put(ostream& os) const
{
    size_t width = (size_t)os.width();
	os << string(width, ' ') << "{movieId = " << movieId << "}" << endl;
    os.width(width);
}