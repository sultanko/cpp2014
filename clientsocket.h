#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <dirent.h>
#include "abstractsocket.h"
#include "supertcpmanager.h"
#include "serversocket.h"
//#include "serversocket.h"

//class AbstractSocket;
class ServerSocket;

class ClientSocket : public AbstractSocket
{
public:
    ClientSocket(int sockfd, SuperTcpManager &tcpManager, ServerSocket* server);
    ClientSocket(int sockfd, SuperTcpManager &tcpManager, std::function<void(int)> dataReceived, char *buffer, size_t bufferSize, ServerSocket *server = nullptr);
    void send(const std::string& message);
    void setFunction(std::function<void(int)> callback);
    void setBuffer(char *buffer);
    void setBufferSize(size_t bufferSize);
    void close();
    ~ClientSocket();
private:
    void receiveData();

    ServerSocket* server;
    std::function<void(int)> userFunc;
    char* buffer;
    size_t bufferSize;
public:
    bool isEnableRead() const
    {
        return enableRead;
    }

    void setEnableRead(bool enableRead)
    {
        ClientSocket::enableRead = enableRead;
    }

private:
    bool enableRead;
    std::string message;
    ssize_t countSended;

    void sendData();
};

#endif // CLIENTSOCKET_H
