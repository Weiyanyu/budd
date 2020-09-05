#include "buffer.h"

const char* Buffer::CRLF = "\r\n";


Buffer::Buffer(size_t initialSize)
    :m_readerIndex(PREPEND_SIZE),
    m_writerIndex(PREPEND_SIZE),
    m_buffer(initialSize)
{

}


void Buffer::append(const char* data, size_t len)
{
    ensureWriterableBytes(len);
    std::copy(data, data + len, writeableBegin());

    assert(len <= writeableBytes());
    m_writerIndex += len;
}   

void Buffer::ensureWriterableBytes(size_t len)
{
    if (writeableBytes() < len) {
        reserve(len);       
    }
    assert(writeableBytes() >= len);
}

void Buffer::reserve(size_t len)
{
    if (writeableBytes() + prependableBytes() < len + PREPEND_SIZE) {
        m_buffer.resize(m_writerIndex + len);
    } else {
        //move content to front
        size_t oldReadableBytes = readableBytes();
        std::copy(begin() + m_readerIndex, begin() + m_writerIndex, begin() + PREPEND_SIZE);
        m_readerIndex = PREPEND_SIZE;
        m_writerIndex = m_readerIndex + oldReadableBytes;
        assert(oldReadableBytes == readableBytes());
    }
}

ssize_t Buffer::readFd(int socketFd, int* errorno)
{
    char extraBuffer[128 * 1024];
    struct iovec vec[2];
    size_t oldWriteableBytes = writeableBytes();
    vec[0].iov_base = begin() + m_writerIndex;
    vec[0].iov_len = oldWriteableBytes;
    vec[1].iov_base = extraBuffer;
    vec[1].iov_len = sizeof(extraBuffer);

    int ioctlCnt = 2;
    size_t readN = 0;
    readN = readv(socketFd, vec, ioctlCnt);
    if (readN < 0) {
        *errorno = errno;
    } else if (readN < oldWriteableBytes) {
        m_writerIndex += readN;
    } else {
        m_writerIndex = m_buffer.size();
        LOG(INFO) << "buffer size: " << m_buffer.size() << " readN : " << readN << " oldWriteableBytes : " << oldWriteableBytes << "  data len " << readN - oldWriteableBytes << "writeable size "<< writeableBytes();
        append(extraBuffer, readN - oldWriteableBytes);
    }

    return readN;
}