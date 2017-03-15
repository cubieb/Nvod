#ifndef _Controller_h_
#define _Controller_h_

/* Controller */
#include "ControllerInterface.h"

/**********************class Controller**********************/
class Controller: public ControllerInterface
{
public:
    friend class ControllerInterface;

    /* Create a new thread and wait for reactor event loop. */
    void Start(const char *xmlPath);
    void Stop();

private: /* members do not need to be initialized */
    Controller();
    ~Controller();
};

#endif