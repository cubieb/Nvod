#include "RefsPlayer.h"

/* Entity */
#include "Entities.h"

/* Foundation */
#include "SystemInclude.h"
#include "Debug.h"

/* Functions */
#include "TransportPacketInterface.h"
#include "PacketHelper.h"

using namespace std;
using namespace std::chrono;
using namespace std::placeholders;

typedef ClassFactoriesRegistor<PlayerInterface, string, shared_ptr<GlobalCfgEntity>, shared_ptr<RefsEntity>> Registor;
RegisterClassFactory(Registor, reg, "RefsPlayer", RefsPlayer::CreateInstance);

/**********************class RefsPlayer**********************/
RefsPlayer::RefsPlayer(shared_ptr<GlobalCfgEntity> globalCfg, shared_ptr<RefsEntity> refs)
{
    isOnGoing = false;

	shared_ptr<TsEntity> ts = refs->GetTs();
	this->globalCfg = make_shared<GlobalCfgEntity>(*globalCfg);
	this->ts = make_shared<TsEntity>(*ts);
	this->refs = make_shared<RefsEntity>(*refs);
	this->refs->SetPsts(make_shared<PstsEntity>(*refs->GetPsts()));

	this->ts->Bind(this->refs);

	list<shared_ptr<RefsEventEntity>> refsEvents = refs->GetRefsEvents();
	for (auto iter = refsEvents.begin(); iter != refsEvents.end(); ++iter)
	{
		shared_ptr<RefsEventEntity> refsEvent = *iter;
		list<shared_ptr<PosterEntity>> posters = refsEvent->GetPosters();

		for (auto iter = posters.begin(); iter != posters.end(); ++iter)
		{
			shared_ptr<PosterEntity> poster = *iter;
			auto cmp = [poster](shared_ptr<PosterViewEntity> right)->bool
			{
				return poster->GetPosterId() == right->posterId;
			};
			auto result = find_if(posterViews.begin(), posterViews.end(), cmp);

			shared_ptr<PosterViewEntity> posterView;
			if (result == posterViews.end())
			{
				posterView = make_shared<PosterViewEntity>();
				posterView->id = poster->GetId();
				posterView->posterId = poster->GetPosterId();
				posterView->remotePath = poster->GetRemotePath();
				posterView->localPath = poster->GetLocalPath();
				posterViews.push_back(posterView);
			}
			else
			{
				posterView = *result;
			}

			posterView->refsEventIds.push_back((*iter)->GetPosterId());
		}
	}
}

RefsPlayer::~RefsPlayer()
{}

void RefsPlayer::Start()
{
    {
        std::lock_guard<std::mutex> lock(mtx);
        isOnGoing = true;
    }

    theDataPipeThread = thread(bind(&RefsPlayer::TheDataPipeThreadMain, this));
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
	int socketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	ContinuityCounter pmtContinuityCounter = 0;
	ContinuityCounter posterContinuityCounter = 0;

	TimerHandler pmtHandler = bind(&RefsPlayer::HandlePmtTimer, this, _1, _2);
	milliseconds pmtInterval = milliseconds::zero();

	TimerHandler posterHandler = bind(&RefsPlayer::HandlePosterTimer, this, _1, _2);
	milliseconds posterInterval = milliseconds::zero();

    while (true)
    {
        std::unique_lock<std::mutex> lock(mtx);
        if (cv.wait_for(lock, milliseconds(10), [this]{return !isOnGoing; }))
            break;
        
		pmtInterval = HandlerTimer(socketFd, pmtInterval, globalCfg->GetPmtInterval(), 
			pmtHandler, pmtContinuityCounter);

		posterInterval = HandlerTimer(socketFd, posterInterval, globalCfg->GetPosterInterval(), 
			posterHandler, posterContinuityCounter);
    }    

    closesocket(socketFd);
}

milliseconds RefsPlayer::HandlerTimer(int socketFd, milliseconds invl, milliseconds initInvl, 
	TimerHandler handler, ContinuityCounter& continuityCounter)
{
    if (invl < milliseconds(10))
    {
		handler(socketFd, continuityCounter);
        invl = initInvl;
    }
    else
    {
        invl = invl - milliseconds(10);
    }
    return invl;
}

void RefsPlayer::HandlePmtTimer(int socketFd, ContinuityCounter& continuityCounter)
{
    uchar_t buffer[TsPacketSize];

	shared_ptr<TransportPacketHelperInterface> tsHelper(
		TransportPacketHelperInterface::CreateInstance(buffer, 1, refs->GetPsts()->GetPmtPid(), continuityCounter)
		);

	shared_ptr<PmtHelperInterface> pmtHelper(
		PmtHelperInterface::CreateInstance(tsHelper->GetPayLoadHeader(), refs->GetPsts()->GetServiceId())
		);

    uchar_t *ptr = pmtHelper->GetHeader() + pmtHelper->GetSize() - pmtHelper->GetCrcCodeSize();
    shared_ptr<PmtElementaryInterface> pmtElementary(PmtElementaryInterface::CreateInstance(ptr));
	pmtElementary->SetStreamType(refs->GetPsts()->GetStreamType());
	pmtElementary->SetElementaryPid(refs->GetPsts()->GetPosterPid());
    pmtElementary->SetEsInfoLength(0);

    pmtHelper->SetSectionLength(pmtHelper->GetSectionLength() + pmtElementary->GetSize());
    pmtHelper->UpdateCrcCode();

    tsHelper->FillPad(pmtHelper->GetSize());
	Send(socketFd, (char*)buffer, TsPacketSize);
}

