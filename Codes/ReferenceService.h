#ifndef _ReferenceService_h_
#define _ReferenceService_h_

#include "SystemInclude.h"
#include "Type.h"
#include "ContainerBase.h"
#include "Movie.h"

class ReferenceServiceEvent: public ContainerBaseTemplate<std::list<Movie*>>
{
public:
    typedef Iterator<Movie>::MyIter      iterator;
    typedef ConstIterator<Movie>::MyIter const_iterator;

    ReferenceServiceEvent(EventId eventId, TimePoint startTimePoint, Seconds duration);
    ~ReferenceServiceEvent();
            
    virtual iterator Begin() = 0;
    virtual iterator End() = 0;
    virtual void Remove(MovieId movieId) = 0;
    virtual void PushBack(MovieId movieId);

private:
    EventId     refsEventId;
    std::list<Movie *> movies;
    TimePoint  startTimePoint;
    Seconds    duration;
};

class ReferenceService: public ContainerBaseTemplate<std::list<ReferenceServiceEvent*>>
{
public:    
    typedef Iterator<ReferenceServiceEvent>::MyIter      iterator;
    typedef ConstIterator<ReferenceServiceEvent>::MyIter const_iterator;

    ReferenceService(TsId refsTsId, ServiceId refsId)
        : refsTsId(refsTsId), refsId(refsId)
    {}

    std::chrono::milliseconds GetSleepDuration();
    
    virtual iterator Begin() = 0;
    virtual iterator End() = 0;
    virtual iterator Find(EventId refsEventId) = 0;    
    virtual iterator Remove(EventId refsEventId) = 0;
    virtual void PushBack(ReferenceServiceEvent *evnt);

private:
	TsId		refsTsId;
	ServiceId	refsId;

    std::list<ReferenceServiceEvent> events;
};

class ReferenceServices: public ContainerBaseTemplate<std::list<ReferenceService*>>
{
public:
    typedef Iterator<ReferenceService>::MyIter      iterator;
    typedef ConstIterator<ReferenceService>::MyIter const_iterator;

    ReferenceServices(TsId refsTsId, ServiceId refsId) {};
    virtual ~ReferenceServices() {};

    virtual iterator Begin() = 0;
    virtual iterator End() = 0;
    virtual iterator Find(TsId tmssTsId, ServiceId tmssId) = 0;    
    virtual iterator Remove(TsId tmssTsId, ServiceId tmssId) = 0;
    virtual void PushBack(ReferenceService *tmss) = 0;

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

    static ReferenceServices * CreateInstance();
};

#endif