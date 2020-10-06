#include "staticServer.h"
#include "../tcpSocket/EventLoopThreadPool.h"
#include "../base/log/Logging.h"
#include "../tcpSocket/EventLoop.h"
#include "../tcpSocket/TcpServer.h"
#include "../tcpSocket/Channel.h"
#include "configLoad.h"
#include <string>

StaticServer::StaticServer(EventLoop *loop,
                            const InetAddress &listenAddr,
                            int poolThreads,
                            int loopThreads,
                            int maxConnections,
                            double idleDownLineTime):
                            loop_(loop),
                            server_(loop, listenAddr, "StaticServer", muduo::net::TcpServer::kReusePort),
                            poolThreads_(poolThreads),
                            loopThreads_(loopThreads),
                            kmaxConnections_(maxConnections),
                            startTime_(Timestamp::now()),
                            idleExpiredTime_(idleDownLineTime)
{
    LOG_DEBUG << "StaticServer init in:" << startTime_.toFormattedString();
    server_.setConnectionCallback(std::bind(&StaticServer::onConnection, this, _1));
}

StaticServer::~StaticServer(){
    LOG_DEBUG << "Destruct StaticServer";
}
void StaticServer::start(){
    //threadPool_.setMaxQueueSize(poolThreads_*1000);//ulimit the size of task deque
    threadPool_.start(poolThreads_);
    server_.setThreadNum(loopThreads_);
    server_.start();
    shared_ptr<EventLoopThreadPool> loops =server_.threadPool();
    for(int j=0;j<loopThreads_;++j){
        BucketPtr buket(new WeakConnectionList);
        buket->resize(idleExpiredTime_);
        EventLoop* loop=loops->getNextLoop();
        map_.insert(make_pair(loop,buket));
    }
    loop_->runEvery(1.0, std::bind(&StaticServer::onTimer, this));
}

void StaticServer::stop(){
    threadPool_.stop();//Can cancel this,stack variab can auto destruct, where it will call ThreadPool::stop()
    shared_ptr<EventLoopThreadPool> loops = server_.threadPool();
    for(int i = 0; i < loopThreads_; ++i)
        loops->getNextLoop()->quit();//cancel this,stack variab can auto destruct EventLoopThreadPool, And auto destruct all EventLoopThread, where it will call EventLoop::quit()
}

void StaticServer::onConnection(const TcpConnectionPtr &conn) {
    LOG_INFO << "StaticServer - " << conn->peerAddress().toIpPort() << " -> "
             << conn->localAddress().toIpPort() << " is "
             << (conn->connected() ? "UP" : "DOWN");
    if(conn->connected()){
        numConnected_.increment();
        if(numConnected_.get() >= kmaxConnections_)
            conn->forceClose();
        else{
            string requestId = conn->peerAddress().toIpPort() + "'"
                                 +std::to_string(time(nullptr))+ + "'" + std::to_string(getpid());
            conn->setContextkv("requestId",requestId);
            conn->setTcpNoDelay(true);
            LOG_DEBUG << "setTcpNoDelay";
            conn->setMessageCallback(std::bind(&StaticServer::onMessage, this, _1, _2, _3));
            //conn->setHighWaterMarkCallback(
              //      std::bind(&StaticServer::highWaterMark, this, _1, _2),5*1024*1024);
            //conn->isHighWaterClose_ = false;
            EntryPtr entry(new Entry(conn));
            map_[conn->getLoop()]->back().insert(entry);
            WeakEntryPtr weakEntry(entry);
            conn->setContext(weakEntry);
        }
    }else{
        numConnected_.decrement();
    }
}

void StaticServer::onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp time) {
    //LOG_INFO << "requestId=" << conn->getContextkv("requestId")<< ";get the request at : "<<time.toFormattedString();
    conn->setWriteCompleteCallback(std::bind(&StaticServer::writeComplete,this,_1));
    //conn->setWriteCompleteCallback(WriteCompleteCallback());//cancel writeCOmpleteCallback
    threadPool_.put(std::bind(&StaticServer::onParser,this,conn,buf));
    assert(!conn->getContext().empty());
    WeakEntryPtr weakEntry(boost::any_cast<WeakEntryPtr>(conn->getContext()));
    EntryPtr entry(weakEntry.lock());
    if (entry)
    {
        map_[conn->getLoop()]->back().insert(entry);
    }
}

void StaticServer::highWaterMark(const TcpConnectionPtr &conn, size_t tosend) {
    LOG_DEBUG << "{" << conn->getContextkv("requestId")<< conn->name() << "}high water mark " << tosend;
    if (tosend < 10 * 1024 * 1024) {
        conn->setHighWaterMarkCallback(
                std::bind(&StaticServer::highWaterMark, this, _1, _2), 10 * 1024 * 1024);
        conn->setWriteCompleteCallback(std::bind(&StaticServer::writeComplete, this, _1));
    }else{
        conn->channel_->disableWriting();
        conn->isHighWaterClose_ = true;
    }
}

void StaticServer::writeComplete(const TcpConnectionPtr &conn) {
    LOG_DEBUG << "{" << conn->getContextkv("requestId")<< conn->name() << "}write complete";
    //conn->setHighWaterMarkCallback(
        //    std::bind(&StaticServer::highWaterMark, this, _1, _2), 5 * 1024 * 1024);
    //conn->setWriteCompleteCallback(WriteCompleteCallback());//cancel writeCOmpleteCallback
    /*if(conn->isHighWaterClose_){
        conn->channel_->enableWriting();
        conn->isHighWaterClose_ = false;
    }*/
    const int kBufSize = 1024*1024;
    FILE* fp = conn->getContextfp();
    if(!fp) {
        conn->stopWrite();
        LOG_INFO << "return - " << conn->getContextkv("file") << "[200 ok]";
    }else{
        char buf[kBufSize];
        size_t nread = ::fread(buf, 1, sizeof buf, fp);
        if (nread > 0)
        {
            conn->send(buf, static_cast<int>(nread));
        }else{
            ::fclose(fp);
            conn->setContextfp(nullptr);
            conn->stopWrite();
            string connectionFlag = conn->getContextkv("connectionFlag");
            if(connectionFlag == "close")
                conn->shutdown();
            LOG_INFO << "return - " << conn->getContextkv("file") << "[200 ok]";
        }
    }
}

void StaticServer::onParser(const TcpConnectionPtr &conn, Buffer *buf) {
    muduo::ThreadLocalSingleton<Parser>::instance().setConn(conn,buf);
    muduo::ThreadLocalSingleton<Parser>::instance().access_request();
    //Parser parser(conn,buf);
    //parser.access_request();
}

void StaticServer::onTimer()
{
    for(auto node:map_){
        node.second->push_back(Bucket());
    }
}
