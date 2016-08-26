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
class TimeShiftedService;
class TimeShiftedServiceEvent;
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

    const RefsEventsType& GetEvents() const {return events;}
    RefsEventsType& GetEvents() { return events; }
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
    ReferenceServiceEvent(TableIndex idx, EventId eventId, time_t startTimePoint, time_t duration);
    ~ReferenceServiceEvent();

    TableIndex GetIndex() const {return idx;}
    void SetIndex(TableIndex idx) {this->idx = idx;}

    EventId GetEventId() const {return eventId;}
    void SetEventId(EventId eventId) {this->eventId = eventId;}

    time_t GetStartTimePoint() const {return startTimePoint;}
    void SetStartTimePoint(time_t startTimePoint) {this->startTimePoint = startTimePoint;}

    time_t GetDuration() const {return duration;}
    void SetDuration(time_t duration) {this->duration = duration;}

    RefsPtrType GetReferenceService() const {return refs;}
    void SetReferenceService(RefsPtrType refs);

    const MoviesType& GetMovies() const {return movies;}
    MoviesType& GetMovies() { return movies; }
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
    time_t startTimePoint; 
    #pragma db column("Duration") get(GetDuration) set(SetDuration) 
    time_t duration; 
    
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

/**********************class Clip**********************/
class Clip
{
public:
    Clip(Milliseconds duration) : duration(duration)
    {}

    Milliseconds GetDuration() { return duration; }

private:
    Milliseconds duration;
};

/**********************class Movie**********************/
#pragma db object table("Movie")
class Movie: public std::enable_shared_from_this<Movie>
{
public:
    typedef std::shared_ptr<ReferenceServiceEvent> RefsEventPtrType;
	typedef std::list<RefsEventPtrType> RefsEventsType;
    typedef std::list<std::shared_ptr<Clip>> ClipsType;
    
    Movie();
    Movie(MovieId movieId, const char *description);

    MovieId GetMovieId() const {return movieId;}
    void SetMovieId(MovieId movieId) {this->movieId = movieId;}

    std::string GetDescription() const {return description;}
    void SetDescription(const std::string& description) {this->description = description;}
    
    const RefsEventsType& GetRefsEvents() const { return events; }
	RefsEventsType& GetRefsEvents() {return events;}
	void SetRefsEvents(const RefsEventsType& events) {this->events = events;}
    void BindRefsEvent(RefsEventPtrType refsEvent);
    void UnbindRefsEvent(RefsEventPtrType refsEvent);
    
    //ClipsType GetClips() const { return clips; }
    ClipsType& GetClips() { return clips; }

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

    #pragma db transient
    ClipsType clips;
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


/**********************class TimeShiftedService**********************/
#pragma db object table("TimeShiftedService")
class TimeShiftedService : public std::enable_shared_from_this<TimeShiftedService>
{
public:
    //typedef odb::lazy_weak_ptr<TimeShiftedServiceEvent> EventPtrType;
    typedef std::weak_ptr<TimeShiftedServiceEvent> TmssEventPtrType;
    typedef std::list<TmssEventPtrType> TmssEventsType;
    TimeShiftedService();

    /*
    Parameter:
    tmssTsId: Time Shifted Service Ts Id
    tmssId  : Time Shifted Service Id
    */
    TimeShiftedService(TsId tmssTsId, ServiceId tmssId, const char *description);
    TimeShiftedService(const TimeShiftedService &tmss);
    ~TimeShiftedService();

    TsSvcId GetTmssId() const { return tmssId; }
    void SetTmssId(TsSvcId tmssId) { this->tmssId = tmssId; }

    std::string GetDescription() const { return description; }
    void SetDescription(const std::string& description) { this->description = description; }

    const TmssEventsType& GetEvents() const { return events; }
    TmssEventsType& GetEvents() { return events; }
    void SetEvents(const TmssEventsType& events) { this->events = events; }
    void BindEvent(TmssEventPtrType evnt);
    void UnbindEvent(TmssEventPtrType evnt);

