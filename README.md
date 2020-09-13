# Budd
## Introduction
A Http and Tcp library in Linux.

You can easily write Http logic code

Most of the Tcp server refers to _muduo_.

## How to compile
In Linux(my OS version is Ubuntu20.04, Kernel version is 5.4.0-47-generic):

```shell
mkdir build && cd build
cmake ..
make
```

You can change CMakeLists.txt, whatever you want.

## How to use
A test Example(in test/testHttpServer.cpp)
```c++
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
```

very easy to use, right?

## more feature
I need think about it......



