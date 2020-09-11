#ifndef HTTPSERVER
#define HTTPSERVER

#include <functional>
#include <memory>
#include "tcpserver.h"
#include "httpContext.h"

class EventLoop;
class TcpServer;
class HttpRequest;
class HttpResponse;
class HttpServer
{
public:
    using HttpCallback = std::function<void(const HttpRequest&, HttpResponse&)>;
    
    HttpServer(EventLoop* loop, int port);

    void setHttpCallback(HttpCallback cb) { m_httpCallback = std::move(cb); }

    void start();
private:
    TcpServer m_tcpServer;  
    HttpCallback m_httpCallback;
    void onConnection(const std::shared_ptr<TcpConnection>& conn);
    void onMessage(const std::shared_ptr<TcpConnection>& conn, Buffer*, int n);
    void onRequest(const std::shared_ptr<TcpConnection>& conn, const HttpRequest& request);

    void setRequestInfo(const std::shared_ptr<TcpConnection>& conn, const std::shared_ptr<HttpContext>& context);
};

#endif