#ifndef HTTPPARSER
#define HTTPPARSER

#include <memory>


class Buffer;
class HttpContext;
class HttpRequest;
class HttpParser
{
public:
    enum HttpParseState {
        EXPECTSTARTLINE,
        EXPECTHEADERLINE,
        EXPECTBODY,
        INVALID,
        FINISH,
    };

    HttpParser() 
        :m_state(EXPECTSTARTLINE)
    {

    }
    bool parse(Buffer* buffer, std::shared_ptr<HttpContext> context);
    HttpParseState getState() { return m_state; }
    void setState(HttpParseState state) { m_state = state; }

private:
    bool parseStartLine(const char* start, const char* end, HttpRequest* request);
    bool parseHeaderLine(const char* start, const char* end, HttpRequest* request);
    bool parseQueryParam(const char* start, const char* end, HttpRequest* request);
    bool parseQueryBody(const char* start, const char* end, HttpRequest* request);


    HttpParseState m_state;
};

#endif