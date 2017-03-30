#include "EntityHelper.h"

/* Cpp Unit */
#include <cppunit/extensions/HelperMacros.h>

/* Foundation */
#include "SystemInclude.h"

/* Entity */
#include "Entities.h"

/* Functions */
#include "TableIndexHelperInterface.h"

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
	Duration offset;
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
    //ServiceId tmssSvcId = 20;
    //EventId eventId = 1;

    Pid refsPmtPid    = 0x1010;
    Pid refsPosterPid = 0x1011;

	RefsEntry refsEntry = { 1, { 1 } };
	PstsEntry pstsEntry = { 101, 0x1001, 0x1101 };

    list<TmssEntry> tmssEntries =
    {
        { 2, Seconds(0),  0x1202, 0x1302, 0x1402, 0x1502, 1, { 1, 2 } },
		{ 3, Seconds(5),  0x1203, 0x1303, 0x1403, 0x1503, 1, { 4, 5 } },
		{ 4, Seconds(10), 0x1204, 0x1304, 0x1404, 0x1504, 1, { 7 } },
		{ 5, Seconds(15), 0x1205, 0x1305, 0x1405, 0x1505, 1, { 10 } },
		{ 6, Seconds(20), 0x1206, 0x1306, 0x1406, 0x1506, 1, { 13 } },
		{ 7, Seconds(25), 0x1207, 0x1307, 0x1407, 0x1507, 1, { 16 } },
        { 8, Seconds(600), 0x1208, 0x1308, 0x1408, 0x1508, 1, { 19 } },
        { 9, Seconds(605), 0x1209, 0x1309, 0x1409, 0x1509, 1, { 22 } },
        { 10, Seconds(610), 0x1210, 0x1310, 0x1410, 0x1510, 1, { 25 } },
    };

    TimePoint now = chrono::steady_clock::now();

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
    TimePoint tm = now;
	for (auto i = refsEntry.refsEvents.begin(); i != refsEntry.refsEvents.end(); ++i)
    {
        tableId = tableIndexHelper.GetUseableTableIndex("RefsEvent");
        shared_ptr<RefsEventEntity> refsEventEntity = make_shared<RefsEventEntity>(tableId, *i,
            tm, Seconds(3600 * 24));
        refsEventEntity->SetRefs(refsEntity);
        tm = tm + Seconds(3600 * 24);

        /* Movie */
        MovieId movie1Id = 1001, movie2Id = 1002;
        tableId = tableIndexHelper.GetUseableTableIndex("Movie");
        shared_ptr<MovieEntity> movieEntity1 = make_shared<MovieEntity>(tableId, movie1Id, "ftp://127.0.0.1/sample1.ts");
        ostringstream movieOs1;
        movieOs1 << "Movies\\" << movie1Id << ".ts";
        movieEntity1->SetLocalPath(movieOs1.str());
        refsEventEntity->Bind(movieEntity1);
        //tableId = tableIndexHelper.GetUseableTableIndex("Movie");
        //shared_ptr<MovieEntity> movieEntity2 = make_shared<MovieEntity>(tableId, movie2Id, "ftp://127.0.0.1/sample1.ts");
        //ostringstream os2;
        //os2 << "Movies\\" << movie2Id << ".ts";
        //movieEntity2->SetLocalPath(os2.str());
        //refsEventEntity->Bind(movieEntity2);

        /* PosterEntity */
        PosterId posterId = 1001;
        tableId = tableIndexHelper.GetUseableTableIndex("Poster");
        shared_ptr<PosterEntity> posterEntity = make_shared<PosterEntity>(tableId, posterId, "ftp://127.0.0.1/sample1.jpg");
        ostringstream postOs;
        postOs << "Movies\\" << posterId << ".jpg";
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
        TimePoint tm = now + iter->offset;
        for (auto i = iter->tmssEvents.begin(); i != iter->tmssEvents.end(); ++i)
        {
            tableId = tableIndexHelper.GetUseableTableIndex("TmssEvent");
            shared_ptr<TmssEventEntity> tmssEventEntity = make_shared<TmssEventEntity>(tableId, *i,
                tm, Seconds(600));
            tmssEventEntity->SetTmss(tmssEntity);
			tmssEventEntity->SetRefsEvent(*refsEntity->GetRefsEvents().begin());

			tm = tm + Seconds(600);
        }
    }

    /* GlobalCfg */
    tableId = tableIndexHelper.GetUseableTableIndex("GlobalCfg");
    globalCfgEntity = make_shared<GlobalCfgEntity>(tableId,
        Milliseconds(100), Milliseconds(100), Milliseconds(100));
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