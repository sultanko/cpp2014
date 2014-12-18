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
    if (countServers == 0)
    {
        serverThread.join();
    }
}

void EpollEngineer::addFileDescriptor(int fd, std::function<void(epoll_event)> callFunc)
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

void EpollEngineer::closeSocket(int sfd)
{
    if (::close(sfd) == -1)
    {
        throw SuperTcpManager::SuperSocketCloseException();
    }
}

void EpollEngineer::removeFileDescriptor(int fd)
{
    closeSocket(fd);
    fdFunc.erase(fd);
}

void EpollEngineer::setFdOpt(int fd, unsigned int opt)
{
    event.data.fd = fd;
    event.events = opt;
    if (epoll_ctl(epollFileDescriptor, EPOLL_CTL_MOD, fd, &event) == -1)
    {
        throw SuperTcpManager::SuperInvalidArgumentException();
    }
}

void EpollEngineer::writeMsg(int fd, const std::string& msg)
{
    ::write(fd, msg.c_str(), msg.size());
}


void EpollEngineer::execute()
{
    SuperTcpManager::printDebug("start epoll");
    std::vector<epoll_event> events(MAXEVENTS);
    while (countServers > 0)
    {
        int nfds = epoll_wait (epollFileDescriptor, events.data(), (int)events.size(), epollTimeout);
        if (nfds == 0)
        {
            continue;
        }
        if (nfds == -1)
        {
            SuperTcpManager::printDebug("epoll error");
            throw SuperTcpManager::SuperTcpManagerException();
//            break;
        }
        SuperTcpManager::printDebug("get epoll events count =", nfds);
        for (int i = 0; i < nfds; i++)
        {
            SuperTcpManager::printDebug("event on ", events[i].data.fd);
            fdFunc[events[i].data.fd](events[i]);
        }
    }
    SuperTcpManager::printDebug("end epoll");

}

EpollEngineer::~EpollEngineer()
{
    SuperTcpManager::printDebug("Destructor epoll start");
    if (serverThread.joinable())
    {
        serverThread.join();
    }
    SuperTcpManager::printDebug("Destructor epoll end");
}
