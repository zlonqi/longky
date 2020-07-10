//
// Created by root on 2019/11/8.
//

#ifndef MUDUO_POLLER_H
#define MUDUO_POLLER_H

#include <map>
#include <vector>

#include "../base/time/Timestamp.h"
#include "../net/EventLoop.h"

namespace muduo
{
    namespace net
    {
        class Channel;
///
/// Base class for IO Multiplexing
///
/// This class doesn't own the Channel objects.
        class Poller : noncopyable
        {
        public:
            typedef std::vector<Channel*> ChannelList;

            Poller(EventLoop* loop);
            virtual ~Poller();

            /// Polls the I/O events.
            /// Must be called in the loop thread.
            virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels) = 0;

            /// Changes the interested I/O events.
            /// Must be called in the loop thread.
            virtual void updateChannel(Channel* channel) = 0;

            /// Remove the channel, when it destructs.
            /// Must be called in the loop thread.
            virtual void removeChannel(Channel* channel) = 0;

            virtual bool hasChannel(Channel* channel) const;

            static Poller* newDefaultPoller(EventLoop* loop);

            void assertInLoopThread() const
            {
                ownerLoop_->assertInLoopThread();
            }

        protected:
            typedef std::map<int, Channel*> ChannelMap;
            ChannelMap channels_;

        private:
            EventLoop* ownerLoop_;
        };
    }  // namespace net
}  // namespace muduo

#endif //MUDUO_POLLER_H
