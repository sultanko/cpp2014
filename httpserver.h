#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "httprequest.h"
#include "httpresponse.h"
#include "supertcpmanager.h"
#include <algorithm>
#include <list>

class HttpResponse;

class HttpServer
{
public:
    struct HttpServerException : public std::exception {};
    struct HttpInvalidHostException : public HttpServerException {};
    struct HttpConnectionException : public HttpServerException {};
public:
    HttpServer();
    ~HttpServer();
    void send(const HttpRequest &request, std::function<void(HttpResponse*, int)> userCallback);
    void close(const HttpResponse* response);

private:

    SuperTcpManager tcpManager;
    std::list<HttpResponse> responses;
};

#endif // HTTPSERVER_H
