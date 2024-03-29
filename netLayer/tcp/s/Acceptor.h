//
// Created by root on 2019/12/23.
//

#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <functional>

#include "netLayer/event/Channel.h"
#include "netLayer/socket/Socket.h"

namespace tank
{
    namespace net
    {

        class EventLoop;
        class InetAddress;

///
/// Acceptor of incoming TCP connections.
///
        class Acceptor : noncopyable
        {
        public:
            typedef std::function<void (int sockfd, const InetAddress&)> NewConnectionCallback;

            Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport);
            ~Acceptor();

            void setNewConnectionCallback(const NewConnectionCallback& cb)
            { newConnectionCallback_ = cb; }

            bool listenning() const { return listenning_; }
            void listen();

        private:
            void handleRead();

            EventLoop* loop_;
            Socket acceptSocket_;
            Channel acceptChannel_;
            NewConnectionCallback newConnectionCallback_;
            bool listenning_;
            int idleFd_;
        };

    }  // namespace net
}  // namespace tank

#endif //ACCEPTOR_H
