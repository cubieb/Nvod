#ifndef _DownloaderInterface_h_
#define _DownloaderInterface_h_

/* Foundation */
#include "SystemInclude.h"
#include "ClassFactories.h"

/* Entity */
#include "BaseType.h"

class MovieEntity;

/**********************class DownloaderInterface**********************/
class DownloaderInterface
{
public:
    typedef std::function<void(std::shared_ptr<MovieEntity>)> Handler;

    DownloaderInterface() {}
    virtual ~DownloaderInterface() {}

    virtual void Download(shared_ptr<MovieEntity> movieEntity) = 0;
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