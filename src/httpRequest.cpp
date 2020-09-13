#include "httpRequest.h"

void HttpRequest::clear()
{
    m_method = NONE;
    m_url = "";
    m_path = "";
    m_httpVersion = "";
    m_query.clear();
    m_headers.clear();
    m_formBody.clear();
    m_remoteAddress = "";
}
bool HttpRequest::setMethod(const std::string method)
{
    if (method == "GET")
    {
        m_method = HttpMethod::GET;
    }
    else if (method == "POST")
    {
        m_method = HttpMethod::POST;
    }
    else if (method == "PUT")
    {
        m_method = HttpMethod::PUT;
    }
    else if (method == "DELETE")
    {
        m_method = HttpMethod::DELETE;
    }
    else if (method == "HEAD")
    {
        m_method = HttpMethod::HEAD;
    }
    else
    {
        return false;
    }
    return true;
}
