#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <vector>
#include <string>
#include <printf.h>
#include "supertcpmanager.h"

class HttpResponse
{
public:
    HttpResponse();
    ~HttpResponse();

    HttpResponse(const HttpResponse& response);
    HttpResponse(HttpResponse&& response);

    void addHeader(const std::pair<std::string, std::string>& header);

    void setHeaders(const std::vector< std::pair<std::string, std::string> >& headers);

    size_t getResponseSize() const
    {
        return responseBuffer.size();
    }

    void setResponseSize(size_t responseSize)
    {
        responseBuffer.resize(responseSize);
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

private:
    std::vector< std::pair<std::string, std::string> > headers;
    std::vector<char> responseBuffer;
};

#endif // HTTPRESPONSE_H
