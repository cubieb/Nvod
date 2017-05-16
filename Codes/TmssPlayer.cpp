#include "TmssPlayer.h"

/* Type */
#include "BaseType.h"

/* Foundation */
#include "SystemInclude.h"
#include "PacketHelper.h"
#include "Debug.h"

/* Entity */
#include "Entities.h"

/* Functions */
#include "TransportPacketInterface.h"

#define MovieBufferSize 188*1024*8

using namespace std;
using namespace std::chrono;
using namespace std::placeholders;

typedef ClassFactoriesRegistor<PlayerInterface, string, shared_ptr<TmssEntity>> Registor;
RegisterClassFactory(Registor, reg, "TmssPlayer", TmssPlayer::CreateInstance);

static set<uint8_t> audioStreamTypes = { 0x03, 0x04, 0x0F, 0x11, 0x81 };
static set<uint8_t> videoStreamTypes = { 0x01, 0x02, 0x10, 0x1B, 0x80 };

/**********************class TmssPlayer**********************/
/* public functions */
TmssPlayer::TmssPlayer(shared_ptr<TmssEntity> tmss)
{
    isOnGoing = false;

    this->ts = make_shared<TsEntity>(*tmss->GetTs());
    this->tmss = make_shared<TmssEntity>(*tmss);

    steady_clock::time_point now = steady_clock::now();
    for (auto iter = tmss->GetTmssEvents().begin(); iter != tmss->GetTmssEvents().end(); ++iter)
    {
        if ((*iter)->GetStartTimePoint() + (*iter)->GetDuration() < now)
        {
            continue;
        }

        /* bind TmssEventEntity to TmssEntity */
        shared_ptr<TmssEventEntity> tmssEvent = make_shared<TmssEventEntity>(**iter);
        this->tmss->Bind(tmssEvent);

        /* bind RefsEventEntity and TmssEventEntity */
        shared_ptr<RefsEventEntity> refsEvent = make_shared<RefsEventEntity>(*(*iter)->GetRefsEvent());
        refsEvent->Bind(tmssEvent);

        list<shared_ptr<MovieEntity>>& movies = (*iter)->GetRefsEvent()->GetMovies();
        for (auto iter = movies.begin(); iter != movies.end(); ++iter)
        {
            shared_ptr<MovieEntity> movie = make_shared<MovieEntity>(**iter);
            refsEvent->Bind(movie);
        }
    }    
}

TmssPlayer::~TmssPlayer()
{
    if (theMovieThread.joinable())
    {
        /* thread exited because of no more movie (not calling of Stop()),
         * join the thread.
         */
        theMovieThread.join();
    }
}

void TmssPlayer::Start()
{
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (isOnGoing)
            return;
        isOnGoing = true;
    }

    theMovieThread = thread(bind(&TmssPlayer::TheMovieThreadMain, this));
}

