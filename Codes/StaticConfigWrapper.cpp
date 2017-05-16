#include "StaticConfigWrapper.h"

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

/**********************class TableIndexHelperInterface**********************/
StaticConfigWrapperInterface* StaticConfigWrapperInterface::CreateInstance(const char *xmlPath)
{
    return new StaticConfigWrapper(xmlPath);
}

/**********************class StaticConfigWrapper**********************/
StaticConfigWrapper::StaticConfigWrapper(const char *xmlPath)
{
    Read(xmlPath);
}

StaticConfigWrapper::~StaticConfigWrapper()
{}

/* private functions */
void StaticConfigWrapper::Read(const char *xmlPath)
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

    /* TransportStreamList */
    xmlChar *xpathExpr = (xmlChar*)"/root/Tx/TransportStreamList/TransportStream[*]";
    auto xpathObjectDeleter = [](xmlXPathObject* ptr){ xmlXPathFreeObject(ptr); };
    shared_ptr<xmlXPathObject> xpathObj(xmlXPathEvalExpression(xpathExpr, xpathCtx.get()), xpathObjectDeleter);
    xmlNodeSetPtr nodes = xpathObj->nodesetval;
    if (nodes == nullptr)
    {
        errstrm << "Error when reading " << xmlPath << endl;
        return;
    }

    TableIndexHelperInterface &tableIndexHelper = TableIndexHelperInterface::GetInstance();

    for (int i = 0; i < nodes->nodeNr; ++i)
    {
        xmlNodePtr node = nodes->nodeTab[i];
        TsId tsId = GetXmlAttrValue<TsId>(node, (const xmlChar*)"TsId");

        node = xmlFirstElementChild(node);
        SharedXmlChar srcIp = GetXmlContent<SharedXmlChar>(node);

        node = xmlNextElementSibling(node);
        SharedXmlChar dstIp = GetXmlContent<SharedXmlChar>(node);

        node = xmlNextElementSibling(node);
        uint16_t dstUdpPort = GetXmlContent<uint16_t>(node);

        struct in_addr srcAddr;
        srcAddr.s_addr = inet_addr((char *)srcIp.get());

        struct sockaddr_in dstAddr;
        dstAddr.sin_family = AF_INET;
        dstAddr.sin_addr.s_addr = inet_addr((char *)dstIp.get());
        dstAddr.sin_port = htons(dstUdpPort);

        TableId tableId = tableIndexHelper.GetUseableTableIndex("Ts");
        shared_ptr<TsEntity> tsEntity = make_shared<TsEntity>(tableId, tsId, srcAddr, dstAddr);
        tsEntities.push_back(tsEntity);
    }

    /* ReferenceTransportStreamList */
    xpathExpr = (xmlChar*)"/root/ReferenceTransportStreamList/ReferenceTransportStream[*]";
    xpathObj.reset(xmlXPathEvalExpression(xpathExpr, xpathCtx.get()), xpathObjectDeleter);
    nodes = xpathObj->nodesetval;
    if (nodes == nullptr)
    {
        errstrm << "Error when reading " << xmlPath << endl;
        return;
    }

    auto cmpts = [](TsId tsId, shared_ptr<TsEntity> tsEntity) ->bool
    { return tsId == tsEntity->GetTsId(); };

    for (int i = 0; i < nodes->nodeNr; ++i)
    {
        xmlNodePtr node = nodes->nodeTab[i];
        TsId tsId = GetXmlAttrValue<TsId>(node, (const xmlChar*)"TsId");
        
        auto iter = find_if(tsEntities.begin(), tsEntities.end(), bind(cmpts, tsId, _1));
        if (iter == tsEntities.end())
            continue;

        for (node = xmlFirstElementChild(node); node != nullptr; node = xmlNextElementSibling(node))
        {
            ServiceId serviceId = GetXmlAttrValue<TsId>(node, (const xmlChar*)"Id");

            xmlNodePtr child = xmlFirstElementChild(xmlFirstElementChild(node));
            ServiceId posterSvcId = GetXmlContent<ServiceId>(child);

            child = xmlNextElementSibling(child);
            Pid pmtPid = GetXmlContent<Pid>(child);

            child = xmlNextElementSibling(child);
            Pid dataPipePid = GetXmlContent<Pid>(child);            

            TableId tableId = tableIndexHelper.GetUseableTableIndex("Refs");
            shared_ptr<RefsEntity> refsEntity = make_shared<RefsEntity>(tableId, serviceId);

            /* refs and psts have the same table id. */
            shared_ptr<PstsEntity> pstsEntity = make_shared<PstsEntity>(tableId, posterSvcId, pmtPid, 0x80, dataPipePid);
            refsEntity->SetPsts(pstsEntity);

            (*iter)->Bind(refsEntity);
        }
    }

    /* TimeShiftedTransportStreamList */
    xpathExpr = (xmlChar*)"/root/TimeShiftedTransportStreamList/TimeShiftedTransportStream[*]";
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

        auto iter = find_if(tsEntities.begin(), tsEntities.end(), bind(cmpts, tsId, _1));
        if (iter == tsEntities.end())
            continue;

        for (node = xmlFirstElementChild(node); node != nullptr; node = xmlNextElementSibling(node))
        {
            ServiceId serviceId = GetXmlAttrValue<TsId>(node, (const xmlChar*)"Id");

            xmlNodePtr child = xmlFirstElementChild(node);
            Pid pmtPid = GetXmlContent<Pid>(child);

            child = xmlNextElementSibling(child);
            Pid pcrPid = GetXmlContent<Pid>(child);

            child = xmlNextElementSibling(child);
            Pid audioPid = GetXmlContent<Pid>(child);

            child = xmlNextElementSibling(child);
            Pid videoPid = GetXmlContent<Pid>(child);

            TableId tableId = tableIndexHelper.GetUseableTableIndex("Tmss");
            auto tmssEntity = make_shared<TmssEntity>(tableId, serviceId, pmtPid, pcrPid, audioPid, videoPid);

            (*iter)->Bind(tmssEntity);
        }
    }

    /* SendingIntervalMilliseconds */
    xpathExpr = (xmlChar*)"/root/SendingIntervalMilliseconds[*]";
    xpathObj.reset(xmlXPathEvalExpression(xpathExpr, xpathCtx.get()), xpathObjectDeleter);
    nodes = xpathObj->nodesetval;
    if (nodes == nullptr)
    {
        errstrm << "Error when reading " << xmlPath << endl;
        return;
    }
    assert(nodes->nodeNr != 0);
    node = nodes->nodeTab[0];

    xmlNodePtr child = xmlFirstElementChild(node);
    uint64_t patIntv = GetXmlContent<uint64_t>(child);

    child = xmlNextElementSibling(child);
    uint64_t pmtIntv = GetXmlContent<uint64_t>(child);

    child = xmlNextElementSibling(child);
    uint64_t posterIntv = GetXmlContent<uint64_t>(child);

    TableId tableId = tableIndexHelper.GetUseableTableIndex("GlobalCfg");
    globalCfgEntity = make_shared<GlobalCfgEntity>(tableId, 
        milliseconds(patIntv), milliseconds(pmtIntv), milliseconds(posterIntv));

    xmlCleanupParser();
}

shared_ptr<GlobalCfgEntity> StaticConfigWrapper::GetGlobalCfgEntity()
{
    return globalCfgEntity;
}

list<shared_ptr<TsEntity>>& StaticConfigWrapper::GetTsEntities()
{
    return tsEntities;
}