#ifndef _RefsPlayer_h_
#define _RefsPlayer_h_

/* Type */
#include "BaseType.h"

/* Foundation */
#include "SystemInclude.h"

/* Entity */
#include "Entities.h"

/* Functions */
#include "PlayerInterface.h"

using std::shared_ptr;
using std::string;

class GlobalCfgEntity;
struct PosterViewEntity;
class RefsEntity;
class RefsEventEntity;

class RefsPlayerCookie;

/**********************class RefsViewEntity**********************/
struct PosterViewEntity
{
	TableId id;                    /* not null */
	PosterId posterId;
	string remotePath;
	string localPath;  /* tansient, not null */

	list<EventId> refsEventIds;
};

/**********************class RefsPlayer**********************/
class RefsPlayer : public PlayerInterface
{
public:
	typedef std::function<void(int, ContinuityCounter&)> TimerHandler;

	RefsPlayer(shared_ptr<GlobalCfgEntity> globalCfg, shared_ptr<RefsEntity> refs);
    ~RefsPlayer();
	static PlayerInterface* CreateInstance(shared_ptr<GlobalCfgEntity> globalCfg,
        shared_ptr<RefsEntity> refs)
    {
		return new RefsPlayer(globalCfg, refs);
    }

    void Start();
    void Stop();
    bool IsRunning();

private:
    void TheDataPipeThreadMain();
	milliseconds HandlerTimer(int socketFd, milliseconds invl, milliseconds initInvl,
		TimerHandler handler, ContinuityCounter& continuityCounter);

	void HandlePmtTimer(int socketFd, ContinuityCounter& continuityCounter);
	void HandlePosterTimer(int socketFd, ContinuityCounter& continuityCounter);
	void HandlePosterDitTimer(int socketFd, ContinuityCounter& continuityCounter);
	void HandlePosterDdtTimer(int socketFd, ContinuityCounter& continuityCounter);
	void Send(int socketFd, char *buffer, size_t size);

private:
    /* member variable initialized by constructor */
    std::mutex mtx;
    std::condition_variable cv;
    bool isOnGoing;
	shared_ptr<GlobalCfgEntity> globalCfg;
	shared_ptr<TsEntity> ts;
	shared_ptr<RefsEntity> refs;
	list<shared_ptr<PosterViewEntity>> posterViews;

    /* thread relative variable */
    std::thread theDataPipeThread;
};

#endif