#ifndef TCPSERVER
#define TCPSERVER

#include <memory>
#include "tcpconnection.h"
#include <unordered_map>

class Buffer;
class EventLoop;
class Acceptor;
class TcpServer {
public:
    typedef std::function<void(const std::shared_ptr<TcpConnection>& conn)> connectionCallback;
    typedef std::function<void(const std::shared_ptr<TcpConnection>& conn, Buffer*, int n)> messageCallback;
    typedef std::function<void(const std::shared_ptr<TcpConnection>& conn)> onCloseCallback;

    TcpServer(EventLoop* eventLoop, int port);

    void start();

    void setConnectionCallback(connectionCallback cb) {
        m_newConnectionCallback = std::move(cb);
    }
    void setMessageCallback(messageCallback cb) {
        m_messageCallback = std::move(cb);
    }

    void removeConection(const std::shared_ptr<TcpConnection> &conn);


private:
    EventLoop* m_eventLoop;
    std::unique_ptr<Acceptor> m_acceptor;
    int m_port;
    bool m_started;
    std::unordered_map<int, std::shared_ptr<TcpConnection>> m_connectionMaps;
    connectionCallback m_newConnectionCallback;
    messageCallback m_messageCallback;

    void newConnection(int connFd, const char* port);
};

#endif