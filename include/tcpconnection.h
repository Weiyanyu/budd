#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "channel.h"
#include <memory>
#include <functional>

class EventLoop;

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
    TcpConnection(EventLoop* eventLoop, int sockfd, const char *clientIp);

    typedef std::function<void(const std::shared_ptr<TcpConnection>& conn)> connectionCallback;

    void setConnnectionCallback(connectionCallback cb) { m_connectionCallbalk = std::move(cb); }
    void connectEstablished();

    int sockFd() { return m_sockfd; }
    const char* clientIp() { return m_clientIp; }
private:
    enum State {
        NONE, CONNECTING, CONNECTED, DISCONNECT,
    };

    EventLoop* m_eventLoop;
    std::unique_ptr<Channel> m_channel;
    const char *m_clientIp;
    int m_sockfd;
    State m_state;

    connectionCallback m_connectionCallbalk;
};

#endif