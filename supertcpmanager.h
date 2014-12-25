#ifndef SUPERTCPMANAGER_H
#define SUPERTCPMANAGER_H

#include <sys/socket.h>
#include <sys/epoll.h>
#include <vector>
#include <thread>
#include <atomic>
#include <iostream>
#include "epollengineer.h"
#include <memory>
#include <string>
#include <functional>
#include "clientsocket.h"
#include "serversocket.h"
#include <list>
#include <dirent.h>

class EpollEngineer;
class AbstractSocket;
class ServerSocket;
class ClientSocket;

class SuperTcpManager
{
// possible exceptions
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

    SuperTcpManager(std::shared_ptr<EpollEngineer> epollEngineer);
    SuperTcpManager();

    // return id of new connection
    ClientSocket *connect(const char *hostname, const char *port, std::function<void(int)> clientFunc, char *clientBuffer, size_t clientBufferSize);

    // listen on default port
    ServerSocket *listen(std::function<void(ClientSocket *)> newConnection);
    ServerSocket *listen(const std::string &port, std::function<void(ClientSocket *)> newConnection);

    void close(int fd);


    std::shared_ptr<EpollEngineer> getEpollEngineer();
    ~SuperTcpManager();

    const std::string& getServerPort() const;
    void setServerPort(const std::string& value);

    int getMaxPendingConnections() const;
    void setMaxPendingConnections(int value);

    const std::string& getServerHostname() const;
    void setServerHostname(const std::string& value);

private:
    SuperTcpManager(const SuperTcpManager& sp) = delete;
    SuperTcpManager(SuperTcpManager&& sp) = delete;
    SuperTcpManager& operator=(SuperTcpManager& ep) = delete;
    void addSocket(int fd, std::function<void(epoll_event)> callback);
    void removeSocket(AbstractSocket* asocket);
    void setFdOptions(int fd, unsigned int opt);
    static int createAndBind(const char* hostname, const char *port);
    static void makeSocketNonBlocking(int sfd);


private:

    std::list<std::unique_ptr<AbstractSocket> > sockets;
    bool serverRunning;
    std::string serverHostname;
    std::string serverPort;
    int maxPendingConnections;
    std::shared_ptr<EpollEngineer> epollEngineer;

private:
    static std::vector<SuperTcpManager*> managers;

friend class EpollEngineer;
friend class ServerSocket;
friend class ClientSocket;

public:

    template<typename T, typename ... Args>
    static void printMyDebug(T a, Args... args)
    {
#ifndef QT_NO_DEBUG
        std::cerr << a << " ";
        printMyDebug(args ...);
#endif
    }

    template<typename T>
    static void printMyDebug(T a)
    {
#ifndef QT_NO_DEBUG
        std::cerr << a << "\n";
#endif
    }

};

#endif // SUPERTCPMANAGER_H
