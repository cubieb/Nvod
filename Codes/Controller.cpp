#include "Controller.h"

/* Foundation */
#include "SystemInclude.h"
#include "Debug.h"

/* Entity */
#include "Entities.h"

/* Functions */
#include "XmlWrapperInterface.h"
#include "WebServiceInterface.h"
#include "PlayerInterface.h"
#include "SocketServerInterface.h"
#include "DownloaderInterface.h"

using namespace std;

/**********************class ControllerInterface**********************/
ControllerInterface &ControllerInterface::GetInstance()
{
    /* In C++11, the following is guaranteed to perform thread-safe initialisation: */
    static Controller instance;
    return instance;
}

/**********************class Controller**********************/
/* public function */
Controller::Controller()
{}

Controller::~Controller()
{}

void Controller::Start(const char *xmlPath)
{    
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (isOnGoing)
            return;

        isOnGoing = true;
    }

	theControllerThread = thread(bind(&Controller::TheControllerThread, this, xmlPath));
}

void Controller::Stop()
{
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (!isOnGoing)
            return;
        isOnGoing = false;
        cv.notify_all();
    }
    theControllerThread.join();
}

bool Controller::IsRunning()
{
    lock_guard<mutex> lock(mtx);
    return isOnGoing;
}

/* private functions */
void Controller::TheControllerThread(const char *xmlPath)
{
    shared_ptr<GlobalCfgEntity> globalCfg;
    list<shared_ptr<TsEntity>> tses;
    Players players;

	SocketServerInterface& socketServer = SocketServerInterface::GetInstance();
	socketServer.Start();

    /* 每天凌晨3点获取新的电影编排信息 */
    time_t now = time(nullptr);
    time_t t = now - (now % (3600 * 24)) - _timezone; //localtime(&t) is today, 00:00:00
    if (std::localtime(&now)->tm_hour <= 3)
        t = t + 3600 * 3;
    else
        t = t + 3600 * 27;

	milliseconds duration = milliseconds::zero(); 
    while (true)
    {
        std::unique_lock<std::mutex> lock(mtx);
        dbgstrm << duration.count() << endl;
		bool  exiting = cv.wait_for(lock, duration, [this]{ return !isOnGoing; });

		ClearPlayers(players.patPlayers);
		ClearPlayers(players.refsPlayers);
		ClearPlayers(players.tmssPlayers);

		if (exiting)
		{			
            break;
        }
        
        CreateEntities(xmlPath, globalCfg, tses);
        if (!CreatePlayers(globalCfg, tses, players))
		{ 
			errstrm << "CreatePlayers() failed, TheControllerThread() is exiting.";
			isOnGoing = false;
		}

		if (duration == milliseconds::zero())
			duration = milliseconds(t - now);
		else
			duration = milliseconds(3600 * 24);
    }

	socketServer.Stop();
}

bool Controller::CreateEntities(const char *xmlPath, shared_ptr<GlobalCfgEntity>& globalCfg, list<shared_ptr<TsEntity>>& tses)
{
    const char* staticConfig = "StaticConfig.xml";
    const char* dynamicConfig = "DynamicConfig.xml";

    if (access(staticConfig, 0) == 0)
    {
        int ret = remove(staticConfig);
        assert(ret == 0);
    }
    if (access(dynamicConfig, 0) == 0)
    {
        int ret = remove(dynamicConfig);
        assert(ret == 0);
    }

    shared_ptr<NvodWebUrlWrapperInterface> webUrl(NvodWebUrlWrapperInterface::CreateInstance(xmlPath));
    WebServiceInterface& webService = WebServiceInterface::GetInstance();

    bool ret;
    ret = webService.DownloadConfigXml(webUrl->GetServiceConfigUrl(), staticConfig);
    if (!ret)
    {
        errstrm << "failed to download service configuration." << endl;
        return ret;
    }

    ret = webService.DownloadConfigXml(webUrl->GetEventConfigUrl(), dynamicConfig);
    if (!ret)
    {
        errstrm << "failed to download event configuration." << endl;
        return ret;
    }

    shared_ptr<StaticConfigWrapperInterface> staticConfigWrapper(StaticConfigWrapperInterface::CreateInstance(staticConfig));
    globalCfg = staticConfigWrapper->GetGlobalCfgEntity();
    tses = staticConfigWrapper->GetTsEntities();

    DynamicConfigWrapperInterface& dynamicConfigWrapper = DynamicConfigWrapperInterface::GetInstance();
    dynamicConfigWrapper.Read(dynamicConfig, tses);

    return true;
}

