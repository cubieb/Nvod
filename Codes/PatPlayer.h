#ifndef _PatPlayer_h_
#define _PatPlayer_h_

/* Foundation */
#include "BaseType.h"
#include "SystemInclude.h"

/* Functions */
#include "PlayerInterface.h"

#include "PatPlayerCookie.h"

using std::shared_ptr;

class PatPlayerCookie;

/**********************class DataPipePlayer**********************/
class PatPlayer : public PlayerInterface
{
public:
    PatPlayer(shared_ptr<GlobalCfgEntity> dataPipeGlobal, shared_ptr<TsEntity> ts);
    ~PatPlayer();
    static PlayerInterface* CreateInstance(shared_ptr<GlobalCfgEntity> dataPipeGlobal, shared_ptr<TsEntity> ts)
    {
		return new PatPlayer(dataPipeGlobal, ts);
    }

    void Start();
    void Stop();
    bool IsRunning();

private:
    void ThePatThreadMain();
    void HandlePatTimer();
    void Send(char *buffer, size_t size);

private:
    /* member variable initialized by constructor */
    std::mutex mtx;
    std::condition_variable cv;
    PatPlayerCookie cookie;
    
    /* thread relative variable */
    std::thread thePatThread;
    bool isOnGoing;
    std::ifstream fstm;
    int socketFd;
};

#endif