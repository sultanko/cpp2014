#include "httpserver.h"
#include <cstring>

HttpServer::HttpServer()
    : client(nullptr)
{
    SuperTcpManager::printDebug("http constructor");
    headerBuffer.resize(DEFAULT_HEADER_BUF);
}

void HttpServer::send(const HttpRequest &requestHttp, std::function<void(HttpResponse)> userCallback)
{
    bufSize = 0;
    try
    {
        if (client != nullptr)
        {
            client->close();
        }
        auto func = std::bind(&HttpServer::getHeaders, this, std::placeholders::_1);
        client = tcpManager.connect(requestHttp.getHost().c_str(), "80", func, headerBuffer.data(), DEFAULT_HEADER_BUF);
        HttpServer::userCallback = userCallback;
        SuperTcpManager::printDebug("connected to host");
    }
    catch (SuperTcpManager::SuperTcpManagerException const& e)
    {
        client = nullptr;
        throw HttpConnectionException();
    }
    std::string request = "GET " + requestHttp.getUrl() + " HTTP/1.1\n" + "Host: " + requestHttp.getHost() + "\n";
    for (auto it = requestHttp.getHeaders().begin(); it != requestHttp.getHeaders().end(); it++)
    {
        request += it->first + ": \"" + it->second + "\"\n";
    }
    request += "\r\n";
    SuperTcpManager::printDebug(request);
    client->send(request);
}


HttpServer::~HttpServer()
{
}

void HttpServer::getHeaders(int v)
{
    if (v == 0)
    {
        client = nullptr;
        throw HttpConnectionException();
    }
    SuperTcpManager::printDebug("get somenthing on port", v, headerBuffer.data());
    auto pch = searchQuote(headerBuffer.begin(), headerBuffer.end(), "\n\r\n");
    bufSize += v;
    if (pch != headerBuffer.end())
    {
        parseResponseHeader();
        auto func = std::bind(&HttpServer::getContent, this, std::placeholders::_1);
        client->setFunction(func);
    }
    else
    {
        client->setBuffer(&headerBuffer.data()[bufSize]);
        if (headerBuffer.size() - bufSize < 2)
        {
            headerBuffer.resize(bufSize * 2);
        }
        client->setBufferSize(headerBuffer.size() - bufSize + 1);
    }
    client->setEnableRead(true);
    SuperTcpManager::printDebug("end headers");
}

void HttpServer::getContent(int i)
{
    responseGet += i;
    SuperTcpManager::printDebug(responseGet);
    if (responseGet == response.getResponseSize())
    {
        userCallback(response);
        client->close();
        client = nullptr;
        return;
    }
    if (i == 0)
    {
        client = nullptr;
        throw HttpConnectionException();
    }
    client->setBuffer(&response.getResponseBuffer().data()[responseGet]);
    client->setBufferSize(response.getResponseSize() - responseGet + 1);
    client->setEnableRead(true);
}

void HttpServer::parseResponseHeader()
{
    SuperTcpManager::printDebug("parse");
    const char* target = ":";
    auto res = searchQuote(headerBuffer.begin(), headerBuffer.end(), "HTTP/1.1");
    if (res == headerBuffer.end())
    {
        SuperTcpManager::printDebug("http 1.0");
        res = searchQuote(headerBuffer.begin(), headerBuffer.end(), "HTTP/1.0");
    }
    SuperTcpManager::printDebug("http", res[0]);
    res = searchQuote(res, headerBuffer.end(), " ");
    while (((*res < '0') || (*res > '9')) && *res != '\n')
    {
        res++;
    }
    SuperTcpManager::printDebug("find status code", res[0]);
    if (!(res[0] == '2' && res[1] == '0' && res[2] == '0'))
    {
        SuperTcpManager::printDebug(res[0]);
        throw HttpInvalidHostException();
    }
    response.getHeaders().clear();
    res = searchQuote(res, headerBuffer.end(), "\n");
    SuperTcpManager::printDebug("start parsing headers", "content-length:", response.getResponseSize());
    while (res[1] != '\r' && res != headerBuffer.end())
    {
        res++;
        auto colon = searchQuote(res, headerBuffer.end(), ":");
        if (strncmp(&(*res), "Content-Length", 14) == 0)
        {
            char* beginLength = &(*colon);
            beginLength++;
            beginLength++;
            response.setResponseSize((size_t) std::atoi(beginLength));
        }
        auto nextTurn = searchQuote(res, headerBuffer.end(), "\n");
//        char* nextTurn = strstr(res, "\n");
        response.getHeaders().push_back(std::make_pair(std::string(res, colon),
                                                       std::string(++colon, nextTurn)));
        res = nextTurn;
    }
    SuperTcpManager::printDebug("end parsing headers", "content-length:", response.getResponseSize());
    responseGet = 0;
    while (res != headerBuffer.end() && (res[0] == '\r' || res[0] == '\n'))
    {
        res++;
    }
    while (*res != '\0')
    {
        res++;
        responseGet++;
    }
    std::copy(res, headerBuffer.end(), response.getResponseBuffer().begin());
    client->setBuffer(&response.getResponseBuffer().data()[responseGet]);
    client->setBufferSize(response.getResponseSize() - responseGet + 1);
//    memcpy(response.getResponseBuffer().data(), src, responseGet);
    SuperTcpManager::printDebug("end parse", response.getResponseSize());
}
