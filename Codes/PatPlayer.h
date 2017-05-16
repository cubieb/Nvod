#ifndef _PatPlayer_h_
#define _PatPlayer_h_

/* Type */
#include "BaseType.h"

/* Foundation */
#include "SystemInclude.h"

/* Entity */
#include "Entities.h"

/* Functions */
#include "PlayerInterface.h"

using std::shared_ptr;

class TransportPacketHelperInterface;
class PatHelperInterface;

/**********************class DataPipePlayer**********************/
class PatPlayer : public PlayerInterface
{
public:
	PatPlayer(shared_ptr<GlobalCfgEntity> globalCfg, shared_ptr<TsEntity> ts);
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
	void HandlePatTimer(int socketFd, ContinuityCounter& continuityCounter);
	void Send(int socketFd, char *buffer, size_t size);

private:
    /* member variable initialized by constructor */
    std::mutex mtx;
    std::condition_variable cv;
    bool isOnGoing;
	shared_ptr<GlobalCfgEntity> globalCfg;
	shared_ptr<TsEntity> ts;
    
    /* thread relative variable */
    std::thread thePatThread;
};

#endif