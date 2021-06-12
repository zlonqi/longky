#ifndef STATICPAGE_STATICSERVER_H
#define STATICPAGE_STATICSERVER_H

#include "base/Atomic.h"
#include "base/log/Logging.h"
#include "base/thread/Thread.h"
#include "base/thread/threadPool.h"
#include "netLayer/event/eventloop/EventLoop.h"
#include "netLayer/tcp/s/server/TcpServer.h"
#include "netLayer/socket/InetAddress.h"
#include "netLayer/tcp/TcpConnection.h"
#include "requestParser.h"

#include <unordered_set>
#include <boost/circular_buffer.hpp>
#include <utility>

using namespace tank;
using namespace tank::net;

class StaticServer {
public:
    StaticServer(EventLoop *loop,
                 const InetAddress &listenAddr,
                 int poolThreads,
                 int loopThreads = 0,
                 int maxConnections = 1008,//1024-16
                 double idleDownLineTime = 600);

    ~StaticServer();

    void start();

    void stop();

    std::shared_ptr<EventLoopThreadPool> loopThreads() {
        return server_.threadPool();
    }

    ThreadPl<Parser> *getThreadPool() {
        return &threadPool_;
    }

    void onTimer();

    typedef std::weak_ptr<tank::net::TcpConnection> WeakTcpConnectionPtr;

    struct Entry : public tank::copyable {
        explicit Entry(WeakTcpConnectionPtr weakConn)
                : weakConn_(std::move(weakConn)) {
        }

        ~Entry() {
            tank::net::TcpConnectionPtr conn = weakConn_.lock();
            if (conn && conn->connected()) {
                conn->forceClose();
                LOG_DEBUG << conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort()
                          << " Idle Connection Down.";
            }
        }

        WeakTcpConnectionPtr weakConn_;
    };

    typedef std::shared_ptr<Entry> EntryPtr;
    typedef std::weak_ptr<Entry> WeakEntryPtr;
    typedef std::unordered_set<EntryPtr> Bucket;
    typedef boost::circular_buffer<Bucket> WeakConnectionList;
    typedef std::shared_ptr<WeakConnectionList> BucketPtr;
    std::unordered_map<EventLoop *, BucketPtr> map_;

    WeakConnectionList connectionBuckets_;
private:
    void onConnection(const TcpConnectionPtr &conn);

    void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp time);

    void highWaterMark(const TcpConnectionPtr &conn, size_t tosend);

    void writeComplete(const TcpConnectionPtr &conn);

    void onParser(const TcpConnectionPtr &conn, Buffer *buf);

    EventLoop *loop_;
    TcpServer server_;
    AtomicInt32 numConnected_;
    int32_t kmaxConnections_;
    int poolThreads_;
    int loopThreads_;
    double idleExpiredTime_;
    Timestamp startTime_;
    ThreadPl<Parser> threadPool_;//Thread Singleton Http Parser in ThreadPool
};

#endif
