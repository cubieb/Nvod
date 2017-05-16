#ifndef _Controller_h_
#define _Controller_h_

/* Controller */
#include "ControllerInterface.h"

/* Foundation */
#include "SystemInclude.h"

/* Entity */
#include "Entities.h"

/* Functions */
#include "PlayerInterface.h"
#include "DownloaderInterface.h"

using std::list;
using std::shared_ptr;


/**********************class Controller**********************/
class Controller : public ControllerInterface
{
public:
    Controller();
    ~Controller();

    /* Create a new thread and wait for reactor event loop. */
    void Start(const char *xmlPath);
    void Stop(); 
    bool IsRunning();

private:
    void TheControllerThread(const char *xmlPath);
    bool CreateEntities(const char *xmlPath, shared_ptr<GlobalCfgEntity>& globalCfg, list<shared_ptr<TsEntity>>& tses);

    template <typename T>
    void Download(shared_ptr<DownloaderInterface> downloader, uint32_t& number, T& t)
    {
        if (access(t->GetLocalPath().c_str(), 0) != 0)
        {
            ++number;
            auto ftpResource = make_shared<FtpResource>(t->GetId(),
                t->GetRemotePath().c_str(),
                t->GetLocalPath().c_str());
            downloader->Download(ftpResource);
        }
    }

    struct Players
    {
        list<shared_ptr<PlayerInterface>> patPlayers;
        list<shared_ptr<PlayerInterface>> refsPlayers;
        list<shared_ptr<PlayerInterface>> tmssPlayers;
    };
	bool CreatePlayers(shared_ptr<GlobalCfgEntity> globalCfg, list<shared_ptr<TsEntity>> tses, Players& players);
    void ClearPlayers(list<shared_ptr<PlayerInterface>>& players);

private:
    std::mutex mtx;
    std::condition_variable cv;
    bool isOnGoing;

    std::thread theControllerThread;
};

#endif