#include "SystemInclude.h"

#pragma warning(push)
#pragma warning(disable:702)   //disable warning caused by ACE library.
#pragma warning(disable:4251)  //disable warning caused by ACE library.
#pragma warning(disable:4996)  //disable warning caused by ACE library.
#include "ace/OS.h"
#include "ace/Singleton.h"

/* Foundation */
#include "Database.h"
#include "PlayerInterface.h"

/* NvodServiceEntity */
#include "NvodService.h"
#include "NvodService-odb.h"

/* Controller */
#include "TableIndexHelperInterface.h"
#include "Controller.h"
using namespace std;

/**********************class ControllerInterface**********************/
ControllerInterface &ControllerInterface::GetInstance()
{
    typedef ACE_Singleton<Controller, ACE_Recursive_Thread_Mutex> TheController;

    return *TheController::instance();
}

/**********************class Controller**********************/
/* public function */
Controller::~Controller()
{}

int Controller::handle_signal(int signum, siginfo_t *, ucontext_t *)
{
    return 0;
}

int Controller::handle_timeout(const ACE_Time_Value &currentTime,
                               const void *act)
{
    return 0;
}

MovieId Controller::AddMovie(MovieId movieId, const char *description)
{
    odb::transaction t (db->begin());
    /* step 1: check reference service */
    MovieResult result(db->query<Movie>(MovieQuery::movieId == movieId));
    if (result.size() == 1)
    {
        t.rollback();
        return InvalidMovieId;
    }

    /* step 2: update database */
    MovieId ret = InsertMovie(movieId, description);

    /* step 3: commit. */
    t.commit(); 
    return ret;
}

TsSvcId Controller::AddReferenceService(TsId refsTsId, ServiceId refsId, const char *description)
{
    odb::transaction t (db->begin());   
    /* step 1: check reference service */
    RefsResult result(db->query<ReferenceService>(RefsQuery::refsId.tsId == refsTsId 
                                                  && RefsQuery::refsId.serviceId == refsId));
    if (result.size() == 1)
    {
        t.rollback();
        return InvalidTsSvcId;
    }

    /* step 2: update database */
    TsSvcId ret = InsertRefs(refsTsId, refsId, description);

    /* step 3: commit. */
    t.commit(); 
    return ret;
}

TableIndex Controller::AddReferenceServiceEvent(TsId refsTsId, ServiceId refsId, EventId eventId, 
                                                TimePoint startTimePoint, Seconds duration)
{
    odb::transaction t (db->begin());   
    /* step 1: check reference service */
    RefsResult result1(db->query<ReferenceService>(RefsQuery::refsId.tsId == refsTsId 
                                                  && RefsQuery::refsId.serviceId == refsId));
    if (result1.size() == 0)
    {
        t.rollback();
        return InvalidTableIndex;
    }   
    RefsEventResult result2(db->query<ReferenceServiceEvent>(RefsEventQuery::eventId == eventId
                                                               && RefsEventQuery::refs.tsId == refsTsId
                                                               && RefsEventQuery::refs.serviceId == refsId));
    if (result2.size() == 1)
    {
        t.rollback();
        return InvalidTableIndex;
    }      

    /* step 2: update database */
    TableIndex ret = InsertRefsEvent(refsTsId, refsId, eventId, startTimePoint, duration);

    /* step 3: commit. */
    t.commit(); 
    return ret;
}

MovieId Controller::BindMovieAndRefsEvent(MovieId movieId, TableIndex eventIdx)
{
    odb::transaction t (db->begin());   
    /* step 1: check reference service */
    MovieResult result1(db->query<Movie>(MovieQuery::movieId == movieId));
    if (result1.size() == 0)
    {
        t.rollback();
        return InvalidMovieId;
    }
    else
    {
        auto events = result1.begin()->GetRefsEvents();
        if (find_if(events.begin(), events.end(), CompareRefsEventIndex(eventIdx)) != events.end())
        {
            t.rollback();
            return InvalidMovieId;
        }
    }
    RefsEventResult result2(db->query<ReferenceServiceEvent>(RefsEventQuery::idx == eventIdx));
    if (result2.size() == 0)
    {
        t.rollback();
        return InvalidMovieId;
    }  

    /* step 2: update database */
    MovieId ret = InsertMovieRefsEvent(movieId, eventIdx);

    /* step 3: commit. */
    t.commit(); 
    return ret;
}

