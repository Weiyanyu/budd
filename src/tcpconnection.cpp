#include "tcpconnection.h"
#include "eventLoop.h"
#include <sys/socket.h>
#include <cstring>
#include <sys/errno.h>

TcpConnection::TcpConnection(EventLoop* eventLoop, int sockfd, const char *clientIp)
    :m_eventLoop(eventLoop),
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
    if (n == 0) {
        handleClose();
    }
    else if (n < 0) {
        handleError();
    }
    else {
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

    close(m_sockfd);
}

void TcpConnection::sendData(const char* data)
{
    m_eventLoop->assertInLoopThread();
    assert(m_state == CONNECTED);

    std::memset(m_buffer, 0, sizeof(m_buffer));
    std::memcpy(m_buffer, data, std::strlen(data));

    m_channel->eanbleWrite();
}