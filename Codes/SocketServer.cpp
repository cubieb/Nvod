#include "SocketServer.h"

SocketServerInterface& SocketServerInterface::GetInstance()
{
	/* In C++11, the following is guaranteed to perform thread-safe initialisation: */
	static SocketServer instance;
	return instance;
}

/**********************class SocketServer**********************/
SocketServer::SocketServer()
{}

SocketServer::~SocketServer()
{}

void SocketServer::Start()
{}

void SocketServer::Stop()
{}