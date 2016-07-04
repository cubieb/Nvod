#ifndef __NVODSERVER_H_TERRY__
#define __NVODSERVER_H_TERRY__
 
#ifdef StudyNvod
#define NVODSERVICETYPE_TIMESHIFT 1
typedef void* PTHREAD_T;
struct CServer{};
struct CEvent{};
struct DataMgr{};
struct CServiceInfo{};
class CPthreadSem
{
public:
    void Wait() {};
};

class CMutex
{
public: 
    CMutex() {}
    void Lock() {};
    void UnLock() {};
};

uint64_t GetSysTick();

class CNVODServer : public CServer,
	                public CEvent,
	                public DataMgr
{
public:
	CNVODServer();
	~CNVODServer();

	int Init( int Id,int ReadPipe,int WritePipe,void *pData );
	void UnInit();

	int Run();

	int ServiceExit( int ServiceID );
	void AddSendNum()
	{
	    m_NumMutex.Lock();
	    m_iSendNum++;
	    m_NumMutex.UnLock();
	}
	void DelSendNum()
	{
	    m_NumMutex.Lock();
	    m_iSendNum--;
	    m_NumMutex.UnLock();
	}
	int GetSendNum()
	{
	    int re = 0;
	    m_NumMutex.Lock();
	    re = m_iSendNum;
	    m_NumMutex.UnLock();
	    return re;
	}

    int BuildPaddingPack(unsigned char *pBuff,int Size);
private:
	void *m_pChannelInfo;
	unsigned char m_PaddingCounter;

#ifdef __WIN32
	static DWORD WINAPI BufferFiles( void *argc );
#else
	static void *BufferFiles( void *argc );
#endif

	int m_ChannelID;
	int m_SockFd;
	int m_iReadPipe;
	int m_iSendNum;
	CMutex  m_NumMutex;

	PTHREAD_T m_BuffThread;
};

#endif //#ifdef StudyNvod
#endif /* NVODServer.h */
