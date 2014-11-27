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


bool SuperTcpManager::running = true;

void superSigHandler(int signo)
{
    if (signo == SIGINT || signo == SIGTERM)
    {
        SuperTcpManager::running = false;
        fprintf(stderr, "You kill me, NOOOOOOOOOOOO\n");
    }
//    exit(0);
}

SuperTcpManager::SuperTcpManager()
    : hostName("127.0.0.1")
{
    events = new epoll_event[MAXEVENTS];
    epollFileDescriptor = epoll_create1(0);
    signal(SIGINT, superSigHandler);
    signal(SIGTERM, superSigHandler);
}

SuperTcpManager::~SuperTcpManager()
{
    delete[] events;
    for (auto pfd : listeningPorts)
    {
        ::close(pfd);
    }
    fprintf(stderr, "Destructor\n");
}

int SuperTcpManager::makeSocketNonBlocking(int sfd)
{
    int flags, s;

    flags = fcntl(sfd, F_GETFL, 0);
    if (flags == -1)
    {
        return -1;
    }

    flags |= O_NONBLOCK;
    s = fcntl(sfd, F_SETFL, flags);
    if (s == -1)
    {
        return -1;
    }

    return 0;
}

int SuperTcpManager::createAndBind(const char *port)
{

    int sock;

    sockaddr_in name;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        return -1;
    }

    name.sin_family = AF_INET;
    name.sin_port = htons(std::strtoul(port, NULL, 0));
//    name.sin_addr.s_addr = htonl(INADDR_ANY);
    name.sin_addr.s_addr = inet_addr(hostName);

    if (bind(sock, (sockaddr*)(&name), sizeof name) == -1)
    {
        fprintf(stderr, "bind problem\n");
        return -1;
    }
    return sock;

    addrinfo hints;
    addrinfo *result;

    int sfd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_addr = INADDR_ANY;

    int s = getaddrinfo(NULL, port, &hints, &result);
    if (s != 0)
    {
        return -1;
    }

    addrinfo *rp;

    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        sfd = socket(rp->ai_family, rp->ai_flags, rp->ai_protocol);
        if (sfd == -1)
        {
            continue;
        }

        s = bind(sfd, rp->ai_addr, rp->ai_addrlen);
        if (s == -1)
        {
            ::close(sfd);
            continue;
        }

        break;
    }

    if (rp == NULL)
    {
        return -1;
    }

    freeaddrinfo(result);

    return sfd;
}

void SuperTcpManager::connect(const char* hostname, const char *port)
{
    fprintf(stderr, "start connecting %s to %s\n", hostname, port);
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        return;
    }

    hostent *hostinfo = gethostbyname(hostname);
    if (hostinfo == NULL)
    {
        fprintf(stderr, "unknown host %s\n", hostname);
        return;
    }
    sockaddr_in servername;
    servername.sin_family = hostinfo->h_addrtype;
    servername.sin_port = htons(std::strtoul(port, NULL, 0));
    servername.sin_addr = *(in_addr*) hostinfo->h_addr;

    if (::connect(sock, (sockaddr*)&servername, sizeof servername) == -1)
    {
        fprintf(stderr, "unable to connect\n");
        return;
    }

    listeningPorts.push_back(sock);
    fprintf(stderr, "end connecting %s to %s\n", hostname, port);
    return;

    addrinfo hints, *serverinfo, *p;
    fprintf(stderr, "start connecting %s to %s\n", hostname, port);
//    сhar buf[MAXDATASIZE];
//    сhar s[INET6_ADDRSTRLEN];

//    hostent *host = gethostbyname(hostname);
//    if (host == NULL)
//    {
//        fprintf(stderr, "unknown host: %s", hostname);
//        return;
//    }

//    int sockfd = socket(AF_UNSPEC, SOCK_STREAM, 0);
//    if (sockfd == -1)
//    {
//        fprintf(stderr, "unable to create socket");
//        return;
//    }

