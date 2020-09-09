#ifndef HTTPCONTEXT
#define HTTPCONTEXT
#include "httpRequest.h"
#include <memory>

class HttpRequest;
class Buffer;
class HttpContext : public std::enable_shared_from_this<HttpContext> {
public:
    
    HttpRequest getRequest() { return m_request; }
    HttpRequest* getRequestPointer(){ return &m_request; }
    bool praseRequest(Buffer* buffer);

    void setParseFinish() { m_parseFinished = true; }
    bool isParseFinished() { return m_parseFinished; }

    void clear()
    {
        LOG(INFO) << "close previous request info";
        m_parseFinished = false;
        m_request.clear();
    }

private:
    HttpRequest m_request;
    bool m_parseFinished;
};

#endif