bool Controller::DeleteMovie(MovieId movieId)
{
    odb::transaction t (db->begin());  
    /* step 1: check reference service */
    MovieResult result1(db->query<Movie>(MovieQuery::movieId == movieId));
    if (result1.size() == 0)
    {
        t.rollback();
        return false;
    } 

    /* step 2: update database */
    DeleteFromMovie(movieId);

    /* step 3: commit. */
    t.commit(); 
    return true;
}

bool Controller::DeleteReferenceService(TsId refsTsId, ServiceId refsId)
{
    odb::transaction t (db->begin());
    /* step 1: check reference service */
    RefsResult result(db->query<ReferenceService>(RefsQuery::refsId.tsId == refsTsId 
                                                  && RefsQuery::refsId.serviceId == refsId));
    if (result.size() == 0)
    {
        t.rollback();
        return false;
    }

    /* step 2: update database */
    DeleteFromReferenceService(refsTsId, refsId);

    /* step 3: commit. */
    t.commit(); 
    return true;
}

bool Controller::DeleteReferenceServiceEvent(TableIndex eventIdx)
{
    odb::transaction t (db->begin());
    /* step 1: check reference service */
    RefsEventResult result(db->query<ReferenceServiceEvent>(RefsEventQuery::idx == eventIdx));
    if (result.size() == 0)
    {
        t.rollback();
        return false;
    }  

    /* step 2: update database */
    DeleteFromReferenceServiceEvent(eventIdx);

    /* step 3: commit. */
    t.commit(); 
    return true;
}

bool Controller::UnbindMovieAndRefsEvent(MovieId movieId, TableIndex eventIdx)
{
    odb::transaction t (db->begin());
    /* step 1: check reference service */
    MovieResult result1(db->query<Movie>(MovieQuery::movieId == movieId));
    if (result1.size() == 0)
    {
        t.rollback();
        return false;
    }
    else
    {
        auto events = result1.begin()->GetRefsEvents();
        if (find_if(events.begin(), events.end(), CompareRefsEventIndex(eventIdx)) == events.end())
        {
            t.rollback();
            return false;
        }
    }
    RefsEventResult result2(db->query<ReferenceServiceEvent>(RefsEventQuery::idx == eventIdx));
    if (result2.size() == 0)
    {
        t.rollback();
        return false;
    }  
    
    /* step 2: update database */
    DeleteFromMovieAndRefsEvent(movieId, eventIdx);

    /* step 3: commit. */
    t.commit(); 
    return true;
}

TsSvcId Controller::AddTimeShiftedService(TsId tmssTsId, ServiceId tmssId, const char *description)
{
    odb::transaction t (db->begin());   
    /* step 1: check reference service */
    TmssResult result(db->query<TimeShiftedService>(TmssQuery::tmssId.tsId == tmssTsId
                                                    && TmssQuery::tmssId.serviceId == tmssId));
    if (result.size() == 1)
    {
        t.rollback();
        return InvalidTsSvcId;
    }      
    
    /* step 2: update database */
    TsSvcId ret = InsertTmss(tmssTsId, tmssId, description);
    
    /* step 3: create player */
    shared_ptr<TimeShiftedService> tmss = db->load<TimeShiftedService>(ret);
    players.push_back(shared_ptr<PlayerInterface>(CreatePlayerInterface("TmssPlayer", db, tmss)));

    /* step 4: commit. */
    t.commit(); 

    return ret;
}

TableIndex Controller::AddTimeShiftedServiceEvent(TsId tmssTsId, ServiceId tmssId, EventId eventId, PosterId posterId,
    TimePoint startTimePoint, Seconds duration, TableIndex refsEventIdx)
{
    odb::transaction t (db->begin()); 
    /* step 1: check reference service */
    TmssResult result1(db->query<TimeShiftedService>(TmssQuery::tmssId.tsId == tmssTsId
                                                    && TmssQuery::tmssId.serviceId == tmssId));
    if (result1.size() == 0)
    {
        t.rollback();
        return InvalidTableIndex;
    }      
    TmssEventResult result2(db->query<TimeShiftedServiceEvent>(TmssEventQuery::eventId == eventId
                                                               && TmssEventQuery::tmss.tsId == tmssTsId
                                                               && TmssEventQuery::tmss.serviceId == tmssId));
    if (result2.size() == 1)
    {
        t.rollback();
        return InvalidTableIndex;
    }
    
    /* step 2: update database */
    TableIndex ret = InsertTmssEvent(tmssTsId, tmssId, eventId, posterId, startTimePoint, duration, refsEventIdx);

    /* step 3: update player */
    PlayerId playerId = MakePlayerId(tmssTsId, tmssId);
    auto player = find_if(players.begin(), players.end(), ComparePlayerId(playerId));
    duration = (*player)->GetWaitingDuration();
    cout << "waiting :" << duration << endl;

    /* step 4: commit. */
    t.commit();

    return ret;
}

