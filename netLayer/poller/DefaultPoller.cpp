//
// Created by root on 2019/11/8.
//

#include "Poller.h"
#include "PollPoller.h"
#include "EPollPoller.h"

#include <stdlib.h>

namespace tank{
    namespace net{
        Poller* Poller::newDefaultPoller(EventLoop* loop)
        {
            if (::getenv("MUDUO_USE_POLL"))
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

