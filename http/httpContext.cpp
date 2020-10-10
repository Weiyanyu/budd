#include "httpContext.h"

using namespace budd::base;
using namespace budd::http;

bool HttpContext::praseRequest(Buffer *buffer)
{
    bool isSuccess = m_parser.parse(buffer, shared_from_this());
    if (isSuccess && !m_request.hasBody())
    {
        setParseFinish(true); //set flag
    }
    return isSuccess;
}