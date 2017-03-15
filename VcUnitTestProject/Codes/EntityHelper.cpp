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
    Pid refsPmtPid;
    Pid refsPosterPid;
    list<EventId> refsEvents;
};

struct TmssEntry
{
    ServiceId tmssSvcId;
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
    ServiceId tmssSvcId = 20;
    EventId eventId = 1;

    Pid refsPmtPid    = 0x1010;
    Pid refsPosterPid = 0x1011;

	RefsEntry refsEntry = { 1, 0x1001, 0x1101, { 1 } };
    list<TmssEntry> tmssEntries =
    {
        { 2, 0x1202, 0x1302, 0x1402, 0x1502, 1, { 1, 2, 3 } },
        //{ 3, 0x1203, 0x1303, 0x1403, 0x1503, 2, { 4, 5, 6 } }
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
	refsEntity = make_shared<RefsEntity>(tableId, tsId, refsEntry.refsSvcId,
		refsEntry.refsPmtPid, 0x80, refsEntry.refsPosterPid);
    tsEntity->Bind(refsEntity);

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
        ostringstream os1;
        os1 << "Movies\\" << movie1Id << ".ts";
        movieEntity1->SetLocalPath(os1.str());
        refsEventEntity->Bind(movieEntity1);
        tableId = tableIndexHelper.GetUseableTableIndex("Movie");
        shared_ptr<MovieEntity> movieEntity2 = make_shared<MovieEntity>(tableId, movie2Id, "ftp://127.0.0.1/sample1.ts");
        ostringstream os2;
        os2 << "Movies\\" << movie2Id << ".ts";
        movieEntity2->SetLocalPath(os2.str());
        refsEventEntity->Bind(movieEntity2);

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
		shared_ptr<TmssEntity> tmssEntity = make_shared<TmssEntity>(tableId, tsId, iter->tmssSvcId,
			iter->PmtPid, iter->PcrPid, iter->AudioPid, iter->VideoPid);
		if (this->tmssEntity == nullptr)
		{
			this->tmssEntity = tmssEntity;
		}
        tsEntity->Bind(tmssEntity);

        /* TmssEvent */
        TimePoint tm = now + Seconds(5);
        for (auto i = iter->tmssEvents.begin(); i != iter->tmssEvents.end(); ++i)
        {
            tableId = tableIndexHelper.GetUseableTableIndex("TmssEvent");
            shared_ptr<TmssEventEntity> tmssEventEntity = make_shared<TmssEventEntity>(tableId, *i,
                tm, Seconds(3600));
            tmssEventEntity->SetTmss(tmssEntity);

            ServiceId refsSvcId = iter->refsSvcId;
            auto cmp = [refsSvcId](shared_ptr<RefsEntity> right) ->bool
            {
                return refsSvcId == *right->GetServiceId();
            };
            auto refs = find_if(tsEntity->GetRefses().begin(), tsEntity->GetRefses().end(), cmp);
			tmssEventEntity->SetRefsEvent(*refsEntity->GetRefsEvents().begin());

            tm = tm + Seconds(3600);
        }
    }

    /* GlobalCfg */
    tableId = tableIndexHelper.GetUseableTableIndex("GlobalCfg");
    globalCfgEntity = make_shared<GlobalCfgEntity>(tableId,
        Milliseconds(500), Milliseconds(500), Milliseconds(500));
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

shared_ptr<TmssEntity> EntityHelper::GetTmssEntity()
{
    return tmssEntity;
}