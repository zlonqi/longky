//
// Created by root on 2019/12/15.
//

#ifndef EVENTLOOPTHREADPOOL_H
#define EVENTLOOPTHREADPOOL_H

#include "base/noncopyable.h"
#include "base/Types.h"

#include <functional>
#include <memory>
#include <vector>

namespace tank
{

    namespace net
    {

        class EventLoop;
        class EventLoopThread;

        class EventLoopThreadPool : noncopyable
        {
        public:
            typedef std::function<void(EventLoop*)> ThreadInitCallback;

            EventLoopThreadPool(EventLoop* baseLoop, const string& nameArg);
            ~EventLoopThreadPool();
            void setThreadNum(int numThreads) { numThreads_ = numThreads; }
            void start(const ThreadInitCallback& cb = ThreadInitCallback());

            // valid after calling start()
            /// round-robin
            EventLoop* getNextLoop();

            /// with the same hash code, it will always return the same EventLoop
            EventLoop* getLoopForHash(size_t hashCode);

            std::vector<EventLoop*> getAllLoops();

            bool started() const
            { return started_; }

            const string& name() const
            { return name_; }

        private:

            EventLoop* baseLoop_;
            string name_;
            bool started_;
            int numThreads_;
            int next_;
            std::vector<std::unique_ptr<EventLoopThread>> threads_;
            std::vector<EventLoop*> loops_;
        };

    }  // namespace net
}  // namespace tank

#endif //EVENTLOOPTHREADPOOL_H
