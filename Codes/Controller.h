#ifndef _Controller_h_
#define _Controller_h_

#include "SystemInclude.h"
#pragma warning(push)
#pragma warning(disable:702)   //disable warning caused by ACE library.
#pragma warning(disable:4251)  //disable warning caused by ACE library.
#pragma warning(disable:4996)  //disable warning caused by ACE library.
#include "ace/OS.h"
#include "ace/Singleton.h"

#include "Type.h"

class Controller
{
public:
    Controller();
    ~Controller();

    /* 
    Step1:
    shared_ptr<ReferenceService> refs = make_shared<ReferenceServices>(refsTsId, refsId);
    refses.Add(refs);
    for (TimeShiftedServices tmss = tmsses.Begin(); tmss != tmsses.End(); ++tmss)
    {
        if (tmss->GetRefsTsId() == refsTsId && tmss->GetRefsId() == refsId)
        {
            shared_ptr<TmssPlayer> player = make_shared<>(tmss, refs);
            players.Add(player);
        }
    }
    */
    void AddReferenceService(TsId refsTsId, ServiceId refsId);

    /* 
    Step1:
    shared_ptr<ReferenceService> refs = *refses.Find(refsTsId, refsId);
    shared_ptr<ReferenceServiceEvent> evnt =  make_shared<ReferenceServiceEvent>(eventId, startTimePoint);
    refs->AddEvent(evnt);
    */
    void AddReferenceServiceEvent(TsId refsTsId, ServiceId refsId, EventId eventId, 
                                  TimePoint startTimePoint, Seconds duration);

    /* 
    Step1:
    shared_ptr<ReferenceService> refs = *refses.Find(refsTsId, refsId);
    shared_ptr<ReferenceServiceEvent> evnt =  *refs.Find(eventId);
    evnt->PushBack(movieId);

    Step2:
    RescheduleRefsTimer(refsTsId, refsId);
    */
    void AddReferenceServiceEventMovie(TsId refsTsId, ServiceId refsId, EventId eventId, MovieId movieId);

    /*
    Step1:
    bool Process(TsId refsTsId, ServiceId refsId)
    {
        if (... ...)
        {
            Command *arg;
            reactor->cancel_timer(player->GetTimerId(), &arg);
            delete arg;
        }
    }
    PlayerInterfaces.RemoveIf(Process);    
    */
    void DeleteReferenceService(TsId refsTsId, ServiceId refsId);

    /*
    Step1:
    shared_ptr<ReferenceService> refs = *refses.Find(refsTsId, refsId);
    refs->Remove(eventId);
    RescheduleRefsTimer(refsTsId, refsId);
    */
    void DeleteReferenceServiceEvent(TsId refsTsId, ServiceId refsId, EventId eventId);

    /*
    Step1:
    shared_ptr<ReferenceService> refs = *refses.Find(refsTsId, refsId);
    shared_ptr<ReferenceServiceEvent> evnt = *refs.Find(eventId);
    evnt->Remove(movieId);

    Step2:
    RescheduleRefsTimer(refsTsId, refsId);
    */
    void DeleteReferenceServiceEventMovie(TsId refsTsId, ServiceId refsId, EventId eventId, MovieId movieId);

    /*
    Step1:    
    shared_ptr<TimeShiftedService> tmss = make_shared<TimeShiftedService>(tmssTsId, tmssId, refsTsId, refsId);
    tmsses.Add(tmss);
    auto refs = refses.Find(refsTsId, refsId);
    if (refs != refses.End())
    {
        shared_ptr<TmssPlayer> player = make_shared<>(tmss, *refs);
        players.Add(player);
    }
    */
    void AddTimeShiftedService(TsId tmssTsId, ServiceId tmssId, TsId refsTsId, ServiceId refsId);

     /*
    Step1:    
    TimeShiftedServices::iterator tmss = *tmsses.Find(tmssTsId, tmssId);
    shared_ptr<TimeShiftedServiceEvent> evnt = make_shared<>(rfsEventId, eventId, posterId, startTimePoint, duration);
    tmss->PushBack(evnt);

    Step2:
    RescheduleTmssTimer(tmssTsId, tmssId);
    */
    void AddTimeShiftedServiceEvent(TsId tmssTsId, ServiceId tmssId, EventId rfsEventId, 
                                    EventId eventId, PosterId posterId,
                                    std::chrono::system_clock::time_point  startTimePoint,
                                    std::chrono::seconds duration);

     /*
    Step1:
    bool Process(TsId tmssTsId, ServiceId tmssId)
    {
        if (... ...)
        {
            Command *arg;
            reactor->cancel_timer(player->GetTimerId(), &arg);
            delete arg;
        }
    }
    PlayerInterfaces.RemoveIf(Process);   
    tmsses.Remove(tmssTsId, tmssId);
    */
    void DeleteTimeShiftedService(TsId tmssTsId, ServiceId tmssId);

    /*
    Step1:
    shared_ptr<TimeShiftedServices> tmss = *tmsses.Find(tmssTsId, tmssId);
    tmss->Remove(eventId);
    
    Step2:
    RescheduleTmssTimer(tmssTsId, tmssId);
    */
    void DeleteTimeShiftedServiceEvent(TsId tmssTsId, ServiceId tmssId, EventId eventId);

    /* 
    Step1:
    PlayerInterfacees::iterator player = players.FindIf(XXX(TsId tmssTsId, ServiceId tmssId));
    Milliseconds duration1, duration2;
    duration1 = GetWaitingDuration();
    player->Reinit();
    duration2 = GetWaitingDuration();
    if (duration1 != duration2)
    {
        const void *arg;
        reactor->cancel_timer(player->GetTimerId(), &arg);
        reactor->schedule_timer(this, arg, ACE_Time_Value::zero, duration2);
    }
    */
    void RescheduleTmssTimer(TsId tmssTsId, ServiceId tmssId);

    /* 
    Step1:
    for (PlayerInterfacees::iterator player = players.Begin(); player != players.End(); ++player)
    {
        if (player->GetRefsTsId() == refsTsId && player->GetRefsId() == refsId)
        {
            RescheduleTmssTimer(player->GetTmssTsId(), player->GetTmssId());
        }
    }
    */
    void RescheduleRefsTimer(TsId refsTsId, ServiceId refsId);

    int Controller::handle_timeout(const ACE_Time_Value &currentTime,
                                   const void *act)
    {
        /* 
        Command *command = (Command *)act;
        command->Execute()
        delete command;
        */
    }
};

#pragma warning(pop)
#endif