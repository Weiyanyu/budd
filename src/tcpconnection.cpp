#include "tcpconnection.h"
#include "eventLoop.h"

TcpConnection::TcpConnection(EventLoop* eventLoop, int sockfd, const char *clientIp)
    :m_eventLoop(eventLoop),
    m_channel(new Channel(eventLoop, sockfd)),
    m_clientIp(clientIp),
    m_sockfd(sockfd),
    m_state(NONE)
{

}

void TcpConnection::connectEstablished()
{
    m_eventLoop->assertInLoopThread();
    assert(m_state == NONE);
    m_state = CONNECTED;

    m_channel->enableRead();
    m_connectionCallbalk(shared_from_this());
    
}