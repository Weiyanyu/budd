#ifndef HTTPSERVER
#define HTTPSERVER

#include <functional>
#include <memory>
#include "tcp/tcpserver.h"
#include "httpContext.h"
#include <unordered_map>

class EventLoop;
class TcpServer;
class HttpRequest;
class HttpResponse;
class HttpServer
{
public:
    using HttpHandleFunc = std::function<void(const HttpRequest &, HttpResponse &)>;
    using HandleMap = std::unordered_map<std::string, HttpHandleFunc>;

    HttpServer(EventLoop *loop, int port);

    void handleFunc(std::string path, HttpHandleFunc func, HttpMethod method);

    void start();

private:
    TcpServer m_tcpServer;
    HandleMap m_handleGetFuncMaps;
    HandleMap m_handlePostFuncMaps;
    HandleMap m_handlePutFuncMaps;
    HandleMap m_handleDeleteFuncMaps;
    HandleMap m_handleHeadFuncMaps;

    void onConnection(const std::shared_ptr<TcpConnection> &conn);
    void onMessage(const std::shared_ptr<TcpConnection> &conn, Buffer *, int n);
    void onRequest(const std::shared_ptr<TcpConnection> &conn, const HttpRequest &request);
    void setRequestInfo(const std::shared_ptr<TcpConnection> &conn, const std::shared_ptr<HttpContext> &context);

    bool processRequest(const HttpRequest &req, HttpResponse &resp);
};

#endif