#include "tcpserver.h"
#include "eventLoop.h"
#include "channel.h"
#include "acceptor.h"
#include "buffer.h"

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;


TcpServer::TcpServer(EventLoop* eventLoop, int port)
    :m_eventLoop(eventLoop),
    m_acceptor(new Acceptor(eventLoop, port)),
    m_port(port),
    m_started(false),
    m_loopPool(new EventLoopThreadPool(eventLoop, LOOP_POOL_SIZE))
{
    m_acceptor->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, _1, _2));
}

void TcpServer::start()
{
    m_eventLoop->assertInLoopThread();

   
    assert(m_acceptor->isListenning() == false);

    m_loopPool->start();

    m_acceptor->listen();
    m_eventLoop->runInLoop(std::bind(&Acceptor::listen, m_acceptor.get()));
    m_started = true;
}

void TcpServer::newConnection(int connFd, const char* clientIp)
{
    m_eventLoop->assertInLoopThread();

    EventLoop* ioLoop = m_loopPool->getNextLoop();
    std::shared_ptr<TcpConnection> conn = std::make_shared<TcpConnection>(
        ioLoop,
        connFd,
        clientIp
        );

    m_connectionMaps[connFd] = conn;
    conn->setConnnectionCallback(m_newConnectionCallback);
    conn->setMessageCallback(m_messageCallback);
    conn->setCloseCallback(std::bind(&TcpServer::removeConection, this, _1));
    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConection(const std::shared_ptr<TcpConnection> &conn)
{
    m_eventLoop->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const std::shared_ptr<TcpConnection> &conn)
{
    m_eventLoop->assertInLoopThread();

    EventLoop* ioLoop = conn->getEventLoop();
    assert(m_connectionMaps.count(conn->sockFd()) > 0);

    m_connectionMaps.erase(conn->sockFd());

    
    ioLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));

}


