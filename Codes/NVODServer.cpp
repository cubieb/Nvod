#ifdef StudyNvod
#include "SystemInclude.h"
#include "NVODServer.h"

#define TSSENDPACKSIZE 1316
#define MAXBUFFERNUM   15000
#define BITRATEPLUP    50

unsigned int crc32(const unsigned char *s, unsigned int len);
#define PADDING_PID 0x1FC0
#define TSSENDPACKSIZE 1316 

static void mysql_thread_init()
{} 
 
static void mysql_thread_end()  
{}
  
uint64_t GetSysTick()
{
    return 0;
}

#define SLEEP(x)

class CBuffer
{
public:
    CBuffer(){m_iFlags = m_iSize = 0;m_PCR = 0;}
    ~CBuffer(){m_iSize = 0;}

    unsigned char  m_pData[TSSENDPACKSIZE];
    unsigned int   m_iSize;
    unsigned char  m_iFlags;
    uint64_t       m_PCR;
};

class TimeShiftService : public DataMgr,public CServiceInfo, public CEvent
{
public:
	TimeShiftService()
	{
	    m_iIsQuit = 0;
	    m_Begin = m_End = m_Cur = NULL;
    }
	~TimeShiftService(){}

	int Init();
	void UnInit();

#ifdef __WIN32
	static DWORD WINAPI SendThread( void *argc );
	static DWORD WINAPI ReadThread( void *argc );
#else
	static void *SendThread( void *argc );
	static void *ReadThread( void *argc );
#endif

	int Run();
	int SendThread();
	int ReadThread();
private:
	int CompareTime( const char *EventTime );
	int GetBuff(unsigned char *pBuff,unsigned int &Size,uint64_t &PCR);
	int PushBuff(unsigned char *pBuff,unsigned int Size,uint64_t PCR);
public:

private:
	char m_iIsQuit;
	PTHREAD_T m_SendThreadID,m_ReadThreadID;
	CMutex    m_BufferMutex;
	CPthreadSem            m_Sem;

	CBuffer  *m_Begin,*m_End,*m_Cur;
    unsigned int m_iSize,m_iMaxSize;

private:
    CNVODServer *m_pFunObj;
    uint16_t     m_iDstPort;
    std::string       m_strDstIp;
};

int TimeShiftService::GetBuff(unsigned char *pBuff,unsigned int &Size,uint64_t &PCR)
{
    if( !pBuff )
        return -1;
    CBuffer *pTSData = NULL;
    if( m_iSize == 0 )
        return 0;
    m_BufferMutex.Lock();
    pTSData = m_Cur++;
    if( m_Cur >= m_End )
        m_Cur = m_Begin;
    m_iSize--;
    Size = pTSData->m_iSize;
    memcpy(pBuff,pTSData->m_pData,Size);
    PCR = pTSData->m_PCR;

    m_BufferMutex.UnLock();

    return 1;
}


int TimeShiftService::PushBuff(unsigned char *pBuff,unsigned int Size,uint64_t PCR)
{
    if( !pBuff || !Size )
        return -1;
    if( m_iSize == m_iMaxSize )
        return 0;
    m_BufferMutex.Lock();
    CBuffer *pTSData = NULL;

    if( m_Cur + m_iSize < m_End )
    {
        pTSData = m_Cur + m_iSize;
    }
    else
    {
        pTSData = m_Begin + (m_Cur + m_iSize - m_End);
    }
    memcpy(pTSData->m_pData,pBuff,Size);
    pTSData->m_iSize = Size;
    pTSData->m_PCR = PCR;
    m_iSize++;
    m_BufferMutex.UnLock();
    return 1;
}

CServiceInfo* CreateServiceInfo( int Type )
{
	CServiceInfo* p_re = NULL;

	switch( Type )
	{
	case NVODSERVICETYPE_TIMESHIFT:
		p_re = new TimeShiftService();
		break;
	default:
		p_re = new CServiceInfo();
		break;
	}

	return p_re;
}

int TimeShiftService::SendThread()
{
    CNVODServer *pNVODServer = (CNVODServer *)m_pFunObj;
    struct sockaddr_in SendAddr;
    memset(&SendAddr,0x00,sizeof(SendAddr));
    SendAddr.sin_family = AF_INET;
    SendAddr.sin_port = htons(m_iDstPort);
	SendAddr.sin_addr.s_addr = inet_addr(m_strDstIp.c_str());
	int iFlags = 0;
	uint64_t PCR;
	unsigned long long CurTime = 0;
	int SockFd = socket(AF_INET, SOCK_DGRAM, 0);
	if( SockFd < 0 )
	{
		//LogDump(LOGMSG_LEVEL_ERROR,"SendThread Create Sockfd Error [%s]",strerror(errno));
		m_iIsQuit = 1;
	}
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(SockFd, &rfds);
	int iSendBuf = 3 * 1024 * 1024;
	setsockopt(SockFd,SOL_SOCKET,SO_SNDBUF,(const char *)&iSendBuf,sizeof(iSendBuf));

    while( !m_iIsQuit )
    {
        m_Sem.Wait();

        //LogDump(LOGMSG_LEVEL_PRINT,"Begin to Send ServiceID: %d ",m_iServiceID);

        unsigned char pBuff[TSSENDPACKSIZE];
        unsigned int   Size  = 0;

        pNVODServer->AddSendNum();
        struct timeval tv;
        tv.tv_sec = 0;

        uint64_t FisrtPCR = 0,FirstSendTime = 0;
        uint64_t TotolSendSize = 0;
        while( !m_iIsQuit )
        {
            iFlags = GetBuff( pBuff, Size, PCR );
            if (Size == 188)
			{
				//LogDump(LOGMSG_LEVEL_ERROR,"SendThread , Send Over", Size);
				break;
			}
            if( iFlags == 0)
            {//无数据发送
                SLEEP(100);
                continue;
            }
            else if(iFlags < 0)
            {//出错
                //LogDump(LOGMSG_LEVEL_ERROR,"Send Thread Quit");
                m_iIsQuit = 100;
                break;
            }
            else
            {//有数据发送
                if( FisrtPCR == 0 )
                {
                    FisrtPCR = PCR;
                    FirstSendTime = GetSysTick();
                    continue;
                }
                if( PCR > 0 )
                {
                    uint64_t TimeAdjust = (PCR - FisrtPCR) / 27;//
                    uint64_t RightTime = FirstSendTime + (++TimeAdjust);
                    uint64_t CurTime = GetSysTick();            //纳秒    
                    if( RightTime > CurTime )
                    {
                        tv.tv_sec  = ( RightTime - CurTime ) / 1000000;
                    	tv.tv_usec = ( RightTime - CurTime ) % 1000000;
                    }
                    else
                    {
                    	tv.tv_usec = 1000;
                    	tv.tv_sec  = 0;
                    }
                }
                else if( FisrtPCR == 0 )
                {
                    tv.tv_sec  = 0;
                    tv.tv_usec = 1500;
                }
                else
                {
                    tv.tv_sec  = 0;
                    tv.tv_usec = 1;
                }
                if( select(1 + SockFd, &rfds, NULL, NULL, &tv) < 0 )
                {
                    //LogDump(LOGMSG_LEVEL_WARING,"Send Thread select error %s %d",strerror(errno),tv.tv_usec);
                }
                int SendSize = sendto(SockFd,(const char *)pBuff,Size,0,(sockaddr *)&SendAddr,sizeof(SendAddr));
                TotolSendSize += SendSize;
            }
            if(Size != TSSENDPACKSIZE)
            {
                unsigned long long int EndTime = GetSysTick();
                break;
            }
        }
        pNVODServer->DelSendNum();

        //LogDump(LOGMSG_LEVEL_PRINT,"End to Send ServiceID: %d ",m_iServiceID);
    }
    return 1;
}

