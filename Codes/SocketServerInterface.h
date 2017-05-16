#ifndef _SocketServerInterface_h_
#define _SocketServerInterface_h_

/**********************class SocketServerInterface**********************/
class SocketServerInterface
{
public:
	SocketServerInterface() {}
	virtual ~SocketServerInterface() {}
	static SocketServerInterface& GetInstance();

	virtual void Start() = 0;
	virtual void Stop() = 0;
    virtual bool IsRunning() = 0;
};

#endif