#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <iostream>
#include <cstring>
#include <string>

#include <thread>
#include <unordered_map>
#include <sys/time.h>

#include <sys/epoll.h>
#include <poll.h>

#include "eventLoop.h"
#include "channel.h"
#include "selector.h"



const int BUFFER_SIZE = 4 * 1024;

void processRead(std::shared_ptr<Channel> clientChannel, char buffers[1024][BUFFER_SIZE]);
void prcessWrite(std::shared_ptr<Channel> clientChannel, char buffers[1024][BUFFER_SIZE]);


void processConn(int listenFd, EventLoop* loop, char buffers[1024][BUFFER_SIZE]) {
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    int connFd = accept(listenFd, (struct sockaddr *)&clientAddr, &addrLen);
    if (connFd == -1)
    {
        std::cerr << "accept error!!!" << std::endl;
        exit(0);
    }
    char clientIP[INET_ADDRSTRLEN];
    std::memset(clientIP, 0, sizeof(clientIP));

    inet_ntop(AF_INET, &clientAddr, clientIP, INET_ADDRSTRLEN);
    std::cout << "accept client ip : " << clientIP << std::endl;

    auto clientChannel = std::make_shared<Channel>(loop, connFd);

    clientChannel->enableRead();

    clientChannel->registeReadCallback(std::bind(processRead, clientChannel, buffers));

    clientChannel->registeWriteCallback(std::bind(prcessWrite, clientChannel, buffers));

}

void processRead(std::shared_ptr<Channel> clientChannel, char buffers[1024][BUFFER_SIZE]) {
    int fd = clientChannel->fd();
    std::memset(buffers[fd], 0, sizeof(buffers[fd]));
    int len = recv(fd, buffers[fd], sizeof(buffers[fd]), 0);
    //if len equal 0, mean client disconnect, we must add this condition, or else it will cause server crash
    if (len == 0)
    {
        clientChannel->disableEvents();
        clientChannel->eventLoop()->removeChannel(clientChannel.get());
        //need user close fd self.
        close(fd);
        std::cout << "client force close connection!!!" << std::endl;
    } else {
        clientChannel->enableWrite();
    }
}

void prcessWrite(std::shared_ptr<Channel> clientChannel, char buffers[1024][BUFFER_SIZE]) {
    int fd = clientChannel->fd();

    send(fd, buffers[fd], sizeof(buffers[fd]), 0);
    clientChannel->enableRead();
}

int main()
{
    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd == -1)
    {
        std::cerr << "Error socket!!!" << std::endl;
        exit(0);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = 8000;
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listenFd, (const struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        std::cerr << "bind server error!!!" << std::endl;
        exit(0);
    }

    if (listen(listenFd, 5) == -1)
    {
        std::cerr << "listen error!!!" << std::endl;
        exit(0);
    }

    char buffers[1024][BUFFER_SIZE];
    EventLoop loop;
    Channel serverChannel(&loop, listenFd);
    serverChannel.enableRead();
    serverChannel.registeReadCallback(std::bind(&processConn, listenFd, &loop, buffers));
    loop.loop();
    serverChannel.disableEvents();
    return 0;
}