//    int broadcast = 1;


    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int rv;
    if ((rv = getaddrinfo(hostname, port, &hints, &serverinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %sn", gai_strerror(rv));
        return;
    }

    int sockfd;
    int optSocket = 1;
    for (p = serverinfo; p != NULL; p = p->ai_next)
    {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1)
        {
            fprintf(stderr, "fail socket: %d\n", p->ai_family);
            continue;
        }

//        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optSocket, sizeof (int)) == -1)
//        {
//            fprintf(stderr, "fail setsockopt: %d\n", p->ai_family);
//            return;
//        }

        if (::connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            fprintf(stderr, "fail connect: %d addr: %d\n", sockfd, p->ai_addr);
            ::close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        // fail to connect
        fprintf(stderr, "fail to connect because null\n");
        return;
    }

    listeningPorts.push_back(sockfd);
    fprintf(stderr, "end connecting %s to %s\n", hostname, port);
}

void SuperTcpManager::sendToAll(const char *message, const size_t len)
{
    for (auto portFd : listeningPorts)
    {
        int s = ::write(portFd, message, len);
        if (s == -1)
        {
            fprintf(stderr, "failed to send: %s", message);
            return;
        }
        fprintf(stderr, "sended: %s", message);
    }
}

void SuperTcpManager::listen(const char* port)
{
    fprintf(stderr, "start listening %s\n", port);
    int sfd = createAndBind(port);
    if (sfd == -1)
    {
        fprintf(stderr, "fail to create %s\n", port);
        return;
    }

    if (::listen(sfd, MAXCONNECTIONS) == -1)
    {
        fprintf(stderr, "fail epoll %d %d\n", epollFileDescriptor, sfd);
        return;
    }


    int s = makeSocketNonBlocking(sfd);
    if (s == -1)
    {
        fprintf(stderr, "fail to non block %s\n", port);
        return;
    }

    event.data.fd = sfd;
    event.events = EPOLLIN | EPOLLET;
    s = epoll_ctl(epollFileDescriptor, EPOLL_CTL_ADD, sfd, &event);
    if (s == -1)
    {
        fprintf(stderr, "fail epoll %d %d\n", epollFileDescriptor, sfd);
        return;
    }
    listeningPorts.push_back(sfd);
    fprintf(stderr, "end listening %s\n", port);
}

void SuperTcpManager::start()
{
    running = true;
    while (running)
    {
        fprintf(stderr, "start epoll\n");
        int nfds = epoll_wait (epollFileDescriptor, events, MAXEVENTS, -1);
        if (nfds == -1)
        {
            break;
        }
        fprintf(stderr, "get epoll events %d\n", nfds);
        for (int i = 0; i < nfds; i++)
        {
            if ((events[i].events & EPOLLERR) ||
                  (events[i].events & EPOLLHUP) ||
                  (!(events[i].events & EPOLLIN)))
            {
                // error
                ::close (events[i].data.fd);
                continue;
            }
            else if (find(listeningPorts.begin(), listeningPorts.end(), events[i].data.fd) != listeningPorts.end())
            {
                // input connection
                auto portFd =find(listeningPorts.begin(), listeningPorts.end(), events[i].data.fd);
                fprintf(stderr, "input connection on %d\n", *portFd);
                while (true)
                {
                    sockaddr in_addr;
                    socklen_t in_len = sizeof (struct sockaddr);
                    int infd = accept(*portFd, &in_addr, &in_len);
                    fprintf(stderr, "get accept: %d\n", infd);
                    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

                    if (infd == -1)
                    {
                        if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
                        {
                            break;
                        }
                        else
                        {
                            perror("accept");
                            break;
                        }
                    }

                    int s = getnameinfo(&in_addr, in_len,
                                    hbuf, sizeof hbuf,
                                    sbuf, sizeof sbuf,
                                    NI_NUMERICHOST | NI_NUMERICSERV);
                    if (s == 0)
                    {
                        printf("Accepted connection desc = %d host = %s port = %s", infd, hbuf, sbuf);
                    }

                    s = makeSocketNonBlocking(infd);
                    if (s == -1)
                    {
                        return;
                    }

                    event.data.fd = infd;
                    event.events = EPOLLIN | EPOLLET;
                    s = epoll_ctl(epollFileDescriptor, EPOLL_CTL_ADD, infd, &event);
                    fprintf(stderr, "get epoll: %d\n", s);
                    if (s == -1)
                    {
                        perror("epoll_ctl");
                        return;
                    }
                    break;
                }
//                listeningPorts.erase(portFd);
                continue;
            }
            else
            {
                bool closeConnection = false;

                while (true)
                {
                    char buf[MAXDATASIZE];
                    memset(buf, 0, sizeof buf);

                    ssize_t count = read(events[i].data.fd, buf, sizeof buf);
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

                    int s = write(1, buf, count);
                    fprintf(stderr, buf);
                    fprintf(stderr, "\n");
                    if (s == -1)
                    {
                        perror("write");
                        return;
                    }
                }

                if (closeConnection)
                {
                    printf("Close descriptor %d\n", events[i].data.fd);

                    ::close(events[i].data.fd);
                }

            }
        }
    }
    running = false;

}