    /* the following function is provided just for debug */
    void Put(std::ostream& os) const;

public:
    typedef std::shared_ptr<TimeShiftedServiceEvent> TmssEventSharedPtrType;
    TmssEventSharedPtrType GetSharedPtr(TmssEventPtrType ptr) { return ptr.lock(); }

private:
#pragma db id column("TmssId") get(GetTmssId) set(SetTmssId)
    TsSvcId	   tmssId; /* Time Shifted Service Ts Id + Time Shifted Service Id */

#pragma db column("Description") get(GetDescription) set(SetDescription)
    std::string description;

#pragma db value_not_null inverse(tmss) get(GetEvents) set(SetEvents)
    TmssEventsType events;
};

inline std::ostream& operator << (std::ostream& os, const TimeShiftedService& value)
{
    value.Put(os);
    return os;
}

/**********************class TimeShiftedServiceEvent**********************/
#pragma db object table("TimeShiftedServiceEvent")
class TimeShiftedServiceEvent : public std::enable_shared_from_this<TimeShiftedServiceEvent>
{
public:
    typedef std::shared_ptr<TimeShiftedService> TmssPtrType;
    typedef std::shared_ptr<ReferenceServiceEvent> RefsEventPtrType;

    TimeShiftedServiceEvent();
    TimeShiftedServiceEvent(TableIndex idx, EventId eventId, PosterId posterId,
        time_t startTimePoint, time_t duration);
    ~TimeShiftedServiceEvent();

    TableIndex GetIndex() const { return idx; }
    void SetIndex(TableIndex idx) { this->idx = idx; }

    EventId GetEventId() const { return eventId; }
    void SetEventId(EventId eventId) { this->eventId = eventId; }

    PosterId GetPosterId() const { return posterId; }
    void SetPosterId(PosterId posterId) { this->posterId = posterId; }

    time_t GetStartTimePoint() const { return startTimePoint; }
    void SetStartTimePoint(time_t startTimePoint) { this->startTimePoint = startTimePoint; }

    time_t GetDuration() const { return duration; }
    void SetDuration(time_t duration) { this->duration = duration; }

    TmssPtrType GetTimeShiftedService() const { return tmss; }
    void SetTimeShiftedService(TmssPtrType tmss);

    RefsEventPtrType GetRefsEvent() const { return refsEvent; }
    void SetRefsEvent(RefsEventPtrType refsEvent) { this->refsEvent = refsEvent; }

    /* the following function is provided just for debug */
    void Put(std::ostream& os) const;

public:
    typedef std::shared_ptr<TimeShiftedService> TmssSharedPtrType;
    typedef std::shared_ptr<ReferenceServiceEvent> RefsEventSharedPtrType;
    TmssSharedPtrType GetSharedPtr(TmssPtrType ptr) { return ptr; }
    RefsEventSharedPtrType GetSharedPtr(RefsEventPtrType ptr) { return ptr; }

private:
#pragma db id column("Idx") get(GetIndex) set(SetIndex)
    TableIndex idx;

#pragma db column("EventId") get(GetEventId) set(SetEventId)
    EventId eventId;
#pragma db column("PosterId") get(GetPosterId) set(SetPosterId)
    PosterId posterId;
#pragma db column("StartTimePoint") get(GetStartTimePoint) set(SetStartTimePoint)
    time_t startTimePoint;
#pragma db column("Duration") get(GetDuration) set(SetDuration) 
    time_t duration;

#pragma db not_null column("Tmss") get(GetTimeShiftedService) set(SetTimeShiftedService) 
    TmssPtrType tmss;

#pragma db column("RefsEventIdx") get(GetRefsEvent) set(SetRefsEvent) 
    RefsEventPtrType refsEvent;
};

inline std::ostream& operator << (std::ostream& os, const TimeShiftedServiceEvent& value)
{
    value.Put(os);
    return os;
}

class CompareTmssEventIndex : public std::unary_function<TimeShiftedServiceEvent, bool>
{
public:
    CompareTmssEventIndex(TableIndex idx)
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

#endif