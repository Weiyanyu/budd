#include "httpContext.h"
#include "buffer.h"

bool HttpContext::praseRequest(Buffer* buffer)
{
    bool isSuccess = m_parser.parse(buffer, shared_from_this());
    if (isSuccess && !m_request.hasBody()) {
        setParseFinish(true);//set flag
    }
    return isSuccess;
}