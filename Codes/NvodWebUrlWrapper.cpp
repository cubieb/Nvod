#include "NvodWebUrlWrapper.h"

/* libxml */
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

/* Foundation */
#include "SystemInclude.h"
#include "Debug.h"
#include "XmlHelper.h"

using namespace std;

NvodWebUrlWrapperInterface* NvodWebUrlWrapperInterface::CreateInstance(const char *xmlPath)
{
    return new NvodWebUrlWrapper(xmlPath);
}

/**********************class NvodWebUrlWrapper**********************/
NvodWebUrlWrapper::NvodWebUrlWrapper(const char *xmlPath)
{
    if ((access(xmlPath, 0)) != 0)
    {
        errstrm << "Error when reading " << xmlPath << endl;
        throw runtime_error("the xmlPath file does not exist!");
        return;
    }

    auto docDeleter = [](xmlDoc* ptr){ xmlFreeDoc(ptr); };
    shared_ptr<xmlDoc> doc(xmlParseFile(xmlPath), docDeleter);
    if (doc == nullptr)
    {
        errstrm << "Error when reading " << xmlPath << endl;
        throw runtime_error("xmlParseFile() failed!");
        return;
    }

    xmlNodePtr node = xmlDocGetRootElement(doc.get());
    assert(node != nullptr);
    /* <ServiceConfigUrl>http://xxxx/xxx</ServiceConfigUrl> */
    node = xmlFirstElementChild(node);
    assert(node != nullptr);
    serviceConfigUrl = GetXmlContent<string>(node);

    /* <EventConfigUrl>http://xxxx/xxx</EventConfigUrl> */
    node = xmlNextElementSibling(node);
    assert(node != nullptr);
    eventConfigUrl = GetXmlContent<string>(node);

    xmlCleanupParser();
}

NvodWebUrlWrapper::~NvodWebUrlWrapper()
{}

const char* NvodWebUrlWrapper::GetServiceConfigUrl() const
{
    return serviceConfigUrl.c_str();
}

const char* NvodWebUrlWrapper::GetEventConfigUrl() const
{
    return eventConfigUrl.c_str();
}