bool Controller::DeleteTimeShiftedService(TsId tmssTsId, ServiceId tmssId)
{
    odb::transaction t (db->begin());   
    /* step 1: check reference service */
    TmssResult result(db->query<TimeShiftedService>(TmssQuery::tmssId.tsId == tmssTsId
                                                    && TmssQuery::tmssId.serviceId == tmssId));
    if (result.size() == 0)
    {
        t.rollback();
        return false;
    }

    /* step 2: delete player */
    PlayersType::iterator player = find_if(players.begin(), players.end(),
                                           ComparePlayerId(MakePlayerId(tmssTsId, tmssId)));
    //cancel timer
    //if ()
    //{
    //}
    players.erase(player);

    /* step 3: update database */
    DeleteFromTimeShiftedService(tmssTsId, tmssId);

    /* step 4: commit. */
    t.commit(); 
    return true;
}

bool Controller::DeleteTimeShiftedServiceEvent(TableIndex eventIdx)
{
    odb::transaction t (db->begin());
    /* step 1: check reference service */
    TmssEventResult result(db->query<TimeShiftedServiceEvent>(TmssEventQuery::idx == eventIdx));
    if (result.size() == 0)
    {
        t.rollback();
        return false;
    }
    
    /* step 2: update database */
    DeleteFromTimeShiftedServiceEvent(eventIdx);

    /* step 3: commit. */
    t.commit(); 
    return true;
}

void Controller::PrintRefsInfo()
{
    odb::transaction t (db->begin());    

    RefsResult result(db->query<ReferenceService>());
    for (RefsResult::iterator i (result.begin()); i != result.end(); ++i)
    {
        cout << *i << endl;
    }

    t.commit();
}

void Controller::PrintTmssInfo()
{    
    odb::transaction t (db->begin());    

    TmssResult result(db->query<TimeShiftedService>());
    for (TmssResult::iterator i (result.begin()); i != result.end(); ++i)
    {
        cout << *i << endl;
    }

    t.commit();
}

bool Controller::Start(const char *user, const char *password, const char *schema)
{
    db = shared_ptr<odb::database>(CreateDatabase(user, password, schema));
    session = make_shared<odb::session>();

    return true;
}

/* private function */
Controller::Controller() :db(nullptr), session(nullptr)
{
}

MovieId Controller::InsertMovie(MovieId movieId, const char *description)
{
    Movie movie(movieId, description);
    db->persist(movie);
    return movieId;
}

TsSvcId Controller::InsertRefs(TsId refsTsId, ServiceId refsId, const char *description)
{
    shared_ptr<ReferenceService> refs = make_shared<ReferenceService>(refsTsId, refsId, description);
    db->persist(*refs);
    return TsSvcId(refsTsId, refsId);
}

TableIndex Controller::InsertRefsEvent(TsId refsTsId, ServiceId refsId, EventId eventId, 
                                       TimePoint startTimePoint, Seconds duration)
{	
    /* get table index */
    TableIndexHelperInterface& helper = GetTableIndexHelperInterface("ReferenceServiceEvent");
    TableIndex idx = helper.GetUseableTableIndex();

    /* create event instance. */
    auto refsEvent = make_shared<ReferenceServiceEvent>(idx, eventId, startTimePoint, duration);

    shared_ptr<ReferenceService> refs = db->load<ReferenceService>(TsSvcId(refsTsId, refsId));
    assert (refs != nullptr);
    refsEvent->SetReferenceService(refs);
    db->persist(refsEvent);

    return idx;
}

MovieId Controller::InsertMovieRefsEvent(MovieId movieId, TableIndex eventIdx)
{
    shared_ptr<Movie> movie = db->load<Movie>(movieId);
    shared_ptr<ReferenceServiceEvent> evnt = db->load<ReferenceServiceEvent>(eventIdx);

    movie->BindRefsEvent(evnt);
    db->update(*movie);

    return movieId;
}

/* 
    Movies_RefEvents table will be deleted by "DELETE CASCADE" when we delete movie. 
    But we have to remove current Movie from Reference Service Event one by one.
*/
void Controller::DeleteFromMovie(MovieId movieId)
{
    shared_ptr<Movie> movie = db->load<Movie>(movieId);
    Movie::RefsEventsType events = movie->GetRefsEvents();
    for (auto i = events.begin(); i != events.end(); ++i)
    {
        movie->UnbindRefsEvent(*i);
    }

    db->erase<Movie>(movieId);
}

