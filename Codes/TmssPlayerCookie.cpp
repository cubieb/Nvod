#include "TmssPlayerCookie.h"

/* Foundation */
#include "BaseType.h"
#include "SystemInclude.h"

/* Entity */
#include "Entities.h"

#define MovieBufferSize 188*1024*1024 /* 188*1024*16 */

using namespace std;
using namespace std::chrono;

/**********************class TmssPlayerCookie**********************/
TmssPlayerCookie::TmssPlayerCookie(shared_ptr<TmssEntity> tmss)
{
    this->ts = make_shared<TsEntity>(*tmss->GetTs().lock());
    this->tmss = make_shared<TmssEntity>(*tmss);

    steady_clock::time_point now = steady_clock::now(); 
    for (auto iter = tmss->GetTmssEvents().begin(); iter != tmss->GetTmssEvents().end(); ++iter)
    {
        if (*(*iter)->GetStartTimePoint() + *(*iter)->GetDuration() < now)
        {
            continue;
        }
        
        /* bind TmssEventEntity to TmssEntity */
        shared_ptr<TmssEventEntity> tmssEvent = make_shared<TmssEventEntity>(**iter);
        this->tmss->Bind(tmssEvent);

        /* bind RefsEventEntity and TmssEventEntity */
        shared_ptr<RefsEventEntity> refsEvent = make_shared<RefsEventEntity>(*(*iter)->GetRefsEvent());
        refsEvent->Bind(tmssEvent);

        list<shared_ptr<MovieEntity>>& movies = (*iter)->GetRefsEvent()->GetMovies();
        for (auto iter = movies.begin(); iter != movies.end(); ++iter)
        {
            shared_ptr<MovieEntity> movie = make_shared<MovieEntity>(**iter);
            refsEvent->Bind(movie);
        }

        if (curTmssEvent == nullptr && !refsEvent->GetMovies().empty())
        {
            assert(curMovie == nullptr);
            curTmssEvent = make_shared<TmssEventIter>(this->tmss->GetTmssEvents().begin());
            curMovie = make_shared<MovieIter>(refsEvent->GetMovies().begin());
        }
    }

    movieRuntimeInfo = make_shared<MovieRuntimeInfoEntity>();
	eventDuration = Duration::zero();
}

TmssPlayerCookie::~TmssPlayerCookie()
{}

shared_ptr<TsEntity> TmssPlayerCookie::GetTs() const
{
    return ts;
}

shared_ptr<TmssEntity> TmssPlayerCookie::GetTmss() const
{
    return tmss;
}

void TmssPlayerCookie::ForwardTmssEvent()
{
    assert(curTmssEvent != nullptr && (*curTmssEvent) != tmss->GetTmssEvents().end());

    while (++(*curTmssEvent) != tmss->GetTmssEvents().end())
    {
        if ((**curTmssEvent)->GetRefsEvent() != nullptr)
        {
            *curMovie = (**curTmssEvent)->GetRefsEvent()->GetMovies().begin();
            break;
        }
    }

    if ((*curTmssEvent) == tmss->GetTmssEvents().end())
    {
        curMovie.reset();
    }

    movieRuntimeInfo->Reset();
}

shared_ptr<TmssEventEntity> TmssPlayerCookie::GetCurTmssEvent() const
{
    if (curTmssEvent == nullptr || (*curTmssEvent) == tmss->GetTmssEvents().end())
    {
        return nullptr;
    }

    return **curTmssEvent;
}

void TmssPlayerCookie::ForwardMovie()
{
    assert(curMovie != nullptr && (*curMovie) != (**curTmssEvent)->GetRefsEvent()->GetMovies().end());
    
    ++(*curMovie);
    movieRuntimeInfo->Reset();
}

shared_ptr<MovieEntity> TmssPlayerCookie::GetCurMovie() const
{
    if (curMovie == nullptr || (*curMovie) == (**curTmssEvent)->GetRefsEvent()->GetMovies().end())
    {
        return nullptr;
    }

    return **curMovie;
}

shared_ptr<MovieRuntimeInfoEntity> TmssPlayerCookie::GetMovieRuntimeInfo() const
{
    return movieRuntimeInfo;
}

Duration TmssPlayerCookie::GetEventDuration()
{
	return eventDuration;
}

void TmssPlayerCookie::SetEventDuration(Duration eventDuration)
{
	this->eventDuration = eventDuration;
}

