#ifndef SUPERTCPMANAGER_H
#define SUPERTCPMANAGER_H

#include <sys/socket.h>
#include <sys/epoll.h>
#include <QtGlobal>
#include <QObject>
#include <vector>

class SuperTcpManager
{
public:
    SuperTcpManager();
    void connect(const char* hostname, const char *port);
    void listen(const char *port);
    void start();
    void close();
    void sendToAll(const char *message, const size_t len);
    ~SuperTcpManager();

signals:
    void newConnection();

protected:
    int createAndBind(const char *port);
    int makeSocketNonBlocking(int sfd);

public:
    static bool running;

private:

    epoll_event event;
    epoll_event *events;
    sockaddr serverSocket;
    int epollFileDescriptor;
    std::vector<int> listeningPorts;
    char *hostName;

private:
    static const int MAXEVENTS = 64;
    static const int MAXDATASIZE = 512;
    static const int MAXCONNECTIONS = 10;

};

#endif // SUPERTCPMANAGER_H
