#ifndef TCPSERVER
#define TCPSERVER

#include <memory>
#include "tcpconnection.h"
#include <unordered_map>


class EventLoop;
class Acceptor;
class TcpServer {
public:
    typedef std::function<void(const std::shared_ptr<TcpConnection>& conn)> connectionCallback;

    TcpServer(EventLoop* eventLoop, int port);

    void start();

    void setConnectionCallback(connectionCallback cb) { m_newConnectionCallback = std::move(cb); } 

private:
    EventLoop* m_eventLoop;
    std::unique_ptr<Acceptor> m_acceptor;
    int m_port;
    bool m_started;
    std::unordered_map<int, std::shared_ptr<TcpConnection>> connectionMaps;
    connectionCallback m_newConnectionCallback;

    void newConnection(int connFd, const char* port);
};

#endif