void Controller::DeleteFromReferenceService(TsId refsTsId, ServiceId refsId)
{
    shared_ptr<ReferenceService> refs = db->load<ReferenceService>(TsSvcId(refsTsId, refsId));

    /* step 1: delete event */
    ReferenceService::RefsEventsType evnts = refs->GetEvents();
    for (auto i = evnts.begin(); i != evnts.end(); ++i)
    {
        ReferenceService::RefsEventSharedPtrType evntPtr = refs->GetSharedPtr(*i);
        DeleteFromReferenceServiceEvent(evntPtr->GetIndex());
    }

    db->erase<ReferenceService>(refs->GetRefsId());
}

void Controller::DeleteFromReferenceServiceEvent(TableIndex eventIdx)
{
    shared_ptr<ReferenceServiceEvent> evnt = db->load<ReferenceServiceEvent>(eventIdx);

    /* step 1: delete event from Movie's member container */
    ReferenceServiceEvent::MoviesType movies = evnt->GetMovies();
    for (auto i = movies.begin(); i != movies.end(); ++i)
    {
        ReferenceServiceEvent::MovieSharedPtrType moviePtr = evnt->GetSharedPtr(*i);
        moviePtr->UnbindRefsEvent(evnt);
        db->update(*moviePtr);
    }

    /* step 2: delete event from ReferenceService's member container. */
    evnt->SetReferenceService(nullptr);

    /* step 3: delete event object. */
    db->erase<ReferenceServiceEvent>(eventIdx);
}

void Controller::DeleteFromMovieAndRefsEvent(MovieId movieId, TableIndex eventIdx)
{
    shared_ptr<Movie> movie = db->load<Movie>(movieId);
    shared_ptr<ReferenceServiceEvent> evnt = db->load<ReferenceServiceEvent>(eventIdx);

    movie->UnbindRefsEvent(evnt);
    db->update(*movie);
}

TsSvcId Controller::InsertTmss(TsId tmssTsId, ServiceId tmssId, const char *description)
{
	shared_ptr<TimeShiftedService> refs = make_shared<TimeShiftedService>(tmssTsId, tmssId, description);
	db->persist(*refs);
	return TsSvcId(tmssTsId, tmssId);
}

TableIndex Controller::InsertTmssEvent(TsId tmssTsId, ServiceId tmssId, EventId eventId, PosterId posterId,
    TimePoint startTimePoint, Seconds duration, TableIndex refsEventIdx)
{
    /* get table index */
    TableIndexHelperInterface& helper = GetTableIndexHelperInterface("TimeShiftedServiceEvent");
    TableIndex idx = helper.GetUseableTableIndex();

    /* create event instance. */
    auto tmssEvent = make_shared<TimeShiftedServiceEvent>(idx, eventId, posterId, startTimePoint, duration);

    shared_ptr<TimeShiftedService> tmss = db->load<TimeShiftedService>(TsSvcId(tmssTsId, tmssId));
    assert (tmss != nullptr);
    tmssEvent->SetTimeShiftedService(tmss);
    
    if (refsEventIdx != InvalidTableIndex)
    {
        shared_ptr<ReferenceServiceEvent> refsEvent = db->load<ReferenceServiceEvent>(refsEventIdx);
        tmssEvent->SetRefsEvent(refsEvent);
    }
    db->persist(tmssEvent);

    return idx;
}

void Controller::DeleteFromTimeShiftedService(TsId tmssTsId, ServiceId tmssId)
{
    shared_ptr<TimeShiftedService> tmss = db->load<TimeShiftedService>(TsSvcId(tmssTsId, tmssId));

    /* step 1: delete event */
    TimeShiftedService::TmssEventsType evnts = tmss->GetEvents();
    for (auto i = evnts.begin(); i != evnts.end(); ++i)
    {
        TimeShiftedService::TmssEventSharedPtrType evntPtr = tmss->GetSharedPtr(*i);
        DeleteFromTimeShiftedServiceEvent(evntPtr->GetIndex());
    }

    db->erase<TimeShiftedService>(tmss->GetTmssId());
}

void Controller::DeleteFromTimeShiftedServiceEvent(TableIndex eventIdx)
{
    shared_ptr<TimeShiftedServiceEvent> evnt = db->load<TimeShiftedServiceEvent>(eventIdx);
    
    /* step 1: delete event from TimeShiftedService's member container. */
    evnt->SetTimeShiftedService(nullptr);

    /* step 2: delete event object. */
    db->erase<TimeShiftedServiceEvent>(eventIdx);
}

#pragma warning(pop)