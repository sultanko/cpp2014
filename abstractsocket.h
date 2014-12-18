#ifndef ABSTRACTSOCKET_H
#define ABSTRACTSOCKET_H

#include <string>
//#include "supertcpmanager.h"
#include <functional>
#include <list>

class SuperTcpManager;

class AbstractSocket
{
public:
    AbstractSocket(int sockfd, SuperTcpManager& tcpManager);
    virtual void close() = 0;
    virtual ~AbstractSocket();
    AbstractSocket(AbstractSocket&& as) = delete;
    virtual void send(const std::string& message) = 0;
    AbstractSocket(const AbstractSocket& as) = delete;
    AbstractSocket& operator=(const AbstractSocket& as) = delete;

protected:
    int sockfd;
    SuperTcpManager& tcpManager;

    friend class SuperTcpManager;
};

#endif // ABSTRACTSOCKET_H
