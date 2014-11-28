#ifndef SUPERTCPMANAGER_H
#define SUPERTCPMANAGER_H

#include <sys/socket.h>
#include <sys/epoll.h>
#include <QtGlobal>
#include <QObject>
#include <vector>
#include <thread>
#include <atomic>
#include <iostream>
#include "epollengineer.h"
#include <memory>

class EpollEngineer;

class SuperTcpManager : public QObject
{
    Q_OBJECT
public:
    struct SuperTcpManagerException : public std::exception {};
    struct SuperBindingException : public SuperTcpManagerException {};
    struct SuperSocketCreateException : public SuperTcpManagerException {};
    struct SuperSocketCloseException : public SuperTcpManagerException {};
    struct SuperSocketOptionsException : public SuperTcpManagerException {};
    struct SuperListeningException : public SuperTcpManagerException {};
    struct SuperEpollException : public SuperTcpManagerException {};
    struct SuperConnectException : public SuperTcpManagerException {};
    struct SuperInvalidArgumentException : public SuperTcpManagerException {};
    struct SuperSendException : public SuperTcpManagerException {};
public:
    SuperTcpManager();

    int connect(const char* hostname, const char *port);
    void disconnect(int sockfd);

    int listen(const char *port);
    int listen();
    void unlisten(const char *port);
    int sendToAll(const char *message, int len = -1);
    int sendToAllExclude(const std::vector<int>& blackList, const char *message, int len = -1);
    void sendTo(int clientId, const char *message, int len = -1);
    ~SuperTcpManager();

signals:
    void newMessageReceived(int clientId, QString message);
    void newConnection(int clientId);
    void closedConnection(int clientId);

public slots:
    void newConnectionEpoll(int clientFd);
    void closedConnectionEpoll(int clientFd);
    void newMessageReceivedEpoll(int clientFd, QString message);

public:
    static bool programRunning;
    static int countRunningServers;

    const char *getServerPort() const;
    void setServerPort(const char *value);

    int getMaxPendingConnections() const;
    void setMaxPendingConnections(int value);

    const char *getServerHostname() const;
    void setServerHostname(const char *value);

protected:
    void stopServer();
    void startServer();
    int createAndBind(const char *port);
    static void makeSocketNonBlocking(int sfd);
    void execute();
    void addSocket(int sockfd);
    void addSocket(int sockfd, const char* port);
    void removeSocket(int sockfd, int sockId);
    int getNewId();
//    void removeId(int dataId);

private:

    sockaddr serverSocket;
    std::vector<int> listenSockets;
    std::vector<int> listenPorts;
    std::map<int, int> dataIdToSocket;
    std::map<int, int> dataSocketToId;
    bool serverRunning;
    char *serverHostname;
    char *serverPort;
    int maxPendingConnections;
    std::shared_ptr<EpollEngineer> epollEngineer;

private:
    static int counterId;

friend class EpollEngineer;

protected:
    template<typename T, typename ... Args>
    static void printDebug(T a, Args... args)
    {
#ifdef QT_DEBUG
        std::cerr << a << " ";
        printDebug(args ...);
#endif
    }

    template<typename T>
    static void printDebug(T a)
    {
#ifdef QT_DEBUG
        std::cerr << a << "\n";
#endif
    }

};

#endif // SUPERTCPMANAGER_H
