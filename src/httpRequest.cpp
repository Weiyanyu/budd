#include "httpRequest.h"

void HttpRequest::clear()
{
    m_method = "";
    m_url = "";
    m_path = "";
    m_httpVersion = "";
    m_query.clear();
    m_headers.clear();
}
