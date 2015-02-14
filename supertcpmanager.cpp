#include "supertcpmanager.h"
#include <algorithm>
#include <random>
#include <ctime>
#include <dirent.h>


SuperTcpManager::SuperTcpManager(std::shared_ptr<Epoll> epollEngineerTmp)
    : epollEngineer(epollEngineerTmp),
      serverPort("34349"),
      serverHostname("INADDR_ANY")
{
    maxPendingConnections = 10;
    epollEngineer->addServer();
}

SuperTcpManager::SuperTcpManager()
    : SuperTcpManager(std::shared_ptr<Epoll>(new Epoll()))
{
}

SuperTcpManager::~SuperTcpManager()
{
    printMyDebug("Destructor manager start");
    while (!sockets.empty())
    {
        sockets.pop_back();
    }
    epollEngineer->removeServer();
    printMyDebug("Destructor manager closed sockets");
    printMyDebug("Destructor manager close");
}

void SuperTcpManager::makeSocketNonBlocking(int sfd)
{
    int flags, s;

    flags = fcntl(sfd, F_GETFL, 0);
    if (flags == -1)
    {
        throw SuperSocketOptionsException();
    }

    flags |= O_NONBLOCK;
    s = fcntl(sfd, F_SETFL, flags);
    if (s == -1)
    {
        throw SuperSocketOptionsException();
    }
}

int SuperTcpManager::createAndBind(const char* hostname, const char *port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        throw SuperSocketCreateException();
    }

    int optval = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (int)) == -1)
    {
        ::close(sock);
        throw SuperSocketCreateException();
    }

    sockaddr_in name;
    name.sin_family = AF_INET;
    name.sin_port = htons(std::strtoul(port, NULL, 0));
    name.sin_addr.s_addr = (strcmp(hostname, "INADDR_ANY") == 0) ? INADDR_ANY : (inet_addr(hostname));

    if (bind(sock, (sockaddr*)(&name), sizeof name) == -1)
    {
        printMyDebug("bind problem on", port, "num =", sock, "host", (inet_addr(hostname)));
        ::close(sock);
        throw SuperBindingException();
    }
    return sock;

}


ClientSocket * SuperTcpManager::connect(const char *hostname, const char *port, std::function<void(int)> clientFunc, char *clientBuffer, size_t clientBufferSize)
{
    printMyDebug("start connecting to", hostname, "on port", port);
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        throw SuperSocketCreateException();
    }

    hostent *hostinfo = gethostbyname(hostname);
    if (hostinfo == NULL)
    {
        ::close(sock);
        throw SuperInvalidArgumentException();
    }
    sockaddr_in servername;
    servername.sin_family = hostinfo->h_addrtype;
    servername.sin_port = htons(std::strtoul(port, NULL, 0));
    servername.sin_addr = *(in_addr*) hostinfo->h_addr;

    if (::connect(sock, (sockaddr*)&servername, sizeof servername) == -1)
    {
        ::close(sock);
        throw SuperConnectException();
    }

    makeSocketNonBlocking(sock);

    printMyDebug("connected to", hostname, " on ", port);
    sockets.push_back(std::unique_ptr<ClientSocket>(new ClientSocket(sock, *this, clientFunc, clientBuffer, clientBufferSize)));
    return (ClientSocket*)sockets.back().get();
}


std::shared_ptr<Epoll> SuperTcpManager::getEpollEngineer()
{
    return epollEngineer;
}

ServerSocket * SuperTcpManager::listen(std::function<void(ClientSocket *)> newConnection)
{
    return listen(serverPort, newConnection);
}

ServerSocket * SuperTcpManager::listen(const std::string &port, std::function<void(ClientSocket *)> newConnection)
{
    printMyDebug("start listening", port);
    int sfd = createAndBind(serverHostname.c_str(), port.c_str());

    if (::listen(sfd, maxPendingConnections) == -1)
    {
        ::close(sfd);
        throw SuperListeningException();
    }

    makeSocketNonBlocking(sfd);

    sockets.push_back(std::unique_ptr<ServerSocket>(new ServerSocket(sfd, *this, newConnection)));
    printMyDebug("end listening", port);
    return (ServerSocket*)sockets.back().get();
}

void SuperTcpManager::close(int fd)
{
    epollEngineer->removeFileDescriptor(fd);
}


const std::string& SuperTcpManager::getServerPort() const
{
    return serverPort;
}

void SuperTcpManager::setServerPort(const std::string& value)
{
    serverPort.assign(value);
}

int SuperTcpManager::getMaxPendingConnections() const
{
    return maxPendingConnections;
}

void SuperTcpManager::setMaxPendingConnections(int value)
{
    maxPendingConnections = value;
}

const std::string& SuperTcpManager::getServerHostname() const
{
    return serverHostname;
}

void SuperTcpManager::setServerHostname(const std::string& value)
{
    serverHostname.assign(value);
}

void SuperTcpManager::addSocket(int fd, std::function<void(epoll_event)> callback)
{
    epollEngineer->addFileDescriptor(fd, callback);
}

void SuperTcpManager::removeSocket(AbstractSocket* asocket)
{
    auto it = std::find_if(sockets.begin(), sockets.end(), [asocket](const std::unique_ptr<AbstractSocket>& socket)
    {
       return asocket == socket.get();
    });
    if (it != sockets.end())
    {
        sockets.erase(it);
    }
    else
    {
        printMyDebug("trouble");
    }
}

void SuperTcpManager::setFdOptions(int fd, unsigned int opt)
{
    epollEngineer->setFdOpt(fd, opt);
}
