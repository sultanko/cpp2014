#include "epollengineer.h"
#include <fcntl.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/un.h>

EpollEngineer::EpollEngineer()
{
    events = new epoll_event[MAXEVENTS];
    epollFileDescriptor = epoll_create1(0);
    epollTimeout = 1500;
    countServers = 0;
}

void EpollEngineer::addServer()
{
    countServers++;
    if (countServers == 1)
    {
        serverThread = std::thread(&EpollEngineer::execute, this);
    }
}

void EpollEngineer::removeServer()
{
    countServers--;
}

void EpollEngineer::addDataSocket(int sockFd)
{
    event.data.fd = sockFd;
    event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epollFileDescriptor, EPOLL_CTL_ADD, sockFd, &event) == -1)
    {
        throw SuperTcpManager::SuperEpollException();
    }
}

void EpollEngineer::addListenSocket(int sockFd)
{
    addDataSocket(sockFd);
    listenSockets.insert(sockFd);
}

void EpollEngineer::closeSocket(int sfd)
{
    if (::close(sfd) == -1)
    {
        throw SuperTcpManager::SuperSocketCloseException();
    }
}

void EpollEngineer::removeDataSocket(int sockFd)
{
    closeSocket(sockFd);
}

void EpollEngineer::removeListenSocket(int sockFd)
{
    closeSocket(sockFd);
    listenSockets.erase(sockFd);
}

void EpollEngineer::execute()
{
    SuperTcpManager::printDebug("start epoll");
    while (countServers > 0)
    {
        int nfds = epoll_wait (epollFileDescriptor, events, MAXEVENTS, epollTimeout);
        if (nfds == 0)
        {
            continue;
        }
        if (nfds == -1)
        {
            SuperTcpManager::printDebug("epoll error");
            throw SuperTcpManager::SuperTcpManagerException();
            break;
        }
        SuperTcpManager::printDebug("get epoll events count =", nfds);
        for (int i = 0; i < nfds; i++)
        {
            SuperTcpManager::printDebug("event on ", events[i].data.fd);
            if ((events[i].events & EPOLLERR) ||
                  (events[i].events & EPOLLHUP) ||
                  (!(events[i].events & EPOLLIN)))
            {
                // error
                if (listenSockets.find(events[i].data.fd) != listenSockets.end())
                {
                    listenSockets.erase(events[i].data.fd);
                }
                closeSocket(events[i].data.fd);

                emit closedConnection(events[i].data.fd);

                continue;
            }
            else if (listenSockets.find(events[i].data.fd) != listenSockets.end())
            {
                // input connection
                auto portFd =find(listenSockets.begin(), listenSockets.end(), events[i].data.fd);
                SuperTcpManager::printDebug("input connection on", *portFd);
                while (true)
                {
                    sockaddr in_addr;
                    socklen_t in_len = sizeof (struct sockaddr);
                    int infd = accept(*portFd, &in_addr, &in_len);

                    if (infd == -1)
                    {
                        if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
                        {
                            break;
                        }
                        else
                        {
                            break;
                        }
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
                    }


                    emit newConnection(infd);
                }
                continue;
            }
            else
            {
                bool closeConnection = false;

                while (true)
                {
                    char buf[MAXDATASIZE];
                    memset(buf, 0, sizeof buf);

                    ssize_t count = read(events[i].data.fd, buf, sizeof buf);
                    if (count == -1)
                    {
                        if (errno != EAGAIN)
                        {
                            closeConnection = true;
                        }
                        break;
                    }
                    else if (count == 0)
                    {
                        closeConnection = true;
                        break;
                    }

                    emit newMessageEpoll(events[i].data.fd, QString::fromUtf8(buf, count));
                }

                if (closeConnection)
                {
                    SuperTcpManager::printDebug("Close descriptor", events[i].data.fd);

                    emit closedConnection(events[i].data.fd);
                }

            }
        }
    }
    SuperTcpManager::printDebug("end epoll");

}

EpollEngineer::~EpollEngineer()
{
    serverThread.join();
    delete[] events;
}
