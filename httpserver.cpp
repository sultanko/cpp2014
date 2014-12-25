#include "httpserver.h"
#include <cstring>

HttpServer::HttpServer()
{
    SuperTcpManager::printMyDebug("http constructor");
}

void HttpServer::send(const HttpRequest &requestHttp, std::function<void(HttpResponse*, int)> userCallback)
{
    try
    {
        responses.emplace_back();
        HttpResponse& response = responses.back();
        response.server = this;
        response.userCallback = userCallback;
        auto func = std::bind(&HttpResponse::getHeadersSocket, &response, std::placeholders::_1);
        response.client = tcpManager.connect(requestHttp.getHost().c_str(), "80", func, response.headerBuffer.data(), response.headerBuffer.size());
        SuperTcpManager::printMyDebug("connected to host");
    }
    catch (SuperTcpManager::SuperTcpManagerException const& e)
    {
        responses.pop_back();
        throw HttpConnectionException();
    }
    std::string request = "GET " + requestHttp.getUrl() + " HTTP/1.1\n" + "Host: " + requestHttp.getHost() + "\n";
    for (auto it = requestHttp.getHeaders().begin(); it != requestHttp.getHeaders().end(); it++)
    {
        request += it->first + ": \"" + it->second + "\"\n";
    }
    request += "\r\n";
    SuperTcpManager::printMyDebug(request);
    responses.back().client->send(request);
}

void HttpServer::close(const HttpResponse *response)
{
    auto it = std::find_if(responses.begin(), responses.end(),
    [&](const HttpResponse& listElement)
    {
       return &listElement == response;
    });
    if (it != responses.end())
    {
        responses.erase(it);
    }
}


HttpServer::~HttpServer()
{
}
