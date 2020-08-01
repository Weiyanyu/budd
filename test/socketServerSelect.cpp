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

    fd_set readfds;
    fd_set writefds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_SET(listenFd, &readfds);

    fd_set tempReadfds;
    fd_set tempWritefds;
    int maxFd = listenFd;
    

    char buffers[1024][BUFFER_SIZE];
    while (true)
    {

        //reinit
        tempReadfds = readfds;
        tempWritefds = writefds;

        struct sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);

        
        int eventNum = select(maxFd+1, &tempReadfds, &tempWritefds, nullptr, nullptr);
        if (FD_ISSET(listenFd, &tempReadfds)) {
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

            FD_SET(connFd, &readfds);
            maxFd = connFd > maxFd ? connFd : maxFd;
            if (--eventNum == 0) {
                continue; //no need wait other 
            }
        }

        for (int fd = 0; fd <= maxFd; fd++) {
            if (fd == listenFd) continue;
            
            if (FD_ISSET(fd, &tempReadfds)) {
                std::memset(buffers[fd], 0, sizeof(buffers[fd]));
                int len = recv(fd, buffers[fd], sizeof(buffers[fd]), 0);
                //if len equal 0, mean client disconnect, we must add this condition, or else it will cause server crash
                if (len == 0)
                {
                    std::cout << "client force close connection!!!" << std::endl;
                    close(fd);
                    FD_CLR(fd, &readfds);
                    if (maxFd == fd) {
                        maxFd--;
                    }
                    continue;
                }
                FD_SET(fd, &writefds);
            }

            if (FD_ISSET(fd, &tempWritefds)) {
                send(fd, buffers[fd], sizeof(buffers[fd]), 0);
                FD_CLR(fd, &writefds);
            }
        }
    }
}