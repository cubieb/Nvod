#ifndef _WebServiceInterface_h_
#define _WebServiceInterface_h_

/**********************class WebPlatformInterface**********************/
class WebServiceInterface
{
public:
    WebServiceInterface() {}
    virtual ~WebServiceInterface() {}
    static WebServiceInterface& GetInstance();

    virtual bool DownloadConfigXml(const char* url, const char* to) = 0;
};

#endif