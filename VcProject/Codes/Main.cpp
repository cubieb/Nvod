#include "SystemInclude.h"

/* Controller */
#include "ControllerInterface.h"

#include "Main.h"
using namespace std;

int main(int argc, char **argv)
{
    ControllerInterface &controller = ControllerInterface::GetInstance();
    controller.StartEventLoop();
    return 0; 
}
