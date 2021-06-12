#include "cgiserver.h"
#include "fastcgi.h"

    CgiServer::CgiServer(EventLoop *loop, const InetAddress &listenAddr, int poolThreads, int loopThreads)
            : server_(loop, listenAddr, "CGIServer", tank::net::TcpServer::kReusePort),
              poolThreads_(poolThreads),
              loopThreads_(loopThreads),
              startTime_(Timestamp::now()) {
        server_.setConnectionCallback(std::bind(&CgiServer::onConnection, this, _1));
        //server_.setMessageCallback(std::bind(&CgiServer::onRequest,this,_1,_2,_3));
    }

    void CgiServer::start() {
//			LOG_INFO << "starting " << numThreads_ << " threads.";
        threadPool_.start(poolThreads_);
        server_.setThreadNum(loopThreads_);
        server_.start();
    }


    void CgiServer::onConnection(const TcpConnectionPtr &conn) {
        if (conn->connected()) {
            typedef shared_ptr<FastCgiCodec> CodecPtr;
            CodecPtr codec(new FastCgiCodec(std::bind(&CgiServer::onRequest, this, _1, _2, _3)));
            conn->setContext(codec);
            conn->setMessageCallback(
                    std::bind(&FastCgiCodec::onMessage, codec, _1, _2, _3));
            conn->setTcpNoDelay(true);
        }
    }

    void CgiServer::onRequest(const TcpConnectionPtr &conn, ParamMap &params, Buffer *in) {

        std::string postData;
        if (in->readableBytes() > 0) {
            postData = in->retrieveAllAsString();
        }

        // Call Backend Process
        threadPool_.put(std::bind(processCgi, conn, params, postData));
    }