/**********************class MovieRuntimeInfoEntity**********************/
MovieRuntimeInfoEntity::MovieRuntimeInfoEntity()
{
    bufferSize = MovieBufferSize;
    buffer = new uchar_t[bufferSize];
    Reset();
}

MovieRuntimeInfoEntity::~MovieRuntimeInfoEntity()
{
    delete[] buffer;
}

void MovieRuntimeInfoEntity::Reset()
{
    pmtPid.reset();
    pcrPid.reset();
    audioPid.reset();
    videoPid.reset();
    firstPcr.reset();
	lastPcr.reset();
    startTimePoint.reset();
    pcrPacketPtr = buffer;
}

shared_ptr<Pid> MovieRuntimeInfoEntity::GetPmtPid() const
{
    return pmtPid;
}

void MovieRuntimeInfoEntity::SetPmtPid(shared_ptr<Pid> pmtPid)
{
    this->pmtPid = pmtPid;
}

void MovieRuntimeInfoEntity::SetPmtPid(Pid pmtPid)
{
    this->pmtPid = make_shared<Pid>(pmtPid);
}

shared_ptr<Pid> MovieRuntimeInfoEntity::GetPcrPid() const
{
    return pcrPid;
}

void MovieRuntimeInfoEntity::SetPcrPid(shared_ptr<Pid> pcrPid)
{
    this->pcrPid = pcrPid;
}

void MovieRuntimeInfoEntity::SetPcrPid(Pid pcrPid)
{
    this->pcrPid = make_shared<Pid>(pcrPid);
}

shared_ptr<Pid> MovieRuntimeInfoEntity::GetAudioPid() const
{
    return audioPid;
}

void MovieRuntimeInfoEntity::SetAudioPid(shared_ptr<Pid> audioPid)
{
    this->audioPid = audioPid;
}

void MovieRuntimeInfoEntity::SetAudioPid(Pid audioPid)
{
    this->audioPid = make_shared<Pid>(audioPid);
}

shared_ptr<Pid> MovieRuntimeInfoEntity::GetVideoPid() const
{
    return videoPid;
}

void MovieRuntimeInfoEntity::SetVideoPid(shared_ptr<Pid> videoPid)
{
    this->videoPid = videoPid;
}

void MovieRuntimeInfoEntity::SetVideoPid(Pid videoPid)
{
    this->videoPid = make_shared<Pid>(videoPid);
}

shared_ptr<Pcr> MovieRuntimeInfoEntity::GetFirstPcr() const
{
    return firstPcr;
}

void MovieRuntimeInfoEntity::SetFirstPcr(shared_ptr<Pcr> firstPcr)
{
    this->firstPcr = firstPcr;
}

void MovieRuntimeInfoEntity::SetFirstPcr(Pcr firstPcr)
{
    this->firstPcr = make_shared<Pcr>(firstPcr);
}

shared_ptr<Pcr> MovieRuntimeInfoEntity::GetLastPcr() const
{
	return lastPcr;
}
void MovieRuntimeInfoEntity::SetLastPcr(shared_ptr<Pcr> lastPcr)
{
	this->lastPcr = lastPcr;
}

void MovieRuntimeInfoEntity::SetLastPcr(Pcr lastPcr)
{
    this->lastPcr = make_shared<Pcr>(lastPcr);
}

shared_ptr<TimePoint> MovieRuntimeInfoEntity::GetStartTimePoint() const
{
    return startTimePoint;
}

void MovieRuntimeInfoEntity::SetStartTimePoint(shared_ptr<TimePoint> startTimePoint)
{
    this->startTimePoint = startTimePoint;
}

void MovieRuntimeInfoEntity::SetStartTimePoint(TimePoint startTimePoint)
{
    this->startTimePoint = make_shared<TimePoint>(startTimePoint);
}

uchar_t* MovieRuntimeInfoEntity::GetBufferHead() const
{
    return buffer;
}

size_t MovieRuntimeInfoEntity::GetBufferSize() const
{
    return bufferSize;
}

uchar_t* MovieRuntimeInfoEntity::GetPcrPacketPtr() const
{
    return pcrPacketPtr;
}

void MovieRuntimeInfoEntity::SetPcrPacketPtr(uchar_t* pcrPacketPtr)
{
    this->pcrPacketPtr = pcrPacketPtr;
}