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
}

void TcpConnection::connectEstablished()
{
    m_eventLoop->assertInLoopThread();
    assert(m_state == NONE);
    m_state = CONNECTED;

    m_connectionCallbalk(shared_from_this());
    m_channel->enableRead();
}

void TcpConnection::handleRead() {
    m_eventLoop->assertInLoopThread();
    assert(m_state == CONNECTED);

    char buf[1024 * 64];
    ssize_t n = read(m_channel->fd(), buf, sizeof(buf));
    if (n == 0) {
        //TODO: close socket
    }
    m_messageCallback(shared_from_this(), buf);
    m_channel->disableEvents();
    m_channel->remove();
}