int TimeShiftService::ReadThread()
{

	CNVODServer *pNVODServer = (CNVODServer *)m_pFunObj;
	int TotleSize = 0;
	unsigned char VideoCounter,AudioCounter,PCRCounter;
	VideoCounter = AudioCounter = PCRCounter = 0;
//std::cout<<"the Service Ip is :"<<m_strDstIp<<" the Port is :"<<m_iDstPort<<std::endl;
	while( !m_iIsQuit )
	{
		CEventTimeShiftInfo* pEvent = NULL;
		GetData( (void **)&pEvent);
		if( !pEvent )
		{
			goto _EXIT;
		}
		
		//
        {
            uint64_t PCR = 0;
            uint64_t CURPCR=0;
            unsigned char pBuff[TSSENDPACKSIZE];
            memset(pBuff,0x00,TSSENDPACKSIZE);
            int iReadedSize = 0;
            //需要在这里首先定位到文件合适的位置
			pEvent->SeekToSendPos();
			//然后才读取
            while(( iReadedSize = pEvent->Read(pBuff, TSSENDPACKSIZE, CURPCR, VideoCounter, AudioCounter, PCRCounter) ) > 0)
            {
            	PCR = 0;
            	if( CURPCR != 0 )
            	{
            		PCR = CURPCR;
            	}
            	while ( iReadedSize < TSSENDPACKSIZE )
            	{
					int iNeededSize = TSSENDPACKSIZE - iReadedSize;
					int iCurReadedSize = pEvent->Read(pBuff+ iReadedSize, iNeededSize, CURPCR, 
                                                      VideoCounter, AudioCounter, PCRCounter);
					if ( CURPCR != 0 )
					{
						PCR = CURPCR;
					}
					if( iCurReadedSize <= 0 )
					{
						//最后发一个空包 ，让SendTread退出当前发送
						unsigned char pBuff[188];
						memset(pBuff, 0xff, 188);
						while (!PushBuff(pBuff, 188, 0) && !m_iIsQuit)
						{
							SLEEP(50000);
						}
						goto _EXIT;
					}
					iReadedSize += iCurReadedSize;
            	}

                while( !PushBuff(pBuff, TSSENDPACKSIZE, PCR) && !m_iIsQuit )
                {
                    SLEEP(50000);
                }
            }

            //最后发一个空包 ，让SendTread退出当前发送
			memset(pBuff, 0xff, TSSENDPACKSIZE);
			while (!PushBuff(pBuff, 188, 0) && !m_iIsQuit)
			{
				SLEEP(50000);
			}
        }
_EXIT:
		pNVODServer->PushEvent(Event_Type_SendOver, pEvent);
		SLEEP(1000);
	}



	return 0;
}

int TimeShiftService::Run()
{
	mysql_thread_init();
//printf("Time ShiftService %d Run\n",m_iServiceID);
	ServiceWork* pServiceWork = 0x00;
	std::vector<CEventTimeShiftInfo* > EventVector;
	std::vector<CEventTimeShiftInfo* > ReadVector;
	while( !m_iIsQuit )
	{
		void *pData = 0x00;
		Event_Type Event = Event_Type_None;
		if( GetEvent(Event,&pData) != 0 )
		{
			switch(Event)
			{
			case Event_Type_Buffer:
				EventVector.push_back( (CEventTimeShiftInfo*)pData );
				PushData(pData);
				break;
			default:
				break;
			}
		}

		if( EventVector.size() )
		{
			std::vector<CEventTimeShiftInfo* >::iterator itr = EventVector.begin();

			int re = CompareTime( (*itr)->m_BeginTime.c_str() );
			if( re <= 0 )
			{
				CNVODServer *pNVODServer = (CNVODServer *)m_pFunObj;
				pNVODServer->PushEvent(Event_Type_SendBegin,(*itr)); //仅仅是写数据库而已
				itr = EventVector.erase(itr);
				//if( re == 0 ) //20110914 litao close
                {
					m_Sem.Post();
                }
			}
		}
		SLEEP(50000);
	}
	m_iIsQuit = 1;

	mysql_thread_end();
	return 0;
}


#ifdef __WIN32
DWORD WINAPI TimeShiftService::SendThread( void *argc )
#else
void *TimeShiftService::SendThread( void *argc )
#endif
{
	mysql_thread_init();
	TimeShiftService *pServiceInfo = (TimeShiftService *) argc;
	pServiceInfo->SendThread();
	mysql_thread_end();
#ifdef __WIN32
	return 0;
#else
	return NULL;
#endif
}

