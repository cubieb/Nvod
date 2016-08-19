#include "SystemInclude.h"
#include <odb/database.hxx>
#include <odb/session.hxx>
#include <odb/transaction.hxx>

#include "Database.h"
#include "TimeShiftedService.h"
#include "TimeShiftedService-odb.h"
#include "ReferenceService.h"
#include "ReferenceService-odb.h"

#include "ControllerInterface.h"

#include "Main.h"

using namespace std;
using namespace odb::core;

int main(int argc, char **argv)
{ 
	ControllerInterface &controller = ControllerInterface::GetInstance();
	controller.Start("odb_test", "", "nvod");

	controller.AddReferenceService(1, 1, "hello reference service 1.");
	controller.AddReferenceServiceEvent(1, 1, 1, 0, 0);
	controller.AddMovie(1, "movie 1");
	controller.AddMovie(2, "movie 2");
	controller.BindMovieAndRefsEvent(1, 1);
	controller.BindMovieAndRefsEvent(2, 1);

	controller.PrintRefsInfo();

    return 0; 
}
