#include "httpServer.h"
#include "eventLoop.h"
#include "httpRequest.h"
#include "httpResponse.h"


void handleHome(const HttpRequest& req, HttpResponse& resp) {
    LOG(INFO) << "handleHome";
    resp.setBody("Hello, World!!");
    resp.setContentType(ContentType::TEXT_PLAIN);
    resp.setStatusCode(HttpResponseStatusCode::OK);
}

void handleImage(const HttpRequest& req, HttpResponse& resp) {
    LOG(INFO) << "handleImage";
    if (!resp.setFile("/home/weiyanyu/learn/cpp/budd/test/static/cool.jpeg")) {
        resp.setStatusCode(HttpResponseStatusCode::NOT_FOUND);
    } else {
        resp.setContentType(ContentType::IMAGE_JPEG);
        resp.setStatusCode(HttpResponseStatusCode::OK);
    }
}



int main()
{
    EventLoop loop;
    HttpServer server(&loop, 9001);
    server.handleFunc("/", handleHome, HttpMethod::GET);
    server.handleFunc("/static/cool.jpeg", handleImage, HttpMethod::GET);
    server.start();
    loop.loop();
    return 0;
}