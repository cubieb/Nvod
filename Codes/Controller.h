#ifndef _Controller_h_
#define _Controller_h_

/* ODB */
#include <odb/database.hxx>    //odb::database
#include <odb/session.hxx>
#include <odb/transaction.hxx>

/* Controller */
#include "ControllerInterface.h"

/**********************class Controller**********************/
class Controller: public ControllerInterface
{
public:	
    typedef odb::query<Movie>  MovieQuery;
	typedef odb::result<Movie> MovieResult;
	typedef odb::query<ReferenceService>    RefsQuery;
	typedef odb::result<ReferenceService>   RefsResult;
	typedef odb::query<ReferenceServiceEvent>    RefsEventQuery;
	typedef odb::result<ReferenceServiceEvent>   RefsEventResult;
    typedef odb::query<TimeShiftedService>  TmssQuery;
	typedef odb::result<TimeShiftedService> TmssResult;
	typedef odb::query<TimeShiftedServiceEvent>    TmssEventQuery;
	typedef odb::result<TimeShiftedServiceEvent>   TmssEventResult;
    typedef std::list<std::shared_ptr<PlayerInterface>> PlayersType;

    friend class ACE_Singleton<Controller, ACE_Recursive_Thread_Mutex>;

    ~Controller();
	
	/* class ACE_Event_Handler's virtual function. */
    int handle_signal(int signum, siginfo_t *, ucontext_t *);
    int handle_timeout(const ACE_Time_Value &currentTime, const void *act);
	
	/* class ControllerInterface's virtual function. */
	MovieId AddMovie(MovieId movieId, const char *description);
	TsSvcId AddReferenceService(TsId refsTsId, ServiceId refsId, const char *description);	
	TableIndex AddReferenceServiceEvent(TsId refsTsId, ServiceId refsId, EventId eventId, 
		                                TimePoint startTimePoint, Seconds duration);
	MovieId BindMovieAndRefsEvent(MovieId movieId, TableIndex eventIdx);
	bool DeleteMovie(MovieId movieId);
	bool DeleteReferenceService(TsId refsTsId, ServiceId refsId);
	bool DeleteReferenceServiceEvent(TableIndex eventIdx);
	bool UnbindMovieAndRefsEvent(MovieId movieId, TableIndex eventIdx);

    TsSvcId AddTimeShiftedService(TsId tmssTsId, ServiceId tmssId, const char *description);
	TableIndex AddTimeShiftedServiceEvent(TsId tmssTsId, ServiceId tmssId, EventId eventId, PosterId posterId,
        TimePoint startTimePoint, Seconds duration, TableIndex refsEventIdx);
	bool DeleteTimeShiftedService(TsId tmssTsId, ServiceId tmssId);
	bool DeleteTimeShiftedServiceEvent(TableIndex eventIdx);

	void PrintRefsInfo();
    void PrintTmssInfo();
	bool Start(const char *user, const char *password, const char *schema);

private:
    Controller();
	MovieId InsertMovie(MovieId movieId, const char *description);
	TsSvcId InsertRefs(TsId refsTsId, ServiceId refsId, const char *description);
	TableIndex InsertRefsEvent(TsId refsTsId, ServiceId refsId, EventId eventId, 
		                       TimePoint startTimePoint, Seconds duration);
	MovieId InsertMovieRefsEvent(MovieId movieId, TableIndex eventIdx);
    void DeleteFromMovie(MovieId movieId);
	void DeleteFromReferenceService(TsId refsTsId, ServiceId refsId);
	void DeleteFromReferenceServiceEvent(TableIndex eventIdx);
    void DeleteFromMovieAndRefsEvent(MovieId movieId, TableIndex eventIdx);
        
	TsSvcId InsertTmss(TsId tmssTsId, ServiceId tmssId, const char *description);
	TableIndex InsertTmssEvent(TsId tmssTsId, ServiceId tmssId, EventId eventId, PosterId posterId,
        TimePoint startTimePoint, Seconds duration, TableIndex refsEventIdx);
	void DeleteFromTimeShiftedService(TsId tmssTsId, ServiceId tmssId);
	void DeleteFromTimeShiftedServiceEvent(TableIndex eventIdx);

private:
    std::shared_ptr<odb::database> db;
    std::shared_ptr<odb::session> session;
    PlayersType players;
};

#endif