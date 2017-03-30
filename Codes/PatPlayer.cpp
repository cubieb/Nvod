#include "PatPlayer.h"

/* Entity */
#include "Entities.h"

/* Functions */
#include "TransportPacketInterface.h"
#include "PacketHelper.h"

#include "PatPlayerCookie.h"

using namespace std;
using namespace std::chrono;

typedef ClassFactoriesRegistor<PlayerInterface, string, shared_ptr<GlobalCfgEntity>, shared_ptr<TsEntity>> Registor;
RegisterClassFactory(Registor, reg, "PatPlayer", PatPlayer::CreateInstance);

PatPlayer::PatPlayer(shared_ptr<GlobalCfgEntity> dataPipeGlobal, shared_ptr<TsEntity> ts)
	: cookie(dataPipeGlobal, ts)
{}

PatPlayer::~PatPlayer()
{}

void PatPlayer::Start()
{
    thePatThread = thread(bind(&PatPlayer::ThePatThreadMain, this));

    std::lock_guard<std::mutex> lock(mtx);
    isOnGoing = true;
    cv.notify_one();
}

void PatPlayer::Stop()
{
    {
        std::lock_guard<std::mutex> lock(mtx);
        isOnGoing = false;
        cv.notify_one();
    }
    thePatThread.join();
}

bool PatPlayer::IsRunning()
{
    lock_guard<mutex> lock(mtx);
    return isOnGoing;
}

/* private functions */
void PatPlayer::ThePatThreadMain()
{
    socketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    Duration duration = *cookie.GetGlobalCfg()->GetPatInterval();
    while (true)
    {
        std::unique_lock<std::mutex> lock(mtx);
        if (cv.wait_for(lock, duration, [this]{return !isOnGoing; }))
            break;

        HandlePatTimer();
    }

    closesocket(socketFd);
}

void PatPlayer::HandlePatTimer()
{
    size_t const bufSize = TsPacketSize * std::numeric_limits<DitSectionNumber>::max();
    uchar_t *buffer = new uchar_t[bufSize];
    uchar_t *bptr = buffer;

    shared_ptr<TransportPacketHelperInterface> tsHelper(TransportPacketHelperInterface::CreateInstance(bptr));
    tsHelper->SetSyncByte(0x47);
    tsHelper->SetTransportErrorIndicator(0);
    tsHelper->SetPayloadUnitStartIndicator(1);
    tsHelper->SetTransportPriority(0);
    tsHelper->SetPid(0);
    tsHelper->SetTransportScramblingControl(0);
    tsHelper->SetAdaptationFieldControl(1);
    tsHelper->SetContinuityCounter(cookie.GetNewPatContinuityCounter());

    shared_ptr<PatHelperInterface> patHelper(PatHelperInterface::CreateInstance(tsHelper->GetPayLoadHeader()));
    patHelper->SetSectionLength(patHelper->GetMinSectionLength());
    patHelper->SetTransportStreamId(*cookie.GetTs()->GetTsId());
    patHelper->SetVersionNumber(0);
    patHelper->SetCurrentNextIndicator(1);
    patHelper->SetSectionMumber(0);
    patHelper->SetLastSectionMumber(0);
    
    auto FillPat = [this, buffer, bufSize, &bptr, &tsHelper, &patHelper](ServiceId serviceId, Pid pmtPid) ->void
    {
        if (tsHelper->GetSize() + patHelper->GetSize() + 4 > TsPacketSize)
        {
            bptr = bptr + TsPacketSize;
            assert(bptr < buffer + bufSize);
            tsHelper.reset(TransportPacketHelperInterface::CreateInstance(bptr));
            tsHelper->SetSyncByte(0x47);
            tsHelper->SetTransportErrorIndicator(0);
            tsHelper->SetPayloadUnitStartIndicator(0);
            tsHelper->SetTransportPriority(0);
            tsHelper->SetPid(0);
            tsHelper->SetTransportScramblingControl(0);
            tsHelper->SetAdaptationFieldControl(1);
            tsHelper->SetContinuityCounter(cookie.GetNewPatContinuityCounter());

            patHelper.reset(PatHelperInterface::CreateInstance(tsHelper->GetPayLoadHeader()));
            patHelper->SetSectionLength(patHelper->GetMinSectionLength());
            patHelper->SetTransportStreamId(*cookie.GetTs()->GetTsId());
            patHelper->SetVersionNumber(0);
            patHelper->SetCurrentNextIndicator(1);
            patHelper->SetSectionMumber(0);
            patHelper->SetLastSectionMumber(0);
        }

        uchar_t *ptr = patHelper->GetHeader() + patHelper->GetSize() - patHelper->GetCrcCodeSize();
        shared_ptr<PatElementaryInterface> patElementary(PatElementaryInterface::CreateInstance(ptr));
        patElementary->SetProgramNumber(serviceId);
        patElementary->SetPmtPid(pmtPid);

        patHelper->SetSectionLength(patHelper->GetSectionLength() + patElementary->GetSize());
    };
	
    list<shared_ptr<RefsEntity>>& refses = cookie.GetTs()->GetRefses();
    list<shared_ptr<TmssEntity>>& tmsses = cookie.GetTs()->GetTmsses();
    for (auto iter = refses.begin(); iter != refses.end(); ++iter)
    {
		shared_ptr<PstsEntity> psts = (*iter)->GetPsts();
		FillPat(*psts->GetServiceId(), *psts->GetPmtPid());
    }

    for (auto iter = tmsses.begin(); iter != tmsses.end(); ++iter)
    {
        FillPat(*(*iter)->GetServiceId(), *(*iter)->GetPmtPid());
    }

    SectionNumber cur = 0;
    SectionNumber last = (bptr - buffer) / TsPacketSize;
    for (uchar_t *ptr = buffer; ptr <= bptr; ptr = ptr + TsPacketSize)
    {
        tsHelper.reset(TransportPacketHelperInterface::CreateInstance(ptr));

        patHelper.reset(PatHelperInterface::CreateInstance(tsHelper->GetPayLoadHeader()));
        patHelper->SetSectionMumber(cur++);
        patHelper->SetLastSectionMumber(last);
        patHelper->UpdateCrcCode();

        tsHelper->FillPad(patHelper->GetSize());
    }
    
    Send((char*)buffer, TsPacketSize);
    delete buffer;
}

void PatPlayer::Send(char *buffer, size_t size)
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

    char* ptr = buffer;
    while (ptr < buffer + size)
    {
        size_t len = std::min(buffer + size - ptr, TsPacketBufSize);
        sendto(socketFd, ptr, (int)len, 0, (SOCKADDR *)&dest, sizeof(struct sockaddr_in));
        ptr = ptr + len;
    }
}