#ifdef __WIN32
DWORD WINAPI TimeShiftService::ReadThread( void *argc )
#else
void *TimeShiftService::ReadThread( void *argc )
#endif
{
	mysql_thread_init();
	TimeShiftService *pServiceInfo = (TimeShiftService *) argc;
	pServiceInfo->ReadThread();

	mysql_thread_end();
#ifdef __WIN32
	return 0;
#else
	return NULL;
#endif
}


int TimeShiftService::Init()
{//�������߳�ReadThread1
    m_Sem.Init();

    m_iSize = 0;
	m_iMaxSize = MAXBUFFERNUM;
	m_Begin = new CBuffer[MAXBUFFERNUM];  //这个内存有点大哦， 1316（1329）*15000= 19Mb
	if ( m_Begin == NULL )
	{
		return 3;
	}
	m_Cur = m_Begin;
	m_End = m_Begin + MAXBUFFERNUM;

#ifdef __WIN32
	m_SendThreadID = CreateThread(NULL,0,SendThread,this,0,NULL);
	if( m_SendThreadID == NULL )
		return 2;
    m_ReadThreadID = CreateThread(NULL,0,ReadThread,this,0,NULL);
    if( m_ReadThreadID == NULL )
        return 2;
#else
	if(pthread_create(&(m_SendThreadID),NULL,SendThread,this) != 0)  //返回0表示成功，否则返回错误号。
		return 2;
	SLEEP(10000);
    if(pthread_create(&(m_SendThreadID),NULL,ReadThread,this) != 0)
        return 2;
    SLEEP(10000);
#endif
	return 1;
}

void TimeShiftService::UnInit()
{
    {
        if( m_Begin != NULL )
        {
            if( m_iSize == 1 )
                delete m_Begin;
            else
                delete []m_Begin;
            m_Begin = m_End = m_Cur = NULL;
        }
    }
    m_Sem.Post();
    m_Sem.UnInit();
	m_iIsQuit = 1;
#ifdef __WIN32
	if( m_SendThreadID )
	{
		WaitForSingleObject(m_SendThreadID,INFINITE);
		CloseHandle( m_SendThreadID );
		m_SendThreadID = NULL;
	}
#else
#endif
	{
		CNVODServer *pServer = (CNVODServer *)m_pFunObj;
		unsigned int *pExitCode = new unsigned int[2];
		pExitCode[0] = m_iServiceID;
		pExitCode[1] = 0x00;
		pServer->PushEvent(Event_Type_ThreadExit,pExitCode);
	}
	return ;
}

int TimeShiftService::CompareTime( const char *pEventTime )
{
	int re = -1;
	if( !pEventTime && strlen(pEventTime) < 18 )
        return -1;
	struct Times_T Time_Evnet,Time_Cur;
	unsigned int Year = atoi(pEventTime);
	unsigned int Month  = atoi( (pEventTime + 5) );
	unsigned int Day    = atoi( (pEventTime + 8) );

	unsigned int Hour   = atoi( (pEventTime + 11) );
	unsigned int Minute = atoi( (pEventTime + 14) );
	unsigned int Second = atoi( (pEventTime + 17) );

	Time_Evnet.YMDH = Year * 10000 + Month * 100 + Day;
	Time_Evnet.MSM  = Hour * 10000 + Minute * 100 + Second;
	Time_Cur = GetTimes();

	Time_Cur.MSM /= 1000;
	if( Time_Evnet.YMDH != Time_Cur.YMDH )
	{
		return 1;
	}
	else
	{
		re = Time_Evnet.MSM - Time_Cur.MSM;
	}



	return re;
}

/*****************************************************************************************************************************************/
int CEventTimeShiftInfo::Init(unsigned int PMTPid,unsigned int PCRPid,unsigned int AudioPid,unsigned int VideoPid)
{
	int re = 2;

	cConfig *pConfig = NULL;
    GetConfig(&pConfig);
    if(pConfig == NULL)
    {//�õ������ļ�
        printf("GetConfig Failed");
        return ERROR_CONFIG_INVALID;
    }
	std::string FileName(pConfig->GetValue("LocalFilePath"));
	FileName += m_FileName;
	//std::cout<<FileName<<std::endl;
/*	m_pFile = fopen(FileName.c_str(),"r");
	if( m_pFile==NULL)
	{
		LogDump(LOGMSG_LEVEL_PRINT, "open file%s FILE=%d, error=%s",FileName.c_str(), m_pFile, strerror(errno)); //test
	}

	if( m_pFile == NULL )
		return 3;
*/
	//这里可能造成资源泄漏， 没有判断m_FileFd是否已经打开了文件了。
	m_FileFd = open(FileName.c_str(),O_BINARY | O_RDONLY | O_LARGEFILE);
	if( m_FileFd == -1 )
	{
		LogDump(LOGMSG_LEVEL_PRINT, "Service %d Open File %s Failed, error: %s", m_iService_ID, FileName.c_str(), strerror(errno)); //test
		return 3;
	}
	m_iPMTPID = PMTPid;
	m_iPCRPID = PCRPid;
	m_iAudioPID = AudioPid;
	m_iVideoPID = VideoPid;
	return 2;
}

void CEventTimeShiftInfo::UnInit()
{
	//fclose( m_pFile );
	close(m_FileFd);
	m_FileName.clear();
}

//返回秒
int CEventTimeShiftInfo::SecondFromCurTime( const char *pEventTime )
{
	int re = -1;
	if( !pEventTime && strlen(pEventTime) < 18 )
        return -1;
	//ת���ַ���Ϊ����
	struct Times_T Time_Evnet,Time_Cur;
	unsigned int Year = atoi(pEventTime);
	unsigned int Month  = atoi( (pEventTime + 5) );
	unsigned int Day    = atoi( (pEventTime + 8) );

	unsigned int Hour   = atoi( (pEventTime + 11) );
	unsigned int Minute = atoi( (pEventTime + 14) );
	unsigned int Second = atoi( (pEventTime + 17) );

	Time_Evnet.YMDH = Year * 10000 + Month * 100 + Day;
	Time_Evnet.MSM  = Hour * 10000 + Minute * 100 + Second;
	Time_Cur = GetTimes();

	Time_Cur.MSM /= 1000;
	if( Time_Evnet.YMDH != Time_Cur.YMDH )
	{
		return 1;
	}
	else
	{
		unsigned int CurHour = Time_Cur.MSM/10000;
		unsigned int CurMinute = Time_Cur.MSM/100 - CurHour*100;
		unsigned int CurSecond = Time_Cur.MSM - CurHour*10000 - CurMinute*100;
		re = (Hour - CurHour)*3600 + (Minute - CurMinute)*60 + Second-CurSecond;
	}

	return re;
}

