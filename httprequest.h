#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <string>
#include <vector>

class HttpRequest
{
public:
    HttpRequest();

    const std::vector<std::pair<std::string, std::string>> &getHeaders() const
    {
        return headers;
    }

    void setHeaders(const std::vector<std::pair<std::string, std::string> >& headers)
    {
        HttpRequest::headers.assign(headers.begin(), headers.end());
    }

    const std::string &getHost() const
    {
        return host;
    }

    void setHost(const std::string &host)
    {
        HttpRequest::host = host;
    }

    const std::string &getUrl() const
    {
        return url;
    }

    void setUrl(const std::string &url)
    {
        HttpRequest::url = url;
    }


    const std::string &getMethod() const
    {
        return method;
    }

    void setMethod(const std::string &method)
    {
        HttpRequest::method = method;
    }

private:
    std::vector< std::pair<std::string, std::string> > headers;
    std::string host;
    std::string url;
    std::string method;
};

#endif // HTTPREQUEST_H