bool Controller::CreatePlayers(shared_ptr<GlobalCfgEntity> globalCfg, list<shared_ptr<TsEntity>> tses, Players& players)
{
	uint32_t fileNumber = 0, finishedFileNumber = 0;
	DownloaderInterface::Handler handler = [&finishedFileNumber](std::shared_ptr<FtpResource> ftpResource, bool finished) ->void
    {
		if (finished)
		{
			++finishedFileNumber;
		}
    };
    shared_ptr<DownloaderInterface> downloader(CreateDownloaderInstance("DownloaderFtp", handler));

    /* download .ts and .jpg files */
    for (auto ts = tses.begin(); ts != tses.end(); ++ts)
    {
        list<shared_ptr<RefsEntity>>& refses = (*ts)->GetRefses();
        for (auto refs = refses.begin(); refs != refses.end(); ++refs)
        {
            list<shared_ptr<RefsEventEntity>>& refsEvents = (*refs)->GetRefsEvents();
            for (auto refsEvent = refsEvents.begin(); refsEvent != refsEvents.end(); ++refsEvent)
            {
                list<shared_ptr<PosterEntity>>& posters = (*refsEvent)->GetPosters();
                for (auto poster = posters.begin(); poster != posters.end(); ++poster)
                {
                    if (access((*poster)->GetLocalPath().c_str(), 0) != 0)
                    {
						++fileNumber;
						auto ftpResource = make_shared<FtpResource>((*poster)->GetId(),
                            (*poster)->GetRemotePath().c_str(),
                            (*poster)->GetLocalPath().c_str());
                        downloader->Download(ftpResource);
                    }
                }
                
                list<shared_ptr<MovieEntity>>& movies = (*refsEvent)->GetMovies();
                for (auto movie = movies.begin(); movie != movies.end(); ++movie)
                {
                    if (access((*movie)->GetLocalPath().c_str(), 0) != 0)
                    {
						++fileNumber;
						auto ftpResource = make_shared<FtpResource>((*movie)->GetId(),
                            (*movie)->GetRemotePath().c_str(),
                            (*movie)->GetLocalPath().c_str());
                        downloader->Download(ftpResource);
                    }
                }
            }            
        }
    }

    while (downloader->IsRunning())
    {
        this_thread::sleep_for(chrono::seconds(1));
    }

	if (fileNumber != finishedFileNumber)
	{
		errstrm << "some ftp downloading failed." << endl;
		return false;
	}

    /* create players */
    for (auto ts = tses.begin(); ts != tses.end(); ++ts)
    {
        shared_ptr<PlayerInterface> patPlayer(CreatePlayerInterface("PatPlayer", globalCfg, *ts));
        players.patPlayers.push_back(patPlayer);
        patPlayer->Start();
        
        list<shared_ptr<RefsEntity>>& refses = (*ts)->GetRefses();
        for (auto refs = refses.begin(); refs != refses.end(); ++refs)
        {
            shared_ptr<PlayerInterface> refsPlayer(CreatePlayerInterface("RefsPlayer", globalCfg, *refs));
            players.refsPlayers.push_back(refsPlayer);
            refsPlayer->Start();
        }

        list<shared_ptr<TmssEntity>>& tmsses = (*ts)->GetTmsses();
        for (auto tmss = tmsses.begin(); tmss != tmsses.end(); ++tmss)
        {
            shared_ptr<PlayerInterface> tmssPlayer(CreatePlayerInterface("TmssPlayer", *tmss));
            players.tmssPlayers.push_back(tmssPlayer);
            tmssPlayer->Start();
        }
    }

	return true;
}

void Controller::ClearPlayers(list<shared_ptr<PlayerInterface>>& players)
{
    for (auto player = players.begin(); player != players.end(); ++player)
    {
        (*player)->Stop();
    }
    players.clear();
}