int CEventTimeShiftInfo::ReadNextPCR(unsigned long long &PCR)
{
	PCR = 0;
	int re = -1;

	unsigned char pBuff[188];
	memset(pBuff, 0 , 188);
	ssize_t iReadedSize = 0;
	while ( (iReadedSize = read(m_FileFd, pBuff, 188)) > 0)
	{
		while ( iReadedSize < 188 )
		{
			int iNeededSize = 188 - iReadedSize;
			int iCurReadedSize = read(m_FileFd, pBuff, iNeededSize);
			if (iCurReadedSize <= 0)
			{				//读错误了
				re = -1;
				PCR = 0;
				goto _EXIT;
			}
			iReadedSize += iCurReadedSize;
		}

		unsigned char *ptemp = pBuff;

		if (*(ptemp++) != 0x47)
		{
			re = -2;
			break;
		}
		unsigned int Pid = 0;
		Pid = (ptemp[0] & 0x1F) << 8 | ptemp[1];

		if ( ( ptemp[2] & 0x20 ) && ( Pid == m_iFilePCRPID ) )
		{//AdaptionField
			uint8_t AdaptFileLen = ptemp[3];
			uint8_t *Ada_Data = ptemp + 4;
			uint8_t Flags = *(Ada_Data++);
			if (Flags & 0x10 && AdaptFileLen > 0)
			{
				uint64_t PCR_Base = ((uint64_t(Ada_Data[0]) << 25)
						| (Ada_Data[1] << 17) | (Ada_Data[2] << 9)
						| Ada_Data[3] << 1);
				Ada_Data += 4;
				PCR_Base |= (*Ada_Data & 0x80) >> 7;
				uint64_t PCR_Ext = (*(Ada_Data++) & 0x01) << 8;
				PCR_Ext |= *(Ada_Data++);
				//printf("%llx %llx \n",PCR_Base,PCR_Ext);
				PCR = PCR_Base * 300 + PCR_Ext;
				re = 0;
				goto _EXIT;
			}
		}
		memset(pBuff, 0 , 188);
	}
	re = -1;
_EXIT:
	return re;
}

int  CEventTimeShiftInfo::SeekToSendPos()
{
	int iRet = 0;
	//如果时间已经过了很多，才需要Seek,并且要同步到 0x47 ts包的开始位置。
	int iTimeLost = SecondFromCurTime(m_BeginTime.c_str());
//	LogDump( LOGMSG_LEVEL_PRINT,"TimeLost=%d", iTimeLost);

	lseek(m_FileFd, 0, SEEK_SET);// 文件开始的地方
	if (iTimeLost < -1)
	{
		int iTimeSeek = -iTimeLost;
		unsigned long long FirstPCR;
		int iResult = ReadNextPCR(FirstPCR);
		unsigned long long SeekPCR = FirstPCR + ((unsigned long long)iTimeSeek * 27000000) ;

//		LogDump( LOGMSG_LEVEL_PRINT, "Event=%d, TimeSeek=%d, FirstPCR=%llu, SeekPCR=%llu", m_iEvent_ID,
//				iTimeSeek, FirstPCR, SeekPCR);

		if (iTimeSeek > m_nDuration)
		{
			//fseek(m_pFile, 0, SEEK_END);
			lseek(m_FileFd, 0, SEEK_END);
		}
		else
		{
			while (true)
			{
				off64_t SendPos = (iTimeSeek * 1.0) / m_nDuration
						* m_ulFileSize;
				SendPos = (SendPos / 188) * 188; //对齐188
				off64_t seekRet = lseek64(m_FileFd, SendPos, SEEK_SET);
	//			LogDump( LOGMSG_LEVEL_PRINT,"Event=%d, Seek to %llu, result=%llu", m_iEvent_ID,
	//					SendPos, seekRet);
				if (seekRet == -1)
				{
					lseek(m_FileFd, 0, SEEK_END);
					break;
				}
				unsigned long long CurPCR = 0;
				int iResult = ReadNextPCR(CurPCR);
				if (iResult != 0)
				{
					break;
				}
				int iDiff = (int) (SeekPCR / 27000000) - (int) (CurPCR / 27000000);
				if ( abs(iDiff) <= 2 ) //找到了
				{
					break;
				}
				else
				{
					iTimeSeek += iDiff;
					LogDump( LOGMSG_LEVEL_PRINT,"iDiff=%d, iTimeSeek=%d, CurPCR=%llu, SeekPCR=%llu, SendPos=%llu, SeekRet=%llu" ,
							iDiff, iTimeSeek,CurPCR, SeekPCR, SendPos, seekRet);
					if (iTimeSeek < 0)
					{
						lseek(m_FileFd, 0, SEEK_SET);
						break;
					}
					else if (iTimeSeek > m_nDuration)
					{
						lseek(m_FileFd, 0, SEEK_END);
						break;
					}
					usleep(10000);
				}
			}
		}
	}

	return iRet;

}



