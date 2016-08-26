#include "SystemInclude.h"
#include <odb/database.hxx>
#include <odb/session.hxx>
#include <odb/transaction.hxx>

#include "Database.h"
#include "NvodService.h"
#include "NvodService-odb.h"

#include "PlayerInterface.h"
#include "ControllerInterface.h"

#include "Main.h"

using namespace std;

void DebugNvodService()
{
    TableIndex eventIdx1;
    TableIndex refsEventIdx1, refsEventIdx2;
    TsId tsId         = 1;
    ServiceId svcId1   = 1;
    ServiceId svcId2   = 2;
    EventId evntId1   = 1;
    EventId evntId2   = 2;
    PosterId posterId = 1;
    MovieId movieId1  = 1;
    MovieId movieId2  = 2;

    ControllerInterface &controller = ControllerInterface::GetInstance();
    controller.Start("odb_test", "", "nvod");

    controller.AddReferenceService(tsId, svcId1, "hello reference service 1.");
    eventIdx1 = controller.AddReferenceServiceEvent(tsId, svcId1, evntId1, 0, 0);
    controller.AddMovie(movieId1, "movie 1");
    controller.AddMovie(movieId2, "movie 2");
    controller.BindMovieAndRefsEvent(movieId1, 1);
    controller.BindMovieAndRefsEvent(movieId2, 1);
    controller.PrintRefsInfo();


    TimePoint curTime = time(nullptr);

    controller.AddTimeShiftedService(tsId, svcId1, "hello time shifted service 1.");
    refsEventIdx1 = controller.AddTimeShiftedServiceEvent(tsId, svcId1, evntId1, posterId, curTime, 3600, eventIdx1);
    refsEventIdx2 = controller.AddTimeShiftedServiceEvent(tsId, svcId1, evntId2, posterId, curTime + 3600, 3600, eventIdx1);
    controller.AddTimeShiftedService(tsId, svcId2, "hello time shifted service 2.");
    refsEventIdx2 = controller.AddTimeShiftedServiceEvent(tsId, svcId2, evntId1, posterId, curTime + 3600, 3600, eventIdx1);
    controller.PrintTmssInfo();

    controller.DeleteTimeShiftedServiceEvent(refsEventIdx1);
    controller.DeleteTimeShiftedServiceEvent(refsEventIdx2);
    controller.PrintTmssInfo();

    controller.DeleteTimeShiftedService(tsId, svcId1);
    controller.DeleteTimeShiftedService(tsId, svcId2);
    controller.PrintTmssInfo();

    controller.UnbindMovieAndRefsEvent(1, 1);
    controller.UnbindMovieAndRefsEvent(1, 1);
    controller.UnbindMovieAndRefsEvent(2, 1);
    controller.PrintRefsInfo();
    
    controller.DeleteReferenceServiceEvent(eventIdx1);
    controller.DeleteReferenceServiceEvent(eventIdx1);
    controller.DeleteReferenceService(tsId, svcId1);
    controller.DeleteReferenceService(tsId, svcId1);
    controller.DeleteMovie(movieId1);
    controller.DeleteMovie(movieId2);
    controller.PrintRefsInfo();
}

int main(int argc, char **argv)
{
    DebugNvodService();

    return 0; 
}
