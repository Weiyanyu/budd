#include "httpParameter.h"

using namespace budd::http;

//--------- CcontentType definetion ---------------
const std::string ContentType::TEXT_PLAIN = "text/plain;charset=utf-8";
const std::string ContentType::TEXT_HTML = "text/html;charset=utf-8";

const std::string ContentType::IMAGE_JPEG = "image/jpeg";
const std::string ContentType::IMAGE_PNG = "image/png";

const std::string ContentType::APPLICATION_JSON = "application/json;charset=utf-8";
const std::string ContentType::APPLICATION_OCTEC_STREAM = "application/octet-stream";
const std::string ContentType::APPLICATION_FORM = "application/x-www-form-urlencoded";

// ----------------- Http Util ---------------------
const std::unordered_map<HttpResponseStatusCode, std::string> HttpUtil::m_httpResponseStatusMessage =
    {
        {UNKNOWN, "unknown"},
        {CONTINUE, "Continue"},
        {OK, "OK"},
        {NOCONTENT, "No Content"},
        {BAD_REQUEST, "Bad Request"},
        {NOT_FOUND, "Not Found"},
        {INTERNAL_SERVER_ERROR, "Internal Server Error"},
};

std::string HttpUtil::getStatusMessageByCode(HttpResponseStatusCode code)
{
    auto it = m_httpResponseStatusMessage.find(code);
    if (it == m_httpResponseStatusMessage.end())
    {
        LOG(ERROR) << "status code error";
        return "";
    }
    return it->second;
}
