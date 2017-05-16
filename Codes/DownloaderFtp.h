#ifndef _DownloaderFtp_h_
#define _DownloaderFtp_h_

/* Curl*/
#include <curl/curl.h>

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

    void Download(shared_ptr<FtpResource> ftpResource);
    bool IsRunning();
    void Stop();

private:
    #define TupleIdxMovieEntity 0
    #define TupleIdxCurlPtr     1
    #define TupleIdxFilePtr     2
    typedef std::tuple<shared_ptr<FtpResource>, CURL*, std::ofstream*> FtpResourceTuple;

    void ThreadMain();
    void CloseDoneCurl(CURLM *mcurl, std::list<FtpResourceTuple>& ftpResourceTuples);
    static size_t HandleCurlMessage(void *buffer, size_t size, size_t nmemb, void *para);

private:    
    std::mutex mtx;
    bool isOnGoing;
    Handler handler;
    std::list<shared_ptr<FtpResource>> ftpResources;

private:
    std::thread threadMain;
};

#endif