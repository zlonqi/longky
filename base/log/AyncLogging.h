//
// Created by root on 2019/11/6.
//

#ifndef MUDUO_AYNCLOGGING_H
#define MUDUO_AYNCLOGGING_H


//#include "BlockingQueue.h"
//#include "BoundedBlockingQueue.h"
#include "../thread/CountDownLatch.h"
#include "../thread/Thread.h"
#include "../Mutex.h"
#include "LogStream.h"

#include <atomic>
#include <vector>

namespace muduo
{

    class AsyncLogging : noncopyable
    {
    public:

        AsyncLogging(const string& basename,
                     off_t rollSize,
                     int rollTime,
                     int timeZone,
                     int flushInterval = 3,
                     int checkEveryNBuffer = 1024);

        ~AsyncLogging()
        {
            if (running_)
            {
                stop();
            }
        }

        void append(const char* logline, int len);

        void start()
        {
            running_ = true;
            thread_.start();
            latch_.wait();
        }

        void stop() NO_THREAD_SAFETY_ANALYSIS
        {
            running_ = false;
            cond_.notify();
            thread_.join();
        }

    private:

        void threadFunc();

        typedef muduo::detail::FixedBuffer<muduo::detail::kLargeBuffer> Buffer;
        typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
        typedef BufferVector::value_type BufferPtr;

        int rollTime_;
        int timeZone_;
        int checkEveryNBuffer_;
        const int flushInterval_;
        std::atomic<bool> running_;
        const string basename_;
        const off_t rollSize_;
        muduo::Thread thread_;
        muduo::CountDownLatch latch_;
        muduo::MutexLock mutex_;
        muduo::Condition cond_ GUARDED_BY(mutex_);
        BufferPtr currentBuffer_ GUARDED_BY(mutex_);
        BufferPtr nextBuffer_ GUARDED_BY(mutex_);
        BufferVector buffers_ GUARDED_BY(mutex_);
    };

}  // namespace muduo
#endif //MUDUO_AYNCLOGGING_H
