#include "UtSocketServer.h"

/* Cpp Unit */
#include <cppunit/extensions/HelperMacros.h>

/* Foundation */
#include "SystemInclude.h"
#include "Debug.h"

/* Functions */
#include "SocketServerInterface.h"

using namespace std;

CxxBeginNameSpace(UnitTest)

/**********************UtSocketServer**********************/
CPPUNIT_TEST_SUITE_REGISTRATION(UtSocketServer);

/* public function */
void UtSocketServer::setUp()
{
}

/* protected function */
void UtSocketServer::TestStart()
{
    list<const char*> functions =
    {
        "SocketServer::TheSocketServerThreadMain",
    };
    DebugFlag& debugFlag = DebugFlag::GetInstance();
    for (list<const char*>::iterator iter = functions.begin(); iter != functions.end(); ++iter)
    {
        debugFlag.SetState(*iter, true);
    }

    char hostname[MAX_PATH] = { 0 };
    gethostname(hostname, MAX_PATH);
    struct hostent FAR* lpHostEnt = gethostbyname(hostname);

    SocketServerInterface& socketServer = SocketServerInterface::GetInstance();
    socketServer.Start();

    int sockfd;
    struct sockaddr_in address;

    for (uint32_t i = 0; i < 3; ++i)
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = inet_addr("127.0.0.1");
        address.sin_port = htons(3000);

        int ret = connect(sockfd, (struct sockaddr *)&address, sizeof(address));
        CPPUNIT_ASSERT(ret != -1);

        const size_t size = 1024;
        char buffer[size] = { 0x1, 0x0 };

        ret = send(sockfd, buffer, 1, 0);
        CPPUNIT_ASSERT(ret != SOCKET_ERROR);

        ret = recv(sockfd, buffer, size, 0);
        CPPUNIT_ASSERT(ret != SOCKET_ERROR);
        CPPUNIT_ASSERT(buffer[0] == 0x1);
        const int MaxAddrNumber = (size - 2) / 4;
        int idx;
        for (idx = 0; lpHostEnt->h_addr_list[idx] != nullptr && idx < MaxAddrNumber; ++idx)
        {
            struct in_addr *ipAddr = (struct in_addr*)lpHostEnt->h_addr_list[idx];
            CPPUNIT_ASSERT(memcmp(buffer + 2 + 4 * idx, ipAddr, 4) == 0);

            // 将IP地址转化成字符串形式  
            cout << inet_ntoa(*ipAddr) << endl;
        }
        CPPUNIT_ASSERT(buffer[1] == idx);

        closesocket(sockfd);
    }

    socketServer.Stop();
    for (list<const char*>::iterator iter = functions.begin(); iter != functions.end(); ++iter)
    {
        debugFlag.SetState(*iter, false);
    }
}

CxxEndNameSpace