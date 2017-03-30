#include "TmssPlayer.h"

/* Foundation */
#include "BaseType.h"
#include "SystemInclude.h"
#include "PacketHelper.h"
#include "Debug.h"

/* Entity */
#include "Entities.h"

/* Functions */
#include "TransportPacketInterface.h"
#include "TmssPlayerCookie.h"

using namespace std;
using namespace std::chrono;

typedef ClassFactoriesRegistor<PlayerInterface, string, shared_ptr<TmssEntity>> Registor;
RegisterClassFactory(Registor, reg, "TmssPlayer", TmssPlayer::CreateInstance);

static set<uint8_t> audioStreamTypes = { 0x03, 0x04, 0x0F, 0x11, 0x81 };
static set<uint8_t> videoStreamTypes = { 0x01, 0x02, 0x10, 0x1B, 0x80 };

/**********************class TmssPlayer**********************/
/* public functions */
TmssPlayer::TmssPlayer(shared_ptr<TmssEntity> tmss)
    : mtx(), cv(), cookie(tmss)
{}

TmssPlayer::~TmssPlayer()
{
    if (theMovieThread.joinable())
    {
        /* thread exited because of no more movie (not calling of Stop()),
           join the thread.
        */
        theMovieThread.join();
    }
}

void TmssPlayer::Start()
{
    theMovieThread = thread(bind(&TmssPlayer::TheMovieThreadMain, this));

    std::lock_guard<std::mutex> lock(mtx);
    isOnGoing = true;
    cv.notify_one();
}

void TmssPlayer::Stop()
{
    {
        std::lock_guard<std::mutex> lock(mtx);
        isOnGoing = false;
        cv.notify_one();
    }
    theMovieThread.join();
}

bool TmssPlayer::IsRunning()
{
    lock_guard<mutex> lock(mtx);
    return isOnGoing;
}

/* private functions */
void TmssPlayer::TheMovieThreadMain()
{	
	socketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	PrepareMovie();
	Duration duration = Duration::zero();
    while (true)
    {
        cout << *cookie.GetTmss()->GetServiceId() << ", "
            << duration_cast<Milliseconds>(duration).count() << endl;
        std::unique_lock<std::mutex> lock(mtx);
		if (cv.wait_for(lock, duration, [this]{return !isOnGoing; }))
            break;

		duration = Play();
    }

    closesocket(socketFd);
}

void TmssPlayer::PrepareMovie()
{
    if (fstm.is_open())
    {
        fstm.close();
    }

    TimePoint now = steady_clock::now();    
    shared_ptr<TmssEventEntity> tmssEvent = cookie.GetCurTmssEvent();
    while (tmssEvent != nullptr)
    {
        if (*tmssEvent->GetStartTimePoint() + *tmssEvent->GetDuration() > now)
        {
            shared_ptr<MovieEntity> movie = cookie.GetCurMovie();
            if (movie != nullptr)
            {
				shared_ptr<MovieRuntimeInfoEntity> movieRuntimeInfo = cookie.GetMovieRuntimeInfo();
                movieRuntimeInfo->SetStartTimePoint(*tmssEvent->GetStartTimePoint() + cookie.GetEventDuration());
                break;
            }            
        }

        cookie.ForwardTmssEvent();
		cookie.SetEventDuration(Duration::zero());
		cookie.GetMovieRuntimeInfo()->SetStartTimePoint(*tmssEvent->GetStartTimePoint());
        tmssEvent = cookie.GetCurTmssEvent();
    }

    if (tmssEvent == nullptr)
    {
        assert(cookie.GetCurMovie() == nullptr);
		isOnGoing = false;
		return;
	}
	
	fstm.open(cookie.GetCurMovie()->GetLocalPath()->c_str(), ios::binary);
}

