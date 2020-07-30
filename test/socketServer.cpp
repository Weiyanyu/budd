#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <iostream>
#include <cstring>
#include <string>


int main() {

    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd == -1) {
        std::cerr << "Error socket!!!" << std::endl;
        exit(0);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = 8080;
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listenFd, (const struct sockaddr*)&addr, sizeof(addr)) == -1) {
        std::cerr << "bind server error!!!" << std::endl;
        exit(0);
    }

    if (listen(listenFd, 5) == -1) {
        std::cerr << "listen error!!!" << std::endl;
        exit(0);
    }

    while (true) {
        struct sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);
        int connFd = accept(listenFd, (struct sockaddr*)&clientAddr, &addrLen);
        if (connFd == -1) {
            std::cerr << "accept error!!!" << std::endl;
            exit(0);
        }

        char clientIP[INET_ADDRSTRLEN];
        std::memset(clientIP, 0, sizeof(clientIP));

        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
        std::cout << "accept client ip : " << clientIP << std::endl;

        char buf[4 * 1024];
        while (true) {
            std::memset(buf, 0, sizeof(buf));
            int len = recv(connFd, buf, sizeof(buf), 0);
            if (len == 0) {
                std::cout << "client force close connection!!!" << std::endl;
                break;
            }
            if (std::strcmp(buf, "exit") == 0) {
                std::string bye = "bye!";
                send(connFd, bye.c_str(), sizeof(bye.c_str()), 0);
                break;
            }
            send(connFd, buf, sizeof(buf), 0);
        }

        close(connFd);
    }
    
}