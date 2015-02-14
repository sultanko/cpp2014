#include "httpresponse.h"

HttpResponse::HttpResponse()
    : headerBuffer(DEFAULT_HEADER_BUFFER),
      server(nullptr),
      client(nullptr)
{
}

HttpResponse::HttpResponse(HttpServer* server, ClientSocket *socket, std::function<void(HttpResponse*, int)> userCallback)
    : HttpResponse()
{
    this->server = server;
    this->client = socket;
    this->userCallback = userCallback;
    this->client->setBuffer(this->headerBuffer.data());
    this->client->setBufferSize(this->headerBuffer.size());
    this->client->setFunction(std::bind(&HttpResponse::getHeadersSocket, this, std::placeholders::_1));
}


void HttpResponse::addHeader(const std::pair<std::string, std::string>& header)
{
    this->headers.push_back(header);
}

void HttpResponse::setHeaders(const std::vector<std::pair<std::string, std::string> >& headers)
{
    this->headers.assign(headers.begin(), headers.end());
}

void HttpResponse::createNewBuffer(size_t bufferSize)
{
    responseBuffer.resize(bufferSize);
}

HttpResponse::~HttpResponse()
{
    if (client != nullptr)
    {
        client->close();
    }
}

HttpResponse::HttpResponse(const HttpResponse& response)
{
    SuperTcpManager::printMyDebug("copy response");
    this->setHeaders(response.getHeaders());
    HttpResponse::responseBuffer.assign(response.getResponseBuffer().begin(), response.getResponseBuffer().end());
}

HttpResponse::HttpResponse(HttpResponse &&response)
    : headers(std::move(response.headers)),
    responseBuffer(std::move(response.responseBuffer))
{
    SuperTcpManager::printMyDebug("move response");

}

void HttpResponse::close()
{
    if (server != nullptr)
    {
        server->close(this);
    }
}

void HttpResponse::getHeadersSocket(int v)
{
    SuperTcpManager::printMyDebug("get somenthing on port", v, headerBuffer.data());
    auto pch = searchQuote(headerBuffer.begin(), headerBuffer.end(), "\n\r\n");
    if (v == 0)
    {
        client = nullptr;
        userCallback(this, -1);
        return;
    }
    if (bufSize == 0)
    {
        userCallback(this, 0);
    }
    bufSize += v;
    if (pch != headerBuffer.end())
    {
        parseResponseHeader();
        auto func = std::bind(&HttpResponse::getContent, this, std::placeholders::_1);
        client->setFunction(func);
    }
    else
    {
        client->setBuffer(&headerBuffer.data()[bufSize]);
        if (headerBuffer.size() - bufSize < 2)
        {
            headerBuffer.resize(bufSize * 2);
            client->setBuffer(headerBuffer.data());
            client->setBufferSize(headerBuffer.size());
        }
        client->setBufferSize(headerBuffer.size() - bufSize + 1);
    }
    client->setEnableRead(true);
    SuperTcpManager::printMyDebug("end headers");
}

void HttpResponse::getContent(int i)
{
    responseGet += i;
    SuperTcpManager::printMyDebug(responseGet);
    userCallback(this, responseGet);
    if (responseGet == getResponseSize())
    {
        client->close();
        client = nullptr;
        return;
    }
    if (i == 0)
    {
        client = nullptr;
        userCallback(this, -1);
        return;
    }
    client->setBuffer(&responseBuffer.data()[responseGet]);
    client->setBufferSize(responseBuffer.size() - responseGet + 1);
    client->setEnableRead(true);
}

void HttpResponse::parseResponseHeader()
{
    SuperTcpManager::printMyDebug("parse");
    const char* target = ":";
    auto res = searchQuote(headerBuffer.begin(), headerBuffer.end(), "HTTP/1.1");
    if (res == headerBuffer.end())
    {
        SuperTcpManager::printMyDebug("http 1.0");
        res = searchQuote(headerBuffer.begin(), headerBuffer.end(), "HTTP/1.0");
    }
    res = searchQuote(res, headerBuffer.end(), " ");
    while (((*res < '0') || (*res > '9')) && *res != '\n')
    {
        res++;
    }
    SuperTcpManager::printMyDebug("find status code", res[0]);
    if (!(res[0] == '2' && res[1] == '0' && res[2] == '0'))
    {
        SuperTcpManager::printMyDebug(res[0]);
        throw SuperTcpManager::SuperSendException();
    }
    headers.clear();
    res = searchQuote(res, headerBuffer.end(), "\n");
    SuperTcpManager::printMyDebug("start parsing headers", "content-length:", getResponseSize());
    while (res[1] != '\r' && res != headerBuffer.end())
    {
        res++;
        auto colon = searchQuote(res, headerBuffer.end(), ":");
        if (strncmp(&(*res), "Content-Length", 14) == 0)
        {
            char* beginLength = &(*colon);
            beginLength++;
            beginLength++;
            setResponseSize((size_t) std::atoi(beginLength));
        }
        auto nextTurn = searchQuote(res, headerBuffer.end(), "\n");
//        char* nextTurn = strstr(res, "\n");
        headers.push_back(std::make_pair(std::string(res, colon),
                                                       std::string(++colon, nextTurn)));
        res = nextTurn;
    }
    SuperTcpManager::printMyDebug("end parsing headers", "content-length:", getResponseSize());
    responseGet = 0;
    while (res != headerBuffer.end() && (res[0] == '\r' || res[0] == '\n'))
    {
        res++;
    }
    size_t pos = res - headerBuffer.begin();
    responseGet = headerBuffer.begin() + bufSize - res;
    std::copy(res, headerBuffer.begin() + bufSize, responseBuffer.begin());
    SuperTcpManager::printMyDebug(responseBuffer.data());
    client->setBuffer(&responseBuffer.data()[responseGet]);
    client->setBufferSize(responseBuffer.size() - responseGet + 1);
    if (responseGet != 0)
    {
        userCallback(this, responseGet);
    }
    SuperTcpManager::printMyDebug("end parse", responseGet, getResponseSize());
}
