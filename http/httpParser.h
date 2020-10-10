#ifndef HTTPPARSER
#define HTTPPARSER
#include <memory>
#include "base/buffer.h"


namespace budd 
{
namespace http 
{
class HttpContext;
class HttpRequest;
class HttpParser
{
public:
    HttpParser()
        : m_state(EXPECTSTARTLINE)
    {
    }
    bool parse(base::Buffer *buffer, std::shared_ptr<HttpContext> context);

private:
    bool parseStartLine(const char *start, const char *end, HttpRequest *request);
    bool parseHeaderLine(const char *start, const char *end, HttpRequest *request);
    bool parseQueryParam(const char *start, const char *end, HttpRequest *request);
    bool parseFormBody(const char *start, const char *end, HttpRequest *request);
    enum HttpParseState
    {
        EXPECTSTARTLINE,
        EXPECTHEADERLINE,
        EXPECTBODY,
        INVALID,
        FINISH,
    };

    HttpParseState m_state;
};
} //namespace budd
} //namespace http
#endif