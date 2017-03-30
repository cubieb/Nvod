#include "RefsPlayer.h"

/* Entity */
#include "Entities.h"

/* Functions */
#include "TransportPacketInterface.h"
#include "PacketHelper.h"


#include "RefsPlayerCookie.h"

using namespace std;
using namespace std::chrono;

typedef ClassFactoriesRegistor<PlayerInterface, string, shared_ptr<GlobalCfgEntity>, shared_ptr<RefsEntity>> Registor;
RegisterClassFactory(Registor, reg, "RefsPlayer", RefsPlayer::CreateInstance);

/**********************class RefsPlayer**********************/
RefsPlayer::RefsPlayer(shared_ptr<GlobalCfgEntity> dataPipeGlobal,  shared_ptr<RefsEntity> refs)
    : cookie(dataPipeGlobal, refs)
{}

RefsPlayer::~RefsPlayer()
{}

void RefsPlayer::Start()
{
    theDataPipeThread = thread(bind(&RefsPlayer::TheDataPipeThreadMain, this));

    std::lock_guard<std::mutex> lock(mtx);
    isOnGoing = true;
    cv.notify_one();
}

void RefsPlayer::Stop()
{
    {
        std::lock_guard<std::mutex> lock(mtx);
        isOnGoing = false;
        cv.notify_one();
    }
    theDataPipeThread.join();
}

bool RefsPlayer::IsRunning()
{
    lock_guard<mutex> lock(mtx);
    return isOnGoing;
}

/* private functions */
void RefsPlayer::TheDataPipeThreadMain()
{
    socketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    TimerHandler pmtHandler = bind(&RefsPlayer::HandlePmtTimer, this);
    TimerHandler posterHandler = bind(&RefsPlayer::HandlePosterTimer, this);

    shared_ptr<RefsRuntimeInfoEntity> rt = cookie.GetDataPipeRuntimeInfo();
    shared_ptr<GlobalCfgEntity> dpg = cookie.GetGlobalCfg();
    while (true)
    {
        std::unique_lock<std::mutex> lock(mtx);
        if (cv.wait_for(lock, Milliseconds(10), [this]{return !isOnGoing; }))
            break;
        
        rt->SetPmtInterval(HandlerTimer(rt->GetPmtInterval(), *dpg->GetPmtInterval(), pmtHandler));
        rt->SetPosterInterval(HandlerTimer(rt->GetPosterInterval(), *dpg->GetPosterInterval(), posterHandler));
    }    

    closesocket(socketFd);
}

Duration RefsPlayer::HandlerTimer(Duration invl, Duration initInvl, TimerHandler handler)
{
    if (invl < Milliseconds(10))
    {
        handler();
        invl = initInvl;
    }
    else
    {
        invl = invl - Milliseconds(10);
    }
    return invl;
}

void RefsPlayer::HandlePmtTimer()
{
    uchar_t buffer[TsPacketSize];
    shared_ptr<RefsEntity> refs = cookie.GetRefs();

    shared_ptr<TransportPacketHelperInterface> tsHelper(TransportPacketHelperInterface::CreateInstance(buffer));
    tsHelper->SetSyncByte(0x47);
    tsHelper->SetTransportErrorIndicator(0);
    tsHelper->SetPayloadUnitStartIndicator(1);
    tsHelper->SetTransportPriority(0);
    tsHelper->SetPid(*refs->GetPsts()->GetPmtPid());
    tsHelper->SetTransportScramblingControl(0);
    tsHelper->SetAdaptationFieldControl(1);
    tsHelper->SetContinuityCounter(cookie.GetDataPipeRuntimeInfo()->GetNewPmtContinuityCounter());

    shared_ptr<PmtHelperInterface> pmtHelper(PmtHelperInterface::CreateInstance(tsHelper->GetPayLoadHeader()));
    pmtHelper->SetSectionLength(pmtHelper->GetMinSectionLength());
	pmtHelper->SetProgramNumber(*refs->GetPsts()->GetServiceId());
    pmtHelper->SetVersionNumber(0);
    pmtHelper->SetCurrentNextIndicator(1);
    pmtHelper->SetSectionMumber(0);
    pmtHelper->SetLastSectionMumber(0);
    pmtHelper->SetPcrPid(0);
    pmtHelper->SetProgramInfoLength(0);

    uchar_t *ptr = pmtHelper->GetHeader() + pmtHelper->GetSize() - pmtHelper->GetCrcCodeSize();
    shared_ptr<PmtElementaryInterface> pmtElementary(PmtElementaryInterface::CreateInstance(ptr));
	pmtElementary->SetStreamType(*refs->GetPsts()->GetStreamType());
	pmtElementary->SetElementaryPid(*refs->GetPsts()->GetPosterPid());
    pmtElementary->SetEsInfoLength(0);

    pmtHelper->SetSectionLength(pmtHelper->GetSectionLength() + pmtElementary->GetSize());
    pmtHelper->UpdateCrcCode();

    tsHelper->FillPad(pmtHelper->GetSize());
    Send((char*)buffer, TsPacketSize);
}

