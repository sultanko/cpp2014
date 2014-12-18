#include "abstractsocket.h"
#include "supertcpmanager.h"
#include <sys/socket.h>
#include <unistd.h>

AbstractSocket::AbstractSocket(int sockfd, SuperTcpManager& tcpManager)
    : sockfd(sockfd),
        tcpManager(tcpManager)
{
    SuperTcpManager::printDebug("Abstract constructor", sockfd);
}

AbstractSocket::~AbstractSocket()
{
    SuperTcpManager::printDebug("Abstract destructor", sockfd);
    tcpManager.close(sockfd);
}

