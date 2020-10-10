#ifndef HTTPCONTEXT
#define HTTPCONTEXT
#include "httpRequest.h"
#include "httpParser.h"
#include <memory>
#include "base/buffer.h"


namespace budd 
{
namespace http 
{
class HttpRequest;
class HttpContext : public std::enable_shared_from_this<HttpContext>
{
public:
    HttpContext()
        : m_parser()
    {
    }
    HttpRequest getRequest() { return m_request; }
    HttpRequest *getRequestPointer() { return &m_request; }
    bool praseRequest(base::Buffer *buffer);

    void setParseFinish(bool finished) { m_parseFinished = finished; }
    bool isParseFinished() { return m_parseFinished; }

    void clear()
    {
        m_parseFinished = false;
        m_request.clear();
    }

private:
    HttpRequest m_request;
    bool m_parseFinished;
    HttpParser m_parser;
};
} //namespace budd
} //namespace http

#endif