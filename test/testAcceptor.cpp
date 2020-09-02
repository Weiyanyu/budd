#include "acceptor.h"
#include "eventLoop.h"
#include "channel.h"
#include "tcpserver.h"
#include <sstream>
#include "buffer.h"
#include "timer.h"

#include <string>

using namespace std;

void newConnectionCallback(const std::shared_ptr<TcpConnection>& conn) 
{
    // send(conn->sockFd(), conn->clientIp(), sizeof(conn->clientIp()), 0);
    // close(conn->sockFd());
}

void messageCallback(const std::shared_ptr<TcpConnection>& conn, Buffer* buf, int n)
{
    LOG(INFO) << "read client ip : " << conn->clientIp() << n << " data : " << buf->retrieveAllAsSrting();
    LOG(INFO) << " and connect name = " << conn->getName() << " address: " << conn.get();
    
    conn->sendData(conn->clientIp());
}

void timerCallbackRunAt()
{
    LOG(INFO) << "timer run at";
}

void timerCallbackRunAfter()
{
    LOG(INFO) << "timer run after";
}

void timerCallbackRunEvey()
{
    LOG(INFO) << "timer run ever";
}

int main() {

    EventLoop loop;
    TcpServer server(&loop, 8000);
    server.setConnectionCallback(newConnectionCallback);
    server.setMessageCallback(messageCallback);
    server.start();

    loop.runAt(TimerQueue::now() + TimerQueue::MICROSECOND_PERSECOND * 2, timerCallbackRunAt);
    loop.runAfter(TimerQueue::MICROSECOND_PERSECOND * 3, timerCallbackRunAfter);
    loop.runEvey(TimerQueue::MICROSECOND_PERSECOND * 1, timerCallbackRunEvey);


    loop.loop();
}