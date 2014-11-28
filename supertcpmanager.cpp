#include "supertcpmanager.h"
#include <random>
#include <ctime>
#include <fcntl.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/un.h>


bool SuperTcpManager::programRunning(true);
int SuperTcpManager::countRunningServers(0);
int SuperTcpManager::counterId = 0;

void superSigHandler(int signo)
{
    if ( signo == SIGINT || signo == SIGTERM )
    {
        fprintf(stderr, "You kill me, NOOOOOOOOOOOO\n");
        SuperTcpManager::programRunning = false;
    }
    while (SuperTcpManager::countRunningServers > 0)
    {
//        fprintf(stderr, "count = %d\n", SuperTcpManager::countRunningServers);
    }

//    exit(0);
}


SuperTcpManager::SuperTcpManager()
    : epollEngineer(new EpollEngineer())
{
    signal(SIGINT, superSigHandler);
    signal(SIGTERM, superSigHandler);
    QObject::connect(epollEngineer.get(), SIGNAL(newMessageEpoll(int, QString)), this, SLOT(newMessageReceivedEpoll(int,QString)));
    QObject::connect(epollEngineer.get(), SIGNAL(newConnection(int)), this, SLOT(newConnectionEpoll(int)));
    QObject::connect(epollEngineer.get(), SIGNAL(closedConnection(int)), this, SLOT(closedConnectionEpoll(int)));
    serverPort = const_cast<char*>("34348");
//    serverHostname = const_cast<char*>("194.85.160.55");
//    defaultHostName = const_cast<char*>("172.21.4.168");
//    serverHostname = const_cast<char*>("188.227.78.184");
//    serverHostname = const_cast<char*>("192.108.2.90");
    serverHostname = const_cast<char*>("INADDR_ANY");
    maxPendingConnections = 10;
    epollEngineer->addServer();
}

SuperTcpManager::~SuperTcpManager()
{
    printDebug("Destructor start");
    for (auto pfd : listenSockets)
    {
        epollEngineer->removeListenSocket(pfd);
    }
    for (auto pfd : dataSocketToId)
    {
        epollEngineer->removeDataSocket(pfd.first);
    }
    epollEngineer->removeServer();
    printDebug("Destructor close");
}

void SuperTcpManager::newConnectionEpoll(int clientFd)
{
    addSocket(clientFd);
    emit newConnection(dataSocketToId[clientFd]);
}

void SuperTcpManager::closedConnectionEpoll(int clientFd)
{
    if (dataSocketToId.find(clientFd) != dataSocketToId.end())
    {
        dataSocketToId.erase(clientFd);
    }
}

void SuperTcpManager::newMessageReceivedEpoll(int clientFd, QString message)
{
    int clientId = dataSocketToId[clientFd];
    emit newMessageReceived(clientId, message);
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

int SuperTcpManager::createAndBind(const char *port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        throw SuperSocketCreateException();
    }

    int optval = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (int)) == -1)
    {
        throw SuperSocketCreateException();
    }

    sockaddr_in name;
    name.sin_family = AF_INET;
    name.sin_port = htons(std::strtoul(port, NULL, 0));
    name.sin_addr.s_addr = (strcmp(serverHostname, "INADDR_ANY") == 0) ? INADDR_ANY : (inet_addr(serverHostname));

    if (bind(sock, (sockaddr*)(&name), sizeof name) == -1)
    {
        printDebug("bind problem on", port, "num =", sock, "host", (inet_addr(serverHostname)));
        throw SuperBindingException();
    }
    return sock;

}


void SuperTcpManager::addSocket(int sockfd)
{
    epollEngineer->addDataSocket(sockfd);
    int newId = getNewId();
    dataIdToSocket[newId] = sockfd;
    dataSocketToId[sockfd] = newId;
}

void SuperTcpManager::addSocket(int sockfd, const char* port)
{
    epollEngineer->addListenSocket(sockfd);
    listenSockets.push_back(sockfd);
    listenPorts.push_back(std::strtoul(port, NULL, 0));
}

void SuperTcpManager::removeSocket(int sockfd, int sockId)
{
    if (sockId != -1)
    {
        epollEngineer->removeDataSocket(sockfd);
        dataIdToSocket.erase(sockId);
        dataSocketToId.erase(sockfd);
    }
    else
    {
        epollEngineer->removeListenSocket(sockfd);
        auto lit = find(listenSockets.begin(), listenSockets.end(), sockfd);
        size_t pos = lit - listenSockets.begin();
        listenSockets.erase(lit);
        listenPorts.erase(listenPorts.begin() + pos);
    }
}

int SuperTcpManager::getNewId()
{
    counterId++;
    if (counterId <= 0)
    {
        counterId = 1;
    }

    return counterId;
}

