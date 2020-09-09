#include "httpServer.h"
#include "httpContext.h"
#include "tcpconnection.h"
#include "httpRequest.h"
#include "httpResponse.h"
#include "acceptor.h"

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;


HttpServer::HttpServer(EventLoop* loop, int port)
    :m_tcpServer(loop, port)
{
    m_tcpServer.setConnectionCallback(std::bind(&HttpServer::onConnection, this, _1));
    m_tcpServer.setMessageCallback(std::bind(&HttpServer::onMessage, this, _1, _2, _3));
}

void HttpServer::start()
{
    m_tcpServer.start();
}

void HttpServer::onConnection(const std::shared_ptr<TcpConnection>& conn)
{

    if (conn->isConnected()) {
        conn->setContext(std::make_shared<HttpContext>());
    }
}
void HttpServer::onMessage(const std::shared_ptr<TcpConnection>& conn, Buffer* buffer, int n)
{
    std::shared_ptr<HttpContext> context = std::static_pointer_cast<HttpContext>(conn->getContext());
    bool isSuccess = context->praseRequest(buffer);
    if (!isSuccess) {
        conn->sendData("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
        return;
    }
    if (context->isParseFinished()) {
        onRequest(conn, context->getRequest());
        context->clear();
    }

}
void HttpServer::onRequest(const std::shared_ptr<TcpConnection>& conn, const HttpRequest& request)
{
    //1. setResponseInfo
    HttpResponse response;
    std::string connectionValue = request.getHeader("Connection");
    bool close = connectionValue == "close" ||
    (request.httpVersion() == "HTTP/1.1" && connectionValue != "keep-alive");
    response.setClose(close);

    //set default info, user can change it in httpCallback
    //set keyyp alive limit default
    response.setKeepAliveLimit(5, 120);
    
    //2. call callback function
    m_httpCallback(request, response);
    //3. send data to client
    Buffer buffer;
    response.fillBuffer(&buffer);
    //4. if http connection close, set close and than shutdown connection
    conn->sendData(&buffer);

    if (response.needClose()) {
        conn->shutdown();
    }
}