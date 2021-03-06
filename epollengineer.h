#ifndef EPOLLENGINEER_H
#define EPOLLENGINEER_H

#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/un.h>
#include <set>
#include "supertcpmanager.h"
#include <atomic>
#include <functional>
#include <map>

class SuperTcpManager;

class Epoll
{
    // using exceptions from SuperTcpManager
public:
    Epoll();
    void addServer();
    void removeServer();
    void addFileDescriptor(int fd, std::function<void(epoll_event)> callFunc);
    void removeFileDescriptor(int fd);
    void setFdOpt(int fd, unsigned int opt);
    void writeMsg(int fd, const std::string& msg);
    void execute();
    ~Epoll();

//signals:
//    void newMessageEpoll(int socketFd, std::string message);
//    void newConnection(int portFd, int socketFd);
//    void closedConnection(int socketFd);

    static const int DEFAULT_EVENTS = EPOLLIN;
    static std::atomic_bool sigHandle;
    static std::atomic_bool epollRunning;
private:
    Epoll(const Epoll& ep) = delete;
    Epoll(Epoll&& ep) = delete;
    Epoll& operator=(Epoll& ep) = delete;
    void closeSocket(int sfd);

    std::atomic_int countServers;
    epoll_event event;
    int epollFileDescriptor;
    int epollTimeout;
    std::thread serverThread;
    std::map<int, std::function<void(epoll_event)> > fdFunc;

    static const int MAXEVENTS = 64;
    static const int MAXDATASIZE = 512;

};

#endif // EPOLLENGINEER_H
