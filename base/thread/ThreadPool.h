//
// Created by root on 2019/11/6.
//

#ifndef MUDUO_THREADPOOL_H
#define MUDUO_THREADPOOL_H

#include "Condition.h"
#include "Mutex.h"
#include "Thread.h"
#include "base/Types.h"

#include <deque>
#include <vector>

namespace tank{
    class ThreadPool:noncopyable{
    public:
        typedef std::function<void ()> Task;

        explicit ThreadPool(const string& nameArg = string("ThreadPool"));
        ~ThreadPool();

        //Must be called before start();
        void setMaxQueueSize(int maxSize){maxQueueSize_ = maxSize;};
        void setThreadInitCallback(const Task& cb){threadInitCallback_ = cb;}

        void start(int numThreads);
        void stop();

        const string& name() const{
            return name_;
        }
        size_t queueSize() const;
        //Could bolck if maxQueueSize>0,
        //So there are no move-only version of std::function in C++ as of C++14
        //So we don't need to overload a const& and an && versions as we do in (Bounded)BlockingQueue.
        void put(Task f);
    private:
        mutable MutexLock mutex_;
        Condition notEmpty_ GUARDED_BY(mutex_);
        Condition notFull_ GUARDED_BY(mutex_);
        std::vector<std::unique_ptr<tank::Thread>> threads_;
        std::deque<Task> queue_ GUARDED_BY(mutex_);
        Task threadInitCallback_;
        size_t maxQueueSize_;
        bool isRunning_;
        string name_;

        void runInThread();
        Task take();
        bool isFull() const REQUIRES(mutex_);
    };
}
#endif //MUDUO_THREADPOOL_H
