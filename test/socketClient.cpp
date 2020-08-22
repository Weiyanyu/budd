#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <iostream>
#include <cstring>

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "usge: please input data for echo" << std::endl;
        exit(0);
    }

    int socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1) {
        std::cerr << "Error socket!!!" << std::endl;
        exit(0);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = 8000;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(socketFd, (const struct sockaddr*)&addr, sizeof(addr)) == -1) {
        std::cerr << "connect server error!!!" << std::endl;
        exit(0);
    }

    char data[64 * 1024];
    char buf[64 * 1024];

    memset(buf, 0, sizeof(buf));
    memset(data, 0, sizeof(data));
    memcpy(data, argv[1], sizeof(argv[1]));
    send(socketFd, data, sizeof(data), 0);
    // int len = recv(socketFd, buf, sizeof(buf), 0);
    // if (std::strcmp("bye!", buf) == 0) {
    //     std::cout << buf << std::endl;
    // } else {
    //     std::cout << buf << std::endl;
    // }

    close(socketFd);

}