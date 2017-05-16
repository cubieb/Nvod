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
    : handler(handler), ftpResources()
{
    isOnGoing = false;
}

DownloaderFtp::~DownloaderFtp()
{
    if (threadMain.joinable())
    {
        threadMain.join();
    }    
}

void DownloaderFtp::Download(shared_ptr<FtpResource> ftpResource)
{    
    lock_guard<mutex> lock(mtx);
    auto cmp = [ftpResource](shared_ptr<FtpResource> iter)
    {
        return (ftpResource->GetId() == iter->GetId());
    };

    if (isOnGoing == false)
    {
        assert(ftpResources.empty());
        isOnGoing = true;
        threadMain = thread(bind(&DownloaderFtp::ThreadMain, this));
    }

    auto iter = find_if(ftpResources.begin(), ftpResources.end(), cmp);
    if (iter != ftpResources.end())
    {
        return;
    }
    ftpResources.push_back(ftpResource);
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
    CURLM *mcurl = curl_multi_init();
    assert(mcurl != nullptr);
    
    std::list<FtpResourceTuple> ftpResourceTuples;
    
    unique_lock<mutex> lock(mtx, std::defer_lock);    
    int runningHandles = 1;
    while (runningHandles != 0)
    {        
        lock.lock();
        if (!isOnGoing)
        {
            break;
        }

        for (auto iter = ftpResources.begin(); iter != ftpResources.end(); ++iter)
        {
            ofstream *fstm = new ofstream((*iter)->GetLocalPath().c_str(), ofstream::binary);
            assert(fstm != nullptr);

            CURL *curl = curl_easy_init();
            assert(curl != nullptr); 

            CURLcode curlCode;
            curlCode = curl_easy_setopt(curl, CURLOPT_URL, (*iter)->GetRemotePath().c_str());
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

            ftpResourceTuples.push_back(make_tuple(*iter, curl, fstm));
        }
        ftpResources.clear();
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
        CloseDoneCurl(mcurl, ftpResourceTuples);

        mcurlCode = curl_multi_perform(mcurl, &runningHandles);
        assert(mcurlCode == CURLM_OK || mcurlCode == CURLM_CALL_MULTI_PERFORM);
    } /* while (isOnGoing) */

    lock.lock();    
    isOnGoing = false;
    
    CloseDoneCurl(mcurl, ftpResourceTuples);
    if (!ftpResourceTuples.empty())
    {
        /* if this function exit because of the calling of Stop(), we need clear up the ftpResourceTuples */
		auto CleanUp = [this](FtpResourceTuple& ftpResourceTuple)
        {
			handler(std::get<TupleIdxMovieEntity>(ftpResourceTuple), false);

			curl_easy_cleanup(std::get<TupleIdxCurlPtr>(ftpResourceTuple));
			std::get<TupleIdxCurlPtr>(ftpResourceTuple) = nullptr;

			std::get<TupleIdxFilePtr>(ftpResourceTuple)->close();
			delete std::get<TupleIdxFilePtr>(ftpResourceTuple);
			std::get<TupleIdxFilePtr>(ftpResourceTuple) = nullptr;
        };
        for_each(ftpResourceTuples.begin(), ftpResourceTuples.end(), CleanUp);
        ftpResourceTuples.clear();
    }
    ftpResources.clear();
    lock.unlock();

    CURLMcode mcurlCode = curl_multi_cleanup(mcurl);
    assert(mcurlCode == CURLM_OK);
}

void DownloaderFtp::CloseDoneCurl(CURLM *mcurl, std::list<FtpResourceTuple>& ftpResourceTuples)
{
    int msgq = 0;
    for (CURLMsg *m = curl_multi_info_read(mcurl, &msgq);
        m != nullptr;
        m = curl_multi_info_read(mcurl, &msgq))
    {
        if (m->msg != CURLMSG_DONE)
            continue;

        CURL *curl = m->easy_handle;
        auto cmp = [curl](FtpResourceTuple& i)->bool { return std::get<TupleIdxCurlPtr>(i) == curl; };
		auto ftpResourceTuple = find_if(ftpResourceTuples.begin(), ftpResourceTuples.end(), cmp);
		assert(ftpResourceTuple != ftpResourceTuples.end());

		std::get<TupleIdxFilePtr>(*ftpResourceTuple)->close();
		delete std::get<TupleIdxFilePtr>(*ftpResourceTuple);
		std::get<TupleIdxFilePtr>(*ftpResourceTuple) = nullptr;

        double filesize = 0.0;
        CURLcode curlCode = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &filesize);
        dbgstrm << "curlCode = " << curlCode << ", filesize = " << filesize << endl;
        // if file not existed, the filesize should be -1. for simple and convenient, we 
        // assume all file size > 16 byte.
        if (curlCode == CURLE_OK && filesize > 16)
        {   
			handler(std::get<TupleIdxMovieEntity>(*ftpResourceTuple), true);
        }
        else
        {
			/* delete the empty file (file size is zero). */
			remove(std::get<TupleIdxMovieEntity>(*ftpResourceTuple)->GetLocalPath().c_str());
			handler(std::get<TupleIdxMovieEntity>(*ftpResourceTuple), false);
        }

		curl_easy_cleanup(std::get<TupleIdxCurlPtr>(*ftpResourceTuple));
		std::get<TupleIdxCurlPtr>(*ftpResourceTuple) = nullptr;
		ftpResourceTuples.erase(ftpResourceTuple);
    }
}

size_t DownloaderFtp::HandleCurlMessage(void *buffer, size_t size, size_t nmemb, void *para)
{
    std::ofstream* fstm = reinterpret_cast<std::ofstream*>(para);
    fstm->write((char*)buffer, size * nmemb);
    return (size * nmemb);
}