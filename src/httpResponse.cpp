#include "httpResponse.h"
#include "buffer.h"
#include <fstream>
#include <cstring>

void HttpResponse::fillBuffer(Buffer *output)
{
    char buf[32];
    snprintf(buf, sizeof buf, "HTTP/1.1 %d ", m_statusCode);
    output->append(buf);
    output->append(m_statusMessage);
    output->append("\r\n");

    if (m_close)
    {
        output->append("Connection: close\r\n");
    }
    else
    {
        snprintf(buf, sizeof buf, "Content-Length: %zd\r\n", m_body.size());
        output->append(buf);
        output->append("Connection: Keep-Alive\r\n");
    }

    for (const auto &header : m_headers)
    {
        output->append(header.first);
        output->append(": ");
        output->append(header.second);
        output->append("\r\n");
    }

    output->append("\r\n");
    output->append(m_body);
}

bool HttpResponse::setFile(const std::string &filename)
{
    std::ifstream input(filename, std::ios::binary);
    if (!input.is_open())
    {
        LOG(ERROR) << "can't open file : " << filename;
        return false;
    }

    char buffer[1024];
    input.seekg(0, std::ios_base::end);
    auto fileSize = input.tellg();
    std::string body(fileSize, '\0');
    int n = 0;
    int readedNum = 0;
    input.seekg(0);

    while ((n = input.readsome(buffer, 1024)) > 0)
    {
        std::copy(buffer, buffer + n, (&*body.begin()) + readedNum);
        readedNum += n;
    }
    setBody(body);
    return true;
}
