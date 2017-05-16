#ifndef _SocketServer_h_
#define _SocketServer_h_

#include "SocketServerInterface.h"

/* Type */
#include "BaseType.h"

/* Foundation */
#include "SystemInclude.h"

/**********************class SocketServer**********************/
class SocketServer : public SocketServerInterface
{
public:
	SocketServer();
	~SocketServer();

	void Start();
	void Stop();
    bool IsRunning();

private:
    void TheSocketServerThreadMain();

private:
    std::mutex mtx;
    std::condition_variable cv;
    bool isOnGoing;
    
    std::thread theSocketServerThreadMain;
};

#endif