int CEventTimeShiftInfo::Read(unsigned char* pBuff,int Size,uint64_t &PCR,unsigned char &VideoCounter,unsigned char &AudioCounter,unsigned char &PCRCounter)
{
    PCR = 0;
	int re = -1;
	//re = fread(pBuff, Size, 1, m_pFile);
	//re = Size*re;  //fread 返回读取的块数 ，与read不同 。
	re = read(m_FileFd, pBuff, Size);
	unsigned char* ptemp = pBuff;
	unsigned char* pStart = pBuff;
	while( ptemp < pBuff + re )
	{
		if( *(ptemp ++) != 0x47 )
			return -2;
		unsigned int Pid = 0;
		Pid = ( ptemp[0] & 0x1F ) << 8 | ptemp[1];
		if( ptemp[2]  & 0x20 && Pid == m_iFilePCRPID )
		{//AdaptionFile
		    uint8_t AdaptFileLen = ptemp[3];
		    uint8_t *Ada_Data = ptemp + 4;
		    uint8_t Flags = *(Ada_Data++);
		    if( Flags & 0x10 && AdaptFileLen > 0 )
		    {
		        uint64_t PCR_Base = (( uint64_t(Ada_Data[0]) << 25) | (Ada_Data[1] << 17) | (Ada_Data[2] << 9) | Ada_Data[3] << 1);
		        Ada_Data += 4;
		        PCR_Base |= (*Ada_Data & 0x80) >> 7;
		        uint64_t PCR_Ext = ( *( Ada_Data ++ ) & 0x01 ) << 8;
		        PCR_Ext |= *( Ada_Data++ );
		        //printf("%llx %llx \n",PCR_Base,PCR_Ext);
		        PCR = PCR_Base * 300 + PCR_Ext;
		        if( PCR < 0 )
		        {
		        	PCR = 0;
		        }
            }
		}
		if( Pid == 0 || Pid == 0x11 || Pid == 0x10 || Pid == 0x12 || Pid == 0x14)
		{//PAT��ֱ����Ϊ�հ�
#ifndef TOFILE
			ptemp[0] = 0x1F;
			ptemp[1] = 0xFF;
			ptemp[2] = 0x10;
#else
			unsigned char *pStart = ptemp + 4;
			ptemp += 14;
			*(ptemp ++) = ( ( m_iPMTPID & 0x00001F00 ) >> 8 ) | 0xE0;
			*(ptemp ++) = m_iPMTPID & 0x000000FF;
			memset(ptemp,0x00,4);
			unsigned int CRC32 = crc32(pStart,ptemp - pStart);
			ptemp[0] = ( CRC32 & 0xFF000000 ) >> 24;
			ptemp[1] = ( CRC32 & 0x00FF0000 ) >> 16;
			ptemp[2] = ( CRC32 & 0x0000FF00 ) >> 8;
			ptemp[3] =   CRC32 & 0x000000FF;
#endif
		}
		else if( Pid == m_iFilePMTPID )
		{//PMT���Ƚ��鷳
#if 1
			ptemp[0] = 0x1F;
			ptemp[1] = 0xFF;
			ptemp[2] = 0x10;
#else
			ptemp[0] = ( ptemp[0] & 0xE0 ) | ( (m_iFilePMTPID & 0x0000FF00) >> 8 );
			ptemp++;
			*(ptemp ++) = m_iPMTPID & 0x000000FF;
			if( (ptemp[0] & 0x20) >> 5 )
			{//adaption_file
				ptemp++;
				ptemp += ptemp[0];//����
				ptemp ++;
			}
			else
				ptemp++;
			unsigned char* pSectionStart = ptemp + 1;
#ifndef TOFILE
			ptemp += 4;
			*(ptemp++) = ( m_iService_ID & 0x0000FF00) >> 8;
			*(ptemp++) = m_iService_ID & 0x000000FF;
			ptemp += 3;
#else
			ptemp += 9;
#endif
			//PCRPID�ֶ�
			if( m_iFilePCRPID != m_iFileVideoPID )
			{
				*(ptemp ++) = ( ( m_iPCRPID & 0x00001F00 ) >> 8 ) | 0xE0;
				*(ptemp ++) = m_iPCRPID & 0x000000FF;
			}Read
			else
			{
				*(ptemp ++) = ( ( m_iVideoPID & 0x00001F00 ) >> 8) | 0xE0;
				*(ptemp ++) = m_iVideoPID & 0x000000FF;
			}

			unsigned short Len = (( ptemp[0] & 0x0F ) << 8) | ptemp[1];
			ptemp += Len + 2;
			for(int i = 0;i < 2 ;i++)
			{
				ptemp ++;
				if( ( ( (ptemp[0] & 0x1F) << 8 ) | ptemp[1] ) == m_iFileAudioPID )
				{
					*(ptemp++) = (( m_iAudioPID & 0x00001F00 ) >> 8) | 0xE0;
					*(ptemp++) = ( m_iAudioPID & 0x000000FF );
				}
				if( ( ( (ptemp[0] & 0x1F) << 8 ) | ptemp[1] ) == m_iFileVideoPID )
				{
					*(ptemp++) = (( m_iVideoPID & 0x00001F00 ) >> 8) | 0xE0;
					*(ptemp++) = ( m_iVideoPID & 0x000000FF );
				}
				unsigned short Len = (( ptemp[0] & 0x0F ) << 8) | ptemp[1];
				ptemp += Len + 2;
			}
			memset(ptemp,0x00,4);
			unsigned int CRC32 = crc32(pSectionStart,ptemp - pSectionStart);
			ptemp[0] = ( CRC32 & 0xFF000000 ) >> 24;
			ptemp[1] = ( CRC32 & 0x00FF0000 ) >> 16;
			ptemp[2] = ( CRC32 & 0x0000FF00 ) >> 8;
			ptemp[3] =   CRC32 & 0x000000FF;
#endif
		}
		else if( Pid == m_iFilePCRPID && m_iFileVideoPID != m_iFilePCRPID )
		{
			ptemp[0] = ( ptemp[0] & 0xE0 ) | ( (m_iPCRPID & 0x0000FF00) >> 8 );
			ptemp++;
			*(ptemp ++) = m_iPCRPID & 0x000000FF;
			*(ptemp) = ( *(ptemp) & 0xF0 ) | ( PCRCounter++ & 0x0F );
			//LogDump(LOGMSG_LEVEL_DEBUG,"the Pid is %x",Pid);
		}
		else if( Pid == m_iFileAudioPID )
		{
			ptemp[0] = ( ptemp[0] & 0xE0 ) | ( (m_iAudioPID & 0x0000FF00) >> 8 );
			ptemp++;
			*(ptemp ++) = m_iAudioPID & 0x000000FF;
			*(ptemp) = ( *(ptemp) & 0xF0 ) | ( AudioCounter++ & 0x0F );
		}
		else if( Pid == m_iFileVideoPID )
		{
			ptemp[0] = ( ptemp[0] & 0xE0 ) | ( (m_iVideoPID & 0x0000FF00) >> 8 );
			ptemp++;
			*(ptemp ++) = m_iVideoPID & 0x000000FF;
			*(ptemp) = ( *(ptemp) & 0xF0 ) | ( VideoCounter++ & 0x0F );
			//LogDump(LOGMSG_LEVEL_DEBUG,"the Pid is %x",Pid);
		}
		else if( Pid != 0x1FFF )
		{

		}
		pStart += 188;
		ptemp = pStart;
	}

	return re;
}

