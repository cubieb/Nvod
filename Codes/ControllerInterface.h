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
	virtual TsSvcId AddReferenceService(TsId tsId, ServiceId refsId, const char *description) = 0;
	virtual TableIndex AddReferenceServiceEvent(TsId refsTsId, ServiceId refsId, EventId eventId, 
		                                        TimePoint startTimePoint, Seconds duration) = 0;
	virtual MovieId BindMovieAndRefsEvent(TableIndex eventIdx, MovieId movieId) = 0;
	virtual bool DeleteMovie(MovieId movieId) = 0;
	virtual bool DeleteReferenceService(TsId tsId, ServiceId refsId) = 0;
	virtual bool DeleteReferenceServiceEvent(TsId refsTsId, ServiceId refsId, EventId eventId) = 0;
	virtual bool DeleteReferenceServiceEventMovie(TsId refsTsId, ServiceId refsId, EventId eventId, 
		                                          MovieId movieId) = 0;

	virtual void PrintRefsInfo() = 0;
	virtual bool Start(const char *user, const char *password, const char *schema) = 0;	

    static ControllerInterface &GetInstance();
};

#endif