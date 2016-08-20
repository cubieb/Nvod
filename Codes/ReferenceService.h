#ifndef _ReferenceServiceService_h_
#define _ReferenceServiceService_h_

#include "SystemInclude.h"
#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>

#include "Type.h"
#include "TsSvcId.h"

#pragma db namespace() pointer(std::shared_ptr)

class ReferenceService;
class ReferenceServiceEvent;
class Movie;

/**********************class ReferenceService**********************/
#pragma db object table("ReferenceService")
class ReferenceService: public std::enable_shared_from_this<ReferenceService>
{
public:
    //typedef odb::lazy_weak_ptr<ReferenceServiceEvent> EventPtrType;
	typedef std::weak_ptr<ReferenceServiceEvent> RefsEventPtrType;
    typedef std::list<RefsEventPtrType> RefsEventsType;
    ReferenceService();

    /*  
    Parameter:
        refsTsId: Time Shifted Service Ts Id
        refsId  : Time Shifted Service Id
    */
    ReferenceService(TsId refsTsId, ServiceId refsId, const char *description);
    ReferenceService(const ReferenceService &refs) ;

    ~ReferenceService();

    TsSvcId GetRefsId() const {return refsId;}
    void SetRefsId(TsSvcId refsId) {this->refsId = refsId;}

    std::string GetDescription() const {return description;}
    void SetDescription(const std::string& description) {this->description = description;}

    RefsEventsType GetEvents() const {return events;}
    void SetEvents(const RefsEventsType& events) {this->events = events;}
	void BindEvent(RefsEventPtrType evnt);
    void UnbindEvent(RefsEventPtrType evnt);

	/* the following function is provided just for debug */
    void Put(std::ostream& os) const;

public:
    typedef std::shared_ptr<ReferenceServiceEvent> RefsEventSharedPtrType;
    RefsEventSharedPtrType GetSharedPtr(RefsEventPtrType ptr) {return ptr.lock();}

private:    
    #pragma db id column("RefsId") get(GetRefsId) set(SetRefsId)
	TsSvcId	   refsId; /* Reference Service Ts Id + Reference Service Id */

    #pragma db column("Description") get(GetDescription) set(SetDescription)
    std::string description;
    
    #pragma db value_not_null inverse(refs) get(GetEvents) set(SetEvents)
    RefsEventsType events;
};

inline std::ostream& operator << (std::ostream& os, const ReferenceService& value) 
{ 
    value.Put(os); 
    return os; 
}

/**********************class ReferenceServiceEvent**********************/
#pragma db object table("ReferenceServiceEvent")
class ReferenceServiceEvent: public std::enable_shared_from_this<ReferenceServiceEvent>
{
public:
    typedef std::shared_ptr<ReferenceService> RefsPtrType;
    typedef std::weak_ptr<Movie> MoviePtrType;
    typedef std::list<MoviePtrType> MoviesType;

    ReferenceServiceEvent();
    ReferenceServiceEvent(TableIndex idx, EventId eventId, TimePoint startTimePoint, Seconds duration);
    ~ReferenceServiceEvent();

    TableIndex GetIndex() const {return idx;}
    void SetIndex(TableIndex idx) {this->idx = idx;}

    EventId GetEventId() const {return eventId;}
    void SetEventId(EventId eventId) {this->eventId = eventId;}

    TimePoint GetStartTimePoint() const {return startTimePoint;}
    void SetStartTimePoint(TimePoint startTimePoint) {this->startTimePoint = startTimePoint;}

    Seconds GetDuration() const {return duration;}
    void SetDuration(Seconds duration) {this->duration = duration;}

    RefsPtrType GetReferenceService() const {return refs;}
    void SetReferenceService(RefsPtrType refs);

    MoviesType GetMovies() const {return movies;}
    void SetMovies(const MoviesType& movies) {this->movies = movies;}
    void BindMovie(MoviePtrType movie);
    void UnbindMovie(MoviePtrType movie);

    /* the following function is provided just for debug */
    void Put(std::ostream& os) const;

public:
    typedef std::shared_ptr<ReferenceService> RefsSharedPtrType;
    typedef std::shared_ptr<Movie> MovieSharedPtrType;
    RefsSharedPtrType GetSharedPtr(RefsPtrType ptr) {return ptr;}
    MovieSharedPtrType GetSharedPtr(MoviePtrType ptr) {return ptr.lock();}

private:
	#pragma db id column("Idx") get(GetIndex) set(SetIndex)
	TableIndex idx;