/* refer to iso13818-1.pdf
: 2.4.3.2 Transport Stream packet layer
: 2.5.2.2 Input to the Program Stream system target decoder 
: D.0.3 System Time Clock recovery in the decoder
: D.0.4 SCR and PCR Jitter
: Table 2-6 – Transport Stream adaptation field (for packet format)
*/
Duration TmssPlayer::Play()
{    	
    shared_ptr<MovieRuntimeInfoEntity> movieRuntimeInfo = cookie.GetMovieRuntimeInfo();	
    if (movieRuntimeInfo->GetBufferHead() != movieRuntimeInfo->GetPcrPacketPtr())
    {
		Send((char*)movieRuntimeInfo->GetBufferHead(),
			movieRuntimeInfo->GetPcrPacketPtr() - movieRuntimeInfo->GetBufferHead());
        movieRuntimeInfo->SetPcrPacketPtr(movieRuntimeInfo->GetBufferHead());
    }    
	
	Duration duration = Duration::zero();
	for (uchar_t *ptr = movieRuntimeInfo->GetBufferHead();
        ptr < movieRuntimeInfo->GetBufferHead() + movieRuntimeInfo->GetBufferSize();
         ptr = ptr + TsPacketSize)
    {
        fstm.read((char*)ptr, TsPacketSize);

		if (fstm.eof())
        {
            Pcr first = *movieRuntimeInfo->GetFirstPcr();
            Pcr last = *movieRuntimeInfo->GetLastPcr();
            cookie.SetEventDuration(cookie.GetEventDuration() + CalcTimeIntv(first, last));
			Send((char*)movieRuntimeInfo->GetBufferHead(), ptr + fstm.gcount() - movieRuntimeInfo->GetBufferHead());

			cookie.ForwardMovie();
			PrepareMovie();
			break;
		}
		assert(fstm.gcount() == TsPacketSize);

        shared_ptr<TransportPacketHelperInterface> tsPacket(TransportPacketHelperInterface::CreateInstance(ptr));
        if (tsPacket->GetSyncByte() != 0x47)
        {
            assert(false);

			cookie.ForwardMovie();
			PrepareMovie();
            break;
        }
        
        Pid pid = tsPacket->GetPid();
        if (pid == PatPid)
        {
            /* Pat Table */
            shared_ptr<PatHelperInterface> patTable(PatHelperInterface::CreateInstance(tsPacket->GetPayLoadHeader()));
            uchar_t *p = patTable->GetHeader() + patTable->GetMinSize() - patTable->GetCrcCodeSize();
            while (p < ptr + patTable->GetSize())
            {
                shared_ptr<PatElementaryInterface> patElementary(PatElementaryInterface::CreateInstance(p));
                if (patElementary->GetProgramNumber() != 0)
                {
                    if (movieRuntimeInfo->GetPmtPid() == nullptr)
                        movieRuntimeInfo->SetPmtPid(patElementary->GetPmtPid());

					patElementary->SetProgramNumber(*cookie.GetTmss()->GetServiceId());
                    patElementary->SetPmtPid(*cookie.GetTmss()->GetPmtPid());
                    break;
                }
                p = p + patElementary->GetSize();
            }
            assert(movieRuntimeInfo->GetPmtPid() != nullptr);
            patTable->UpdateCrcCode();

			/* discard pat packet */
			ptr = ptr - TsPacketSize;			
        }

        if (movieRuntimeInfo->GetPmtPid() != nullptr && pid == *movieRuntimeInfo->GetPmtPid())
        {
            /* Pmt Table */
            shared_ptr<PmtHelperInterface> pmtTable(PmtHelperInterface::CreateInstance(tsPacket->GetPayLoadHeader()));
            if (movieRuntimeInfo->GetPcrPid() == nullptr)
            {
                movieRuntimeInfo->SetPcrPid(pmtTable->GetPcrPid());
            } 

            uint32_t flag = 0;
            uchar_t *p = pmtTable->GetHeader() + pmtTable->GetMinSize() 
                - pmtTable->GetCrcCodeSize() + pmtTable->GetProgramInfoLength();
            while (p < ptr + pmtTable->GetSize() && flag != 0x3)
            {
                shared_ptr<PmtElementaryInterface> pmtElementary(PmtElementaryInterface::CreateInstance(p));
                if (audioStreamTypes.find(pmtElementary->GetStreamType()) != audioStreamTypes.end())
                {
                    if (movieRuntimeInfo->GetAudioPid() == nullptr)
                        movieRuntimeInfo->SetAudioPid(pmtElementary->GetElementaryPid());
                    pmtElementary->SetElementaryPid(*cookie.GetTmss()->GetAudioPid());
                    flag = flag | 1;
                }

                if (videoStreamTypes.find(pmtElementary->GetStreamType()) != videoStreamTypes.end())
                {
                    if (movieRuntimeInfo->GetVideoPid() == nullptr)
                        movieRuntimeInfo->SetVideoPid(pmtElementary->GetElementaryPid());
                    pmtElementary->SetElementaryPid(*cookie.GetTmss()->GetVideoPid());
                    flag = flag | 2;
                }
                p = p + pmtElementary->GetSize();
            }
            // false means we failed to get the right audio or video pid.
            assert(flag == 0x3);
            
			pmtTable->SetProgramNumber(*cookie.GetTmss()->GetServiceId());
            pmtTable->SetPcrPid(*cookie.GetTmss()->GetPcrPid());
            pmtTable->UpdateCrcCode();
            tsPacket->SetPid(*cookie.GetTmss()->GetPmtPid());
        }

        if (movieRuntimeInfo->GetAudioPid() != nullptr && pid == *movieRuntimeInfo->GetAudioPid())
        { 
            tsPacket->SetPid(*cookie.GetTmss()->GetAudioPid());
        }

        if (movieRuntimeInfo->GetVideoPid() != nullptr && pid == *movieRuntimeInfo->GetVideoPid())
        {
            tsPacket->SetPid(*cookie.GetTmss()->GetVideoPid());
        }

        AdaptationFieldControl afc = tsPacket->GetAdaptationFieldControl();
        if (afc != 0x2 && afc != 0x3) 
        {
            //there is no pcr field if the adaptation_field_control not in {0x2, 0x3}.
            continue;
        }

        if (movieRuntimeInfo->GetPcrPid() != nullptr && pid == *movieRuntimeInfo->GetPcrPid())
        {
            tsPacket->SetPid(*cookie.GetTmss()->GetPcrPid());
            if (tsPacket->GetAdaptationFieldLength() == 0)
            {
                continue;
            }

            if (tsPacket->GetPcrFlag() != 1)
            {
                continue;
            }

            Pcr pcr = tsPacket->GetPcr();
            if (movieRuntimeInfo->GetFirstPcr() == 0)
			{
				movieRuntimeInfo->SetFirstPcr(pcr);
                movieRuntimeInfo->SetPcrPacketPtr(ptr + TsPacketSize);
            }
            else
            {
				movieRuntimeInfo->SetPcrPacketPtr(ptr + TsPacketSize);
                Pcr firstPcr = *movieRuntimeInfo->GetFirstPcr();
                TimePoint startTimePoint = *movieRuntimeInfo->GetStartTimePoint();
                duration = startTimePoint + CalcTimeIntv(firstPcr, pcr) - steady_clock::now();
            }
			movieRuntimeInfo->SetLastPcr(pcr);
			break;
        } /* if (pid == FilePcrPid) */
    } /* for (ptr = movie->GetBufferHead(); ...;  ptr = ptr + TsPacketSize) */
    
	/* buffer is not enought */
    assert(movieRuntimeInfo->GetPcrPacketPtr() < movieRuntimeInfo->GetBufferHead() + movieRuntimeInfo->GetBufferSize());
    return duration;
}

