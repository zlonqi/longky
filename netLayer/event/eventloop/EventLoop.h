//
// Created by root on 2019/12/11.
//

#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "base/thread/Mutex.h"
#include "base/thread/CurrentThread.h"
#include "base/time/Timestamp.h"
#include "netLayer/Callbacks.h"
#include "netLayer/timer/TimerId.h"

#include <atomic>
#include <functional>
#include <vector>
#include <boost/any.hpp>

namespace tank{
    namespace net{
        class Channel;
        class Poller;
        class TimerQueue;

        ///
        ///  Rector,at most one per thread
        ///
        /// This is an interface class, so don't expose too much details.
        class EventLoop:noncopyable{
        public:
            typedef std::function<void ()> Functor;

            EventLoop();
            ~EventLoop();  // force out-line dtor, for std::unique_ptr members.

            ///
            /// Loops forever.
            ///
            /// Must be called in the same thread as creation of the object.
            ///
            void loop();

            /// Quits loop.
            ///
            /// This is not 100% thread safe, if you call through a raw pointer,
            /// better to call through shared_ptr<EventLoop> for 100% safety.
            void quit();

            ///
            /// Time when poll returns, usually means data arrival.
            ///
            Timestamp pollReturnTime() const { return pollReturnTime_; }

            int64_t iteration() const { return iteration_; }

            /// Runs callback immediately in the loop thread.
            /// It wakes up the loop, and run the cb.
            /// If in the same loop thread, cb is run within the function.
            /// Safe to call from other threads.
            void runInLoop(Functor cb);
            /// Queues callback in the loop thread.
            /// Runs after finish pooling.
            /// Safe to call from other threads.
            void queueInLoop(Functor cb);

            size_t queueSize() const;

            // timers

            ///
            /// Runs callback at 'time'.
            /// Safe to call from other threads.
            ///
            TimerId runAt(Timestamp time, TimerCallback cb);
            ///
            /// Runs callback after @c delay seconds.
            /// Safe to call from other threads.
            ///
            TimerId runAfter(double delay, TimerCallback cb);
            ///
            /// Runs callback every @c interval seconds.
            /// Safe to call from other threads.
            ///
            TimerId runEvery(double interval, TimerCallback cb);
            ///
            /// Cancels the timer.
            /// Safe to call from other threads.
            ///
            void cancel(TimerId timerId);

            // internal usage
            void wakeup();
            void updateChannel(Channel* channel);
            void removeChannel(Channel* channel);
            bool hasChannel(Channel* channel);

            // pid_t threadId() const { return threadId_; }
            void assertInLoopThread()
            {
                if (!isInLoopThread())
                {
                    abortNotInLoopThread();
                }
            }
            bool isInLoopThread() const {
                //printf("%d, %d\n",threadId_,CurrentThread::tid());
                return threadId_ == CurrentThread::tid();
            }
            // bool callingPendingFunctors() const { return callingPendingFunctors_; }
            bool eventHandling() const { return eventHandling_; }

            void setContext(const boost::any& context)
            { context_ = context; }

            const boost::any& getContext() const
            { return context_; }

            boost::any* getMutableContext()
            { return &context_; }

            static EventLoop* getEventLoopOfCurrentThread();

        private:
            typedef std::vector<Channel*> ChannelList;

            bool looping_;//atomic
            std::atomic<bool> quit_;
            bool eventHandling_;//atomic
            bool callingPendingFunctors_;//atomic
            int64_t iteration_;
            const pid_t threadId_;
            Timestamp pollReturnTime_;
            std::unique_ptr<Poller> poller_;
            std::unique_ptr<TimerQueue> timerQueue_;

            int wakeupFd_;
            std::unique_ptr<Channel> wakeupChannel_;
            boost::any context_;

            //scratch variables
            ChannelList activeChannels_;
            Channel* currentActiveChannel_;

            mutable MutexLock mutex_;
            std::vector<Functor> pendingFunctors_ GUARDED_BY(mutex_);

            void abortNotInLoopThread();
            void handleRead(); //waked up
            void doPendingFunctors();
            void printActiveChannels() const;//debug
        };
    }
}
#endif //EVENTLOOP_H
