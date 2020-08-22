#include "tcpserver.h"
#include "eventLoop.h"
#include "channel.h"
#include "acceptor.h"

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;


TcpServer::TcpServer(EventLoop* eventLoop, int port)
    :m_eventLoop(eventLoop),
    m_acceptor(new Acceptor(eventLoop, port)),
    m_port(port),
    m_started(false)
{

}

void TcpServer::start()
{
    m_eventLoop->assertInLoopThread();

    m_acceptor->listen();
    assert(m_acceptor->isListenning() == true);

    m_acceptor->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, _1, _2));

    m_started = true;
}

void TcpServer::newConnection(int connFd, const char* clientIp)
{
    std::shared_ptr<TcpConnection> conn = std::make_shared<TcpConnection>(
        m_eventLoop,
        connFd,
        clientIp
        );

    m_connectionMaps[connFd] = conn;
    conn->setConnnectionCallback(m_newConnectionCallback);
    conn->setMessageCallback(m_messageCallback);
    conn->setCloseCallback(std::bind(&TcpServer::removeConection, this, _1));
    conn->connectEstablished();
}

void TcpServer::removeConection(const std::shared_ptr<TcpConnection> &conn)
{
    m_eventLoop->assertInLoopThread();
    assert(m_connectionMaps.count(conn->sockFd()) > 0);


    m_connectionMaps.erase(conn->sockFd());


    m_eventLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));

}


