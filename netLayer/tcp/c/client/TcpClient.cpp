//
// Created by root on 2019/12/30.
//

#include "TcpClient.h"
#include "base/log/Logging.h"
#include "../Connector.h"
#include "netLayer/event/eventloop/EventLoop.h"
#include "netLayer/socket/SocketsOps.h"

#include <stdio.h>  // snprintf

using namespace tank;
using namespace tank::net;

// TcpClient::TcpClient(EventLoop* loop)
//   : loop_(loop)
// {
// }

// TcpClient::TcpClient(EventLoop* loop, const string& host, uint16_t port)
//   : loop_(CHECK_NOTNULL(loop)),
//     serverAddr_(host, port)
// {
// }

namespace tank
{
    namespace net
    {
        namespace detail
        {

            void removeConnection(EventLoop* loop, const TcpConnectionPtr& conn)
            {
                loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
            }

            void removeConnector(const ConnectorPtr& connector)
            {
                //connector->
            }

        }  // namespace detail
    }  // namespace net
}  // namespace tank

TcpClient::TcpClient(EventLoop* loop,
                     const InetAddress& serverAddr,
                     const string& nameArg)
        : loop_(CHECK_NOTNULL(loop)),
          connector_(new Connector(loop, serverAddr)),
          name_(nameArg),
          connectionCallback_(defaultConnectionCallback),
          messageCallback_(defaultMessageCallback),
          retry_(false),
          connect_(true),
          nextConnId_(1)
{
    connector_->setNewConnectionCallback(
            std::bind(&TcpClient::newConnection, this, _1));
    // FIXME setConnectFailedCallback
    LOG_INFO << "TcpClient::TcpClient[" << name_
             << "] - connector " << get_pointer(connector_);
}

TcpClient::~TcpClient()
{
    LOG_INFO << "TcpClient::~TcpClient[" << name_
             << "] - connector " << get_pointer(connector_);
    TcpConnectionPtr conn;
    bool unique = false;
    {
        MutexLockGuard lock(mutex_);
        unique = connection_.unique();
        conn = connection_;
    }
    if (conn)
    {
        assert(loop_ == conn->getLoop());
        // FIXME: not 100% safe, if we are in different thread
        CloseCallback cb = std::bind(&detail::removeConnection, loop_, _1);
        loop_->runInLoop(
                std::bind(&TcpConnection::setCloseCallback, conn, cb));
        if (unique)
        {
            conn->forceClose();
        }
    }
    else
    {
        connector_->stop();
        // FIXME: HACK
        loop_->runAfter(1, std::bind(&detail::removeConnector, connector_));
    }
}

void TcpClient::connect()
{
    // FIXME: check state
    LOG_INFO << "TcpClient::connect[" << name_ << "] - connecting to "
             << connector_->serverAddress().toIpPort();
    connect_ = true;
    connector_->start();
}

void TcpClient::disconnect()//断开连接，主动下线
{
    connect_ = false;

    {
        MutexLockGuard lock(mutex_);
        if (connection_)
        {
            connection_->shutdown();
        }
    }
}

void TcpClient::stop()//停止正在进行的尝试连接
{
    connect_ = false;
    connector_->stop();
}

void TcpClient::newConnection(int sockfd)
{
    loop_->assertInLoopThread();
    InetAddress peerAddr(sockets::getPeerAddr(sockfd));
    char buf[32];
    snprintf(buf, sizeof buf, ":%s#%d", peerAddr.toIpPort().c_str(), nextConnId_);
    ++nextConnId_;
    string connName = name_ + buf;

    InetAddress localAddr(sockets::getLocalAddr(sockfd));
    // FIXME poll with zero timeout to double confirm the new connection
    // FIXME use make_shared if necessary
    TcpConnectionPtr conn(new TcpConnection(loop_,
                                            connName,
                                            sockfd,
                                            localAddr,
                                            peerAddr));

    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(
            std::bind(&TcpClient::removeConnection, this, _1)); // FIXME: unsafe
    {
        MutexLockGuard lock(mutex_);
        connection_ = conn;
    }
    conn->connectEstablished();
}

void TcpClient::removeConnection(const TcpConnectionPtr& conn)//具有重连功能的removeConnection
{
    loop_->assertInLoopThread();
    assert(loop_ == conn->getLoop());

    {
        MutexLockGuard lock(mutex_);
        assert(connection_ == conn);
        connection_.reset();
    }

    loop_->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    if (retry_ && connect_) //连接被断开后的重连，如意外时刻的断网
    {
        LOG_INFO << "TcpClient::connect[" << name_ << "] - Reconnecting to "
                 << connector_->serverAddress().toIpPort();
        connector_->restart();
    }
}