//void SuperTcpManager::removeId(int dataId)
//{
//    if (dataIdToSocket.find(dataId) == dataIdToSocket.end())
//    {
//        throw SuperInvalidArgumentException();
//    }
//    int sockfd = dataIdToSocket[dataId];
//    closeSocket(sockfd);
//    dataIdToSocket.erase(dataId);
//    dataSocketToId.erase(sockfd);
//}

int SuperTcpManager::connect(const char* hostname, const char *port)
{
    printDebug("start connecting to", hostname, "on port", port);
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        throw SuperSocketCreateException();
    }

    hostent *hostinfo = gethostbyname(hostname);
    if (hostinfo == NULL)
    {
        throw SuperInvalidArgumentException();
    }
    sockaddr_in servername;
    servername.sin_family = hostinfo->h_addrtype;
    servername.sin_port = htons(std::strtoul(port, NULL, 0));
    servername.sin_addr = *(in_addr*) hostinfo->h_addr;

    if (::connect(sock, (sockaddr*)&servername, sizeof servername) == -1)
    {
        throw SuperConnectException();
    }

    makeSocketNonBlocking(sock);

    addSocket(sock);
    printDebug("connected to", hostname, " on ", port);
    return sock;
}

void SuperTcpManager::disconnect(int dataId)
{
    auto fit = dataIdToSocket.find(dataId);
    if (fit == dataIdToSocket.end())
    {
        throw SuperInvalidArgumentException();
    }
    removeSocket(fit->second, fit->first);
}

int SuperTcpManager::sendToAll(const char *message, int len)
{
    if (len == -1)
    {
        len = strlen(message);
    }
    int result = 0;
    for (auto portFd : dataIdToSocket)
    {
        int s = ::write(portFd.second, message, len);
        if (s == -1)
        {
            printDebug("failed to send to", portFd.first);
            continue;
        }
        printDebug("sended to", portFd.first);
        result++;
    }
    return result;
}

int SuperTcpManager::sendToAllExclude(const std::vector<int> &blackList, const char *message, int len)
{
    if (len == -1)
    {
        len = strlen(message);
    }

    int result = 0;
    for (auto client : dataIdToSocket)
    {
        if (find(blackList.begin(), blackList.end(), client.first) == blackList.end())
        {
            try
            {
                sendTo(client.first, message, len);
            }
            catch (SuperTcpManagerException e)
            {
                result--;
            }
            result++;
        }
    }
    return result;
}

void SuperTcpManager::sendTo(int clientId, const char *message, int len)
{
    auto dit = dataIdToSocket.find(clientId);
    if (dit == dataIdToSocket.end())
    {
        throw SuperInvalidArgumentException();
    }

    if (len == -1)
    {
        len = strlen(message);
    }
    int s = ::write(dit->second, message, len);
    if (s == -1)
    {
        throw SuperSendException();
    }
}

int SuperTcpManager::listen()
{
    return listen(serverPort);
}

int SuperTcpManager::listen(const char* port)
{
    printDebug("start listening", port);
    int sfd = createAndBind(port);

    if (::listen(sfd, maxPendingConnections) == -1)
    {
        throw SuperListeningException();
    }

    makeSocketNonBlocking(sfd);

    addSocket(sfd, port);
    printDebug("end listening", port);
    return sfd;
}

void SuperTcpManager::unlisten(const char *port)
{
    int portNum = std::strtoul(port, NULL, 0);
    auto fit = find(listenPorts.begin(), listenPorts.end(), portNum);
    if (fit == listenPorts.end())
    {
        throw SuperInvalidArgumentException();
    }

    size_t pos = fit - listenPorts.begin();
    removeSocket(listenSockets[pos], -1);
}


void SuperTcpManager::execute()
{
}

const char *SuperTcpManager::getServerPort() const
{
    return serverPort;
}

void SuperTcpManager::setServerPort(const char *value)
{
    strcpy(serverPort, value);
}
int SuperTcpManager::getMaxPendingConnections() const
{
    return maxPendingConnections;
}

void SuperTcpManager::setMaxPendingConnections(int value)
{
    maxPendingConnections = value;
}

const char *SuperTcpManager::getServerHostname() const
{
    return serverHostname;
}

void SuperTcpManager::setServerHostname(const char *value)
{
    strcpy(serverHostname, value);
}

void SuperTcpManager::startServer()
{
//    serverRunning = true;
//    countRunningServers++;
//    try
//    {
//        execute();
//    }
//    catch (SuperTcpManagerException e)
//    {
//        serverRunning = false;
//        countRunningServers--;
//        throw e;
//    }
//    serverRunning = false;
//    countRunningServers--;
}

void SuperTcpManager::stopServer()
{
    serverRunning = false;
}
