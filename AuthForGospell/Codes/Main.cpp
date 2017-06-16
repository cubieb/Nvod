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
	"main_board" : "xxxx",
	"bios"       : "xxxx",
	"begin_time" : "2017-01-01",
	"end_time"   : "2018-01-01"
}
*/
int main(int argc, char **argv)
{
    const size_t size = 512;
    char ciphertext[size] = { 0 }, plaintext[size] = { 0 };

    ifstream machine("Machine.dat");
    machine.read(ciphertext, size);
    assert(machine.eof());
    
    size_t ciphertextSize = static_cast<size_t>(machine.gcount());
    size_t plaintextSize = Des3::Decrypt(ciphertext, ciphertextSize, plaintext, size);
    assert(plaintextSize == ciphertextSize);
    //cout << plaintext << endl;

    Json::Reader reader;
    Json::Value  value;
    bool ret = reader.parse(plaintext, plaintext + strlen(plaintext), value, false);
    assert(ret);
    value["begin_time"] = "2017-01-01";
    value["end_time"]   = "2018-01-01";

    Json::FastWriter writer;
    string valueText = writer.write(value);
    //cout << valueText << endl;

    ciphertextSize = Des3::Encrypt(valueText.c_str(), valueText.size(), ciphertext, size);
    assert(ciphertextSize < size);

    ofstream auth("Auth.dat");
    auth.write(ciphertext, ciphertextSize);
    auth.close();

    return 0; 
}
