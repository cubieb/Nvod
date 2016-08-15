#ifndef _Player_h_
#define _Player_h_

#include "SystemInclude.h"
#include "Type.h"
#include "TimeShiftedService.h"
#include "ReferenceService.h"

class PlayerInterface
{
public:
    PlayerInterface() {}
    virtual ~PlayerInterface() {}

    virtual Milliseconds GetWaitingDuration() = 0;
    virtual std::pair<uchar_t *, size_t> GetCurrentClip() = 0;

    static PlayerInterface *GetInstance();

};

class PlayerInterfaces: public ContainerBase
{
};


class TmssPlayer: public PlayerInterface
{
public:
    TmssPlayer(TimeShiftedService *tmss, ReferenceService *refs)
        : tmss(tmss), refs(refs)
    {
        Reinit();
    }

    /*
    Step1:
    currentRefsEvent currentTmssEvent currentTmssEvent+1 Action
    ---------------- ---------------- ------------------ --------------------------------------------
    refs->End()      tmss->End()      -                  return Milliseconds::max();
    refs->End()      !tmss->End()     tmss->End()        return Milliseconds::max();
    refs->End()      !tmss->End()     !tmss->End()       ++currentTmssEvent; 
                                                         currentRefsEvent = currentTmssEvent->Begin();
                                                         return Step1();
    not refs->End()  -                -                  return Step2();

    Step2:
    currentMovie            currentRefsEvent.begin() Action
    ----------------------- ------------------------ ---------------------------------------
    nullptr                 currentRefsEvent.end()   currentRefsEvent = refs->End();
                                                     return Step1();
    nullptr                 !currentRefsEvent.end()  currentMovie = currentRefsEvent.begin();
                                                     return Step3(); 
    currentRefsEvent.end()  -                        currentRefsEvent = refs->End();
                                                     return Step1();
    !currentRefsEvent.end() -                        return Step3(); 

    Step3:
    currentMovieClip        currentMovie.begin() Action
    ----------------------  -------------------- ----------------------------------------
    nullptr                 currentMovie.end()   ++currentMovie;
                                                 return Step2();
    nullptr                 !currentMovie.end()  currentMovieClip = currentMovie.begin();
                                                 return Step4();
    currentMovieClip.end()  -                    ++currentMovie;
                                                 return Step2();
    !currentMovieClip.end() -                    return Step4();

    Step4:
    tmssEvntStartPoint = currentTmssEvent == tmss->End()? Milliseconds::max() : currentTmssEvent.GetStartPoint();
    currentTmssEvent tmssEvntStartPoint          currentMovieClip     Action
    ---------------- --------------------------  -------------------  ----------------------------------------
    tmss->End()      -                           -                    return Milliseconds::max();
    !tmss->End()     tmssEvntStartPoint > now()  -                    return tmssEvntStartPoint - now();
    !tmss->End()     tmssEvntStartPoint <= now() nullptr              return Milliseconds::max();
    !tmss->End()     tmssEvntStartPoint <= now() currentMovie.end()   return Milliseconds::max();
    !tmss->End()     tmssEvntStartPoint <= now() !currentMovie.end()  return currentMovieClip.GetClipDuration();
    */
    Milliseconds GetWaitingDuration()
    {
        return Milliseconds(0);
    }

    /* 
    Step1:
    tmssEvntStartPoint = currentTmssEvent == tmss->End()? Milliseconds::max() : currentTmssEvent.GetStartPoint();
    currentTmssEvent tmssEvntStartPoint          currentMovieClip     Action
    ---------------- --------------------------  -------------------  ----------------------------------------
    tmss->End()      -                           -                    return Milliseconds::max();
    !tmss->End()     tmssEvntStartPoint > now()  -                    return make_pair(nullptr, 0);
    !tmss->End()     tmssEvntStartPoint <= now() nullptr              return make_pair(nullptr, 0);
    !tmss->End()     tmssEvntStartPoint <= now() currentMovie.end()   return make_pair(nullptr, 0);
    !tmss->End()     tmssEvntStartPoint <= now() !currentMovie.end()  ptr  = currentMovieClip->GetPtr();
                                                                      size = currentMovieClip->GetSize();
                                                                      ++currentMovieClip;
                                                                      return make_pair(ptr, size);
    */
    std::pair<uchar_t *, size_t> GetCurrentClip();
    

    /*
    Step1:
    currentTmssEvent = find_if(tmss->Begin(), tmss->End(), ...);  //find first entry which is not expired
    if (currentTmssEvent == tmss->End())
        currentRefsEvent = refs.End();
    milliseconds offset = now() - currentTmssEvent->GetStartTimePoint();
    ... ...
    */
    void Reinit();

private:
    TimeShiftedService *tmss;
    TimeShiftedService::iterator currentTmssEvent;

    ReferenceService *refs;
    ReferenceService::iterator currentRefsEvent;
    ReferenceServiceEvent::iterator *currentMovie;
    Movie::iterator *currentMovieClip;
};

#endif