#include "http/httpServer.h"
#include "tcp/eventLoop.h"
#include "http/httpRequest.h"
#include "http/httpResponse.h"

// using namespace budd::tcp;
using namespace budd::http;


void handleHome(const HttpRequest& req, HttpResponse& resp) {
    LOG(INFO) << "handleHome";
    resp.setBody("Hello, World!!");
    resp.setContentType(ContentType::TEXT_PLAIN);
    resp.setStatusCode(HttpResponseStatusCode::OK);
}

void handleImage(const HttpRequest& req, HttpResponse& resp) {
    LOG(INFO) << "handleImage";
    if (!resp.setFile("./static/cool.jpeg")) {
        resp.setStatusCode(HttpResponseStatusCode::NOT_FOUND);
    } else {
        resp.setContentType(ContentType::IMAGE_JPEG);
        resp.setStatusCode(HttpResponseStatusCode::OK);
    }
}

void handlePost(const HttpRequest& req, HttpResponse& resp)
{
    std::string name = req.getFromBody("name");
    resp.setBody("Hello, " + name);
    resp.setContentType(ContentType::TEXT_PLAIN);
    resp.setStatusCode(HttpResponseStatusCode::OK);
}



int main()
{
    EventLoop loop;
    HttpServer server(&loop, 9001);
    server.handleFunc("/", handleHome, HttpMethod::GET);
    server.handleFunc("/static/cool.jpeg", handleImage, HttpMethod::GET);
    server.handleFunc("/hello", handlePost, HttpMethod::POST);
    server.start();
    loop.loop();
    return 0;
}