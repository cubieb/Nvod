#ifndef _ControllerInterface_h_
#define _ControllerInterface_h_

#include "ace/Reactor.h"
#include "Type.h"

class ControllerInterface: public ACE_Event_Handler
{
public:    
    ControllerInterface() {};
    virtual ~ControllerInterface() {};

	virtual MovieId AddMovie(MovieId movieId, const char *description) = 0;
	virtual TsSvcId AddReferenceService(TsId rfsTsId, ServiceId refsId, const char *description) = 0;
	virtual TableIndex AddReferenceServiceEvent(TsId refsTsId, ServiceId refsId, EventId eventId, 
		                                        TimePoint startTimePoint, Seconds duration) = 0;
	virtual MovieId BindMovieAndRefsEvent(TableIndex eventIdx, MovieId movieId) = 0;
	virtual bool DeleteMovie(MovieId movieId) = 0;
	virtual bool DeleteReferenceService(TsId tsId, ServiceId refsId) = 0;
	virtual bool DeleteReferenceServiceEvent(TableIndex eventIdx) = 0;
	virtual bool UnbindMovieAndRefsEvent(MovieId movieId, TableIndex eventIdx) = 0;
    
	virtual TsSvcId AddTimeShiftedService(TsId tmssTsId, ServiceId tmssId, const char *description) = 0;
	virtual TableIndex AddTimeShiftedServiceEvent(TsId tmssTsId, ServiceId tmssId, EventId eventId, PosterId posterId,
		                                          TimePoint startTimePoint, Seconds duration) = 0;
	virtual bool DeleteTimeShiftedService(TsId tmssTsId, ServiceId tmssId) = 0;
	virtual bool DeleteTimeShiftedServiceEvent(TableIndex eventIdx) = 0;

    virtual void PrintRefsInfo() = 0;
    virtual void PrintTmssInfo() = 0;
	virtual bool Start(const char *user, const char *password, const char *schema) = 0;	

    static ControllerInterface &GetInstance();
};

#endif