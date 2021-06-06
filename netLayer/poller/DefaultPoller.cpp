//
// Created by root on 2019/12/8.
//

#include "Poller.h"
#include "PollPoller.h"
#include "EPollPoller.h"

#include <cstdlib>

namespace tank{
    namespace net{
        Poller* Poller::newDefaultPoller(EventLoop* loop)
        {
            if (::getenv("MUDUO_USE_POLL"))//from libmuduo, mit license.
            {
                return new PollPoller(loop);
            }
            else
            {
                return new EPollPoller(loop);
            }
        }
    }
}

