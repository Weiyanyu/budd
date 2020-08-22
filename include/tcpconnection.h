#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "channel.h"
#include <memory>
#include <functional>

class EventLoop;

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
    TcpConnection(EventLoop* eventLoop, int sockfd, const char *clientIp);

    typedef std::function<void(const std::shared_ptr<TcpConnection>&)> connectionCallback;
    typedef std::function<void(const std::shared_ptr<TcpConnection>&, char*, int)> messageCallback;
    typedef std::function<void(const std::shared_ptr<TcpConnection>&)> closeCallback;


    void setConnnectionCallback(connectionCallback cb) { m_connectionCallbalk = std::move(cb); }
    void setMessageCallback(messageCallback cb) { m_messageCallback = std::move(cb); }
    void setCloseCallback(closeCallback cb) { m_closeCallback = std::move(cb); }

    void connectEstablished();
    void connectDestroyed();

    int sockFd() { return m_sockfd; }
    const char* clientIp() { return m_clientIp; }
private:

    void handleRead();
    void handleClose();
    void handleError();
    void handleWrite();

    enum State {
        NONE, CONNECTING, CONNECTED, DISCONNECT,
    };

    EventLoop* m_eventLoop;
    std::unique_ptr<Channel> m_channel;
    const char *m_clientIp;
    int m_sockfd;
    State m_state;

    connectionCallback m_connectionCallbalk;
    messageCallback m_messageCallback;
    closeCallback m_closeCallback;

};

#endif