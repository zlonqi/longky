//
// Created by root on 2019/11/8.
//

#ifndef MUDUO_POLLPOLLER_H
#define MUDUO_POLLPOLLER_H

#include "Poller.h"

#include <vector>

struct pollfd;

namespace tank
{
    namespace net
    {

///
/// IO Multiplexing with poll(2).
///
        class PollPoller : public Poller
        {
        public:

            PollPoller(EventLoop* loop);
            ~PollPoller() override;

            Timestamp poll(int timeoutMs, ChannelList* activeChannels) override;
            void updateChannel(Channel* channel) override;
            void removeChannel(Channel* channel) override;

        private:
            void fillActiveChannels(int numEvents,
                                    ChannelList* activeChannels) const;

            typedef std::vector<struct pollfd> PollFdList;
            PollFdList pollfds_;
        };

    }  // namespace net
}  // namespace tank

#endif //MUDUO_POLLPOLLER_H
