#include <functional>
#include "HttpServer.h"
#include "../base/Logging.h"
#include "../net/MySQLsOps.h"
#include "HttpContext.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
using namespace ssxrver;
using namespace ssxrver::net;

namespace ssxrver
{
namespace net
{
namespace detail
{

void defaultHttpCallback(const HttpRequest &, HttpResponse *resp, const MySQLsOps *)
{
    resp->setStatusCode(HttpResponse::k404NotFound);
    resp->setStatusMessage("Not Found");
}

} // namespace detail
} // namespace net
} // namespace ssxrver

HttpServer::HttpServer(EventLoop *loop,
                       const struct sockaddr_in listenAddr)
    : server_(loop, listenAddr),
      httpCallback_(detail::defaultHttpCallback)
{
    server_.setMessageCallback(bind(&HttpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2));
    server_.setConnectionCallback(bind(&HttpServer::onConnection, this, std::placeholders::_1));
    /* server_.setThreadInitCallback(bind(&HttpServer::onThreadInit, this, std::placeholders::_1)); */
}

void HttpServer::start()
{
    LOG_WARN << "HttpServer on";
    server_.start();
}

void HttpServer::onConnection(const TcpConnectionPtr &conn)
{
    if (conn->connected())
        conn->setContext(HttpContext());
}

void HttpServer::onMessage(const TcpConnectionPtr &conn,
                           Buffer *buf)
{
    HttpContext *context = boost::any_cast<HttpContext>(conn->getMutableContext()); //获取的是可以改变的
    if (!context->parseRequest(buf))                                                //获取请求包，更好的做法是让parserequest作为httpcontext的成员函数
    {
        conn->send("REQNUM -1 \r\nConntent-Length: 21\r\n\r\n{\"400\":\"Bad Request\"}"); //客户端请求失败
        conn->shutdown();
    }
    //请求消息解析完毕
    if (context->gotAll())
    {
        onRequest(conn, context->request()); //连接和请求对象传过来
        context->reset();                    //本次请求处理完毕，重置httpcontext，适用于长连接
    }
}

void HttpServer::onRequest(const TcpConnectionPtr &conn, const HttpRequest &req)
{
    MySQLsOps *mysql = conn->getLoop()->getMySQL();
    HttpResponse response;
    httpCallback_(req, &response, mysql);
    Buffer buf;
    response.appendToBuffer(&buf); //将对象转化为一个字符串到buf中
    conn->send(&buf);              //将缓冲区发送到客户端
}
