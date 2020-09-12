#ifndef HTTPPARSER
#define HTTPPARSER

#include <memory>

class Buffer;
class HttpContext;
class HttpRequest;
class HttpParser
{
public:
    bool parse(Buffer* buffer, std::shared_ptr<HttpContext> context);

private:
    bool parseStartLine(const char* start, const char* end, HttpRequest* request);
    bool parseHeaderLine(const char* start, const char* end, HttpRequest* request);
    bool parseQueryParam(const char* start, const char* end, HttpRequest* request);
    bool parseQueryBody(const char* start, const char* end, HttpRequest* request);


    enum HttpParseState {
        EXPECTSTARTLINE,
        EXPECTHEADERLINE,
        EXPECTBODY,
        INVALID,
        FINISH,
    };

    HttpParseState m_state;
};

#endif