void RefsPlayer::HandlePosterTimer()
{
    HandlePosterDitTimer();
    HandlePosterDdtTimer();
}

void RefsPlayer::HandlePosterDitTimer()
{
    auto WriteTs = [this](shared_ptr<TransportPacketHelperInterface> tsHelper,
        PayloadUnitStartIndicator payloadUnitStartIndicator)
    {
        shared_ptr<RefsEntity> refs = cookie.GetRefs();
        shared_ptr<RefsRuntimeInfoEntity> rtInfo = cookie.GetDataPipeRuntimeInfo();

        tsHelper->SetSyncByte(0x47);
        tsHelper->SetTransportErrorIndicator(0);
        tsHelper->SetPayloadUnitStartIndicator(payloadUnitStartIndicator);
        tsHelper->SetTransportPriority(0);
		tsHelper->SetPid(*refs->GetPsts()->GetPosterPid());
        tsHelper->SetTransportScramblingControl(0);
        tsHelper->SetAdaptationFieldControl(1);
        tsHelper->SetContinuityCounter(rtInfo->GetNewPosterContinuityCounter());
    };

    auto WriteDit = [](shared_ptr<DitHelperInterface> ditHelper)
    {
        ditHelper->SetSectionLength(ditHelper->GetMinSectionLength());
        ditHelper->SetVersionNumber(0);
        ditHelper->SetSectionMumber(0);
        ditHelper->SetLastSectionMumber(0);
    };

    size_t const bufSize = TsPacketSize * std::numeric_limits<DitSectionNumber>::max();
    uchar_t *buffer = new uchar_t[bufSize];
    uchar_t *bptr = buffer;
    
    shared_ptr<TransportPacketHelperInterface> tsHelper(TransportPacketHelperInterface::CreateInstance(bptr));
    WriteTs(tsHelper, 1);
    shared_ptr<DitHelperInterface> ditHelper(DitHelperInterface::CreateInstance(tsHelper->GetPayLoadHeader()));    
    WriteDit(ditHelper);

    shared_ptr<list<shared_ptr<PosterViewEntity>>::iterator> posterIter;
	shared_ptr<list<EventId>::iterator> refsEventIdIter;

    list<shared_ptr<PosterViewEntity>>& posters = cookie.GetPosterViews();
    for (auto iter1 = posters.begin(); iter1 != posters.end(); ++iter1)
    {
        if (posterIter != nullptr)
        {
            iter1 = *posterIter;
            posterIter.reset();
        }
        if (tsHelper->GetSize() + ditHelper->GetSize() + 6 > TsPacketSize)
        {
            bptr = bptr + TsPacketSize;
            assert(bptr < buffer + bufSize);
            tsHelper.reset(TransportPacketHelperInterface::CreateInstance(bptr));
            WriteTs(tsHelper, 0);
            ditHelper.reset(DitHelperInterface::CreateInstance(tsHelper->GetPayLoadHeader()));
            WriteDit(ditHelper);
        }
		
        uchar_t *ptr = ditHelper->GetHeader() + ditHelper->GetSize() - ditHelper->GetCrcCodeSize();
        shared_ptr<DitElementaryInterface> ditElementary(DitElementaryInterface::CreateInstance(ptr));
        ditElementary->SetFileId((*iter1)->posterId);
        ditElementary->SetEventLoopLength(0);        

		list<EventId>& refsEventIds = (*iter1)->refsEventIds;
		for (auto iter2 = refsEventIds.begin(); iter2 != refsEventIds.end(); ++iter2)
        {
            if (tsHelper->GetSize() + ditHelper->GetSize() + ditElementary->GetSize() + 2 > TsPacketSize)
            {
                posterIter = make_shared<list<shared_ptr<PosterViewEntity>>::iterator>(iter1);
				refsEventIdIter = make_shared<list<EventId>::iterator>(iter2);
                break;
            }

			if (refsEventIdIter != nullptr)
            {
				iter2 = *refsEventIdIter;
				refsEventIdIter.reset();
            }
            WriteBuffer((uint16_t*)(ditElementary->GetHeader() + ditElementary->GetSize()), *iter2);
            ditElementary->SetEventLoopLength(ditElementary->GetEventLoopLength() + sizeof(EventId));
        }
        ditHelper->SetSectionLength(ditHelper->GetSectionLength() + ditElementary->GetSize());
    }
    
    DitSectionNumber cur = 0;
    DitSectionNumber last = (bptr - buffer) / TsPacketSize;
    for (uchar_t *ptr = buffer; ptr <= bptr; ptr = ptr + TsPacketSize)
    {
        tsHelper.reset(TransportPacketHelperInterface::CreateInstance(ptr));
        ditHelper.reset(DitHelperInterface::CreateInstance(tsHelper->GetPayLoadHeader()));
        ditHelper->SetSectionMumber(cur++);
        ditHelper->SetLastSectionMumber(last);
        ditHelper->UpdateCrcCode();

        tsHelper->FillPad(ditHelper->GetSize());
    }

    Send((char*)buffer, bptr + TsPacketSize - buffer);
    delete[] buffer;
}

