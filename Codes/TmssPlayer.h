#ifndef _Player_h_
#define _Player_h_

/* ODB */
#include <odb/database.hxx>    //odb::database

#include "PlayerInterface.h"

/**********************class Player**********************/
class TmssPlayer: public PlayerInterface
{
public:
    typedef TimeShiftedService::TmssEventPtrType TmssEventPtrType;
    typedef TimeShiftedService::TmssEventsType TmssEventsType;
    typedef TimeShiftedService::TmssEventSharedPtrType TmssEventSharedPtrType;

    typedef ReferenceService::RefsEventPtrType RefsEventPtrType;
    typedef ReferenceService::RefsEventsType RefsEventsType;
    typedef ReferenceService::RefsEventSharedPtrType RefsEventSharedPtrType;

    typedef ReferenceServiceEvent::MoviePtrType MoviePtrType;
    typedef ReferenceServiceEvent::MoviesType MoviesType;
    typedef ReferenceServiceEvent::MovieSharedPtrType MovieSharedPtrType;
    typedef TimeShiftedServiceEvent::RefsEventPtrType TmsseRefsEventPtrType;
    typedef TimeShiftedServiceEvent::RefsEventSharedPtrType TmsseRefsEventSharedPtrType;

    typedef Movie::ClipsType ClipsType;

    ~TmssPlayer();    

    PlayerId GetPlayerId() const;
    TimerId GetTimerId() const;
    void SetTimerId(TimerId timerId);
    /*
    Step1:
    Step1.1:
    currentRefsEvent = FindRefsEvent(currentTmssEvent);

    Step1.2:
    currentRefsEvent currentTmssEvent currentTmssEvent+1 Action
    ---------------- ---------------- ------------------ --------------------------------------------
    _                tmss->End()      -                  return Milliseconds::max();
    nullptr          !tmss->End()     tmss->End()        return Milliseconds::max();
    nullptr          !tmss->End()     !tmss->End()       ++currentTmssEvent; 
                                                         return Step1();
    !rnullptr        !tmss->End()     -                  if (currentTmssEvent.GetStartTimePoint() > now)
                                                            return currentTmssEvent.GetStartTimePoint() - now;
                                                         else
                                                            return Step2();

    Step2:
    currentMovie            currentRefsEvent.begin() Action
    ----------------------- ------------------------ ---------------------------------------
    nullptr                 -                        currentMovie = currentRefsEvent.begin();
                                                     return Step2();
    currentRefsEvent.end()  -                        ++currentTmssEvent;
                                                     return Step1();
    !currentRefsEvent.end() -                        return Step3(); 


    Step3:
    currentMovieClip        currentMovie.begin() Action
    ----------------------  -------------------- ----------------------------------------
    nullptr                 -                    currentMovieClip = currentMovie.begin();
                                                 return Step3();
    currentMovieClip.end()  -                    ++currentMovie;
                                                 return Step2();
    !currentMovieClip.end() -                    return Step4();

    Step4:
    return currentMovieClip.GetDuration();
    */
    Milliseconds GetWaitingDuration();

    std::pair<uchar_t *, size_t> GetCurrentClip();

    static PlayerInterface* CreateInstance(std::shared_ptr<odb::database> db, 
                                           std::shared_ptr<TimeShiftedService> tmss);

private:
    typedef TmssEventsType::iterator TmssEventsIterator;
    typedef RefsEventsType::iterator RefsEventsIterator;
    typedef MoviesType::iterator MoviesIterator;
    typedef ClipsType::iterator ClipseIterator;

    TmssPlayer(std::shared_ptr<odb::database> db, std::shared_ptr<TimeShiftedService> tmss);
    TmssEventsIterator FindCurrentEvent(TmssEventsType& events);

    Milliseconds GetWaitingDuration(TmssEventsType& tmssEvents, 
        TmssEventsIterator*& evnt, MoviesIterator*& currentMovie, ClipseIterator*& currentClip);
    
    template<typename T>
    void ResetPointer(T*& ptr)
    {
        if (ptr != nullptr)
        {
            delete ptr;
            ptr = nullptr;
        }
    }

private:
    std::shared_ptr<odb::database> db;
    std::shared_ptr<TimeShiftedService> tmss;

    TmssEventsIterator *currentTmssEvent;
    MoviesIterator *currentMovie;
    ClipseIterator *currentClip;

    TimerId timerId;
};

#endif