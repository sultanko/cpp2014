#include "epollengineer.h"
#include <fcntl.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/un.h>

std::atomic_bool Epoll::sigHandle(false);
std::atomic_bool Epoll::epollRunning(true);

Epoll::Epoll()
{
    epollFileDescriptor = epoll_create1(0);
    epollTimeout = 3000;
    countServers = 0;
}

void Epoll::addServer()
{
    countServers++;
    if (countServers == 1)
    {
        serverThread = std::thread(&Epoll::execute, this);
    }
}

void Epoll::removeServer()
{
    countServers--;
    if (countServers == 0)
    {
        serverThread.join();
    }
}

void Epoll::addFileDescriptor(int fd, std::function<void(epoll_event)> callFunc)
{
    event.data.fd = fd;
    event.events = DEFAULT_EVENTS;
    if (epoll_ctl(epollFileDescriptor, EPOLL_CTL_ADD, fd, &event) != -1)
    {
        fdFunc.insert(make_pair(fd, callFunc));
    }
    else
    {
        throw SuperTcpManager::SuperEpollException();
    }
}

void Epoll::closeSocket(int sfd)
{
    SuperTcpManager::printMyDebug("close", sfd);
    if (::close(sfd) == -1)
    {
        throw SuperTcpManager::SuperSocketCloseException();
    }
}

void Epoll::removeFileDescriptor(int fd)
{
    closeSocket(fd);
    fdFunc.erase(fd);
}

void Epoll::setFdOpt(int fd, unsigned int opt)
{
    event.data.fd = fd;
    event.events = opt;
    if (epoll_ctl(epollFileDescriptor, EPOLL_CTL_MOD, fd, &event) == -1)
    {
        throw SuperTcpManager::SuperInvalidArgumentException();
    }
}

void Epoll::writeMsg(int fd, const std::string& msg)
{
    ::write(fd, msg.c_str(), msg.size());
}


void Epoll::execute()
{
    SuperTcpManager::printMyDebug("start epoll");
    std::vector<epoll_event> events(MAXEVENTS);
    while (countServers > 0)
    {
        int nfds = epoll_wait (epollFileDescriptor, events.data(), (int)events.size(), epollTimeout);
        if (sigHandle)
        {
            for (auto it : fdFunc)
            {
                closeSocket(it.first);
            }
            fdFunc.clear();
            countServers = 0;
            epollRunning = false;
            break;
        }
        if (nfds == 0)
        {
            continue;
        }
        if (nfds == -1)
        {
            SuperTcpManager::printMyDebug("epoll error");
            throw SuperTcpManager::SuperTcpManagerException();
//            break;
        }
        SuperTcpManager::printMyDebug("get epoll events count =", nfds);
        for (int i = 0; i < nfds; i++)
        {
            SuperTcpManager::printMyDebug("event on ", events[i].data.fd);
            fdFunc[events[i].data.fd](events[i]);
        }
    }
    SuperTcpManager::printMyDebug("end epoll");

}

Epoll::~Epoll()
{
    SuperTcpManager::printMyDebug("Destructor epoll start");
    if (serverThread.joinable())
    {
        serverThread.join();
    }
    epollRunning = false;
    SuperTcpManager::printMyDebug("Destructor epoll end");
}