void RefsPlayer::HandlePosterDdtTimer()
{
    auto WriteTs = [this](shared_ptr<TransportPacketHelperInterface> tsHelper,
        PayloadUnitStartIndicator payloadUnitStartIndicator)
    {
        shared_ptr<RefsEntity> refs = cookie.GetRefs();
        shared_ptr<RefsRuntimeInfoEntity> rtInfo = cookie.GetDataPipeRuntimeInfo();

        tsHelper->SetSyncByte(0x47);
        tsHelper->SetTransportErrorIndicator(0);
        tsHelper->SetPayloadUnitStartIndicator(payloadUnitStartIndicator);
        tsHelper->SetTransportPriority(0);
        tsHelper->SetPid(*refs->GetPsts()->GetPosterPid());
        tsHelper->SetTransportScramblingControl(0);
        tsHelper->SetAdaptationFieldControl(1);
        tsHelper->SetContinuityCounter(rtInfo->GetNewPosterContinuityCounter());
    };

    auto WriteDdt = [](shared_ptr<DdtHelperInterface> ddtHelper, PosterId posterId)
    {
        ddtHelper->SetSectionLength(ddtHelper->GetMinSectionLength());
        ddtHelper->SetPosterId(posterId);
        ddtHelper->SetVersionNumber(0);
        ddtHelper->SetSectionMumber(0);
        ddtHelper->SetLastSectionMumber(0);
    };

    size_t const bufSize = TsPacketSize * std::numeric_limits<DdtSectionNumber>::max();
    uchar_t *buffer = new uchar_t[bufSize];    
        
    list<shared_ptr<PosterViewEntity>>& posters = cookie.GetPosterViews();
    for (auto iter = posters.begin(); iter != posters.end(); ++iter)
    {
        uchar_t *bptr = buffer;

        ifstream ifstrm((*iter)->localPath.c_str(), ios::binary);
		assert(ifstrm.good());
		while (!ifstrm.eof() && bptr < buffer + bufSize)
        {
            shared_ptr<TransportPacketHelperInterface> tsHelper(TransportPacketHelperInterface::CreateInstance(bptr));
            WriteTs(tsHelper, bptr == buffer ? 1 : 0);

            shared_ptr<DdtHelperInterface> ddtHelper(DdtHelperInterface::CreateInstance(tsHelper->GetPayLoadHeader()));            
            WriteDdt(ddtHelper, (*iter)->posterId);

            size_t readSize = TsPacketSize - tsHelper->GetSize() - ddtHelper->GetSize();
            ifstrm.read((char*)ddtHelper->GetHeader() + ddtHelper->GetSize() - ddtHelper->GetCrcCodeSize(), readSize);
			SectionLength secLength = (SectionLength)ifstrm.gcount();
			ddtHelper->SetSectionLength(ddtHelper->GetSectionLength() + secLength);
            ddtHelper->UpdateCrcCode();
            tsHelper->FillPad(ddtHelper->GetSize());

            bptr = bptr + TsPacketSize;
        }

        Send((char*)buffer, bptr - buffer);
    }

    delete[] buffer;
}

void RefsPlayer::Send(char *buffer, size_t size)
{
    struct sockaddr_in dest = *cookie.GetTs()->GetDstAddr();

#ifdef _DEBUG
    //ofstream ofstrm;    
    //while (!ofstrm.is_open())
    //{
    //    ofstrm.open("Movies\\output1_data.ts", ios::binary | ios::app);        
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