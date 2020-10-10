#ifndef HTTPSERVER
#define HTTPSERVER

#include <functional>
#include <memory>
#include "tcp/tcpserver.h"
#include "httpContext.h"
#include <unordered_map>

using namespace budd::tcp;

namespace budd 
{
namespace http 
{
class HttpRequest;
class HttpResponse;

class HttpServer
{
public:
    using HttpHandleFunc = std::function<void(const HttpRequest &, HttpResponse &)>;
    using HandleMap = std::unordered_map<std::string, HttpHandleFunc>;

    HttpServer(tcp::EventLoop *loop, int port);

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
    void onMessage(const std::shared_ptr<TcpConnection> &conn, base::Buffer *, int n);
    void onRequest(const std::shared_ptr<TcpConnection> &conn, const HttpRequest &request);
    void setRequestInfo(const std::shared_ptr<TcpConnection> &conn, const std::shared_ptr<HttpContext> &context);

    bool processRequest(const HttpRequest &req, HttpResponse &resp);
};
} //namespace budd
} //namespace http

#endif