/****************************************************************************************************************************************/

CNVODServer::CNVODServer()
{
	m_ChannelID = -1;
	m_iReadPipe = -1;
	m_iSendNum  = 0;
	m_PaddingCounter = 0;
}

CNVODServer::~CNVODServer()
{
}

extern int g_ChannelID ;

int CNVODServer::Init( int Id,int ReadPipe,int WritePipe,void *pData )
{
	m_pDataBase = new CNVODDataBase();
	m_iReadPipe = ReadPipe;
    LogDump(LOGMSG_LEVEL_PRINT,"Channel %d init in Pid:%d",Id,getpid());
	SChannelInfo *pChannelInfo= new SChannelInfo();
	m_pChannelInfo = pChannelInfo;

	m_SockFd = socket(AF_INET, SOCK_DGRAM, 0);

	cConfig *pConfig = NULL;
    GetConfig(&pConfig);
    if(pConfig == NULL)
    {//�õ������ļ�
        LogDump(LOGMSG_LEVEL_ERROR,"GetConfig Failed");
        return ERROR_CONFIG_INVALID;
    }
/*
#ifdef __WIN32
    m_BuffThread = CreateThread(NULL,0,BufferFiles,this),0,NULL);
    if( m_BuffThread == NULL )
#else
    if(pthread_create(&m_BuffThread,NULL,BufferFiles,this) != 0)
#endif
    {
        LogDump(LOGMSG_LEVEL_ERROR,"CreateThread BufferFiles Failed");
        return -1;
    }
*/
	if(m_pDataBase->Init(pConfig->GetValue("DataBaseHost"),pConfig->GetValue("DataBaseUser"),
                       pConfig->GetValue("DataBasePasswd"),pConfig->GetValue("DataBaseName")) < 0 )
	{
		LogDump(LOGMSG_LEVEL_ERROR,"Connect Database Failed, Host=%s,User=%s,passwd=%s,dbname=%s",
				pConfig->GetValue("DataBaseHost"),pConfig->GetValue("DataBaseUser"),
				                       pConfig->GetValue("DataBasePasswd"),pConfig->GetValue("DataBaseName"));
		return -1;
	}
	g_ChannelID = m_ChannelID = Id;

	//litao 20110913 added  初始化event表
	if(((CNVODDataBase* )m_pDataBase)->ResetEventFlags( m_ChannelID ) != 0 )
	{
		LogDump(LOGMSG_LEVEL_ERROR, "ResetEventFlags Failed");
		return -1;
	}

	((CNVODDataBase* )m_pDataBase)->GetChannelInformation(m_ChannelID,*pChannelInfo);
//printf("Channel %d Service Size %d\n",Id,pChannelInfo->m_ServerVector.size());
	for(std::vector<CServiceInfo *>::iterator itr = pChannelInfo->m_ServerVector.begin();itr != pChannelInfo->m_ServerVector.end();itr++)
	{
		sleep(1);  //litao added 20111007 每秒启动一个
		int RunStatue = -1;
		(*itr)->m_pFunObj = (void *)this;
		(*itr)->m_strDstIp = pChannelInfo->m_strChannel_DstIP;
		(*itr)->m_iDstPort = pChannelInfo->m_iChannel_DstPort;
		(*itr)->m_SockFd   = m_SockFd;
		if( (RunStatue = (*itr)->Init() ) == 1 )
		{
#ifdef __WIN32//�������߳�
			(*itr)->m_ThreadID = CreateThread(NULL,0,(*itr)->Run,(*itr),0,NULL);
			if( (*itr)->m_ThreadID == NULL )
				RunStatue = 3;
#else
			if(pthread_create(&((*itr)->m_ThreadID),NULL,(*itr)->Run,(*itr)) != 0)
				RunStatue = 3;
#endif
		}
		else
		{
			LogDump(LOGMSG_LEVEL_ERROR,"TimeshiftService Init Failed");
			return -1;
		}

		if( RunStatue != 1 ) //litao added20110910  创建线程失败的时候，直接返回-1，让进程结束。
		{
			LogDump(LOGMSG_LEVEL_ERROR,"CreateThread TimeshiftService.Run Failed");
			return -1;
		}
//		printf("%d Run Stuta %d Start\n",(*itr)->m_iServiceID,RunStatue);
		((CNVODDataBase* )m_pDataBase)->UpDateService((*itr)->m_iServiceID,RunStatue);
//		printf("%d Run Stuta %d End\n",(*itr)->m_iServiceID,RunStatue);

	}
	//���°汾�š�

	return 0;
}

void CNVODServer::UnInit()
{

	SChannelInfo *pChannelInfo = (SChannelInfo *) m_pChannelInfo;
	for(std::vector<CServiceInfo *>::iterator itr = pChannelInfo->m_ServerVector.begin();itr != pChannelInfo->m_ServerVector.end();itr++)
	{
		((CNVODDataBase* )m_pDataBase)->UpDateService((*itr)->m_iServiceID,0);
	}
	SAFEDELETE(pChannelInfo);
	pChannelInfo = 0;

	SAFEDELETE(m_pDataBase);
}

