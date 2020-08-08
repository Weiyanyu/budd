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

const int BUFFER_SIZE = 4 * 1024;

void processConn(int connFd, struct in_addr clientAddr);

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
    addr.sin_port = 8080;
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

    int epollfd = epoll_create(1024);

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = listenFd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenFd, &event);

    struct epoll_event revents[1024];
    char buffers[1024][BUFFER_SIZE];
    while (true)
    {

        struct sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);

        int eventNum = epoll_wait(epollfd, revents, 1024, -1);
        std::cout << "event num " << eventNum << std::endl;

        for (int i = 0; i < eventNum; i++)
        {
            if (revents[i].events & EPOLLIN)
            {
                if (revents[i].data.fd == listenFd)
                {
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

                    struct epoll_event newEvent;
                    newEvent.events = EPOLLIN;
                    newEvent.data.fd = connFd;
                    epoll_ctl(epollfd, EPOLL_CTL_ADD, connFd, &newEvent);
                }
                else
                {
                    int fd = revents[i].data.fd;
                    std::memset(buffers[fd], 0, sizeof(buffers[fd]));
                    int len = recv(fd, buffers[fd], sizeof(buffers[fd]), 0);
                    //if len equal 0, mean client disconnect, we must add this condition, or else it will cause server crash
                    if (len == 0)
                    {
                        std::cout << "client force close connection!!!" << std::endl;
                        epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &revents[i]);
                        close(fd);
                        continue;
                    }

                    revents[i].events = EPOLLOUT;
                    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &revents[i]);
                }
            }
            else if (revents[i].events & EPOLLOUT)
            {
                int fd = revents[i].data.fd;
                send(fd, buffers[fd], sizeof(buffers[fd]), 0);
                struct epoll_event newEvent;
                newEvent.events = EPOLLIN;
                newEvent.data.fd = fd;
                // epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &revents[i]);
                epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &newEvent);
            }
        }
    }
    close(listenFd);
    close(epollfd);
    return 0;
}
