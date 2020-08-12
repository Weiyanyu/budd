#include "acceptor.h"
#include "eventLoop.h"
#include "channel.h"

#include <string>

using namespace std;

void newConnectionCallback(int connfd, const char* clientIp) 
{
    std::string str = "Hello, man!";
    send(connfd, str.c_str(), str.size(), 0);
    close(connfd);
}

int main() {

    EventLoop loop;
    Acceptor acceptor(&loop, 8000);
    acceptor.setNewConnectionCallback(newConnectionCallback);

    acceptor.listen();

    loop.loop();
}