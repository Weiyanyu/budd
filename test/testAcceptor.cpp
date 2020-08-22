#include "acceptor.h"
#include "eventLoop.h"
#include "channel.h"
#include "tcpserver.h"

#include <string>

using namespace std;

void newConnectionCallback(const std::shared_ptr<TcpConnection>& conn) 
{
    // send(conn->sockFd(), conn->clientIp(), sizeof(conn->clientIp()), 0);
    // close(conn->sockFd());
}

void messageCallback(const std::shared_ptr<TcpConnection>& conn, char* buf)
{
    LOG(INFO) << "read data: " << buf;
    // close(conn->sockFd());
}

int main() {

    EventLoop loop;
    TcpServer server(&loop, 8000);
    server.setConnectionCallback(newConnectionCallback);
    server.setMessageCallback(messageCallback);
    server.start();

    loop.loop();
}