void TmssPlayer::Send(char *buffer, size_t size)
{
	struct sockaddr_in dest = *cookie.GetTs()->GetDstAddr();
    
#ifdef _DEBUG
    //ofstream ofstrm;    
    //while (!ofstrm.is_open())
    //{
    //    ofstrm.open("Movies\\output.ts", ios::binary | ios::app);        
    //    if (!ofstrm.good())
    //    {
    //        /* 如果频繁调用ofstream::wirte(), 可能会失败, errono == EACCES.
    //            strerror(errno) should return "Permission denied" */
    //        cout << "!!!" << endl;
    //        assert(errno == EACCES);
    //        this_thread::sleep_for(milliseconds(20));
    //        ofstrm.clear();
    //    }
    //}
    //ofstrm.write(buffer, size);
    //ofstrm.flush();  //避免频繁写文件造成的错误。
    //assert(ofstrm.good());
    //ofstrm.close();
#endif

	shared_ptr<MovieRuntimeInfoEntity> movieRuntimeInfo = cookie.GetMovieRuntimeInfo();
	Duration duration = CalcTimeIntv(*movieRuntimeInfo->GetFirstPcr(), *movieRuntimeInfo->GetLastPcr());
	if (*movieRuntimeInfo->GetStartTimePoint() + duration + Milliseconds(200) > steady_clock::now())
	{
		char* ptr = buffer;
		while (ptr < buffer + size)
		{
			size_t len = std::min(buffer + size - ptr, TsPacketBufSize);
			sendto(socketFd, ptr, (int)len, 0, (SOCKADDR *)&dest, sizeof(struct sockaddr_in));
			ptr = ptr + len;
		}
	}
}

Duration TmssPlayer::CalcTimeIntv(Pcr t0, Pcr t1)
{
	return Milliseconds((t1 - t0) / 27000);
}