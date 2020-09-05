#include "httpServer.h"
#include "eventLoop.h"

int main()
{
    EventLoop loop;
    HttpServer server(&loop, 9001);
    server.start();
    loop.loop();
    return 0;
}