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
	typedef odb::query<ReferenceService>  RefsQuery;
	typedef odb::result<ReferenceService> RefsResult;

    Controller();
    ~Controller();
	
	/* class ACE_Event_Handler's virtual function. */
    int handle_signal(int signum, siginfo_t *, ucontext_t *);
    int handle_timeout(const ACE_Time_Value &currentTime, const void *act);
	
	/* class ControllerInterface's virtual function. */
	MovieId AddMovie(MovieId movieId, const char *description);
	TsSvcId AddReferenceService(TsId tsId, ServiceId refsId, const char *description);	
	TableIndex AddReferenceServiceEvent(TsId refsTsId, ServiceId refsId, EventId eventId, 
		                                TimePoint startTimePoint, Seconds duration);
	MovieId BindMovieAndRefsEvent(MovieId movieId, TableIndex eventIdx);
	bool DeleteMovie(MovieId movieId);
	bool DeleteReferenceService(TsId tsId, ServiceId refsId);
	bool DeleteReferenceServiceEvent(TsId refsTsId, ServiceId refsId, EventId eventId);
	bool DeleteReferenceServiceEventMovie(TsId refsTsId, ServiceId refsId, EventId eventId, 
	                                      MovieId movieId);

	void PrintRefsInfo();
	bool Start(const char *user, const char *password, const char *schema);

private:
	MovieId InsertMovie(MovieId movieId, const char *description);
	TsSvcId InsertRefs(TsId tsId, ServiceId refsId, const char *description);
	TableIndex InsertRefsEvent(TsId refsTsId, ServiceId refsId, EventId eventId, 
		                       TimePoint startTimePoint, Seconds duration);
	MovieId InsertMovieRefsEvent(MovieId movieId, TableIndex eventIdx);

private:
	odb::database *db;
	odb::session  *session;
};

#endif