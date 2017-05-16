#include "WebServiceInterface.h"

/* Foundation */
#include "SystemInclude.h"
#include "ResultCode.h"     /* ResultCode */

/**********************class WebPlatform**********************/
class WebService : public WebServiceInterface
{
public:
    WebService();
    ~WebService();

    bool DownloadConfigXml(const char* url, const char* to);

private:
    /* disable default copy constructor */
    WebService(const WebService& right);

    size_t HandleHttpString(void *ptr, size_t size, size_t nmemb, ResultCode& resultCode, std::ofstream& ofstrm);
};