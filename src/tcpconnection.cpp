#include "tcpconnection.h"
#include "eventLoop.h"

TcpConnection::TcpConnection(EventLoop* eventLoop, int sockfd, const char *clientIp)
    :m_eventLoop(eventLoop),
    m_channel(new Channel(eventLoop, sockfd)),
    m_clientIp(clientIp),
    m_sockfd(sockfd),
    m_state(NONE)
{
    m_channel->registeReadCallback(std::bind(&TcpConnection::handleRead, this));
    m_channel->registeErrorCallback(std::bind(&TcpConnection::handleClose, this));
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

    char buf[1024 * 64];
    ssize_t n = read(m_channel->fd(), buf, sizeof(buf));
    if (n == 0) {
        handleClose();
    } else if (n < 0) {
        handleError();
    } else {
        m_messageCallback(shared_from_this(), buf);
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
    LOG(ERROR) << "connection errro!!!";
}

void TcpConnection::handleWrite()
{

}

void TcpConnection::connectDestroyed()
{
    m_eventLoop->assertInLoopThread();
    assert(m_state == CONNECTED);
    m_state = DISCONNECT;
    m_channel->disableEvents();
    m_eventLoop->removeChannel(m_channel.get());

    close(m_sockfd);
}