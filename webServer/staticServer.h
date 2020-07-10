#ifndef MUDUO_STATICPAGE_STATICSERVER_H
#define MUDUO_STATICPAGE_STATICSERVER_H

#include "../base/Atomic.h"
#include "../base/log/Logging.h"
#include "../base/thread/Thread.h"
#include "../base/thread/threadPool.h"
#include "../tcpSocket/EventLoop.h"
#include "../tcpSocket/TcpServer.h"
#include "../tcpSocket/InetAddress.h"
#include "../tcpSocket/TcpConnection.h"
#include "requestParser.h"

#include <unordered_set>
#include <boost/circular_buffer.hpp>
#include <utility>
// RFC 862
using namespace muduo;
using namespace muduo::net;

class StaticServer{
public:
    StaticServer(EventLoop * loop,
                 const InetAddress & listenAddr,
                 int poolThreads,
                 int loopThreads = 0,
                 int maxConnections=1008,//1024-16
                 double idleDownLineTime = 600);
    ~StaticServer();
    void start();
    void stop();
    std::shared_ptr<EventLoopThreadPool> loopThreads(){
        return server_.threadPool();
    }

    ThreadPl<Parser>* getThreadPool(){
        return &threadPool_;
    }

    void onTimer();

    typedef std::weak_ptr<muduo::net::TcpConnection> WeakTcpConnectionPtr;

    struct Entry : public muduo::copyable
    {
        explicit Entry(const WeakTcpConnectionPtr&  weakConn)
                : weakConn_(weakConn)
        {
        }

        ~Entry()
        {
            muduo::net::TcpConnectionPtr conn = weakConn_.lock();
            if (conn)
            {
                conn->forceClose();
                LOG_DEBUG <<conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort() << " Idle Connection Down.";
            }
        }

        WeakTcpConnectionPtr weakConn_;
    };
    typedef std::shared_ptr<Entry> EntryPtr;
    typedef std::weak_ptr<Entry> WeakEntryPtr;
    typedef std::unordered_set<EntryPtr> Bucket;
    typedef boost::circular_buffer<Bucket> WeakConnectionList;

    WeakConnectionList connectionBuckets_;
private:
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time);
    void highWaterMark(const TcpConnectionPtr& conn, size_t tosend);
    void writeComplete(const TcpConnectionPtr& conn);
    void onParser(const TcpConnectionPtr& conn, Buffer* buf);

    TcpServer       server_;
    AtomicInt32     numConnected_;
    int32_t         kmaxConnections_;
    int 			poolThreads_;
    int 			loopThreads_;
    double             idleExpiredTime_;
    Timestamp		startTime_;
    ThreadPl<Parser>		threadPool_;
};

#endif
