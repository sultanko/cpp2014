#include "serversocket.h"
#include "supertcpmanager.h"
#include <algorithm>


void ServerSocket::send(const std::string &message)
{
    for (auto it = clientSockets.begin(); it != clientSockets.end(); it++)
    {
        (*it)->send(message);
    }
}

ServerSocket::ServerSocket(int sockfd, SuperTcpManager &manager, std::function<void(ClientSocket *)> newConnection)
    : AbstractSocket(sockfd, manager),
    newConnection(newConnection)
{
    SuperTcpManager::printDebug("Server constructor", sockfd);
    tcpManager.addSocket(sockfd, [&](epoll_event ev)
    {
        if ((ev.events & EPOLLERR) ||
                (ev.events & EPOLLHUP))
        {
            return;
        }
        auto portFd = ev.data.fd;
        SuperTcpManager::printDebug("input connection on", portFd);
        while (true)
        {
            sockaddr in_addr;
            socklen_t in_len = sizeof (struct sockaddr);
            int infd = accept(portFd, &in_addr, &in_len);

            if (infd == -1)
            {
                break;
            }

            SuperTcpManager::makeSocketNonBlocking(infd);

            SuperTcpManager::printDebug("get accept on: ", infd);
            char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

            int s = getnameinfo(&in_addr, in_len,
                                hbuf, sizeof hbuf,
                                sbuf, sizeof sbuf,
                                NI_NUMERICHOST | NI_NUMERICSERV);
            if (s == 0)
            {
                SuperTcpManager::printDebug("Accepted connection desc =", infd, "host = ", hbuf, "port = ", sbuf);
                this->clientSockets.push_back(std::unique_ptr<ClientSocket>(new ClientSocket(infd, this->tcpManager, this)));
                this->newConnection((this->clientSockets.back().get()));
            }
        }
    });
}

void ServerSocket::removeClient(ClientSocket* client)
{
    auto it = std::find_if(clientSockets.begin(), clientSockets.end(), [client](const std::unique_ptr<ClientSocket>& p)
        {
            return client == p.get();
        });
    if (it != clientSockets.end())
    {
        clientSockets.erase(it);
    }
}

ServerSocket::~ServerSocket()
{
    SuperTcpManager::printDebug("Server destructor", sockfd);
    while (!clientSockets.empty())
    {
        clientSockets.pop_back();
    }
}

void ServerSocket::close()
{
    tcpManager.removeSocket(this);
}
