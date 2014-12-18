#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "httprequest.h"
#include "httpresponse.h"
#include <algorithm>

class HttpServer
{
public:
    struct HttpServerException : public std::exception {};
    struct HttpInvalidHostException : public HttpServerException {};
    struct HttpConnectionException : public HttpServerException {};
public:
    HttpServer();
    ~HttpServer();
    void send(const HttpRequest &request, std::function<void(HttpResponse)> userCallback);

private:
    void getHeaders(int count);
    void getContent(int count);
    void parseResponseHeader();
    template<typename ForwardIt>
    ForwardIt searchQuote(ForwardIt begin, ForwardIt end, const std::string& str)
    {
        return std::search(begin, end, str.begin(), str.end());
    }

    std::function<void(HttpResponse)> userCallback;
    HttpResponse response;
    SuperTcpManager tcpManager;
    ClientSocket* client;
    std::vector<char> headerBuffer;
    size_t bufSize;
    size_t responseGet;
    static const int DEFAULT_HEADER_BUF = 1024;
};

#endif // HTTPSERVER_H
