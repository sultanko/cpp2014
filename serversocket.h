#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include "abstractsocket.h"
#include "clientsocket.h"
#include "supertcpmanager.h"
#include <list>
#include <memory>

class ClientSocket;

class ServerSocket : public AbstractSocket
{
public:
    ServerSocket(int sockfd, SuperTcpManager &tcpManager, std::function<void(ClientSocket *)> newConnection);
    void send(const std::string& message);
    void removeClient(ClientSocket* client);
    void close();
    ~ServerSocket();
private:
    std::list<std::unique_ptr<ClientSocket> > clientSockets;
    std::function<void(ClientSocket*)> newConnection;

};

#endif // SERVERSOCKET_H
