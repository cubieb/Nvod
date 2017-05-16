#include "PatPlayer.h"

/* Entity */
#include "Entities.h"

/* Functions */
#include "TransportPacketInterface.h"
#include "PacketHelper.h"

using namespace std;
using namespace std::chrono;

typedef ClassFactoriesRegistor<PlayerInterface, string, shared_ptr<GlobalCfgEntity>, shared_ptr<TsEntity>> Registor;
RegisterClassFactory(Registor, reg, "PatPlayer", PatPlayer::CreateInstance);

PatPlayer::PatPlayer(shared_ptr<GlobalCfgEntity> globalCfg, shared_ptr<TsEntity> ts)
{
    isOnGoing = false;

	this->globalCfg = make_shared<GlobalCfgEntity>(*globalCfg);

	this->ts = make_shared<TsEntity>(*ts);
	list<shared_ptr<RefsEntity>>& refses = ts->GetRefses();
	for (auto iter = refses.begin(); iter != refses.end(); ++iter)
	{
		shared_ptr<RefsEntity> refs = make_shared<RefsEntity>(**iter);

		refs->SetPsts(make_shared<PstsEntity>(*(*iter)->GetPsts()));
		this->ts->Bind(refs);
	}

	list<shared_ptr<TmssEntity>>& tmsses = ts->GetTmsses();
	for (auto iter = tmsses.begin(); iter != tmsses.end(); ++iter)
	{
		this->ts->Bind(make_shared<TmssEntity>(**iter));
	}
}

PatPlayer::~PatPlayer()
{}

void PatPlayer::Start()
{
    {
        std::lock_guard<std::mutex> lock(mtx);
        isOnGoing = true;
    }

    thePatThread = thread(bind(&PatPlayer::ThePatThreadMain, this));
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
	int socketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	ContinuityCounter patContinuityCounter = 0;

	milliseconds duration = globalCfg->GetPatInterval();
    while (true)
    {
        std::unique_lock<std::mutex> lock(mtx);
        if (cv.wait_for(lock, duration, [this]{return !isOnGoing; }))
            break;

		HandlePatTimer(socketFd, patContinuityCounter);
    }

    closesocket(socketFd);
}

void PatPlayer::HandlePatTimer(int socketFd, ContinuityCounter& continuityCounter)
{
    size_t const size = TsPacketSize * std::numeric_limits<DitSectionNumber>::max();
    uchar_t *buffer = new uchar_t[size];
    uchar_t *ptr = buffer;

	shared_ptr<TransportPacketHelperInterface> tsHelper(
		TransportPacketHelperInterface::CreateInstance(buffer, 1, 0, continuityCounter)
		);

	shared_ptr<PatHelperInterface> patHelper(
		PatHelperInterface::CreateInstance(tsHelper->GetPayLoadHeader(), ts->GetTsId())
		);
 	
    list<shared_ptr<RefsEntity>>& refses = ts->GetRefses();
    list<shared_ptr<TmssEntity>>& tmsses = ts->GetTmsses();
    for (auto iter = refses.begin(); iter != refses.end(); ++iter)
    {
		shared_ptr<PstsEntity> psts = (*iter)->GetPsts();

		uchar_t *ptr = patHelper->GetHeader() + patHelper->GetSize() - patHelper->GetCrcCodeSize();
		shared_ptr<PatElementaryInterface> patElementary(PatElementaryInterface::CreateInstance(ptr));
		patElementary->SetProgramNumber(psts->GetServiceId());
		patElementary->SetPmtPid(psts->GetPmtPid());
		patHelper->SetSectionLength(patHelper->GetSectionLength() + patElementary->GetSize());
		assert(tsHelper->GetSize() + patHelper->GetSize() <= TsPacketSize);
    }

    for (auto iter = tmsses.begin(); iter != tmsses.end(); ++iter)
    {
		if (tsHelper->GetSize() + patHelper->GetSize() + 4 > TsPacketSize)
		{
			tsHelper.reset(TransportPacketHelperInterface::CreateInstance(buffer, 0, 0, continuityCounter));
			patHelper.reset(PatHelperInterface::CreateInstance(tsHelper->GetPayLoadHeader(), ts->GetTsId()));
		}

		uchar_t *ptr = patHelper->GetHeader() + patHelper->GetSize() - patHelper->GetCrcCodeSize();
		shared_ptr<PatElementaryInterface> patElementary(PatElementaryInterface::CreateInstance(ptr));
		patElementary->SetProgramNumber((*iter)->GetServiceId());
		patElementary->SetPmtPid((*iter)->GetPmtPid());
		patHelper->SetSectionLength(patHelper->GetSectionLength() + patElementary->GetSize());
		assert(tsHelper->GetSize() + patHelper->GetSize() <= TsPacketSize);
    }

    SectionNumber cur = 0;
	SectionNumber last = (patHelper->GetHeader() + patHelper->GetSize() - buffer) / TsPacketSize;
	for (uchar_t *ptr = buffer; ptr <= patHelper->GetHeader() + patHelper->GetSize(); ptr = ptr + TsPacketSize)
    {
        tsHelper.reset(TransportPacketHelperInterface::CreateInstance(ptr));
        patHelper.reset(PatHelperInterface::CreateInstance(tsHelper->GetPayLoadHeader()));

        patHelper->SetSectionMumber(cur++);
        patHelper->SetLastSectionMumber(last);
        patHelper->UpdateCrcCode();

        tsHelper->FillPad(patHelper->GetSize());
    }
    
	Send(socketFd, (char*)buffer, TsPacketSize);
    delete buffer;
}

void PatPlayer::Send(int socketFd, char *buffer, size_t size)
{
    const struct sockaddr_in& dest = ts->GetDstAddr();

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