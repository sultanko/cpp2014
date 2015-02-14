#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <vector>
#include <string>
#include <printf.h>
#include <algorithm>
#include <functional>
#include "httpserver.h"
#include "supertcpmanager.h"

class HttpServer;


class HttpResponse
{
public:
    HttpResponse();
    HttpResponse(HttpServer* server, ClientSocket* socket, std::function<void(HttpResponse*, int)> userCallback);
    ~HttpResponse();

    HttpResponse(const HttpResponse& response);
    HttpResponse(HttpResponse&& response);

    void close();

    void addHeader(const std::pair<std::string, std::string>& header);

    void setHeaders(const std::vector< std::pair<std::string, std::string> >& headers);

    size_t getResponseSize() const
    {
        return responseBuffer.size();
    }

    void setResponseSize(size_t responseSize)
    {
        responseBuffer.resize(responseSize);
        client->setBuffer(responseBuffer.data());
        client->setBufferSize(responseSize);
    }

    const std::vector<char>& getResponseBuffer() const
    {
        return responseBuffer;
    }

    std::vector<char>& getResponseBuffer()
    {
        return responseBuffer;
    }

    void setResponse(const std::vector<char>& response)
    {
        HttpResponse::responseBuffer.assign(response.begin(), response.end());
    }

    std::vector<std::pair<std::string, std::string> > &getHeaders()
    {
        return headers;
    }

    const std::vector<std::pair<std::string, std::string> > &getHeaders() const
    {
        return headers;
    }

    void createNewBuffer(size_t bufferSize);

    template<typename ForwardIt>
    ForwardIt searchQuote(ForwardIt begin, ForwardIt end, const std::string& str)
    {
        return std::search(begin, end, str.begin(), str.end());
    }


private:
    void getHeadersSocket(int count);
    void getContent(int count);
    void parseResponseHeader();

    std::vector< std::pair<std::string, std::string> > headers;
    std::vector<char> responseBuffer;
    std::vector<char> headerBuffer;
    std::function<void(HttpResponse*, int)> userCallback;
    ClientSocket* client;
    HttpServer* server;
    size_t responseGet;
    size_t bufSize;
    static const size_t DEFAULT_HEADER_BUFFER = 1024;

    friend class HttpServer;
};

#endif // HTTPRESPONSE_H
