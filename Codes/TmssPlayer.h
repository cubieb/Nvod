#ifndef _Player_h_
#define _Player_h_

#include "PlayerInterface.h"

/**********************class Player**********************/
class TmssPlayer: public PlayerInterface
{
public:
    TmssPlayer();
    ~TmssPlayer();    

    Milliseconds GetWaitingDuration();
    std::pair<uchar_t *, size_t> GetCurrentClip();

    static PlayerInterface* CreateInstance();
};

#endif