void TmssPlayer::Stop()
{
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (!isOnGoing)
            return;
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
    int socketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    shared_ptr<char> buffer(new char[MovieBufferSize], [](void* ptr){ delete[] ptr; });
    size_t tsSize = 0;
    /* Pcr information in movie .ts file. */
    shared_ptr<Pcr> firstPcr, lastPcr;    
    /* eventDuration: 当前tmss event 相关的所有已播放完成的电影的总时长。*/
    milliseconds eventDuration = milliseconds::zero();

    std::map<Pid, SectionHandler> sectionHandlers;
    sectionHandlers.insert(make_pair(PatPid, bind(&TmssPlayer::HandlePatSection, this, _1, _2)));

    TmssEntity::TmssEvents::iterator  curTmssEvent = SelectEvent(tmss->GetTmssEvents().begin());
    if (curTmssEvent == tmss->GetTmssEvents().end())
    {
        std::lock_guard<std::mutex> lock(mtx);
        isOnGoing = false;
        return;
    }
    RefsEventEntity::Movies::iterator curMovie = (*curTmssEvent)->GetRefsEvent()->GetMovies().begin();
    std::ifstream ifstrm((*curMovie)->GetLocalPath().c_str(), ios::binary);
    milliseconds duration = duration_cast<milliseconds>((*curTmssEvent)->GetStartTimePoint() - steady_clock::now());
    while (true)
    {
        std::unique_lock<std::mutex> lock(mtx);
        dbgstrm << duration.count() << endl;
        if (cv.wait_for(lock, duration, [this]{ return !isOnGoing; }))
        {
            if (ifstrm.is_open())
                ifstrm.close();
            break;
        }

        system_clock::time_point now = steady_clock::now();
        if (tsSize != 0)
        {
            milliseconds mtime = (firstPcr != nullptr ? CalcTimeIntv(*firstPcr, *lastPcr) : milliseconds::zero());
            if ((*curTmssEvent)->GetStartTimePoint() + eventDuration + mtime + milliseconds(200) > now)
            {
                Send(socketFd, buffer.get(), tsSize);
            }

            tsSize = 0;
        }

        /* if current movie has been finished */
        if (!ifstrm.is_open())
        {
            if (++curMovie == (*curTmssEvent)->GetRefsEvent()->GetMovies().end())
            {
                curTmssEvent = SelectEvent(++curTmssEvent);
                if (curTmssEvent == tmss->GetTmssEvents().end())
                {
                    isOnGoing = false;
                    break;
                }
                curMovie = (*curTmssEvent)->GetRefsEvent()->GetMovies().begin();
                eventDuration = milliseconds::zero();
            }
            else
            {
                if (firstPcr != nullptr && lastPcr != nullptr)
                {
                    eventDuration = eventDuration + CalcTimeIntv(*firstPcr, *lastPcr);
                }
            }

            firstPcr.reset();
            lastPcr.reset();
            sectionHandlers.clear();
            sectionHandlers.insert(make_pair(PatPid, bind(&TmssPlayer::HandlePatSection, this, _1, _2)));
            ifstrm.open((*curMovie)->GetLocalPath().c_str(), ios::binary);
            duration = duration_cast<milliseconds>((*curTmssEvent)->GetStartTimePoint() + eventDuration - now);
            continue;
        }
        
        auto pair = Play(sectionHandlers, ifstrm, buffer.get(), MovieBufferSize);
        tsSize = pair.first;
        if (firstPcr == nullptr)
        {            
            firstPcr = lastPcr = pair.second;
            duration = duration_cast<milliseconds>((*curTmssEvent)->GetStartTimePoint() + eventDuration - now);
        }
        else
        { 
            if (pair.second != nullptr)
                lastPcr = pair.second;
            milliseconds mtime = CalcTimeIntv(*firstPcr, *lastPcr);
            duration = duration_cast<milliseconds>((*curTmssEvent)->GetStartTimePoint() + mtime + eventDuration - now);
        }        
    }

    closesocket(socketFd);
}

TmssEntity::TmssEvents::iterator TmssPlayer::SelectEvent(TmssEntity::TmssEvents::iterator curTmssEvent)
{
    while (curTmssEvent != tmss->GetTmssEvents().end())
    {
        if ((*curTmssEvent)->GetStartTimePoint() + (*curTmssEvent)->GetDuration() > steady_clock::now())
        {
            list<shared_ptr<MovieEntity>>& movies = (*curTmssEvent)->GetRefsEvent()->GetMovies();
            if (!movies.empty())
                break;
        }

        ++curTmssEvent;
    }
    return curTmssEvent;
}

