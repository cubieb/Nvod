#ifndef _Command_h_
#define _Command_h_

#include "SystemInclude.h"
#include "Type.h"
#include "TimeShiftedService.h"

class Command
{
public:
    Command() {};
    virtual ~Command() {};

private:
    virtual void Execute() = 0;
};

class PosterCommand: public Command
{
};

class PlayerTimerCommand: public Command
{
public:
    PlayerTimerCommand(TimeShiftedService &tms)
        : tms(tms)
    {};
    virtual ~PlayerTimerCommand() {};

    void Execute()
    { 
    }

private:
	TimeShiftedService &tms;
};

#endif