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
        setRequestInfo(conn, context);
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
    
    //2. process request
    if (!processRequest(request, response)) {
        conn->sendData("HTTP/1.1 404 NOT FOUND\r\n\r\n");
        conn->shutdown();
    }

    response.setStatusMessage(HttpUtil::getStatusMessageByCode(response.getStatusCode()));
    //3. send data to client
    Buffer buffer;
    response.fillBuffer(&buffer);
    //4. if http connection close, set close and than shutdown connection
    conn->sendData(&buffer);

    if (response.needClose()) {
        conn->shutdown();
    }
}

void HttpServer::setRequestInfo(const std::shared_ptr<TcpConnection>& conn, const std::shared_ptr<HttpContext>& context)
{
    HttpRequest request = context->getRequest();
    request.setRemoteAddress(conn->clientIp());
}

void HttpServer::handleFunc(std::string path, HttpHandleFunc func, HttpMethod method) 
{ 
    switch (method)
    {
    case HttpMethod::GET:
        m_handleGetFuncMaps[path] = func;
        break;
    case HttpMethod::HEAD:
        m_handleHeadFuncMaps[path] = func;
        break;
    case HttpMethod::POST:
        m_handlePostFuncMaps[path] = func;
        break;
    case HttpMethod::PUT:
        m_handlePutFuncMaps[path] = func;
        break;
    case HttpMethod::DELETE:
        m_handleDeleteFuncMaps[path] = func;
        break;
    default:
        break;
    }
    m_handleGetFuncMaps[path] = std::move(func); 
}


bool HttpServer::processRequest(const HttpRequest& request, HttpResponse& response)
{
    std::string path = request.path();
    HttpMethod method = request.method();
    HttpHandleFunc handleFunc;
    switch (method)
    {
    case HttpMethod::GET:
        handleFunc = m_handleGetFuncMaps[path];
        break;
    case HttpMethod::HEAD:
        handleFunc = m_handleHeadFuncMaps[path];
        break;
    case HttpMethod::POST:
        handleFunc = m_handlePostFuncMaps[path];
        break;
    case HttpMethod::PUT:
        handleFunc = m_handlePutFuncMaps[path];
        break;
    case HttpMethod::DELETE:
        handleFunc = m_handleDeleteFuncMaps[path];
        break;
    default:
        break;
    }
    if (handleFunc == nullptr) {
        DLOG(ERROR) << "can not find handle func for path : " << path;
        return false;
    }
    handleFunc(request, response);

    return true;
}

