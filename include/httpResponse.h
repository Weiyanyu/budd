#ifndef HTTPRESPONSE
#define HTTPRESPONSE
#include <unordered_map>
#include <cstring>
#include <glog/logging.h>
#include <string>
#include <sstream>

class Buffer;

enum HttpResponseStatusCode {
    UNKNOWN,
    OK = 200,
    NOCONTENT = 204,
    BAD_REQUEST = 400,
    NOT_FOUND = 404,
    INTERNAL_SERVER_ERROR = 500,
};

class ContentType {
public:
    //TEXT
    static const std::string TEXT_PLAIN;
    static const std::string TEXT_HTML;
    //IMAGE
    static const std::string IMAGE_JPEG;
    static const std::string IMAGE_PNG;

    //Application
    static const std::string APPLICATION_JSON;
};


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

    static std::string getStatusMessageByCode(HttpResponseStatusCode code) {
        if (httpResponseStatusMessage.count(code) < 0) {
            LOG(ERROR) << "status code error";
            return "";
        }
        return httpResponseStatusMessage[code];

    }
private:
    static std::unordered_map<HttpResponseStatusCode, std::string> httpResponseStatusMessage;

    HttpResponseStatusCode m_statusCode;
    std::string m_statusMessage;
    std::unordered_map<std::string, std::string> m_headers;
    bool m_close;
    std::string m_body;
};



#endif