std::pair<bool, shared_ptr<Pcr>> TmssPlayer::HandlePatSection(TransportPacketHelperInterface& tsPacket, void* handlers)
{
    /* Pat Table */
    shared_ptr<PatHelperInterface> patTable(PatHelperInterface::CreateInstance(tsPacket.GetPayLoadHeader()));

    uchar_t *ptr = patTable->GetHeader() + patTable->GetMinSize() - patTable->GetCrcCodeSize();
    while (ptr < patTable->GetHeader() + patTable->GetSize())
    {
        shared_ptr<PatElementaryInterface> patElementary(PatElementaryInterface::CreateInstance(ptr));
        if (patElementary->GetProgramNumber() != 0)
        {
            std::map<Pid, SectionHandler>& sectionHandlers = *reinterpret_cast<std::map<Pid, SectionHandler>*>(handlers);
            Pid pmtPid = patElementary->GetPmtPid();
            if (sectionHandlers.find(pmtPid) == sectionHandlers.end())
            {
                /* save ts file's program_map_PID */
                dbgstrm << "pmtPid = " << hex << setfill('0') << setw(4) << pmtPid << dec << endl;                
                sectionHandlers.insert(make_pair(pmtPid, bind(&TmssPlayer::HandlePmtSection, this, _1, _2)));
            }

            /* replace pat table's program_number and program_map_PID */
            patElementary->SetProgramNumber(tmss->GetServiceId());
            patElementary->SetPmtPid(tmss->GetPmtPid());
            break;
        }
        ptr = ptr + patElementary->GetSize();
    }
    patTable->UpdateCrcCode();

    /* discard pat packet */
    return make_pair(false, nullptr);
}

std::pair<bool, shared_ptr<Pcr>> TmssPlayer::HandlePmtSection(TransportPacketHelperInterface& tsPacket, void* handlers)
{
    /* Pmt Table */
    std::map<Pid, SectionHandler>& sectionHandlers = *reinterpret_cast<std::map<Pid, SectionHandler>*>(handlers);

    shared_ptr<PmtHelperInterface> pmtTable(PmtHelperInterface::CreateInstance(tsPacket.GetPayLoadHeader()));
    Pid pcrPid = pmtTable->GetPcrPid();
    if (sectionHandlers.find(pcrPid) == sectionHandlers.end())
    {
        dbgstrm << "pcrPid = " << pcrPid << endl;

        sectionHandlers.insert(make_pair(pcrPid, bind(&TmssPlayer::HandlePcrSection, this, _1, _2)));
    }

    uint32_t flag = 0;
    uchar_t *ptr = pmtTable->GetHeader() + pmtTable->GetMinSize() - pmtTable->GetCrcCodeSize() 
        + pmtTable->GetProgramInfoLength();
    while (ptr < pmtTable->GetHeader() + pmtTable->GetSize() && flag != 0x3)
    {
        shared_ptr<PmtElementaryInterface> pmtElementary(PmtElementaryInterface::CreateInstance(ptr));
        if (audioStreamTypes.find(pmtElementary->GetStreamType()) != audioStreamTypes.end())
        {
            Pid audioPid = pmtElementary->GetElementaryPid();
            if (sectionHandlers.find(audioPid) == sectionHandlers.end())
            {
                /* save ts file's audio pid */
                dbgstrm << "audioPid = " << audioPid << endl;

                sectionHandlers.insert(make_pair(audioPid, bind(&TmssPlayer::HandleAudioSection, this, _1, _2)));
            }
            /* replace pmt table's elementary_PID */
            pmtElementary->SetElementaryPid(tmss->GetAudioPid());
            flag = flag | 1;
        }

        if (videoStreamTypes.find(pmtElementary->GetStreamType()) != videoStreamTypes.end())
        {
            Pid videoPid = pmtElementary->GetElementaryPid();
            if (sectionHandlers.find(videoPid) == sectionHandlers.end())
            {
                /* save ts file's video pid */
                dbgstrm << "videoPid = " << videoPid << endl;

                sectionHandlers.insert(make_pair(videoPid, bind(&TmssPlayer::HandleVideoSection, this, _1, _2)));
            }
            /* replace pmt table's elementary_PID */
            pmtElementary->SetElementaryPid(tmss->GetVideoPid());
            flag = flag | 2;
        }
        ptr = ptr + pmtElementary->GetSize();
    }
    // false means we failed to get the right audio or video pid.
    assert(flag == 0x3);

    /* replace pmt table's program number and pcr pid */
    pmtTable->SetProgramNumber(tmss->GetServiceId());
    pmtTable->SetPcrPid(tmss->GetPcrPid());
    pmtTable->UpdateCrcCode();
    tsPacket.SetPid(tmss->GetPmtPid());

    return make_pair(true, nullptr);
}

