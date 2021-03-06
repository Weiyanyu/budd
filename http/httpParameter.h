#ifndef HTTPPARAMETER
#define HTTPPARAMETER

#include <string>
#include <unordered_map>
#include <glog/logging.h>
namespace budd 
{
namespace http 
{
enum HttpResponseStatusCode
{
    UNKNOWN,
    CONTINUE = 100,
    OK = 200,
    NOCONTENT = 204,
    BAD_REQUEST = 400,
    NOT_FOUND = 404,
    INTERNAL_SERVER_ERROR = 500,
};

class ContentType
{
public:
    //TEXT
    static const std::string TEXT_PLAIN;
    static const std::string TEXT_HTML;
    //IMAGE
    static const std::string IMAGE_JPEG;
    static const std::string IMAGE_PNG;

    //Application
    static const std::string APPLICATION_JSON;
    static const std::string APPLICATION_OCTEC_STREAM;
    static const std::string APPLICATION_FORM;
};

enum HttpMethod
{
    NONE,
    GET,
    POST,
    PUT,
    DELETE,
    HEAD,
};

class HttpUtil
{
public:
    static std::string getStatusMessageByCode(HttpResponseStatusCode code);
private:
    static const std::unordered_map<HttpResponseStatusCode, std::string> m_httpResponseStatusMessage;

};
} //namespace budd
} //namespace http

#endif
