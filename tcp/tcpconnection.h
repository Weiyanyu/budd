#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <memory>
#include <functional>
#include <sys/socket.h>
#include <cstring>
#include <utility>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <sys/errno.h>

#include "channel.h"
#include "base/buffer.h"

class EventLoop;

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop *eventLoop, int sockfd, const char *clientIp, std::string name);

    typedef std::function<void(const std::shared_ptr<TcpConnection> &)> connectionCallback;
    typedef std::function<void(const std::shared_ptr<TcpConnection> &, Buffer *, int)> messageCallback;
    typedef std::function<void(const std::shared_ptr<TcpConnection> &)> closeCallback;

    void setConnnectionCallback(connectionCallback cb)
    {
        m_connectionCallbalk = std::move(cb);
    }
    void setMessageCallback(messageCallback cb)
    {
        m_messageCallback = std::move(cb);
    }
    void setCloseCallback(closeCallback cb)
    {
        m_closeCallback = std::move(cb);
    }

    void connectEstablished();
    void connectDestroyed();

    void sendData(const std::string &data);
    void sendData(Buffer *buffer);
    void shutdown();

    int sockFd()
    {
        return m_sockfd;
    }
    std::string clientIp()
    {
        return m_clientIp;
    }

    EventLoop *getEventLoop() { return m_eventLoop; }
    std::string getName() { return m_name; }

    void setTcpNoDelay(bool on);
    void setKeepAlive(bool on);

    void setContext(std::shared_ptr<void> context) { m_context = context; }
    std::shared_ptr<void> getContext() { return m_context; }

    bool isConnected() { return m_state == State::CONNECTED; }

private:
    void handleRead();
    void handleClose();
    void handleError();
    void handleWrite();

    void sendDataInLoop(const std::string &data);
    void shutdownInLoop();

    enum State
    {
        NONE,
        CONNECTING,
        CONNECTED,
        DISCONNECTING,
        DISCONNECT,
    };

    EventLoop *m_eventLoop;
    std::unique_ptr<Channel> m_channel;
    std::string m_clientIp;
    int m_sockfd;
    State m_state;
    std::string m_name;

    connectionCallback m_connectionCallbalk;
    messageCallback m_messageCallback;
    closeCallback m_closeCallback;

    Buffer m_inputBuffer;
    Buffer m_outputBuffer;
    std::shared_ptr<void> m_context;
};

#endif