std::pair<bool, shared_ptr<Pcr>> TmssPlayer::HandleAudioSection(TransportPacketHelperInterface& tsPacket, void*)
{
    /* replace audio pid for audio PES packets */
    tsPacket.SetPid(tmss->GetAudioPid());

    return make_pair(true, nullptr);
}

std::pair<bool, shared_ptr<Pcr>> TmssPlayer::HandleVideoSection(TransportPacketHelperInterface& tsPacket, void*)
{
    /* replace video pid for video PES packets */
    tsPacket.SetPid(tmss->GetVideoPid());

    return make_pair(true, nullptr);
}

std::pair<bool, shared_ptr<Pcr>> TmssPlayer::HandlePcrSection(TransportPacketHelperInterface& tsPacket, void*)
{
    tsPacket.SetPid(tmss->GetPcrPid());

    AdaptationFieldControl afc = tsPacket.GetAdaptationFieldControl();
    if (afc != 0x2 && afc != 0x3)
    {
        //there is no pcr field if the adaptation_field_control not in {0x2, 0x3}.
        return make_pair(true, nullptr);
    }

    if (tsPacket.GetAdaptationFieldLength() == 0)
    {
        return make_pair(true, nullptr);
    }

    if (tsPacket.GetPcrFlag() != 1)
    {
        return make_pair(true, nullptr);
    }

    Pcr pcr = tsPacket.GetPcr();
    dbgstrm << "pcr = " << pcr << endl;
    
    return make_pair(true, make_shared<Pcr>(pcr));;
}

/* refer to iso13818-1.pdf
: 2.4.3.2 Transport Stream packet layer
: 2.5.2.2 Input to the Program Stream system target decoder 
: D.0.3 System Time Clock recovery in the decoder
: D.0.4 SCR and PCR Jitter
: Table 2-6 – Transport Stream adaptation field (for packet format)
*/
std::pair<size_t, shared_ptr<Pcr>> TmssPlayer::Play(SectionHandlers& handlers, std::ifstream& ifstrm, char* buffer, size_t size)
{
    std::pair<size_t, shared_ptr<Pcr>> pair(0, nullptr);
    char *ptr = buffer;
    while (ptr + TsPacketSize < buffer + size)
    {
        ifstrm.read((char*)ptr, TsPacketSize);

        if (ifstrm.eof())
        {
            ifstrm.close();
            pair.first = ptr - buffer;
            break;
        }
        assert(ifstrm.gcount() == TsPacketSize);

        shared_ptr<TransportPacketHelperInterface> tsPacket(TransportPacketHelperInterface::CreateInstance((uchar_t*)ptr));
        assert(tsPacket->GetSyncByte() == 0x47);

        Pid pid = tsPacket->GetPid();
        //dbgstrm << "pcrPid = " << hex << setfill('0') << setw(4) << pid << dec << endl;
        auto handler = handlers.find(tsPacket->GetPid());
        if (handler == handlers.end())
            continue;
        
        auto ret = handler->second(*tsPacket, &handlers);
        if (ret.second != nullptr)
        {
            pair.first = ptr + TsPacketSize - buffer;
            pair.second = ret.second;
            break;
        }

        if (ret.first)
        {
            ptr = ptr + TsPacketSize;
        }
    } /* while (ptr < buffer + bufferSize && duration == nullptr) */

    /* buffer is not enought */
    assert(ptr < buffer + size);
    return pair;
}

void TmssPlayer::Send(int socketFd, char *buffer, size_t size)
{
    const struct sockaddr_in& dest = ts->GetDstAddr();

#ifdef _DEBUG
    //ofstream ofstrm;    
    //while (!ofstrm.is_open())
    //{
    //    ofstrm.open("Movies\\output.ts", std::ofstream::binary | std::ofstream::app);        
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

milliseconds TmssPlayer::CalcTimeIntv(Pcr t0, Pcr t1)
{
    return milliseconds((t1 - t0) / 27000);
}