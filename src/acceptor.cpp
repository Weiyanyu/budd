#include "acceptor.h"
#include "eventLoop.h"

Acceptor::Acceptor(EventLoop* eventLoop, int port)
    :m_port(port),
    m_listenning(false),
    m_listenChannel(eventLoop, socket(AF_INET, SOCK_STREAM, 0)),
    m_listenFd(m_listenChannel.fd())
{
    LOG_ASSERT(m_listenFd >= 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = m_port;
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(m_listenFd, (const struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        LOG(FATAL) << "bind server error!!!";
    }
}

void Acceptor::listen()
{
    if (::listen(m_listenFd, 5) == -1)
    {
        LOG(FATAL) << "listen error!!!";
    }
    m_listenChannel.enableRead();
    m_listenChannel.registeReadCallback(std::bind(&Acceptor::accept, this));
    m_listenning = true;
}

void Acceptor::accept()
{
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    int connFd = ::accept(m_listenFd, (struct sockaddr *)&clientAddr, &addrLen);
    if (connFd == -1)
    {
        LOG(FATAL) << "accept error!!!";
    }

    char clientIP[INET_ADDRSTRLEN];
    std::memset(clientIP, 0, sizeof(clientIP));

    inet_ntop(AF_INET, &clientAddr, clientIP, INET_ADDRSTRLEN);

    if (connFd >= 0 && m_newConnectionCallback != nullptr) {
        m_newConnectionCallback(connFd, clientIP);
    }
    else {
        //close fd
        ::close(connFd);
    }
}