#include "httpServer.h"
#include "eventLoop.h"
#include "httpRequest.h"
#include "httpResponse.h"

void httpCallback(const HttpRequest& req, HttpResponse& resp)
{
    resp.setStatusCode(HttpResponseStatusCode::OK);
    resp.setStatusMessage("OK");
    resp.setBody("Hello, World!!");
    resp.setHeader("Content-Type", "text/plain;charset=utf-8");
}

int main()
{
    EventLoop loop;
    HttpServer server(&loop, 9001);
    server.setHttpCallback(httpCallback);
    server.start();
    loop.loop();
    return 0;
}