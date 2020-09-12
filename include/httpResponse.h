#ifndef HTTPRESPONSE
#define HTTPRESPONSE
#include <unordered_map>
#include <cstring>
#include <glog/logging.h>
#include <string>
#include <sstream>
#include "httpParameter.h"

class Buffer;
class HttpResponse
{
public:
    HttpResponse() 
    : m_statusCode(HttpResponseStatusCode::UNKNOWN)
    {

    }

    HttpResponseStatusCode getStatusCode() { return m_statusCode; }

    void setStatusMessage(const std::string& statusMessage) { m_statusMessage = statusMessage; }
    void setStatusCode(HttpResponseStatusCode code) { m_statusCode = code; }
    void setClose(bool on) { m_close = on; }
    void setBody(const std::string& body) { m_body = body; }
    void setHeader(const std::string& key, const std::string& value) { m_headers[key] = value; }
    void setContentType(const std::string contentType) { setHeader("Content-Type", contentType); }
    void setKeepAliveLimit(int max, int timeout) {
        std::stringstream ss;
        ss << "max=" << max << ", timeout="<<timeout;
        setHeader("Keep-Alive", ss.str());
    }

    bool setFile(const std::string& filename);

    void fillBuffer(Buffer* buffer);

    bool needClose() { return m_close; }

private:
    static std::unordered_map<HttpResponseStatusCode, std::string> httpResponseStatusMessage;

    HttpResponseStatusCode m_statusCode;
    std::string m_statusMessage;
    std::unordered_map<std::string, std::string> m_headers;
    bool m_close;
    std::string m_body;
};



#endif