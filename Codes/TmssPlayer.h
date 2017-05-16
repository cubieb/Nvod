#ifndef _TmssPlayer_h_
#define _TmssPlayer_h_

/* Foundation */
#include "SystemInclude.h"

/* Type */
#include "BaseType.h"

/* Entity */
#include "Entities.h"

/* Functions */
#include "PlayerInterface.h"

using std::shared_ptr;
class TransportPacketHelperInterface;

typedef std::function<std::pair<bool, shared_ptr<Pcr>>(TransportPacketHelperInterface&, void*)> SectionHandler;

/**********************class TmssPlayer**********************/
class TmssPlayer : public PlayerInterface
{
public:
    TmssPlayer(shared_ptr<TmssEntity> tmss);
    ~TmssPlayer();
    static PlayerInterface* CreateInstance(shared_ptr<TmssEntity> tmss)
    {
        return new TmssPlayer(tmss);
    }

    void Start();
    void Stop();
    bool IsRunning();

private:
    void TheMovieThreadMain();
    TmssEntity::TmssEvents::iterator  SelectEvent(TmssEntity::TmssEvents::iterator curTmssEvent);
        
    /* pair.first :  if current data should be sent. for now, pat section should not be resent.
     * pair.second:  the pcr value.
    */
    std::pair<bool, shared_ptr<Pcr>> HandlePatSection(TransportPacketHelperInterface&, void*);
    std::pair<bool, shared_ptr<Pcr>> HandlePmtSection(TransportPacketHelperInterface&, void*);
    std::pair<bool, shared_ptr<Pcr>> HandleAudioSection(TransportPacketHelperInterface&, void*);
    std::pair<bool, shared_ptr<Pcr>> HandleVideoSection(TransportPacketHelperInterface&, void*);
    std::pair<bool, shared_ptr<Pcr>> HandlePcrSection(TransportPacketHelperInterface&, void*);

    typedef std::map<Pid, SectionHandler> SectionHandlers;
    std::pair<size_t, shared_ptr<Pcr>> Play(SectionHandlers& handlers, std::ifstream& ifstrm, char* buffer, size_t size);
    void Send(int socketFd, char *buffer, size_t size);
    milliseconds CalcTimeIntv(Pcr t0, Pcr t1);

private:
    /* member variable initialized by constructor */
    std::mutex mtx;
    std::condition_variable cv;
    bool isOnGoing;
    shared_ptr<TsEntity>   ts;
    shared_ptr<TmssEntity> tmss;    

    /* thread relative variable */
    std::thread theMovieThread;
};

#endif