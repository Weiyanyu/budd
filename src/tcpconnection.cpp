#include "tcpconnection.h"
#include "eventLoop.h"
#include "buffer.h"

TcpConnection::TcpConnection(EventLoop *eventLoop, int sockfd, const char *clientIp, std::string name)
    : m_eventLoop(eventLoop),
      m_channel(new Channel(eventLoop, sockfd)),
      m_clientIp(clientIp),
      m_sockfd(sockfd),
      m_state(NONE),
      m_name(name)
{
    m_channel->registeReadCallback(std::bind(&TcpConnection::handleRead, this));
    m_channel->registeErrorCallback(std::bind(&TcpConnection::handleError, this));
    m_channel->registeWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    m_channel->registeCloseCallback(std::bind(&TcpConnection::handleClose, this));
}

void TcpConnection::connectEstablished()
{
    m_eventLoop->assertInLoopThread();
    assert(m_state == State::NONE);
    m_state = State::CONNECTED;

    m_connectionCallbalk(shared_from_this());
    m_channel->enableRead();
}

void TcpConnection::handleRead()
{
    m_eventLoop->assertInLoopThread();

    int savedErrono = 0;
    
    ssize_t n = m_inputBuffer.readFd(m_sockfd, &savedErrono);
    LOG(INFO) << "handle read n : " << n;
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
    assert(m_state == State::CONNECTED || m_state == State::DISCONNECTING);

    LOG(INFO) << "close connection";

    m_channel->disableEvents();
    m_closeCallback(shared_from_this());
}

void TcpConnection::handleError()
{
    LOG(INFO) << "connection name = " << getName() << "and thread id = " << m_eventLoop->getCurrentThreadId();

    LOG(ERROR) << "connection errro!!!  errno: " << strerror(errno);
}

void TcpConnection::handleWrite()
{
    m_eventLoop->assertInLoopThread();
    assert(m_state == State::CONNECTED);
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
                LOG(INFO) << "handleWitre finish";
                if (m_state == State::DISCONNECTING) {
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
    assert(m_state == State::CONNECTED || m_state == State::DISCONNECTING);
    m_state = State::DISCONNECT;
    m_channel->disableEvents();
    m_eventLoop->removeChannel(m_channel.get());
    close(m_channel->fd());
}

void TcpConnection::sendData(const std::string & data)
{
    m_eventLoop->assertInLoopThread();
    if (m_state == State::CONNECTED) {
        if (m_eventLoop->isInLoopThread())
        {
            sendDataInLoop(data);
        }
        else
        {
            m_eventLoop->runInLoop(std::bind(&TcpConnection::sendDataInLoop, this, data));
        }
    }

}

void TcpConnection::sendData(Buffer* buffer) 
{
    m_eventLoop->assertInLoopThread();
    if (m_state == State::CONNECTED) {
        if (m_eventLoop->isInLoopThread())
        {
            sendDataInLoop(buffer->retrieveAllAsSrting());
        }
        else
        {
            m_eventLoop->runInLoop(std::bind(&TcpConnection::sendDataInLoop, this, buffer->retrieveAllAsSrting()));
        }
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
    size_t remain = data.size() - sentN;
    assert(remain <= data.size());
    
    if (remain > 0)
    {
        const char *newDataBegin = data.data() + sentN;
        m_outputBuffer.append(newDataBegin, remain);
        m_channel->enableWrite();
    }
}

void TcpConnection::shutdown()
{
    if (m_state == State::CONNECTED) {
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
    m_state = State::DISCONNECTING;

}

void TcpConnection::setTcpNoDelay(bool on)
{
    int option = on ? 1 : 0;
    ::setsockopt(m_sockfd, IPPROTO_TCP, TCP_NODELAY, &option, static_cast<socklen_t>(sizeof(option)));
}

void TcpConnection::setKeepAlive(bool on)
{
    int option = on ? 1 : 0;
    ::setsockopt(m_sockfd, SOL_SOCKET, SO_KEEPALIVE, &option, static_cast<socklen_t>(sizeof(option)));
}

