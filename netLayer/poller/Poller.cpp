//
// Created by root on 2019/11/8.
//

#include "Poller.h"
#include "netLayer/event/Channel.h"
#include "netLayer/poller/EPollPoller.h"
#include "netLayer/poller/PollPoller.h"

using namespace tank;
using namespace tank::net;

Poller::Poller(EventLoop* loop)
        : ownerLoop_(loop)
{
}

Poller::~Poller() = default;

bool Poller::hasChannel(Channel* channel) const
{
    assertInLoopThread();
    ChannelMap::const_iterator it = channels_.find(channel->fd());
    return it != channels_.end() && it->second == channel;
}

/*Poller* Poller::newDefaultPoller(EventLoop* loop)
{
    if (::getenv("MUDUO_USE_POLL"))
        return new PollPoller(loop);
    else
        return new EPollPoller(loop);
}*/