#include "EntityHelper.h"

/* Cpp Unit */
#include <cppunit/extensions/HelperMacros.h>

/* Type */
#include "BaseType.h"

/* Foundation */
#include "SystemInclude.h"

/* Functions */
#include "TableIndexHelperInterface.h"

using std::chrono::seconds;
using std::chrono::milliseconds;
using std::chrono::system_clock;

using namespace std;

struct RefsEntry
{
    ServiceId refsSvcId;
    list<EventId> refsEvents;
};

struct PstsEntry
{
	ServiceId pstsSvcId;
	Pid pstsPmtPid;
	Pid pstsPosterPid;
};

struct TmssEntry
{
    ServiceId tmssSvcId;
    seconds offset;
    Pid PmtPid;
    Pid PcrPid;
    Pid AudioPid;
    Pid VideoPid;
    ServiceId refsSvcId;
    list<EventId> tmssEvents;
};

/**********************EntityHelper**********************/
EntityHelper::EntityHelper() 
{
    TableIndexHelperInterface &tableIndexHelper = TableIndexHelperInterface::GetInstance();
    TableId tableId;
    TsId tsId = 1;
    ServiceId refsSvcId = 1;

	RefsEntry refsEntry = { 1, { 1 } };
	PstsEntry pstsEntry = { 2, 201, 202 };

    EventId eventId = 0;
    list<TmssEntry> tmssEntries =
    {
        { 3, seconds(5), 301, 302, 303, 304, refsSvcId, { ++eventId, ++eventId } },
        { 4, seconds(10), 401, 402, 403, 404, refsSvcId, { ++eventId } },
        { 5, seconds(15), 501, 502, 503, 504, refsSvcId, { ++eventId } },
        { 6, seconds(20), 601, 602, 603, 604, refsSvcId, { ++eventId } },
        { 7, seconds(25), 701, 702, 703, 704, refsSvcId, { ++eventId } },
        { 8, seconds(30), 801, 802, 803, 804, refsSvcId, { ++eventId } },
        { 9, seconds(35), 901, 902, 903, 904, refsSvcId, { ++eventId } },
        { 10, seconds(40), 1001, 1002, 1003, 1004, refsSvcId, { ++eventId } },
    };

    system_clock::time_point now = chrono::steady_clock::now();

	/* TransportStream */
	in_addr srcAddr;
	srcAddr.s_addr = inet_addr("127.0.0.1");
	sockaddr_in dstAddr;
	dstAddr.sin_family = AF_INET;
	dstAddr.sin_port = htons(1234);
	dstAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	tableId = tableIndexHelper.GetUseableTableIndex("Ts");
    tsEntity = make_shared<TsEntity>(tableId, tsId, srcAddr, dstAddr);

    /* Refs */
    tableId = tableIndexHelper.GetUseableTableIndex("Refs");
	refsEntity = make_shared<RefsEntity>(tableId, refsEntry.refsSvcId);
    tsEntity->Bind(refsEntity);

	/* Psts, same table id with Refs */
	shared_ptr<PstsEntity> pstsEntity = make_shared<PstsEntity>(tableId, pstsEntry.pstsSvcId,
		pstsEntry.pstsPmtPid, 0x80, pstsEntry.pstsPosterPid);
	refsEntity->SetPsts(pstsEntity);

    /* RefsEvent */
    system_clock::time_point tm = now;
	for (auto i = refsEntry.refsEvents.begin(); i != refsEntry.refsEvents.end(); ++i)
    {
        tableId = tableIndexHelper.GetUseableTableIndex("RefsEvent");
        shared_ptr<RefsEventEntity> refsEventEntity = make_shared<RefsEventEntity>(tableId, *i,
            tm, seconds(3600 * 24));
        refsEventEntity->SetRefs(refsEntity);
        tm = tm + seconds(3600 * 24);

        /* Movie */
        MovieId movie1Id = 1001, movie2Id = 1002;
        tableId = tableIndexHelper.GetUseableTableIndex("MovieOrPoster");
        shared_ptr<MovieEntity> movieEntity1 = make_shared<MovieEntity>(tableId, movie1Id, "ftp://127.0.0.1/sample1.ts");
        ostringstream movieOs1;
        movieOs1 << "Movies/" << movie1Id << ".ts";
        movieEntity1->SetLocalPath(movieOs1.str());
        refsEventEntity->Bind(movieEntity1);
        tableId = tableIndexHelper.GetUseableTableIndex("MovieOrPoster");
        shared_ptr<MovieEntity> movieEntity2 = make_shared<MovieEntity>(tableId, movie2Id, "ftp://127.0.0.1/sample2.ts");
        ostringstream os2;
        os2 << "Movies/" << movie2Id << ".ts";
        movieEntity2->SetLocalPath(os2.str());
        refsEventEntity->Bind(movieEntity2);

        /* PosterEntity */
        PosterId posterId = 1001;
        tableId = tableIndexHelper.GetUseableTableIndex("MovieOrPoster");
        shared_ptr<PosterEntity> posterEntity = make_shared<PosterEntity>(tableId, posterId, "ftp://127.0.0.1/sample1.jpg");
        ostringstream postOs;
        postOs << "Movies/" << posterId << ".jpg";
        posterEntity->SetLocalPath(postOs.str());
        refsEventEntity->Bind(posterEntity);
    }  

    /* Tmss */
    for (auto iter = tmssEntries.begin(); iter != tmssEntries.end(); ++iter)
    {
        tableId = tableIndexHelper.GetUseableTableIndex("Tmss");
		shared_ptr<TmssEntity> tmssEntity = make_shared<TmssEntity>(tableId, iter->tmssSvcId,
			iter->PmtPid, iter->PcrPid, iter->AudioPid, iter->VideoPid);
        tsEntity->Bind(tmssEntity);

        /* TmssEvent */
        system_clock::time_point tm = now + iter->offset;
        for (auto i = iter->tmssEvents.begin(); i != iter->tmssEvents.end(); ++i)
        {
            tableId = tableIndexHelper.GetUseableTableIndex("TmssEvent");
            shared_ptr<TmssEventEntity> tmssEventEntity = make_shared<TmssEventEntity>(tableId, *i,
                tm, seconds(600));
            tmssEventEntity->SetTmss(tmssEntity);
			tmssEventEntity->SetRefsEvent(*refsEntity->GetRefsEvents().begin());

			tm = tm + seconds(600);
        }
    }

    /* GlobalCfg */
    tableId = tableIndexHelper.GetUseableTableIndex("GlobalCfg");
    globalCfgEntity = make_shared<GlobalCfgEntity>(tableId,
        milliseconds(100), milliseconds(100), milliseconds(100));
}

EntityHelper::~EntityHelper() 
{}

shared_ptr<GlobalCfgEntity> EntityHelper::GetGlobalCfgEntity()
{
    return globalCfgEntity;
}

shared_ptr<TsEntity> EntityHelper::GetTsEntity()
{
    return tsEntity;
}

shared_ptr<RefsEntity> EntityHelper::GetRefsEntity()
{
    return refsEntity;
}