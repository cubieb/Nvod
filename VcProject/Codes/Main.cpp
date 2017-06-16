#include "Main.h"

/* Curl*/
#include <curl/curl.h>
#include <json/json.h>

/* Foundation */
#include "SystemInclude.h"
#include "Debug.h"
#include "Des3.h"

/* Controller */
#include "ControllerInterface.h"

#include "Version.h"
#include "WMI_DeviceQuery.h"

using namespace std;
using namespace std::chrono;

/*
{
    "cpu"        : "xxxx",
    "bios"       : "xxxx",
    "begin_time" : "2017-01-01",
    "end_time"   : "2018-01-01"
}
*/
bool CheckSoftwarekLicence()
{
    const size_t size = 512;
    char ciphertext[size] = { 0 }, plaintext[size] = { 0 };

    /* check software license information. */
    ifstream auth("Auth.dat");
    if (access("Auth.dat", 0) != 0)
    {
        errstrm << "Auth.dat does not existed." << endl;
        return 1;
    }
    auth.read(ciphertext, size);
    assert(auth.eof());
    auth.close();

    size_t ciphertextSize = static_cast<size_t>(auth.gcount());
    size_t plaintextSize = Des3::Decrypt(ciphertext, ciphertextSize, plaintext, size);
    assert(plaintextSize == ciphertextSize);

    Json::Reader reader;
    Json::Value  value;
    reader.parse(plaintext, plaintext + strlen(plaintext), value, false);

    list<pair<int, const char*>> devices =
    {
        { 3, "cpu" },
        { 4, "bios" },
    };
    for (auto i = devices.begin(); i != devices.end(); ++i)
    {
        T_DEVICE_PROPERTY device;
        auto ret = WMI_DeviceQuery(i->first, &device, 1);
        assert(ret > 0);
        if (strcmp(value[i->second].asCString(), (char*)device.szProperty) != 0)
        {
            cout << "invalid software license!" << endl;
            return false;
        }
    }

    tm timeInfo = { 0 };
    std::istringstream ss1(value["begin_time"].asCString());
    ss1 >> std::get_time(&timeInfo, "%Y-%m-%d");
    system_clock::time_point now = system_clock::now();
    system_clock::time_point beginTime = system_clock::from_time_t(mktime(&timeInfo));
    if (beginTime > now)
    {
        cout << "invalid software license!" << endl;
        return false;
    }
    std::istringstream ss2(value["end_time"].asCString());
    ss2 >> std::get_time(&timeInfo, "%Y-%m-%d");
    system_clock::time_point endTime = system_clock::from_time_t(mktime(&timeInfo));
    if (endTime < now)
    {
        cout << "invalid software license!" << endl;
        return false;
    }

    return true;
}

int main(int argc, char **argv)
{
    if (!CheckSoftwarekLicence())
    {
        return 1;
    }

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
	if (access(xmlPath, 0) != 0)
	{
		errstrm << "NvodWebUrl.xml does not existed." << endl;
		return 1;
	}
    controller.Start(xmlPath);

    while (true)
    {
        this_thread::sleep_for(chrono::seconds(100));
    }

    curl_global_cleanup();
    WSACleanup();

    return 0;
}
