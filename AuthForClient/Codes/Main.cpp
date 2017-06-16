#include "Main.h"

/* Curl*/
#include <json/json.h>

/* CommonCodes */
#include "SystemInclude.h"
#include "Des3.h"

#include "version.h"
#include "WMI_DeviceQuery.h"

using namespace std;

#ifdef _WIN32
#	pragma comment(lib, "jsoncpp.lib")
#endif

/*
{
	"cpu"        : "xxxx",
	"bios"       : "xxxx",
	"begin_time" : "2017-01-01",
	"end_time"   : "2018-01-01"
}
*/
int main(int argc, char **argv)
{
    list<pair<int, const char*>> devices =
    {
        { 3, "cpu" },
        { 4, "bios" },
    };
    
    Json::Value machine;
    for (auto i = devices.begin(); i != devices.end(); ++i)
    {
        T_DEVICE_PROPERTY device;
        auto ret = WMI_DeviceQuery(i->first, &device, 1);
        assert(ret > 0);
        machine[i->second] = (char*)device.szProperty;
    }
    
    Json::FastWriter writer;
    string plaintext = writer.write(machine);

    const size_t size = 512;
    char ciphertext[size] = { 0 };
    size_t ciphertextSize = Des3::Encrypt(plaintext.c_str(), plaintext.size(), ciphertext, size);
    assert(ciphertextSize < size);

    ofstream auth("Machine.dat");
    auth.write(ciphertext, ciphertextSize);
    auth.close();

    return 0; 
}
