//
// Created by root on 2019/11/9.
//

#ifndef MUDUO_EVENTLOOPTHREAD_H
#define MUDUO_EVENTLOOPTHREAD_H

#include "base/thread/Condition.h"
#include "base/thread/Mutex.h"
#include "base/thread/Thread.h"

namespace tank
{
    namespace net
    {

        class EventLoop;

        class EventLoopThread : noncopyable
        {
        public:
            typedef std::function<void(EventLoop*)> ThreadInitCallback;

            EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(),
                            const string& name = string());
            ~EventLoopThread();
            EventLoop* startLoop();

        private:
            void threadFunc();

            EventLoop* loop_ GUARDED_BY(mutex_);
            bool exiting_;
            Thread thread_;
            MutexLock mutex_;
            Condition cond_ GUARDED_BY(mutex_);
            ThreadInitCallback callback_;
        };

    }  // namespace net
}  // namespace tank

#endif //MUDUO_EVENTLOOPTHREAD_H
