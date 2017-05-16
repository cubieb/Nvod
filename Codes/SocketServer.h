#ifndef _SocketServer_h_
#define _SocketServer_h_

#include "SocketServerInterface.h"

/**********************class SocketServer**********************/
class SocketServer : public SocketServerInterface
{
public:
	SocketServer();
	~SocketServer();

	void Start();
	void Stop();
};

#endif