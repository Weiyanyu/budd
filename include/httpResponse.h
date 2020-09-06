#ifndef HTTPRESPONSE
#define HTTPRESPONSE
#include <unordered_map>
#include <cstring>

class Buffer;

enum HttpResponseStatusCode {
    UNKNOWN,
    OK = 200,
    NOCONTENT = 204,
    BAD_REQUEST = 400,
    NOT_FOUND = 404,
    BAD_SERVER = 500,
};



class HttpResponse
{
public:

    HttpResponse() 
    : m_statusCode(HttpResponseStatusCode::UNKNOWN)
    {

    }

    void setStatusMessage(const std::string& statusMessage) { m_statusMessage = statusMessage; }
    void setStatusCode(HttpResponseStatusCode code) { m_statusCode = code; }
    void setClose(bool on) { m_close = on; }
    void setBody(const std::string& body) { m_body = body; }
    void setHeader(const std::string& key, const std::string& value) { m_headers[key] = value; }

    void fillBuffer(Buffer* buffer);

    bool needClose() { return m_close; }
private:
    HttpResponseStatusCode m_statusCode;
    std::string m_statusMessage;
    std::unordered_map<std::string, std::string> m_headers;
    bool m_close;
    std::string m_body;
};



#endif