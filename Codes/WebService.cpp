#include "WebService.h"

/* Curl */
#include <curl/curl.h>

/* Type */
#include "BaseType.h"

/* Foundation */
#include "SystemInclude.h"
#include "ResultCode.h"
#include "Debug.h"

/* local header files */
#include "CurlOutputHandler.h"

using namespace std;
using namespace std::placeholders;

using std::chrono::milliseconds;

WebServiceInterface& WebServiceInterface::GetInstance()
{
    /* In C++11, the following is guaranteed to perform thread-safe initialisation: */
    static WebService instance;
    return instance;
}

/**********************class WebPlatform**********************/
WebService::WebService()
{}

WebService::~WebService()
{}

bool WebService::DownloadConfigXml(const char* url, const char* to)
{
    CURLcode curlCode;
    CURL *curl = curl_easy_init();
    curlCode = curl_easy_setopt(curl, CURLOPT_URL, url);
    assert((curlCode == CURLE_OK));
    curlCode = curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 4);
    assert((curlCode == CURLE_OK));
    curlCode = curl_easy_setopt(curl, CURLOPT_TIMEOUT, 4);
    assert((curlCode == CURLE_OK));
    curlCode = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlOutputHandler);
    assert((curlCode == CURLE_OK));

    curlCode = curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    assert((curlCode == CURLE_OK));
    curlCode = curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
    assert((curlCode == CURLE_OK));

    ofstream ofstrm(to, std::ofstream::binary | std::ofstream::trunc);
    assert(ofstrm.is_open());

    ResultCode resultCode = ResultCodeOk;
    HttpStringHandler handler = bind(&WebService::HandleHttpString, this, _1, _2, _3, ref(resultCode), ref(ofstrm));
    curlCode = curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&handler);
    assert((curlCode == CURLE_OK));

    curlCode = curl_easy_perform(curl);
    if (curlCode != CURLE_OK)
    {
        static set<CURLcode> ignorableCurlErrores =
        {
            CURLE_GOT_NOTHING,
            CURLE_COULDNT_CONNECT,
            CURLE_OPERATION_TIMEDOUT,
            CURLE_COULDNT_RESOLVE_HOST
        };

        if (ignorableCurlErrores.find(curlCode) == ignorableCurlErrores.end())
            resultCode = MakeResultCode(SBitNotOk, FBitFalse, WBitTrue, curlCode);
        else
            resultCode = MakeResultCode(SBitNotOk, FBitTrue, WBitTrue, curlCode);
    }

    bool ret = true;
    if (GetResultSBit(resultCode) != SBitOk)
    {
        if (GetResultWBit(resultCode) == WBitTrue)
        {
            errstrm << "oops, curl_easy_perform failed. curlCode = " << curlCode << endl;
        }

        if (GetResultFBit(resultCode) == FBitTrue)
            ret = DownloadConfigXml(url, to);
        else
            ret = false;
    }

    ofstrm.close();
    curl_easy_cleanup(curl);
    return ret;
}

size_t WebService::HandleHttpString(void *ptr, size_t size, size_t nmemb, ResultCode& resultCode, ofstream& ofstrm)
{
    ofstrm << (char*)ptr;
    return (nmemb * size);
}