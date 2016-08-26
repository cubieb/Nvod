#include "SystemInclude.h"

/* NvodServiceEntity */
#include "NvodService.h"

#include "TmssPlayer.h"
using namespace std;

typedef ClassFactoriesRegistor<PlayerInterface, string, shared_ptr<odb::database>, shared_ptr<TimeShiftedService>> Registor;
static Registor r0001("TmssPlayer", TmssPlayer::CreateInstance);

/**********************class TmssPlayer**********************/
/* public function */
TmssPlayer::TmssPlayer(shared_ptr<odb::database> db, shared_ptr<TimeShiftedService> tmss)
: db(db), tmss(tmss), currentTmssEvent(nullptr), currentMovie(nullptr), currentClip(nullptr),
  timerId(InvalidTimerId)
{}

TmssPlayer::~TmssPlayer()
{
    ResetPointer(currentTmssEvent);
    ResetPointer(currentMovie);
    ResetPointer(currentClip);
}

PlayerId TmssPlayer::GetPlayerId() const
{
    TsSvcId tmssId = tmss->GetTmssId();
    return MakePlayerId(tmssId.GetTsId(), tmssId.GetServiceId());
}

TimerId TmssPlayer::GetTimerId() const
{
    return timerId;
}

void TmssPlayer::SetTimerId(TimerId timerId)
{
    this->timerId = timerId;
}

Milliseconds TmssPlayer::GetWaitingDuration()
{
    TmssEventsType& events = tmss->GetEvents();
    return GetWaitingDuration(events, currentTmssEvent, currentMovie, currentClip);
}

pair<uchar_t *, size_t> TmssPlayer::GetCurrentClip()
{
    return make_pair<uchar_t *, size_t>(nullptr, 0);
}

/* static function */
PlayerInterface* TmssPlayer::CreateInstance(shared_ptr<odb::database> db, 
                                            shared_ptr<TimeShiftedService> tmss)
{
    return new TmssPlayer(db, tmss);
}

TmssPlayer::TmssEventsIterator TmssPlayer::FindCurrentEvent(TmssEventsType& events)
{
    TmssEventsIterator evnt;

    time_t curTime = time(nullptr);
    for (evnt = events.begin(); evnt != events.end(); ++evnt)
    {
        shared_ptr<TimeShiftedServiceEvent> ptr = tmss->GetSharedPtr(*evnt);
        if (curTime < ptr->GetStartTimePoint() + ptr->GetDuration())
        {
            break;
        }
    }
    return evnt;
}

Milliseconds TmssPlayer::GetWaitingDuration(TmssEventsType& tmssEvents, 
    TmssEventsIterator*& currentTmssEvent, MoviesIterator*& currentMovie, ClipseIterator*& currentClip)
{
    /* Step 1 */
    if (currentTmssEvent == nullptr)
    {
        currentTmssEvent = new TmssEventsIterator;
        *currentTmssEvent = FindCurrentEvent(tmssEvents);
        ResetPointer(currentMovie);
        ResetPointer(currentClip);
    }
    if (*currentTmssEvent == tmssEvents.end())
    {
        return std::numeric_limits<Milliseconds>::max();
    }

    TmssEventSharedPtrType curTmssEvntPtr = tmss->GetSharedPtr(**currentTmssEvent);
    TmsseRefsEventSharedPtrType refsEvent = curTmssEvntPtr->GetSharedPtr(curTmssEvntPtr->GetRefsEvent());    
    if (refsEvent == nullptr)
    {
        ++*currentTmssEvent;
        ResetPointer(currentMovie);
        ResetPointer(currentClip);
        if (*currentTmssEvent == tmssEvents.end())
        {
            return std::numeric_limits<Milliseconds>::max();
        }
        return GetWaitingDuration(tmssEvents, currentTmssEvent, currentMovie, currentClip);
    }
    time_t startTime = curTmssEvntPtr->GetStartTimePoint();
    time_t curTime = time(nullptr);
    if (startTime > curTime)
    {
        return (startTime - curTime) * 1000;
    }

    /* Step 2 */
    MoviesType& movies = refsEvent->GetMovies();
    if (currentMovie == nullptr)
    {
        currentMovie = new MoviesIterator;
        *currentMovie = movies.begin();
        ResetPointer(currentClip);
    }
    if (*currentMovie == movies.end())
    {
        ++currentTmssEvent;
        ResetPointer(currentMovie);
        ResetPointer(currentClip);
        return GetWaitingDuration(tmssEvents, currentTmssEvent, currentMovie, currentClip);
    }

    /* Step 3 */
    MovieSharedPtrType movie = refsEvent->GetSharedPtr(**currentMovie);
    ClipsType& clips = movie->GetClips();
    if (currentClip == nullptr)
    {
        currentClip = new ClipseIterator;
        *currentClip = clips.begin();
    }
    if (*currentClip == clips.end())
    {
        ++*currentMovie;
        ResetPointer(currentClip);
        return GetWaitingDuration(tmssEvents, currentTmssEvent, currentMovie, currentClip);
    }

    /* Step 4 */
    return (**currentClip)->GetDuration();
}
