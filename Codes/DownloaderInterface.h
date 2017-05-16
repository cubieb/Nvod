#ifndef _DownloaderInterface_h_
#define _DownloaderInterface_h_

/* Foundation */
#include "SystemInclude.h"
#include "ClassFactories.h"

/* Entity */
#include "BaseType.h"

/**********************class FtpResource**********************/
class FtpResource
{
public:
    FtpResource(TableId id, const char *remotePath, const char *localPath)
        : id(id), remotePath(remotePath), localPath(localPath)
    {}
	TableId GetId() const
    {
        return id;
    }

    const std::string& GetRemotePath() const
    {
        return remotePath;
    }

    const std::string& GetLocalPath() const
    {
        return localPath;
    }

private:
	TableId id;
    std::string remotePath;
    std::string localPath;  /* tansient, not null */
};

/**********************class DownloaderInterface**********************/
class DownloaderInterface
{
public:
    typedef std::function<void(std::shared_ptr<FtpResource>, bool)> Handler;

    DownloaderInterface() {}
    virtual ~DownloaderInterface() {}

    virtual void Download(std::shared_ptr<FtpResource> movieEntity) = 0;
    virtual bool IsRunning() = 0;
    virtual void Stop() = 0;
};

/**********************class DownloaderInterface Factory**********************/
template<typename ... Types>
DownloaderInterface* CreateDownloaderInstance(const char *downloaderName, Types ... args)
{
    typedef ClassFactories<DownloaderInterface, std::string, Types ...> ClassFactoriesType;
    ClassFactoriesType& instance = ClassFactoriesType::GetInstance();

    return instance.CreateInstance(downloaderName, args ...);
}

#endif