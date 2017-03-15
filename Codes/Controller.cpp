#include "Controller.h"

/* Foundation */
#include "SystemInclude.h"
#include "PlayerInterface.h"

using namespace std;

/**********************class ControllerInterface**********************/
ControllerInterface &ControllerInterface::GetInstance()
{
    /* In C++11, the following is guaranteed to perform thread-safe initialisation: */
    static Controller instance;
    return instance;
}

/**********************class Controller**********************/
/* public function */
Controller::Controller()
{}

Controller::~Controller()
{}

void Controller::Start(const char *xmlPath)
{}

void Controller::Stop()
{}