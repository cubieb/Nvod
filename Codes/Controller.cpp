#include "SystemInclude.h"

#pragma warning(push)
#pragma warning(disable:702)   //disable warning caused by ACE library.
#pragma warning(disable:4251)  //disable warning caused by ACE library.
#pragma warning(disable:4996)  //disable warning caused by ACE library.
#include "ace/OS.h"
#include "ace/Singleton.h"

/* Foundation */
#include "Database.h"
#include "InstanceHelper.h"

/* NvodServiceEntity */
#include "TimeShiftedService.h"
#include "TimeShiftedService-odb.h"
#include "ReferenceService.h"
#include "ReferenceService-odb.h"

/* Controller */
#include "TableIndexHelper.h"
#include "Controller.h"
using namespace std;

/**********************RefsEventTableIndexHelper**********************/
typedef InstanceHelper<TableIndexHelperInterface, std::string> IndexInstHelper;
typedef InstanceHelperRegisterSuite<IndexInstHelper> IndexInstHelperRegisterSuite;
static IndexInstHelperRegisterSuite refsEventIndex("Refs Event", RefsEventTableIndexHelper::GetInstance);

/**********************class ControllerInterface**********************/
ControllerInterface &ControllerInterface::GetInstance()
{
    typedef ACE_Singleton<Controller, ACE_Recursive_Thread_Mutex> TheController;

    return *TheController::instance();
}

/**********************class Controller**********************/
/* public function */
Controller::Controller()
	:db(nullptr), session(nullptr)
{
}

Controller::~Controller()
{
	if (session != nullptr)
		delete session;
	if (db != nullptr)
		delete db;
}

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

	MovieId ret = InsertMovie(movieId, description);
	t.commit(); 
	return ret;
}

TsSvcId Controller::AddReferenceService(TsId tsId, ServiceId refsId, const char *description)
{
    odb::transaction t (db->begin());   

    TsSvcId ret = InsertRefs(tsId, refsId, description);	
    t.commit(); 
    return TsSvcId(tsId, refsId);
}

TableIndex Controller::AddReferenceServiceEvent(TsId refsTsId, ServiceId refsId, EventId eventId, 
										  TimePoint startTimePoint, Seconds duration)
{
    odb::transaction t (db->begin());   

    TableIndex ret = InsertRefsEvent(refsTsId, refsId, eventId, startTimePoint, duration);
    t.commit(); 
    return ret;
}

MovieId Controller::BindMovieAndRefsEvent(MovieId movieId, TableIndex eventIdx)
{
    odb::transaction t (db->begin());   

    MovieId ret = InsertMovieRefsEvent(movieId, eventIdx);
    t.commit(); 
    return true;
}

bool Controller::DeleteMovie(MovieId movieId)
{
	return true;
}

bool Controller::DeleteReferenceService(TsId tsId, ServiceId refsId)
{
	return true;
}

bool Controller::DeleteReferenceServiceEvent(TsId refsTsId, ServiceId refsId, EventId eventId)
{
	return true;
}

bool Controller::DeleteReferenceServiceEventMovie(TsId refsTsId, ServiceId refsId, EventId eventId, 
												  MovieId movieId)
{
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

bool Controller::Start(const char *user, const char *password, const char *schema)
{
	db = CreateDatabase(user, password, schema);
	session = new odb::session();

	return true;
}

/* private function */
MovieId Controller::InsertMovie(MovieId movieId, const char *description)
{
	Movie movie(movieId, description);
	db->persist(movie);
	return movieId;
}

TsSvcId Controller::InsertRefs(TsId tsId, ServiceId refsId, const char *description)
{
	shared_ptr<ReferenceService> refs = make_shared<ReferenceService>(tsId, refsId, description);
	db->persist(*refs);
	return TsSvcId(tsId, refsId);
}

TableIndex Controller::InsertRefsEvent(TsId refsTsId, ServiceId refsId, EventId eventId, 
								       TimePoint startTimePoint, Seconds duration)
{	
	TableIndexHelperInterface& indexHelper = IndexInstHelper::GetInstance().GetInstance("Refs Event");
	TableIndex idx = indexHelper.GetUseableTableIndex();
	ReferenceServiceEvent::RefsPtrType refs = db->load<ReferenceService>(TsSvcId(refsTsId, refsId));
	auto refsEvent = make_shared<ReferenceServiceEvent>(idx, eventId, startTimePoint, duration);
    refs->BindEvent(refsEvent);
	db->persist(refsEvent);

	return idx;
}

MovieId Controller::InsertMovieRefsEvent(MovieId movieId, TableIndex eventIdx)
{	
	shared_ptr<ReferenceServiceEvent> evnt = db->load<ReferenceServiceEvent>(eventIdx);
	shared_ptr<Movie> movie = db->load<Movie>(movieId);

    movie->BindRefsEvent(evnt);
	db->update(*movie);

	return movieId;
}

#pragma warning(pop)