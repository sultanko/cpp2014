#ifndef EPOLLENGINEER_H
#define EPOLLENGINEER_H

#include <sys/socket.h>
#include <sys/epoll.h>
#include <set>
#include "supertcpmanager.h"

class SuperTcpManager;

class EpollEngineer : public QObject
{
    Q_OBJECT;
public:
    EpollEngineer();
    void addServer();
    void removeServer();
    void addDataSocket(int sockFd);
    void addListenSocket(int sockFd);
    void removeDataSocket(int sockFd);
    void removeListenSocket(int sockFd);
    void execute();
    ~EpollEngineer();

signals:
    void newMessageEpoll(int socketFd, QString message);
    void newConnection(int socketFd);
    void closedConnection(int socketFd);

private:
    void closeSocket(int sfd);

    int countServers;
    epoll_event event;
    epoll_event* events;
    std::set<int> listenSockets;
    int epollFileDescriptor;
    int epollTimeout;
    std::thread serverThread;

    static const int MAXEVENTS = 64;
    static const int MAXDATASIZE = 512;

};

#endif // EPOLLENGINEER_H
