#include <dirent.h>
#include "httpresponse.h"

HttpResponse::HttpResponse()
{
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
}

HttpResponse::HttpResponse(const HttpResponse& response)
{
    SuperTcpManager::printDebug("copy response");
//    SuperTcpManager::printDebug("buffer", response.getResponseBuffer().size());
    this->setHeaders(response.getHeaders());
    HttpResponse::responseBuffer.assign(response.getResponseBuffer().begin(), response.getResponseBuffer().end());
}

HttpResponse::HttpResponse(HttpResponse &&response)
    : headers(std::move(response.headers)),
    responseBuffer(std::move(response.responseBuffer))
{
    SuperTcpManager::printDebug("move response");

}
