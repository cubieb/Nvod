#ifndef _PlayerInterface_h_
#define _PlayerInterface_h_

#include <utility>    //std::pair

/* Foundation */
#include "Type.h"

/**********************class PlayerInterface**********************/
class PlayerInterface
{
public:
    PlayerInterface() {};
    virtual ~PlayerInterface() {};    

    virtual Milliseconds GetWaitingDuration() = 0;
    virtual std::pair<uchar_t *, size_t> GetCurrentClip() = 0;
};

#endif