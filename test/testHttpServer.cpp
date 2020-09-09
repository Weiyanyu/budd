#include "httpServer.h"
#include "eventLoop.h"
#include "httpRequest.h"
#include "httpResponse.h"

void httpCallback(const HttpRequest& req, HttpResponse& resp)
{
    LOG(INFO) << "req path : " << req.path();
    if (req.path() == "/static/cool.jpeg") {
        if (!resp.setFile("/home/weiyanyu/learn/cpp/budd/test/static/cool.jpeg")) {
            resp.setStatusCode(HttpResponseStatusCode::NOT_FOUND);
            resp.setStatusMessage(HttpResponse::getStatusMessageByCode(HttpResponseStatusCode::NOT_FOUND));
        } else {
            resp.setContentType(ContentType::IMAGE_JPEG);
            resp.setStatusCode(HttpResponseStatusCode::OK);
            resp.setStatusMessage(HttpResponse::getStatusMessageByCode(HttpResponseStatusCode::OK));
        }

    } else {
        resp.setBody("Hello, World!!");
        resp.setContentType(ContentType::TEXT_PLAIN);
        resp.setStatusCode(HttpResponseStatusCode::OK);
        resp.setStatusMessage(HttpResponse::getStatusMessageByCode(HttpResponseStatusCode::OK));
    }


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