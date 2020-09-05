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
    LOG(INFO) << "onRequest";
    //TODO:
    //1. setResponseInfo
    //2. call callback function
    //3. send data to client
    //4. if http connection close, set close and than shutdown connection
    conn->sendData("HTTP/1.1 200 OK\r\n\r\n");
    conn->shutdown();
}