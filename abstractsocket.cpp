#include "abstractsocket.h"
#include "supertcpmanager.h"
#include <sys/socket.h>
#include <unistd.h>

AbstractSocket::AbstractSocket(int sockfd, SuperTcpManager& tcpManager)
    : sockfd(sockfd),
        tcpManager(tcpManager)
{
    SuperTcpManager::printMyDebug("Abstract constructor", sockfd);
}

AbstractSocket::~AbstractSocket()
{
    SuperTcpManager::printMyDebug("Abstract destructor", sockfd);
    tcpManager.close(sockfd);
}

