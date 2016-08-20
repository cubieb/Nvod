#include "SystemInclude.h"

#include "TmssPlayer.h"
using namespace std;

/**********************class TmssPlayer**********************/
/* public function */
TmssPlayer::TmssPlayer() 
{}

TmssPlayer::~TmssPlayer() 
{}

Milliseconds TmssPlayer::GetWaitingDuration()
{
    return 0;
}

pair<uchar_t *, size_t> TmssPlayer::GetCurrentClip()
{
    return make_pair<uchar_t *, size_t>(nullptr, 0);
}

/* static function */
PlayerInterface* TmssPlayer::CreateInstance()
{
    return new TmssPlayer;
}
