#include "buffer.h"
#include "httpContext.h"
#include "httpParser.h"
#include "httpParameter.h"
#include <iostream>

bool HttpParser::parse(Buffer *buffer, std::shared_ptr<HttpContext> context)
{
    bool isSuccess = false;
    bool hasNextLine = true;
    HttpRequest *req = context->getRequestPointer();
    while (hasNextLine)
    {
        if (m_state == HttpParser::EXPECTSTARTLINE || m_state == HttpParser::EXPECTHEADERLINE)
        {

            const char *crlf = buffer->findCrlf();
            if (crlf != nullptr)
            {
                if (m_state == HttpParser::EXPECTSTARTLINE)
                {
                    //parseStartLine need set m_state
                    isSuccess = parseStartLine(buffer->peek(), crlf, req);
                }
                else if (m_state == HttpParser::EXPECTHEADERLINE)
                {
                    //parseHeaderLine need set m_state
                    isSuccess = parseHeaderLine(buffer->peek(), crlf, req);
                }
                if (isSuccess)
                {
                    //process buffer
                    buffer->retrieveUntil(crlf + 2);
                }
                else
                {
                    hasNextLine = false;
                }
            }
            else
            {
                hasNextLine = false;
            }
        }
        else if (m_state == HttpParser::EXPECTBODY)
        {
            if (m_state == HttpParser::EXPECTBODY)
            {
                isSuccess = parseFormBody(buffer->peek(), buffer->peek() + buffer->readableBytes(), req);
            }
            if (isSuccess)
            {
                buffer->retrieveAll();
            }
            hasNextLine = false;
        }
        else
        {
            hasNextLine = false;
        }
    }

    return isSuccess;
}

/**
 * parse http start line 
 * 
 * 
 * */
bool HttpParser::parseStartLine(const char *start, const char *end, HttpRequest *request)
{
    bool isSuccess = false;
    //1. process method
    const char *space = std::find(start, end, ' ');
    if (space != end && request->setMethod(std::string(start, space)))
    {
        std::string method(start, space);
        start = space + 1;
        //2. process uri
        space = std::find(start, end, ' ');
        if (space != end)
        {
            //2.1 process URL
            request->setUrl(std::string(start, space));
            //2.2 process path and query param
            const char *question = std::find(start, space, '?');
            if (question != space)
            {
                request->setPath(std::string(start, question));
                if (!parseQueryParam(question + 1, space, request))
                {
                    return false;
                }
            }
            else
            {
                request->setPath(std::string(start, space));
            }
        }
        start = space + 1;
        isSuccess = end - start == 8 && std::equal(start, end - 1, "HTTP/1.");
        if (isSuccess)
        {
            if (*(end - 1) == '0' || *(end - 1) == '1')
            {
                request->setHttpVersion(std::string(start, end));
            }
            else
            {
                isSuccess = false;
            }
        }
    }
    if (isSuccess)
    {
        m_state = HttpParser::EXPECTHEADERLINE;
    }
    return isSuccess;
}

/**
 * parse http header line 
 * if occur some error or occur empty line, will return false
 * 
 * */
bool HttpParser::parseHeaderLine(const char *start, const char *end, HttpRequest *request)
{
    bool isSuccess = true;
    const char *colon = std::find(start, end, ':');

    if (colon != end)
    {
        request->setHeader(std::string(start, colon), std::string(colon + 2, end));
    }
    else
    {
        //empty line
        if (request->contentLenth() > 0)
        {
            request->setHasBody(true);
        }

        if (request->contentType() == ContentType::APPLICATION_FORM &&
            request->method() == HttpMethod::POST &&
            request->hasBody())
        {
            m_state = HttpParser::EXPECTBODY;
        }
        else
        {
            m_state = HttpParser::EXPECTSTARTLINE;
        }
    }
    return isSuccess;
}

bool HttpParser::parseQueryParam(const char *start, const char *end, HttpRequest *request)
{
    bool isSuccess = true;

    const char *et = std::find(start, end, '&');
    while (et != end)
    {
        const char *equal = std::find(start, et, '=');
        if (equal != et)
        {
            request->setQueryParam(std::string(start, equal), std::string(equal + 1, et));
            start = et + 1;
            et = std::find(start, end, '&');
        }
        else
        {
            isSuccess = false;
            break;
        }
    }
    //process last element
    const char *equal = std::find(start, end, '=');
    if (equal != end)
    {
        request->setQueryParam(std::string(start, equal), std::string(equal + 1, end));
    }
    else
    {
        isSuccess = false;
    }
    return isSuccess;
}

bool HttpParser::parseFormBody(const char *start, const char *end, HttpRequest *request)
{
    bool isSuccess = true;
    if (start == end)
    {
        if (!request->hasBody())
        {
            m_state = HttpParser::EXPECTSTARTLINE;
        }
        return true;
    }
    if (request->contentType() == ContentType::APPLICATION_FORM)
    {
        //parse form body
        const char *et = std::find(start, end, '&');
        while (et != end)
        {
            const char *equal = std::find(start, et, '=');
            if (equal != et)
            {
                request->setFormBody(std::string(start, equal), std::string(equal + 1, et));
                start = et + 1;
                et = std::find(start, end, '&');
            }
            else
            {
                isSuccess = false;
                break;
            }
        }
        //process last element
        const char *equal = std::find(start, end, '=');
        if (equal != end)
        {
            request->setFormBody(std::string(start, equal), std::string(equal + 1, end));
        }
        else
        {
            isSuccess = false;
        }
        isSuccess = true;
    }
    if (isSuccess)
    {
        m_state = HttpParser::EXPECTSTARTLINE;
        request->setHasBody(false);
    }
    else
    {
        m_state = HttpParser::INVALID;
    }
    return isSuccess;
}