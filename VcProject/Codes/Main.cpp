#include "Main.h"

/* Curl*/
#include <curl/curl.h>

/* Foundation */
#include "SystemInclude.h"

/* Controller */
#include "ControllerInterface.h"

#include "version.h"

using namespace std;

int main(int argc, char **argv)
{
    /* to keep there is only one instance is running */
    HANDLE mutex;
    mutex = CreateMutex(NULL, TRUE, TEXT("NvodSender"));
    if ((mutex != nullptr) && (GetLastError() == ERROR_ALREADY_EXISTS))
    {
        /* One another instance is running! */
        return 0;
    }

    cout << "Software Svn Version: " << ExeVersion << endl;
    WSADATA wsaData;
    int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
    assert(ret == 0);

    CURLcode curlCode = curl_global_init(CURL_GLOBAL_ALL);
    assert(curlCode == CURLE_OK);

    ControllerInterface &controller = ControllerInterface::GetInstance();

    char *xmlPath = (argc == 1 ? "NvodWebUrl.xml" : argv[1]);
    controller.Start(xmlPath);

    while (true)
    {
        this_thread::sleep_for(chrono::seconds(100));
    }

    curl_global_cleanup();
    WSACleanup();

    return 0; 
}
