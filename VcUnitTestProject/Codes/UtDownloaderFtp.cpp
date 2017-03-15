#include "UtDownloaderFtp.h"

/* Cpp Unit */
#include <cppunit/extensions/HelperMacros.h>

/* Foundation */
#include "SystemInclude.h"

/* Entity */
#include "Entities.h"

/* Functions */
#include "DownloaderInterface.h"

/* local head files */
#include "EntityHelper.h"

using namespace std;

CxxBeginNameSpace(UnitTest)

/**********************UtDownloaderFtp**********************/
CPPUNIT_TEST_SUITE_REGISTRATION(UtDownloaderFtp);

/* public function */
void UtDownloaderFtp::setUp()
{
    entityHelper = make_shared<EntityHelper>();
}

/* protected function */
void UtDownloaderFtp::TestDownload()
{
    //DownloaderInterface::Handler handler = [](std::shared_ptr<MovieEntity> movieEntity) ->void
    //{
    //    cout << movieEntity->GetLocalPath()->c_str() << endl;
    //};

    //shared_ptr<DownloaderInterface> downloader(CreateDownloaderInstance("DownloaderFtp", handler));

    //shared_ptr<RefsEntity> refsEntity = entityHelper->GetRefsEntity();
    //list<weak_ptr<RefsEventEntity>>& refsEvents = refsEntity->GetRefsEvents();
    //for (auto e = refsEvents.begin(); e != refsEvents.end(); ++e)
    //{
    //    list<shared_ptr<MovieEntity>>& movies = e->lock()->GetMovies();
    //    for (auto m = movies.begin(); m != movies.end(); ++m)
    //    {
    //        if (access((*m)->GetLocalPath()->c_str(), 0) != 0)
    //        {
    //            downloader->Download(*m);
    //        }
    //    }
    //}
    //
    //while (downloader->IsRunning())
    //{
    //    this_thread::sleep_for(chrono::seconds(1));
    //}
}

CxxEndNameSpace