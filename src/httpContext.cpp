#include "httpContext.h"
#include "buffer.h"
#include "httpParser.h"

bool HttpContext::praseRequest(Buffer* buffer)
{
    HttpParser parser;
    bool isSuccess = parser.parse(buffer, shared_from_this());
    setParseFinish();//set flag
    return isSuccess;
}