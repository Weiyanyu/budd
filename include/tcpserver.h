#ifndef TCPSERVER
#define TCPSERVER

#include <memory>
#include <unordered_map>
#include "eventLoopThreadPool.h"
#include <atomic>
#include "tcpconnection.h"
#include "acceptor.h"

class Buffer;
class EventLoop;
class Acceptor;
class TcpServer {
public:
    typedef std::function<void(const std::shared_ptr<TcpConnection>& conn)> connectionCallback;
    typedef std::function<void(const std::shared_ptr<TcpConnection>& conn, Buffer*, int n)> messageCallback;
    typedef std::function<void(const std::shared_ptr<TcpConnection>& conn)> onCloseCallback;

    static const int LOOP_POOL_SIZE = 8;

    TcpServer(EventLoop* eventLoop, int port);

    void start();

    void setConnectionCallback(connectionCallback cb) {
        m_newConnectionCallback = std::move(cb);
    }
    void setMessageCallback(messageCallback cb) {
        m_messageCallback = std::move(cb);
    }

    void removeConection(const std::shared_ptr<TcpConnection> &conn);
    void removeConnectionInLoop(const std::shared_ptr<TcpConnection> &conn);

private:
    void newConnection(int connFd, const char* port);

    EventLoop* m_eventLoop;
    std::unique_ptr<Acceptor> m_acceptor;
    int m_port;
    bool m_started;
    std::unordered_map<std::string, std::shared_ptr<TcpConnection>> m_connectionMaps;
    connectionCallback m_newConnectionCallback;
    messageCallback m_messageCallback;
    std::shared_ptr<EventLoopThreadPool> m_loopPool;
    std::atomic_ulong m_nextConnectionId;

};

#endif