#include "DownloaderFtp.h"

/* Foundation */
#include "SystemInclude.h"
#include "ClassFactories.h"
#include "Debug.h"

/* Entity */
#include "Entities.h"

using namespace std;

typedef ClassFactoriesRegistor<DownloaderInterface, std::string, DownloaderInterface::Handler> ClassFactoriesType;
RegisterClassFactory(ClassFactoriesType, reg, "DownloaderFtp", DownloaderFtp::CreateInstance);

/**********************class DownloaderFtp**********************/
DownloaderFtp::DownloaderFtp(DownloaderInterface::Handler handler)
: movieTuples(), movieEntities(), handler(handler), mtx()
{
    mcurl = curl_multi_init();
    assert(mcurl != nullptr);
    isOnGoing = false;
}

DownloaderFtp::~DownloaderFtp()
{
    if (threadMain.joinable())
    {
        threadMain.join();
    }
    CURLMcode mcurlCode = curl_multi_cleanup(mcurl);
    assert(mcurlCode == CURLM_OK);
}

void DownloaderFtp::Download(shared_ptr<MovieEntity> movieEntity)
{    
    lock_guard<mutex> lock(mtx);
    auto cmp = [movieEntity](shared_ptr<MovieEntity> iter)
    {
        return (movieEntity->GetId() == iter->GetId());
    };

    if (isOnGoing == false)
    {
        assert(movieEntities.empty() && movieTuples.empty());
        isOnGoing = true;
        threadMain = thread(bind(&DownloaderFtp::ThreadMain, this));
    }

    auto iter = find_if(movieEntities.begin(), movieEntities.end(), cmp);
    if (iter != movieEntities.end())
    {
        return;
    }
    movieEntities.push_back(movieEntity);
}

bool DownloaderFtp::IsRunning()
{
    lock_guard<mutex> lock(mtx);
    return isOnGoing;
}

void DownloaderFtp::Stop()
{
    lock_guard<mutex> lock(mtx);
    if (!isOnGoing)
    {
        return;
    }

    isOnGoing = false;
    threadMain.join();
}

/* private functions */
void DownloaderFtp::ThreadMain()
{
    unique_lock<mutex> lock(mtx, std::defer_lock);    
    int runningHandles = 1;
    while (runningHandles != 0)
    {        
        lock.lock();
        if (!isOnGoing)
        {
            break;
        }

        for (auto iter = movieEntities.begin(); iter != movieEntities.end(); ++iter)
        {
            ofstream *fstm = new ofstream((*iter)->GetLocalPath()->c_str(), ofstream::binary);
            assert(fstm != nullptr);

            CURL *curl = curl_easy_init();
            assert(curl != nullptr); 

            CURLcode curlCode;
            curlCode = curl_easy_setopt(curl, CURLOPT_URL, (*iter)->GetRemotePath()->c_str());
            assert(curl != nullptr);
            //curlCode = curl_easy_setopt(curl, CURLOPT_USERPWD, "anonymous:password");
            //assert(curlCode == CURLE_OK);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, DownloaderFtp::HandleCurlMessage);
            curlCode = curl_easy_setopt(curl, CURLOPT_WRITEDATA, fstm);
            assert(curlCode == CURLE_OK);
            //curlCode = curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
            //assert(curlCode == CURLE_OK);

            CURLMcode mcurlCode = curl_multi_add_handle(mcurl, curl);
            assert(mcurlCode == CURLM_OK);

            movieTuples.push_back(make_tuple(*iter, curl, fstm));
        }
        movieEntities.clear();
        lock.unlock();
        
        struct timeval timeout = { 0, 500 * 1000 };
        long curlTimeOut;
        CURLMcode mcurlCode = curl_multi_timeout(mcurl, &curlTimeOut);
        if (curlTimeOut >= 0)
        {
            /* no set timeout, use a default */
            timeout.tv_sec = curlTimeOut / 1000;
            timeout.tv_usec = (curlTimeOut % 1000) * 1000;
        }

        fd_set fdRead;
        fd_set fdWrite;
        fd_set fdExcep;        
        FD_ZERO(&fdRead);
        FD_ZERO(&fdWrite);
        FD_ZERO(&fdExcep);

        int maxFd = -1;
        mcurlCode = curl_multi_fdset(mcurl, &fdRead, &fdWrite, &fdExcep, &maxFd);
        assert(mcurlCode == CURLM_OK);
        if (maxFd == -1)
        {
            this_thread::sleep_for(chrono::milliseconds(100));
        }
        else
        {
            /* wait for activities no longer than the set timeout */
            int retCode = select(maxFd + 1, &fdRead, &fdWrite, &fdExcep, &timeout);
            assert(retCode != -1);
        }

        lock.lock();
        int msgq = 0;
        for (CURLMsg *m = curl_multi_info_read(mcurl, &msgq); 
            m != nullptr; 
            m = curl_multi_info_read(mcurl, &msgq))
        {
            if (m->msg != CURLMSG_DONE)
                continue;

            CURL *curl = m->easy_handle;
            auto cmp = [curl](MovieTuple& i)->bool { return std::get<TupleIdxCurlPtr>(i) == curl; };
            MovieTuple movieTuple = *find_if(movieTuples.begin(), movieTuples.end(), cmp);
            handler(std::get<TupleIdxMovieEntity>(movieTuple));
        }
        lock.unlock();

        mcurlCode = curl_multi_perform(mcurl, &runningHandles);
        assert(mcurlCode == CURLM_OK || mcurlCode == CURLM_CALL_MULTI_PERFORM);        
    } /* while (isOnGoing) */

    lock.lock();
    isOnGoing = false;
    movieEntities.clear();

    auto CleanUp = [](MovieTuple& movieTuple)
    {
        curl_easy_cleanup(std::get<TupleIdxCurlPtr>(movieTuple));
        std::get<TupleIdxCurlPtr>(movieTuple) = nullptr;

        std::get<TupleIdxFilePtr>(movieTuple)->close();
        delete std::get<TupleIdxFilePtr>(movieTuple);
        std::get<TupleIdxFilePtr>(movieTuple) = nullptr;
    };
    for_each(movieTuples.begin(), movieTuples.end(), CleanUp);
    movieTuples.clear();
    lock.unlock();
}

size_t DownloaderFtp::HandleCurlMessage(void *buffer, size_t size, size_t nmemb, void *para)
{
    std::ofstream* fstm = reinterpret_cast<std::ofstream*>(para);
    fstm->write((char*)buffer, size * nmemb);
    return (size * nmemb);
}