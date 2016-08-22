#include "SystemInclude.h"
#include <odb/database.hxx>
#include <odb/session.hxx>
#include <odb/transaction.hxx>

#include "Database.h"
#include "TimeShiftedService.h"
#include "TimeShiftedService-odb.h"
#include "ReferenceService.h"
#include "ReferenceService-odb.h"

#include "PlayerInterface.h"
#include "PlayerInterfaceFactory.h"
#include "ControllerInterface.h"

#include "Main.h"

using namespace std;

void DebugNvodService()
{
    TableIndex eventIdx1, eventIdx2;
    TsId tsId         = 1;
    ServiceId svcId   = 1;
    EventId evntId1   = 1;
    EventId evntId2   = 2;
    PosterId posterId = 1;
    MovieId movieId1  = 1;
    MovieId movieId2  = 2;

    ControllerInterface &controller = ControllerInterface::GetInstance();
    controller.Start("odb_test", "", "nvod");

    eventIdx1 = controller.AddReferenceServiceEvent(tsId, svcId, evntId1, 0, 0);
    controller.AddReferenceService(tsId, svcId, "hello reference service 1.");
    controller.AddReferenceService(tsId, svcId, "hello reference service 1.");
    eventIdx1 = controller.AddReferenceServiceEvent(tsId, svcId, evntId1, 0, 0);
    controller.AddMovie(movieId1, "movie 1");
    controller.AddMovie(movieId1, "movie 1");
    controller.AddMovie(movieId2, "movie 2");
    controller.BindMovieAndRefsEvent(movieId1, 1);
    controller.BindMovieAndRefsEvent(movieId1, 1);
    controller.BindMovieAndRefsEvent(movieId2, 1);
    controller.PrintRefsInfo();

    controller.UnbindMovieAndRefsEvent(1, 1);
    controller.UnbindMovieAndRefsEvent(1, 1);
    controller.UnbindMovieAndRefsEvent(2, 1);
    controller.PrintRefsInfo();
    
    controller.DeleteReferenceServiceEvent(eventIdx1);
    controller.DeleteReferenceServiceEvent(eventIdx1);
    controller.DeleteReferenceService(tsId, svcId);
    controller.DeleteReferenceService(tsId, svcId);
    controller.DeleteMovie(movieId1);
    controller.DeleteMovie(movieId2);
    controller.PrintRefsInfo();

    controller.AddTimeShiftedService(tsId, svcId, "hello time shifted service 1.");
    eventIdx1 = controller.AddTimeShiftedServiceEvent(tsId, svcId, evntId1, posterId, 0, 0);
    eventIdx1 = controller.AddTimeShiftedServiceEvent(tsId, svcId, evntId1, posterId, 0, 0);
    eventIdx2 = controller.AddTimeShiftedServiceEvent(tsId, svcId, evntId2, posterId, 0, 0);
    controller.PrintTmssInfo();

    controller.DeleteTimeShiftedServiceEvent(eventIdx2);
    controller.DeleteTimeShiftedServiceEvent(eventIdx2);
    controller.PrintTmssInfo();

    controller.DeleteTimeShiftedService(tsId, svcId);
    controller.PrintTmssInfo();
}

int main(int argc, char **argv)
{
    PlayerInterface *player = CreatePlayerInterface("TmssPlayer");
    cout << player->GetWaitingDuration() << endl;
        
    return 0; 
}
