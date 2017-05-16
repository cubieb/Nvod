#include "SocketServer.h"

/* Foundation */
#include "SystemInclude.h"
#include "Debug.h"

using namespace std;
using namespace std::chrono;
using namespace std::placeholders;

#pragma comment(lib, "iphlpapi.lib")

SocketServerInterface& SocketServerInterface::GetInstance()
{
	/* In C++11, the following is guaranteed to perform thread-safe initialisation: */
	static SocketServer instance;
	return instance;
}

/**********************class SocketServer**********************/
SocketServer::SocketServer()
{}

SocketServer::~SocketServer()
{}

void SocketServer::Start()
{
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (isOnGoing)
            return;
        isOnGoing = true;
    }

    theSocketServerThreadMain = thread(bind(&SocketServer::TheSocketServerThreadMain, this));
}

void SocketServer::Stop()
{
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (!isOnGoing)
            return;
        isOnGoing = false;
        cv.notify_one();
    }
    theSocketServerThreadMain.join();
}

bool SocketServer::IsRunning()
{
    lock_guard<mutex> lock(mtx);
    return isOnGoing;
}

/* private functions */
void SocketServer::TheSocketServerThreadMain()
{
    const size_t size = 1024;
    char buffer[size];
    
    char hostname[MAX_PATH] = { 0 };
    int ret = gethostname(hostname, MAX_PATH);
    assert(ret == 0);
    struct hostent *lpHostEnt = gethostbyname(hostname);

    struct addrinfo *result = NULL, hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    ret = getaddrinfo(NULL, "3000", &hints, &result);
    assert(ret == 0);

    int serverSocketFd = ::socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    assert(serverSocketFd != INVALID_SOCKET);
    
    ret = ::bind(serverSocketFd, result->ai_addr, (int)result->ai_addrlen);
    assert(ret != SOCKET_ERROR);

    //start listen, maximum length of the queue of pending connections is 1  
    dbgstrm << "start listen..." << endl;
    ret = ::listen(serverSocketFd, 10);
    if (ret == SOCKET_ERROR)
    {
        errstrm << "listen failed!" << endl;;
        return;
    }

#ifdef _WIN32
    unsigned long noneblock = 1;
    /* set socket to non-blocking i/o mode */
    ret = ioctlsocket(serverSocketFd, FIONBIO, &noneblock);
    assert(ret != SOCKET_ERROR);
#else
    //int flags = fcntl(listenSock, F_GETFL, 0);
    //fcntl(listenSock, F_SETFL, flags | O_NONBLOCK);
#endif


    fd_set readFds, testFds;
    FD_ZERO(&readFds);
    FD_SET(serverSocketFd, &readFds);

    while (true)
    {
        std::unique_lock<std::mutex> lock(mtx);
        if (cv.wait_for(lock, milliseconds(100), [this]{ return !isOnGoing; }))
        {
            /* close all socket */
            for (decltype(readFds.fd_count) fd = 0; fd < readFds.fd_count; fd++)
            {
                int socketFd = readFds.fd_array[fd];
                closesocket(socketFd);
            }
            break;
        }

        testFds = readFds;
        struct timeval timeout = { 0, 100 * 1000 };
        ret = select(FD_SETSIZE, &testFds, nullptr, nullptr, &timeout);
        assert(ret != SOCKET_ERROR);
        if (ret < 1)
        {
            continue;
        }

        for (decltype(readFds.fd_count) fd = 0; fd < readFds.fd_count; fd++)
        {
            int socketFd = readFds.fd_array[fd];
            if (!FD_ISSET(socketFd, &testFds))
            {
                continue;
            }

            if (socketFd == serverSocketFd)
            {
                struct sockaddr_in clientAddress;
                int addrLen = static_cast<int>(sizeof(sockaddr_in));
                int clientSockfd = ::accept(serverSocketFd, (struct sockaddr *)&clientAddress, &addrLen);
                FD_SET(clientSockfd, &readFds);
            }
            else
            {
                //MSG_WAITALL
                ret = recv(socketFd, buffer, size, 0);
                if (ret == 0 || ret == SOCKET_ERROR)
                {
                    closesocket(socketFd);
                    FD_CLR(socketFd, &readFds);
                    dbgstrm << "client socket closed, socketFd = " << socketFd << endl;
                    continue;
                }

                if (buffer[0] != 1)
                {
                    /* for now, we only support 1 type of command: command_tag == 1 */
                    continue;
                }
                char *ptr = buffer + 2;
                const int MaxAddrNumber = (size - 2) / 4;
                int idx;
                for (idx = 0; lpHostEnt->h_addr_list[idx] != nullptr && idx < MaxAddrNumber; ++idx, ptr = ptr + 4)
                {
                    struct in_addr *ipAddr = (struct in_addr*)lpHostEnt->h_addr_list[idx];
                    memcpy(ptr, &ipAddr->s_addr, 4);

                    // 将IP地址转化成字符串形式  
                    dbgstrm << inet_ntoa(*ipAddr) << endl;
                }
                buffer[1] = static_cast<uchar_t>(idx);

                ret = ::send(socketFd, buffer, ptr - buffer, 0);
                assert(ret != SOCKET_ERROR);
            }
        } /* for (int fd = 0; fd < FD_SETSIZE; fd++) */
    }
}