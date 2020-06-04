#include <iostream>
#include <string>
/* #include "http/echoServertest.h" */
#include "http/HttpServer.h"
#include "http/HttpRequest.h"
#include "http/HttpResponse.h"
#include "net/EventLoop.h"
#include "net/MySQLsOps.h"
#include "net/CJsonObject.hpp"
#include "base/Logging.h"
#include "base/AsyncLogThread.h"
using namespace ssxrver;
using namespace ssxrver::net;
bool flag = false;

void message(const HttpRequest &req, HttpResponse *resp, MySQLsOps *mysql)
{
    CJsonObject obj1;
    obj1 = req.body();
    if (!flag)
    {
        const std::map<string, string> &headers = req.headers();
        for (const auto &x : headers)
        {
            std::cout << x.first << " " << x.second << std::endl;
        }
        /* std::cout << req.body() << std::endl; */
        /* std::cout << obj1.ToFormattedString() << std::endl; */
    }

    if (req.path() == "/")
    {
        int x = atoi(req.query().c_str());
        resp->setStatusCode(x);
        CJsonObject reback;
        if (x > MySQLsOps::MIN && x < MySQLsOps::MID)
        {
            if (mysql->queryNoResult(x, obj1) == -1)
            {
                reback.Add("state", 400);
                reback.Add("message", "传递信息");
            }
            else
            {
                reback.Add("state", 200);
                reback.Add("message", "传递信息");
            }
        }
        else if (x > MySQLsOps::MID && x < MySQLsOps::MAX)
        {
            if (mysql->queryHasResult(x, obj1, reback) == -1)
            {
                reback.Add("state", 400);
                reback.Add("message", "传递信息");
            }
            else
            {
                reback.Add("state", 200);
                reback.Add("message", "传递信息");
            }
        }
        std::cout << reback.ToFormattedString() << std::endl;
        resp->setBody(reback.ToString());
    }
}

ssxrver::base::AsyncLogThread *g_asyncLog = NULL;
void asyncOutput(const char *msg, int len)
{
    g_asyncLog->append(msg, len);
}

int main(int argv, char *argc[])
{

    /* string logName = "ssxrver"; */
    /* ssxrver::base::AsyncLogThread log_(logName); */
    /* log_.start(); */
    /* g_asyncLog = &log_; */
    /* ssxrver::Logger::setOutput(asyncOutput); */

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_port = htons(4507);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int threads = 2;
    EventLoop loop;
    /* loop.setname("大循环"); */
    HttpServer server(&loop, serv_addr);
    server.setHttpCallback(message);
    server.setThreadNum(threads);
    server.start();
    loop.loop();

    return 0;
}
