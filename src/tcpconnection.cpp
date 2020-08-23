#include "tcpconnection.h"
#include "eventLoop.h"

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

    std::memset(m_buffer, 0, sizeof(m_buffer));
    ssize_t n = recv(m_channel->fd(), m_buffer, sizeof(m_buffer), 0);
    if (n == 0)
    {
        handleClose();
    }
    else if (n < 0)
    {
        handleError();
    }
    else
    {
        m_messageCallback(shared_from_this(), m_buffer, n);
    }
}

void TcpConnection::handleClose()
{
    m_eventLoop->assertInLoopThread();
    assert(m_state == CONNECTED);

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

    send(m_channel->fd(), m_buffer, sizeof(m_buffer), 0);

    //need enable read, or else crash 100%
    m_channel->enableRead();
}

void TcpConnection::connectDestroyed()
{
    m_eventLoop->assertInLoopThread();
    assert(m_state == CONNECTED);
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
    size_t sentN = send(m_sockfd, data.data(), sizeof(data), 0);

    if (sentN < 0)
    {
        if (errno != EWOULDBLOCK)
        {
            LOG(ERROR) << "send data error!!";
        }
    }

    assert(sentN >= 0);

    if (sentN < sizeof(data))
    {
        const char *newDataBegin = data.data() + sentN;
        std::memset(m_buffer, 0, sizeof(m_buffer));
        std::memcpy(m_buffer, newDataBegin, sizeof(data) - sentN);
        m_channel->eanbleWrite();
    }
}