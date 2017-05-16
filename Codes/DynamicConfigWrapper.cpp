#include "DynamicConfigWrapper.h"

/* libxml */
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

/* Foundation */
#include "SystemInclude.h"
#include "Debug.h"
#include "XmlHelper.h"

/* Entity */
#include "Entities.h"

/* Functions */
#include "TableIndexHelperInterface.h"

using namespace std;
using namespace std::placeholders;

/**********************class DynamicEventWrapperInterface**********************/
DynamicConfigWrapperInterface& DynamicConfigWrapperInterface::GetInstance()
{
    /* In C++11, the following is guaranteed to perform thread-safe initialisation: */
    static DynamicConfigWrapper instance;
    return instance;
}

/**********************class StaticConfigWrapper**********************/
void DynamicConfigWrapper::Read(const char *xmlPath, list<shared_ptr<TsEntity>>& tsEntities)
{
    if ((access(xmlPath, 0)) != 0)
    {
        errstrm << "Error when reading " << xmlPath << endl;
        return;
    }

    auto docDeleter = [](xmlDoc* ptr){ xmlFreeDoc(ptr); };
    shared_ptr<xmlDoc> doc(xmlParseFile(xmlPath), docDeleter);
    if (doc == nullptr)
    {
        errstrm << "Error when reading " << xmlPath << endl;
        return;
    }

    xmlNodePtr node = xmlDocGetRootElement(doc.get());
    auto xpathCtxDeleter = [](xmlXPathContext* ptr){ xmlXPathFreeContext(ptr); };
    shared_ptr<xmlXPathContext> xpathCtx(xmlXPathNewContext(doc.get()), xpathCtxDeleter);
    if (xpathCtx == nullptr)
    {
        errstrm << "Error when reading " << xmlPath << endl;
        return;
    }

    /* ReferenceServiceList */
    xmlChar *xpathExpr = (xmlChar*)"/root/ReferenceServiceList/ReferenceService[*]";
    auto xpathObjectDeleter = [](xmlXPathObject* ptr){ xmlXPathFreeObject(ptr); };
    shared_ptr<xmlXPathObject> xpathObj(xmlXPathEvalExpression(xpathExpr, xpathCtx.get()), xpathObjectDeleter);
    xmlNodeSetPtr nodes = xpathObj->nodesetval;
    if (nodes == nullptr)
    {
        errstrm << "Error when reading " << xmlPath << endl;
        return;
    }

    TableIndexHelperInterface &tableIndexHelper = TableIndexHelperInterface::GetInstance();

    auto cmpts = [](TsId tsId, shared_ptr<TsEntity> tsEntity) ->bool
    { return tsId == tsEntity->GetTsId(); };

    auto cmprefs = [](ServiceId serviceId, shared_ptr<RefsEntity> refs) ->bool
    { return serviceId == refs->GetServiceId(); };

    auto cmprefsevent = [](EventId eventId, shared_ptr<RefsEventEntity> refsEvent) ->bool
    { return eventId == refsEvent->GetEventId(); };

    auto cmptmss = [](ServiceId serviceId, shared_ptr<TmssEntity> tmss) ->bool
    { return serviceId == tmss->GetServiceId(); };
    
    for (int i = 0; i < nodes->nodeNr; ++i)
    {
        xmlNodePtr node = nodes->nodeTab[i];
        TsId tsId = GetXmlAttrValue<TsId>(node, (const xmlChar*)"TsId");
        ServiceId serviceId = GetXmlAttrValue<TsId>(node, (const xmlChar*)"ServiceID");
        VersionNumber version = GetXmlAttrValue<VersionNumber>(node, (const xmlChar*)"Version");

        auto iterTs = find_if(tsEntities.begin(), tsEntities.end(), bind(cmpts, tsId, _1));
        if (iterTs == tsEntities.end())
            continue;

        list<shared_ptr<RefsEntity>>& refses = (*iterTs)->GetRefses();
        auto iterRefs = find_if(refses.begin(), refses.end(), bind(cmprefs, serviceId, _1));
        if (iterRefs == refses.end())
            continue;

        /* Event */
        for (node = xmlFirstElementChild(node); node != nullptr; node = xmlNextElementSibling(node))
        {
            EventId eventId = GetXmlAttrValue<TsId>(node, (const xmlChar*)"Id");
            SharedXmlChar startTime = GetXmlAttrValue<SharedXmlChar>(node, (const xmlChar*)"StartTime");
            uint32_t duration = GetXmlAttrValue<uint32_t>(node, (const xmlChar*)"Duration");

            TableId tableId = tableIndexHelper.GetUseableTableIndex("RefsEvent");

            tm timeInfo = { 0 };
            std::istringstream ss((const char*)startTime.get());
            ss >> std::get_time(&timeInfo, "%Y-%m-%d %H:%M:%S");
            /* refer to http://www.cplusplus.com/reference/ctime/gmtime/ */
            shared_ptr<RefsEventEntity> refsEvent = make_shared<RefsEventEntity>(tableId, eventId,
                system_clock::from_time_t(mktime(&timeInfo)), seconds(duration));
            (*iterRefs)->Bind(refsEvent);

            /* Poster and Movie */
            for (xmlNodePtr child = xmlFirstElementChild(node); child != nullptr; child = xmlNextElementSibling(child))
            {
                if (strcmp("Poster", (const char*)child->name) == 0)
                {
                    PosterId posterId = GetXmlAttrValue<PosterId>(child, (const xmlChar*)"Id");
                    SharedXmlChar url = GetXmlContent<SharedXmlChar>(child);

                    tableId = tableIndexHelper.GetUseableTableIndex("MovieOrPoster");
                    shared_ptr<PosterEntity> poster = make_shared<PosterEntity>(tableId, posterId, (const char*)url.get());

                    ostringstream postOs;
                    postOs << "Movies\\" << posterId << ".jpg";
                    poster->SetLocalPath(postOs.str());

                    refsEvent->Bind(poster);
                }
                else
                {
                    MovieId movieId = GetXmlAttrValue<MovieId>(child, (const xmlChar*)"Id");
                    SharedXmlChar url = GetXmlContent<SharedXmlChar>(child);

                    tableId = tableIndexHelper.GetUseableTableIndex("MovieOrPoster");
                    shared_ptr<MovieEntity> movie = make_shared<MovieEntity>(tableId, movieId, (const char*)url.get());

                    ostringstream movieOs;
                    movieOs << "Movies\\" << movieId << ".ts";
                    movie->SetLocalPath(movieOs.str());

                    refsEvent->Bind(movie);
                }
            }
        }
    }

    /* TimeShiftedServiceList */
    xpathExpr = (xmlChar*)"/root/TimeShiftedServiceList/TimeShiftedService[*]";
    xpathObj.reset(xmlXPathEvalExpression(xpathExpr, xpathCtx.get()), xpathObjectDeleter);
    nodes = xpathObj->nodesetval;
    if (nodes == nullptr)
    {
        errstrm << "Error when reading " << xmlPath << endl;
        return;
    }

    for (int i = 0; i < nodes->nodeNr; ++i)
    {
        xmlNodePtr node = nodes->nodeTab[i];
        TsId tsId = GetXmlAttrValue<TsId>(node, (const xmlChar*)"TsId");
        ServiceId serviceId = GetXmlAttrValue<TsId>(node, (const xmlChar*)"ServiceID");
        VersionNumber version = GetXmlAttrValue<VersionNumber>(node, (const xmlChar*)"Version");

        auto iterTs = find_if(tsEntities.begin(), tsEntities.end(), bind(cmpts, tsId, _1));
        if (iterTs == tsEntities.end())
            continue;
        
        list<shared_ptr<TmssEntity>>& tmsses = (*iterTs)->GetTmsses();
        auto iterTmss = find_if(tmsses.begin(), tmsses.end(), bind(cmptmss, serviceId, _1));
        if (iterTmss == tmsses.end())
            continue;

        /* Event */
        for (node = xmlFirstElementChild(node); node != nullptr; node = xmlNextElementSibling(node))
        {
            EventId eventId = GetXmlAttrValue<TsId>(node, (const xmlChar*)"Id");
            SharedXmlChar startTime = GetXmlAttrValue<SharedXmlChar>(node, (const xmlChar*)"StartTime");
            uint32_t duration = GetXmlAttrValue<uint32_t>(node, (const xmlChar*)"Duration");

            xmlNodePtr child = xmlFirstElementChild(node);
            assert(child != nullptr && strcmp("ReferenceService", (const char*)child->name) == 0);
            TsId refsTsId = GetXmlAttrValue<TsId>(child, (const xmlChar*)"TsId");
            ServiceId refsServiceId = GetXmlAttrValue<ServiceId>(child, (const xmlChar*)"ServiceID");
            EventId refsEventId = GetXmlAttrValue<EventId>(child, (const xmlChar*)"EventId");

            auto iterTs = find_if(tsEntities.begin(), tsEntities.end(), bind(cmpts, refsTsId, _1));
            if (iterTs == tsEntities.end())
                continue;

            list<shared_ptr<RefsEntity>>& refses = (*iterTs)->GetRefses();
            auto iterRefs = find_if(refses.begin(), refses.end(), bind(cmprefs, refsServiceId, _1));
            if (iterRefs == refses.end())
                continue;

            list<shared_ptr<RefsEventEntity>>& refsEvents = (*iterRefs)->GetRefsEvents();
            auto iterRefsEvent = find_if(refsEvents.begin(), refsEvents.end(), bind(cmprefsevent, refsEventId, _1));
            if (iterRefsEvent == refsEvents.end())
                continue;
            shared_ptr<RefsEventEntity> refsEvent = *iterRefsEvent;

            TableId tableId = tableIndexHelper.GetUseableTableIndex("RefsEvent");

            tm timeInfo = { 0 };
            std::istringstream ss((const char*)startTime.get());
            ss >> std::get_time(&timeInfo, "%Y-%m-%d %H:%M:%S");

            shared_ptr<TmssEventEntity> tmssEvent = make_shared<TmssEventEntity>(tableId, eventId,
                system_clock::from_time_t(mktime(&timeInfo)), seconds(duration));
            (*iterTmss)->Bind(tmssEvent);

            /* Find and Bind RefsEventEntity */
            refsEvent->Bind(tmssEvent);            
        }
    }

    xmlCleanupParser();
}