#ifdef __WIN32
DWORD WINAPI CNVODServer::BufferFiles( void *argc )
#else
void * CNVODServer::BufferFiles( void *argc )
#endif
{
	mysql_thread_init();
	int ReadSize = 0;
	CNVODServer *pServer = (CNVODServer *)(argc);

	cConfig *pConfig = NULL;
    GetConfig(&pConfig);
    if(pConfig == NULL)
    {//�õ������ļ�
        printf("GetConfig Failed");
        return NULL;
    }

    while( !pServer->m_iIsQuit )
    {
        SEventRefInfo *pEventRefInfo = NULL;
        pServer->GetData( (void **)&pEventRefInfo ); //CNVODServer的GetData, 查看CNVODServer::Run();
        std::string SrcFile,DstFile,TempFile;
        SrcFile += pConfig->GetValue("SrcFilePath") ;
        DstFile += pConfig->GetValue("LocalFilePath");
        SrcFile += pEventRefInfo->m_FileName;
        DstFile += pEventRefInfo->m_FileName;
        TempFile = DstFile + ".cop";
//printf("SrcFile is %s\n DstFile is %s\n",SrcFile.c_str(),DstFile.c_str());

        int SrcFileFd = -1,DstFileFd = -1;

        SrcFileFd = open(SrcFile.c_str(),O_BINARY | O_RDONLY | O_LARGEFILE );
	
        if( SrcFileFd < 0 )
        {//NAS�ϵ�Դ�ļ�������
            pEventRefInfo->m_Flags = 9;
            goto _EXIT;
        }
        DstFileFd = open(DstFile.c_str(),O_BINARY | O_CREAT | O_EXCL | O_RDWR | O_LARGEFILE,S_IREAD | S_IWRITE);
        if( DstFileFd < 0 )
        {
            if( errno == EEXIST )
            {
            	//检查是否有cop文件，有 cop文件表示上次缓冲没有完成。
            	int TempFileFd = open(TempFile.c_str(), O_RDONLY);
            	if( TempFileFd != -1 ) //标识文件存在 ，说明目标文件可能不正确
            	{
					close(TempFileFd);
					char buffer[2048];
					::memset(buffer, 0, sizeof(buffer));
					sprintf(buffer, "rm -rf %s && rm -rf %s ", DstFile.c_str(), TempFile.c_str());
					system(buffer);
					pEventRefInfo->m_Flags = 0;  //flag置0， 下次继续buffer
            	}
            	else
            	{
            		pEventRefInfo->m_Flags = 2;
            	}
            }
            else
            {//Ŀ���ļ���������
                pEventRefInfo->m_Flags = 8;
            }
            goto _EXIT;
        }

        //创建临时文件
        char buffer[2048];
        ::memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "touch %s", TempFile.c_str());
        system(buffer);

        unsigned char pBuff[2048];
        while( (ReadSize = read(SrcFileFd,pBuff,2048)) > 0 )
        {
            int re = write(DstFileFd,pBuff,ReadSize);
            if( re != ReadSize )
            {//д������
                pEventRefInfo->m_Flags = 7;
                goto _EXIT;
            }
        }

        if( ReadSize == 0 )
        {
        	::memset(buffer, 0, sizeof(buffer));
			sprintf(buffer, "rm -rf %s", TempFile.c_str());
			system(buffer);
        	pEventRefInfo->m_Flags = 2;
        }
        else //读错误
        {
        	pEventRefInfo->m_Flags = 6;
        }

_EXIT:
        SAFECLOSE( SrcFileFd );
        SAFECLOSE( DstFileFd );
        DstFile.clear();
        SrcFile.clear();
        TempFile.clear();

        // 无论如何 ，必须要执行Event_Type_CopyOver， 因为开始发送启动拷贝的指令前，将bIsCopyFile设置为了true，只有bIsCopyFile==false时才会
        // 再次发起拷贝， 只有收到Event_Type_CopyOver时才会把bIsCopyFile设置为了false
        pServer->PushEvent(Event_Type_CopyOver, pEventRefInfo);
    }

    mysql_thread_end();

#ifdef __WIN32
    ExitThread( 0 );
#else
	return NULL;
#endif
}

