#ifndef _RefsPlayer_h_
#define _RefsPlayer_h_

/* Foundation */
#include "SystemInclude.h"

/* Entity */
#include "BaseType.h"

/* Functions */
#include "PlayerInterface.h"
#include "RefsPlayerCookie.h"

using std::shared_ptr;

class GlobalCfgEntity;
class PosterViewEntity;
class RefsEntity;
class RefsEventEntity;

class RefsPlayerCookie;

/**********************class RefsPlayer**********************/
class RefsPlayer : public PlayerInterface
{
public:
    typedef std::function<void()> TimerHandler;

    RefsPlayer(shared_ptr<GlobalCfgEntity> dataPipeGlobal,
        shared_ptr<RefsEntity> refs);
    ~RefsPlayer();
    static PlayerInterface* CreateInstance(shared_ptr<GlobalCfgEntity> dataPipeGlobal,
        shared_ptr<RefsEntity> refs)
    {
        return new RefsPlayer(dataPipeGlobal, refs);
    }

    void Start();
    void Stop();
    bool IsRunning();

private:
    void TheDataPipeThreadMain();
    Duration HandlerTimer(Duration invl, Duration initInvl, TimerHandler handler);
    void HandlePmtTimer();
    void HandlePosterTimer();
    void HandlePosterDitTimer();
    void HandlePosterDdtTimer();
	void Send(char *buffer, size_t size);

private:
    /* member variable initialized by constructor */
    std::mutex mtx;
    std::condition_variable cv;
    RefsPlayerCookie cookie;

    /* thread relative variable */
    std::thread theDataPipeThread;
    bool isOnGoing;
    std::ifstream fstm;
    int socketFd;
};

#endif