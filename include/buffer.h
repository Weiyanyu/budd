#ifndef BUDDBUFFER_H
#define BUDDBUFFER_H

#include <cassert>
#include <vector>
#include <string>
#include <cstring>
#include <sys/uio.h>
#include <glog/logging.h>
#include <algorithm>

class Buffer
{
public:
    static const size_t INITIAL_SIZE = 1024 * 128;
    static const size_t PREPEND_SIZE = 8;
    static const char* CRLF;

    explicit Buffer(size_t initialSize = INITIAL_SIZE);
    //just use default copy, move constructor

    void append(const std::string &data) { append(data.data(), data.size()); }

    void append(const void *data, size_t len) { append(static_cast<const char *>(data), len); }

    void append(const char *data, size_t len);

    const char *begin() const {return &*m_buffer.begin();}
    char *begin() { return &*m_buffer.begin();}

    const char* writeableBegin() const{ return begin() + m_writerIndex; }
    char* writeableBegin() { return begin() + m_writerIndex; }

    size_t writerIndex() { return m_writerIndex; }
    size_t readerIndex() { return m_readerIndex; }

    size_t writeableBytes() const { return m_buffer.size() - m_writerIndex; }
    size_t readableBytes() const { return m_writerIndex - m_readerIndex; }

    size_t prependableBytes() { return m_readerIndex; }

    const char* peek() const { return begin() + m_readerIndex; }
    int peekInt() const
    {
        int res;
        std::memcpy(&res, peek(), sizeof(int));
        return res;
    }

    void retrieve(size_t len) {
        if (len < readableBytes()) {
        assert(len <= readableBytes());
            m_readerIndex += len;
        } else {
            retrieveAll();
        }
    }

    void retrieveAll() 
    {
        m_readerIndex = PREPEND_SIZE;
        m_writerIndex = PREPEND_SIZE;
    }

    void retrieveInt() {
        retrieve(sizeof(int));
    }

    std::string retrieveAsString(size_t len)
    {
        assert(len <= readableBytes());
        std::string res(peek(), len);
        retrieve(len);
        return res;
    }

    std::string retrieveAllAsSrting()
    {
        return retrieveAsString(readableBytes());
    }

    void retrieveUntil(const char* end)
    {
        assert(peek() <= end);
        assert(end <= writeableBegin());
        retrieve(end - peek());
    }

    const char* findCrlf() {
        const char* crlf = std::search(peek(), (const char*)writeableBegin(), CRLF, CRLF + 2);
        return crlf == writeableBegin() ? nullptr : crlf;
    }


    ssize_t readFd(int socketFd, int* errorno);

private:
    size_t m_readerIndex;
    size_t m_writerIndex;

    void ensureWriterableBytes(size_t len);
    void reserve(size_t len);

    std::vector<char> m_buffer;
};

#endif