int CNVODServer::Run()
{
//printf("NVODServer Run\n");
	SChannelInfo *pChannelInfo = (SChannelInfo *) m_pChannelInfo;
	CNVODDataBase* pDataBase = (CNVODDataBase* )m_pDataBase;
	fd_set ReadSet;
	struct timeval tv;
	unsigned long long LastSendTime = GetSysTick();
	unsigned long long CurTime = GetSysTick();

	unsigned long long LastSendTime_Ref = GetSysTick();
	unsigned long long CurTime_Ref = GetSysTick();

	struct sockaddr_in SendAddr;
    memset(&SendAddr,0x00,sizeof(SendAddr));
    SendAddr.sin_family = AF_INET;
    SendAddr.sin_port = htons(pChannelInfo->m_iChannel_DstPort);
	SendAddr.sin_addr.s_addr = inet_addr(pChannelInfo->m_strChannel_DstIP.c_str());

	//memset(&tv,0x00,sizeof(tv));
	tv.tv_sec = 1;
	tv.tv_usec = 50;
	ServiceWork* pServiceWork = 0x00;
	bool bIsCopyFile = false;
	while( !m_iIsQuit )
	{
		void *pData = 0x00;
		FD_ZERO(&ReadSet);
		FD_SET(m_iReadPipe,&ReadSet);

		int re = select(m_iReadPipe + 1,&ReadSet,NULL,NULL,&tv);
//LogDump(LOGMSG_LEVEL_DEBUG,"select return %d",re);
		if( re < 0 )
		{//select Error
			printf("Select Error\n");
		}
		else if( re == 0 )
		{	//timeout
			Event_Type Event = Event_Type_None;
			if( GetEvent(Event,&pData) != 0 )
			{
				if( pServiceWork )
				{
					pServiceWork->UnInit();
					delete pServiceWork;
					pServiceWork = 0x00;
				}
				pServiceWork = Server::CreateWork(Event,pData);
			}
			if( pServiceWork )
			{
				pServiceWork->Work(m_pDataBase,this,m_pChannelInfo);
				pServiceWork->UnInit();
				delete pServiceWork;
				pServiceWork = 0x00;
				if( Event == Event_Type_CopyOver )
				{
					bIsCopyFile = false;
				}
			}

/*
			CurTime_Ref = GetSysTick();
			if( CurTime_Ref - LastSendTime_Ref >= 1000000 && !bIsCopyFile )
			{
				//发起拷贝bufferfile
				std::vector<SEventRefInfo* > EventVector;
				pDataBase->GetRefEventInfo(m_ChannelID, EventVector);
				//printf("The Channel %d RefEventInfo is %d\n",m_ChannelID,EventVector.size());
				for(std::vector<SEventRefInfo *>::iterator itr = EventVector.begin();itr != EventVector.end();itr++)
				{
					bIsCopyFile = true;
					PushData( *(itr) );
					pDataBase->UpDateEventFlags( (*itr)->m_iEvent_ID, 1 );
					break; //一次只发起一个拷贝，
				}
				EventVector.clear();
				LastSendTime_Ref = GetSysTick();
			}
*/
			CurTime = GetSysTick();
			if( CurTime - LastSendTime >= 1000000 )
			{
				//检查时间，开始发送
				for(std::vector<CServiceInfo *>::iterator itr = pChannelInfo->m_ServerVector.begin();itr != pChannelInfo->m_ServerVector.end();itr++)
				{//ÿ��ͨ�����Ҫ����һ���¼�����Сʱ���Ƿ�����Ҫ�������¼�
					std::vector<CEventTimeShiftInfo *> EventVector;
					pDataBase->GetEventInfo( (*itr)->m_iServiceID, EventVector );
					if( EventVector.size() >1 )
					{
						LogDump(LOGMSG_LEVEL_PRINT, "warning: %d GetEventInfo Size %d\n", (*itr)->m_iServiceID, EventVector.size() );
					}
					// 这里获取的 EventVector.size() 应该是1 ，不然就是编排错误了，因为每个Service同一个时刻只有一个播放。
					for(std::vector<CEventTimeShiftInfo *>::iterator itrEvent = EventVector.begin();itrEvent != EventVector.end();)
					{
						int re = (*itrEvent)->Init((*itr)->m_iPMTPID,(*itr)->m_iPCRPID,(*itr)->m_iAudioPid,(*itr)->m_iVideoPid);
						(*itrEvent)->m_iService_ID = (*itr)->m_iServiceID;
						//pDataBase->UpDateEventFlags((*itrEvent)->m_iEvent_ID,re,1);
						if( re == 2 )
						{
							pDataBase->UpDateEventFlags((*itrEvent)->m_iEvent_ID,2,1);
							( (TimeShiftService*)(*itr) )->PushEvent(Event_Type_Buffer,(*itrEvent));
							itrEvent = EventVector.erase(itrEvent);
							//每次只发起一个buffer，
							break;
						}
						else
						{
							LogDump(LOGMSG_LEVEL_PRINT,"Event %d Buffer Init Failed , code %d , reset flag to 1 ",(*itrEvent)->m_iEvent_ID, re );
							//打开文件失败时
							pDataBase->UpDateEventFlags((*itrEvent)->m_iEvent_ID,1,1); //设置成1， 下次才会再次尝试缓冲文件。
							(*itrEvent)->UnInit();
							SAFEDELETE( (*itrEvent) );
							itrEvent = EventVector.erase(itrEvent);
						}
						SLEEP(5000);
					}
					for(std::vector<CEventTimeShiftInfo *>::iterator itrEvent = EventVector.begin();itrEvent != EventVector.end();itrEvent++)
					{
						SAFEDELETE( (*itrEvent) );
					}
					EventVector.clear();

					SLEEP(5000);
				}
				LastSendTime = GetSysTick();
			}
#if 0
			if( GetSendNum() == 0 /*&& (CurTime < LastSendTime)*/)
			{
                unsigned char pBuff[TSSENDPACKSIZE];
                memset(pBuff,0xFF,TSSENDPACKSIZE);
                int re = BuildPaddingPack(pBuff,TSSENDPACKSIZE);
                re = sendto(m_SockFd,(const char *)pBuff,TSSENDPACKSIZE,0,(sockaddr *)&SendAddr,sizeof(SendAddr));
			    //LastSendTime = CurTime;
			}
			//LogDump(LOGMSG_LEVEL_DEBUG,"Recv a Cmd %d\n",re);
#endif
		}
		else
		{//readPipe can read
			if( FD_ISSET( m_iReadPipe,&ReadSet) )
			{
				int Cmd = 0;
				re = read(m_iReadPipe,&Cmd,4);
				if(re <= 0)
				{
					LogDump(LOGMSG_LEVEL_WARING,"ReadPipe Failed");
				}
				LogDump(LOGMSG_LEVEL_PRINT,"Recv a Cmd %d",Cmd);
				switch(Cmd)
				{
					case 0xA2:
						pDataBase->UpDataChannelService(m_ChannelID,0);
						exit(m_ChannelID);
						break;
					default:
						LogDump(LOGMSG_LEVEL_WARING,"UnKonw Cmd %d",Cmd);
						break;
				}
			}
		}
		SLEEP(1000);
	}
	return 0;
}

int CNVODServer::BuildPaddingPack(unsigned char *pBuff,int Size)
{
    int re = 0;
    unsigned char *ptemp = pBuff;

    while(ptemp < pBuff + Size)
    {
        *(ptemp ++) = 0x47;
        *(ptemp ++) = ( (PADDING_PID & 0xFF00) >> 8 ) | 0x40;
        *(ptemp ++) = PADDING_PID & 0x00FF;
        *(ptemp ++) = (m_PaddingCounter ++) & 0x0F;

        ptemp += 184;
    }

    return re;
}

int CNVODServer::ServiceExit( int ServiceID )
{
	SChannelInfo *pChannelInfo = (SChannelInfo *) m_pChannelInfo;
	for(std::vector<CServiceInfo *>::iterator itr = pChannelInfo->m_ServerVector.begin();itr != pChannelInfo->m_ServerVector.end();)
	{
		if( (*itr)->m_iServiceID == ServiceID )
		{
			SAFEDELETE( (*itr) );
			itr = pChannelInfo->m_ServerVector.erase( itr );
		}
		else
			itr++;
	}

	return 0;
}
#endif //#ifdef StudyNvod