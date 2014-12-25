#include <monetary.h>
#include "clientsocket.h"
#include "supertcpmanager.h"


ClientSocket::ClientSocket(int sockfd, SuperTcpManager &tcpManager, std::function<void(int)> dataReceived, char *buffer, size_t bufferSize, ServerSocket *server)
    : AbstractSocket(sockfd, tcpManager),
    server(server),
    userFunc(dataReceived),
    buffer(buffer),
    bufferSize(bufferSize),
    enableRead(true)
{
    SuperTcpManager::printMyDebug("Client constuctor full", sockfd);
    tcpManager.addSocket(sockfd, [&](epoll_event ev){
        if ((ev.events & EPOLLERR)
            || (ev.events & EPOLLHUP))
        {
            this->tcpManager.close(this->sockfd);
            this->userFunc(0);
        }
        else if (ev.events & EPOLLIN)
        {
            this->receiveData();
        }
        else if (ev.events & EPOLLOUT)
        {
            this->sendData();
        }
    });
}

ClientSocket::ClientSocket(int sockfd, SuperTcpManager &tcpManager, ServerSocket* server)
    : ClientSocket(sockfd, tcpManager, std::function<void(int)>(), nullptr, 0, server)
{
}

void ClientSocket::receiveData()
{
    if (buffer == nullptr || !enableRead)
    {
        return;
    }
    bool closeConnection = false;
    size_t total = 0;
    while (true)
    {
        ssize_t count = read(sockfd, buffer + total, bufferSize - total - 1);
        if (count == -1)
        {
            if (errno != EAGAIN)
            {
                closeConnection = true;
            }
            break;
        }
        else if (count == 0)
        {
            closeConnection = true;
            break;
        }

        total += count;
        buffer[total] = '\0';
        if (total + 1 >= bufferSize)
        {
            break;
        }
    }

    enableRead = false;
    if (closeConnection)
    {
        SuperTcpManager::printMyDebug("Close descriptor", sockfd);

        userFunc(0);
        close();
    }
    else
    {
        userFunc(total);
    }
}

ClientSocket::~ClientSocket()
{
    SuperTcpManager::printMyDebug("Client destructor", sockfd);
}

void ClientSocket::close()
{
    if (server == nullptr)
    {
        tcpManager.removeSocket(this);
    }
    else
    {
        server->removeClient(this);
    }
}

void ClientSocket::send(const std::string &message)
{
    countSended = ::send(sockfd, message.c_str(), message.size(), 0);
    if (countSended != message.size())
    {
        if (countSended < 0)
        {
            countSended = 0;
        }
        this->message = message;
        tcpManager.setFdOptions(sockfd, EpollEngineer::DEFAULT_EVENTS | EPOLLOUT);
    }
}

void ClientSocket::setFunction(std::function<void (int)> callback)
{
    userFunc = callback;
}

void ClientSocket::setBuffer(char *buffer)
{
    this->buffer = buffer;
}

void ClientSocket::setBufferSize(size_t bufferSize)
{
    this->bufferSize = bufferSize;
}

void ClientSocket::sendData()
{
    ssize_t writeBytes = ::send(sockfd, message.data() + sizeof(char) * countSended, message.size() - countSended, 0);
    if (writeBytes > 0)
    {
        countSended += writeBytes;
    }
    if (writeBytes <= 0 || countSended == message.size())
    {
        tcpManager.setFdOptions(sockfd, EpollEngineer::DEFAULT_EVENTS);
    }
}
