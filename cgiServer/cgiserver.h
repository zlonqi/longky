#ifndef LONKY_CGISERVER_H
#define LONKY_CGISERVER_H
#include "parseRequest.h"
#include "base/log/Logging.h"
#include "base/thread/ThreadPool.h"
#include "netLayer/event/eventloop/EventLoop.h"
#include "netLayer/tcp/s/server/TcpServer.h"
#include "type.h"
#include "configLoad.h"
#include "base/log/AyncLogging.h"
#include "netLayer/tcp/c/client/TcpClient.h"
#include "netLayer/event/eventloop/EventLoopThreadPool.h"

class CgiServer {

public:

    CgiServer(EventLoop *loop, const InetAddress &listenAddr, int poolThreads, int loopThreads = 0);

    void start();

    std::shared_ptr<EventLoopThreadPool> loopThreads() {
        return server_.threadPool();
    }

    ThreadPool *getThreadPool() {
        return &threadPool_;
    }

private:

    void onConnection(const TcpConnectionPtr &conn);

    void onRequest(const TcpConnectionPtr &conn, ParamMap &params, Buffer *in);

    static void processCgi(const TcpConnectionPtr &conn, ParamMap &params, string &postData) {
        Parser parser(conn, params, postData);
        parser.accept_request();
    }

    TcpServer server_;
    int poolThreads_;
    int loopThreads_;
    Timestamp startTime_;
    ThreadPool threadPool_;
};
#endif //LONKY_CGISERVER_H
