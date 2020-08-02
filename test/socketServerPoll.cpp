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

    struct pollfd pollfds[1024];
    pollfds[0].fd = listenFd;
    pollfds[0].events |= POLLIN;

    int maxNum = 1;

    for (int i = 1; i < 1024; i++) {
        pollfds[i].fd = -1;
    }


    char buffers[1024][BUFFER_SIZE];
    while (true)
    {

        struct sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);

        int eventNum = poll(pollfds, maxNum, -1);
        std::cout << "event num " << eventNum << std::endl;

        for (int i = 0; i < maxNum; i++) {
            if (pollfds[i].revents & POLLIN) {
                if (pollfds[i].fd == listenFd) {
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

                    pollfds[maxNum].fd = connFd;
                    pollfds[maxNum].events |= POLLIN;
                    maxNum++;

                    if (--eventNum == 0) {
                        continue; //no need wait other 
                    }
                } else {
                    int fd = pollfds[i].fd;
                    std::memset(buffers[fd], 0, sizeof(buffers[fd]));
                    int len = recv(fd, buffers[fd], sizeof(buffers[fd]), 0);
                    //if len equal 0, mean client disconnect, we must add this condition, or else it will cause server crash
                    if (len == 0)
                    {
                        std::cout << "client force close connection!!!" << std::endl;
                        pollfds[fd].fd = -1;
                        pollfds[fd].events |= 0;
                        close(fd);
                        maxNum--;
                        continue;
                    }
                    pollfds[i].events = POLLOUT;
                }
            } else if (pollfds[i].revents & POLLOUT) {
                int fd = pollfds[i].fd;
                send(fd, buffers[fd], sizeof(buffers[fd]), 0);
                pollfds[i].events = POLLIN;
            }

        }
    }
    return 0;
}