    #pragma db column("EventId") get(GetEventId) set(SetEventId)
    EventId eventId; 
    #pragma db column("StartTimePoint") get(GetStartTimePoint) set(SetStartTimePoint)
    TimePoint startTimePoint; 
    #pragma db column("Duration") get(GetDuration) set(SetDuration) 
    Seconds duration; 
    
    #pragma db not_null column("Refs") get(GetReferenceService) set(SetReferenceService) 
    RefsPtrType refs;

    //Many-to-Many Relationships with "Movie"
    #pragma db value_not_null inverse(events) get(GetMovies) set(SetMovies)
    MoviesType movies;
};

inline std::ostream& operator << (std::ostream& os, const ReferenceServiceEvent& value) 
{ 
    value.Put(os); 
    return os; 
}

class CompareRefsEventIndex: public std::unary_function<ReferenceServiceEvent, bool>
{
public:
    CompareRefsEventIndex(TableIndex idx)
        : idx(idx)
    {}

    result_type operator()(const argument_type &evnt)
    {
        return (result_type)(evnt.GetIndex() == idx);
    }    

    result_type operator()(const argument_type *evnt)
    {
        return this->operator()(*evnt);
    }

    result_type operator()(const std::shared_ptr<argument_type> evnt)
    {
        return this->operator()(*evnt);
    }    

    result_type operator()(const std::weak_ptr<argument_type> evnt)
    {
        return this->operator()(evnt.lock());
    }

private:
    TableIndex idx;
};

/**********************class Movie**********************/
#pragma db object table("Movie")
class Movie: public std::enable_shared_from_this<Movie>
{
public:
    typedef std::shared_ptr<ReferenceServiceEvent> RefsEventPtrType;
	typedef std::list<RefsEventPtrType> RefsEventsType;

    Movie();
    Movie(MovieId movieId, const char *description);

    MovieId GetMovieId() const {return movieId;}
    void SetMovieId(MovieId movieId) {this->movieId = movieId;}

    std::string GetDescription() const {return description;}
    void SetDescription(const std::string& description) {this->description = description;}
    
	RefsEventsType GetRefsEvents() const {return events;}
	void SetRefsEvents(const RefsEventsType& events) {this->events = events;}
    void BindRefsEvent(RefsEventPtrType refsEvent);
    void UnbindRefsEvent(RefsEventPtrType refsEvent);

	/* the following function is provided just for debug */
    void Put(std::ostream& os) const;

public:
    typedef std::shared_ptr<ReferenceServiceEvent> RefsSharedEventPtrType;
    RefsSharedEventPtrType GetSharedPtr(RefsEventPtrType ptr) {return ptr;}

private:
    #pragma db id column("MovieId") get(GetMovieId) set(SetMovieId)
    MovieId movieId;

    #pragma db column("Description") get(GetDescription) set(SetDescription)
    std::string description;

    /* 
      Many-to-Many Relationships with "ReferenceServiceEvent"
      table is "ON DELETE CASCADE",  but we can't depend on this "DELETE CASCADE", because we 
      have to delete movie object from ReferenceServiceEvent's movies member.
    */
	#pragma db value_not_null unordered table("Movies_RefEvents") \
		id_column("MovieId") value_column("RefsEventIdx") \
		get(GetRefsEvents) set(SetRefsEvents)
	RefsEventsType events;
};

inline std::ostream& operator << (std::ostream& os, const Movie& value) 
{ 
    value.Put(os); 
    return os; 
}

class CompareMovieId: public std::unary_function<Movie, bool>
{
public:
    CompareMovieId(MovieId movieId)
        : movieId(movieId)
    {}

    result_type operator()(const argument_type &movie)
    {
        return (result_type)(movie.GetMovieId() == movieId);
    }    

    result_type operator()(const argument_type *movie)
    {
        return this->operator()(*movie);
    }

    result_type operator()(const std::shared_ptr<argument_type> movie)
    {
        return this->operator()(*movie);
    }    

    result_type operator()(const std::weak_ptr<argument_type> movie)
    {
        return this->operator()(movie.lock());
    }

private:
    MovieId movieId;
};

#endif