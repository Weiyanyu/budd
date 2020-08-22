#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "channel.h"

#include <functional>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>

class EventLoop;

class Acceptor {
public:
    typedef std::function<void(int fd, const char* ip)> newConnectionCallback;

    Acceptor(EventLoop* eventLoop, int port);

    void listen();
    void accept();

    void setNewConnectionCallback(newConnectionCallback callback) {
        m_newConnectionCallback = std::move(callback);
    }

    bool isListenning() {
        return m_listenning;
    }

private:
    int m_port;
    bool m_listenning;

    //note order
    Channel m_listenChannel;
    int m_listenFd;

    newConnectionCallback m_newConnectionCallback;
};

#endif