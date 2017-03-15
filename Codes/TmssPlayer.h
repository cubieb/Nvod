#ifndef _TmssPlayer_h_
#define _TmssPlayer_h_

/* Foundation */
#include "SystemInclude.h"

/* Entity */
#include "BaseType.h"

/* Functions */
#include "PlayerInterface.h"

#include "TmssPlayerCookie.h"

using std::shared_ptr;
class TmssEntity;
class ExTmssEntity;
class ExRefsEventEntity;

/**********************class TmssPlayer**********************/
class TmssPlayer: public PlayerInterface
{
public:   
    TmssPlayer(shared_ptr<TmssEntity> tmss);
    ~TmssPlayer();
    static PlayerInterface* CreateInstance(shared_ptr<TmssEntity> tmss)
    {
        return new TmssPlayer(tmss);
    }
    
    void Start();
    void Stop();
    bool IsRunning();
    
private:
    void TheMovieThreadMain();
	void PrepareMovie();
    Duration Play();
	void Send(char *buffer, size_t size);
	Duration CalcTimeIntv(Pcr t0, Pcr t1);

private:
    /* member variable initialized by constructor */
    std::mutex mtx;
    std::condition_variable cv;
    TmssPlayerCookie cookie;

    /* thread relative variable */
    std::thread theMovieThread;
    bool isOnGoing;
    std::ifstream fstm;
    int socketFd;
};

#endif