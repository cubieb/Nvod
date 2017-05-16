#include "UtDownloaderFtp.h"

/* Cpp Unit */
#include <cppunit/extensions/HelperMacros.h>

/* Foundation */
#include "SystemInclude.h"

/* Functions */
#include "DownloaderInterface.h"

using namespace std;

CxxBeginNameSpace(UnitTest)

/**********************UtDownloaderFtp**********************/
CPPUNIT_TEST_SUITE_REGISTRATION(UtDownloaderFtp);

/* public function */
void UtDownloaderFtp::setUp()
{
    TableId tableId = 1;
    MovieId movie1Id = 1001, movie2Id = 1002;
    
    ostringstream movieOs1;
    movieOs1 << "Movies/" << movie1Id << ".ts";
    shared_ptr<FtpResource> ftpResource1 = make_shared<FtpResource>(movie1Id,
        "ftp://127.0.0.1/sample1.ts", movieOs1.str().c_str());
    ftpResources.push_back(ftpResource1);

    ostringstream os2;
    os2 << "Movies/" << movie2Id << ".ts";
    shared_ptr<FtpResource> ftpResource2 = make_shared<FtpResource>(movie2Id,
        "ftp://127.0.0.1/sample2.ts", os2.str().c_str());
    ftpResources.push_back(ftpResource2);
    
    for (auto ftpResource = ftpResources.begin(); ftpResource != ftpResources.end(); ++ftpResource)
    {
        if (access((*ftpResource)->GetLocalPath().c_str(), 0) == 0)
        {
            int ret = remove((*ftpResource)->GetLocalPath().c_str());
            assert(ret == 0);
        }
    }
}

/* protected function */
void UtDownloaderFtp::TestDownload()
{
	DownloaderInterface::Handler handler = [](std::shared_ptr<FtpResource> ftpResource, bool finished) ->void
    {
		cout << "Done, local path = " << ftpResource->GetLocalPath().c_str() << endl;
    };

    shared_ptr<DownloaderInterface> downloader(CreateDownloaderInstance("DownloaderFtp", handler));

    for (auto ftpResource = ftpResources.begin(); ftpResource != ftpResources.end(); ++ftpResource)
    {
        CPPUNIT_ASSERT(_access((*ftpResource)->GetLocalPath().c_str(), 0) != 0);
        downloader->Download(*ftpResource);
    }
    
    while (downloader->IsRunning())
    {
        this_thread::sleep_for(chrono::seconds(1));
    }

    for (auto ftpResource = ftpResources.begin(); ftpResource != ftpResources.end(); ++ftpResource)
    {
        CPPUNIT_ASSERT(access((*ftpResource)->GetLocalPath().c_str(), 0) == 0);
    }
}

CxxEndNameSpace