#include "tcpconnection.h"
#include "eventLoop.h"
#include "buffer.h"

TcpConnection::TcpConnection(EventLoop *eventLoop, int sockfd, const char *clientIp)
    : m_eventLoop(eventLoop),
      m_channel(new Channel(eventLoop, sockfd)),
      m_clientIp(clientIp),
      m_sockfd(sockfd),
      m_state(NONE)
{
    m_channel->registeReadCallback(std::bind(&TcpConnection::handleRead, this));
    m_channel->registeErrorCallback(std::bind(&TcpConnection::handleError, this));
    m_channel->registeWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    m_channel->registeCloseCallback(std::bind(&TcpConnection::handleClose, this));
}

void TcpConnection::connectEstablished()
{
    m_eventLoop->assertInLoopThread();
    assert(m_state == NONE);
    m_state = CONNECTED;

    m_connectionCallbalk(shared_from_this());
    m_channel->enableRead();
}

void TcpConnection::handleRead()
{
    m_eventLoop->assertInLoopThread();
    assert(m_state == CONNECTED);

    int savedErrono = 0;
    
    ssize_t n = m_inputBuffer.readFd(m_sockfd, &savedErrono);
    if (n == 0)
    {
        handleClose();
    }
    else if (n < 0)
    {
        errno = savedErrono;
        handleError();
    }
    else
    {
        m_messageCallback(shared_from_this(), &m_inputBuffer, n);
    }
}

void TcpConnection::handleClose()
{
    m_eventLoop->assertInLoopThread();
    assert(m_state == CONNECTED || m_state == DISCONNECTING);

    LOG(INFO) << "close connection";

    m_channel->disableEvents();
    m_closeCallback(shared_from_this());
}

void TcpConnection::handleError()
{

    LOG(ERROR) << "connection errro!!!  errno: " << strerror(errno);
}

void TcpConnection::handleWrite()
{
    m_eventLoop->assertInLoopThread();
    assert(m_state == CONNECTED);
    //TODO: handleWrite

    int sendN = write(m_sockfd, m_outputBuffer.peek(), m_outputBuffer.readableBytes());
    if (m_channel->isWriting()) {
        if (sendN < 0) {
            LOG(ERROR) << "send data error";
            m_channel->enableRead();
        } else {
            //clear
            m_outputBuffer.retrieve(sendN);
            if (m_outputBuffer.readableBytes() == 0) {
                m_channel->enableRead();
                if (m_state == DISCONNECTING) {
                    shutdownInLoop();
                }
            } else {
                LOG(INFO) << "continue write more data";
            }
        }
    }
}

void TcpConnection::connectDestroyed()
{
    m_eventLoop->assertInLoopThread();
    assert(m_state == CONNECTED || m_state == DISCONNECTING);
    m_state = DISCONNECT;
    m_channel->disableEvents();
    m_eventLoop->removeChannel(m_channel.get());
}

void TcpConnection::sendData(const std::string & data)
{
    m_eventLoop->assertInLoopThread();
    assert(m_state == CONNECTED);

    if (m_eventLoop->isInLoopThread())
    {
        sendDataInLoop(data);
    }
    else
    {
        m_eventLoop->runInLoop(std::bind(&TcpConnection::sendDataInLoop, this, data));
    }
}

void TcpConnection::sendDataInLoop(const std::string& data)
{
    m_eventLoop->assertInLoopThread();
    size_t sentN = write(m_sockfd, data.data(), data.size());

    if (sentN < 0)
    {
        if (errno != EWOULDBLOCK)
        {
            LOG(ERROR) << "send data error!!";
        }
    }

    assert(sentN >= 0);

    if (sentN < data.size())
    {
        const char *newDataBegin = data.data() + sentN;
        m_outputBuffer.append(newDataBegin, data.size() - sentN);
        m_channel->eanbleWrite();
    }
}

void TcpConnection::shutdown()
{
    if (m_state == CONNECTED) {
        m_state = DISCONNECTING;
        m_eventLoop->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
    }

}

void TcpConnection::shutdownInLoop()
{
    m_eventLoop->assertInLoopThread();
    if (!m_channel->isWriting()) {
        //close write port
        ::shutdown(m_sockfd, SHUT_WR);
    }
}