void RefsPlayer::HandlePosterTimer(int socketFd, ContinuityCounter& continuityCounter)
{
	HandlePosterDitTimer(socketFd, continuityCounter);
	HandlePosterDdtTimer(socketFd, continuityCounter);
}

void RefsPlayer::HandlePosterDitTimer(int socketFd, ContinuityCounter& continuityCounter)
{
    size_t const size = TsPacketSize * std::numeric_limits<DitSectionNumber>::max();
	uchar_t *buffer = new uchar_t[size];
    uchar_t *ptr = buffer;
    
	Pid pid = refs->GetPsts()->GetPosterPid();
	shared_ptr<TransportPacketHelperInterface> tsHelper(
		TransportPacketHelperInterface::CreateInstance(ptr, 1, pid, continuityCounter)
		);

	shared_ptr<DitHelperInterface> ditHelper(
		DitHelperInterface::CreateInstance(tsHelper->GetPayLoadHeader(), VersionNumber(0))
		);

    shared_ptr<list<shared_ptr<PosterViewEntity>>::iterator> posterIter;
	shared_ptr<list<EventId>::iterator> refsEventIdIter;

	for (auto iter1 = posterViews.begin(); iter1 != posterViews.end(); ++iter1)
    {
        if (posterIter != nullptr)
        {
            iter1 = *posterIter;
            posterIter.reset();
        }
        if (tsHelper->GetSize() + ditHelper->GetSize() + 6 > TsPacketSize)
        {
			ptr = ptr + TsPacketSize;
			assert(ptr < buffer + size);
			tsHelper.reset(TransportPacketHelperInterface::CreateInstance(ptr, 0, pid, continuityCounter));
			ditHelper.reset(DitHelperInterface::CreateInstance(tsHelper->GetPayLoadHeader(), VersionNumber(0)));
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
    DitSectionNumber last = (ptr - buffer) / TsPacketSize;
    for (uchar_t *p = buffer; p <= ptr; p = p + TsPacketSize)
    {
		shared_ptr<TransportPacketHelperInterface> tsHelper(TransportPacketHelperInterface::CreateInstance(p));
		shared_ptr<DitHelperInterface> ditHelper(DitHelperInterface::CreateInstance(tsHelper->GetPayLoadHeader()));
        ditHelper->SetSectionMumber(cur++);
        ditHelper->SetLastSectionMumber(last);
        ditHelper->UpdateCrcCode();

        tsHelper->FillPad(ditHelper->GetSize());
    }

	Send(socketFd, (char*)buffer, ptr + TsPacketSize - buffer);
    delete[] buffer;
}

void RefsPlayer::HandlePosterDdtTimer(int socketFd, ContinuityCounter& continuityCounter)
{
	Pid pid = refs->GetPsts()->GetPosterPid();

    size_t const size = TsPacketSize * std::numeric_limits<DdtSectionNumber>::max();
	uchar_t *buffer = new uchar_t[size];

	for (auto iter = posterViews.begin(); iter != posterViews.end(); ++iter)
    {
        uchar_t *ptr = buffer;
        PayloadUnitStartIndicator payloadUnitStartIndicator = 1;

        ifstream ifstrm((*iter)->localPath.c_str(), ios::binary);
		assert(ifstrm.good());
		while (!ifstrm.eof() && ptr < buffer + size)
        {
			shared_ptr<TransportPacketHelperInterface> tsHelper(
				TransportPacketHelperInterface::CreateInstance(ptr, payloadUnitStartIndicator, pid, continuityCounter)
				);

			shared_ptr<DdtHelperInterface> ddtHelper(
				DdtHelperInterface::CreateInstance(tsHelper->GetPayLoadHeader(), (*iter)->posterId)
				);

            size_t readSize = TsPacketSize - tsHelper->GetSize() - ddtHelper->GetSize();
            ifstrm.read((char*)ddtHelper->GetHeader() + ddtHelper->GetSize() - ddtHelper->GetCrcCodeSize(), readSize);
			SectionLength secLength = (SectionLength)ifstrm.gcount();
			ddtHelper->SetSectionLength(ddtHelper->GetSectionLength() + secLength);
            ddtHelper->UpdateCrcCode();
            tsHelper->FillPad(ddtHelper->GetSize());

			ptr = ptr + TsPacketSize;
            payloadUnitStartIndicator = 0;
        }

		DdtSectionNumber cur = 0;
		DdtSectionNumber last = (ptr - buffer) / TsPacketSize;
		for (uchar_t *p = buffer; p <= ptr; p = p + TsPacketSize)
		{
			shared_ptr<TransportPacketHelperInterface> tsHelper(TransportPacketHelperInterface::CreateInstance(p));
			shared_ptr<DdtHelperInterface> ditHelper(DdtHelperInterface::CreateInstance(tsHelper->GetPayLoadHeader()));
			ditHelper->SetSectionMumber(cur++);
			ditHelper->SetLastSectionMumber(last);
			ditHelper->UpdateCrcCode();

			tsHelper->FillPad(ditHelper->GetSize());
		}
		
		Send(socketFd, (char*)buffer, ptr - buffer);
    }

    delete[] buffer;
}

void RefsPlayer::Send(int socketFd, char *buffer, size_t size)
{
    const struct sockaddr_in& dest = ts->GetDstAddr();

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