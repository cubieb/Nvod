#ifndef _DownloaderFtp_h_
#define _DownloaderFtp_h_

/* Curl*/
#include <curl/curl.h>

/* Entity */
#include "Entities.h"

/* Functions */
#include "DownloaderInterface.h"

using std::shared_ptr;

/**********************class DownloaderFtp**********************/
class DownloaderFtp : public DownloaderInterface
{
public:
    DownloaderFtp(DownloaderInterface::Handler handler);
    ~DownloaderFtp();
    static DownloaderFtp* CreateInstance(DownloaderInterface::Handler handler)
    {
        return new DownloaderFtp(handler);
    }

    void Download(shared_ptr<MovieEntity> movieEntity);
    bool IsRunning();
    void Stop();

private:
    void ThreadMain();
    static size_t HandleCurlMessage(void *buffer, size_t size, size_t nmemb, void *para);

private:
    #define TupleIdxMovieEntity 0
    #define TupleIdxCurlPtr     1
    #define TupleIdxFilePtr     2
    typedef std::tuple<shared_ptr<MovieEntity>, CURL*, std::ofstream*> MovieTuple;
    std::list<MovieTuple> movieTuples;
    std::list<shared_ptr<MovieEntity>> movieEntities;
    Handler handler;
    std::mutex mtx;
    CURLM *mcurl;
    bool isOnGoing;

private:
    std::thread threadMain;
};

#endif