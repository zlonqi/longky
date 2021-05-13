//
// Created by root on 2019/11/8.
//

#ifndef MUDUO_TIMERID_H
#define MUDUO_TIMERID_H

#include <stdint-gcc.h>
#include "base/copyable.h"

namespace tank
{
    namespace net
    {

        class Timer;

///
/// An opaque identifier, for canceling Timer.
///
        class TimerId : public tank::copyable
        {
        public:
            TimerId()
                    : timer_(nullptr),
                      sequence_(0)
            {
            }

            TimerId(Timer* timer, int64_t seq)
                    : timer_(timer),
                      sequence_(seq)
            {
            }

            // default copy-ctor, dtor and assignment are okay

            friend class TimerQueue;

        private:
            Timer* timer_;
            int64_t sequence_;
        };

    }  // namespace net
}  // namespace tank

#endif //MUDUO_TIMERID_H
