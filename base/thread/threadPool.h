//
// Created by root on 2019/11/6.
//

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "Condition.h"
#include "Mutex.h"
#include "Thread.h"
#include "../Types.h"
#include "../Exception.h"
#include "ThreadLocalSingleton.h"

#include <deque>
#include <vector>

namespace tank{
    template <typename T>
    class ThreadPl:noncopyable{
    public:
        typedef std::function<void ()> Task;

        explicit ThreadPl(const string& nameArg = string("ThreadPool"))
                :mutex_(),
                 notFull_(mutex_),
                 notEmpty_(mutex_),
                 name_(nameArg),
                 maxQueueSize_(0),
                 isRunning_(false){
        }
        ~ThreadPl() {
            if(isRunning_)
                stop();
        }

        //Must be called before start();
        void setMaxQueueSize(int maxSize){maxQueueSize_ = maxSize;};
        void setThreadInitCallback(const Task& cb){threadInitCallback_ = cb;}

        void start(int numThreads) {
            assert(threads_.empty());
            isRunning_=true;
            threads_.reserve(numThreads);
            for(int i=0;i<numThreads;++i){
                char id[32];
                snprintf(id, sizeof(id),"%d",i+1);
                threads_.emplace_back(new tank::Thread(
                        std::bind(&ThreadPl::runInThread,this),name_+id
                ));
                threads_[i]->start();
            }
            if(numThreads==0 && threadInitCallback_)
                threadInitCallback_();
        }
        void  stop() {
            {
                MutexLockGuard lock(mutex_);
                isRunning_= false;
                notEmpty_.notifyAll();
            }
            for(auto& t:threads_)
                t->join();
        }

        const string& name() const{
            return name_;
        }
        size_t  queueSize() const {
            MutexLockGuard lock(mutex_);
            return queue_.size();
        }

        //Could bolck if maxQueueSize>0,
        //So there are no move-only version of std::function in C++ as of C++14
        //So we don't need to overload a const& and an && versions as we do in (Bounded)BlockingQueue.
        void  put(Task f) {
            if(threads_.empty())
                f();
            else{
                MutexLockGuard lock(mutex_);
                while(isFull())
                    notFull_.wait();
                assert(!isFull());
                queue_.emplace_back(f);
                notEmpty_.notify();
            }
        }
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

        void  runInThread() {
            try{
                if(threadInitCallback_){
                    threadInitCallback_();
                }
                tank::ThreadLocalSingleton<T>::instance();
                while (isRunning_){
                    Task task(take());
                    if(task){
                        task();
                        //LOG_INFO<<"take a task";
                    }

                }
                LOG_DEBUG << "stop thread";
            }catch (const Exception& ex){
                fprintf(stderr, "exception caught in ThreadPl %s\n", name_.c_str());
                fprintf(stderr, "reason: %s\n", ex.what());
                fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
                abort();
            }catch (const std::exception& ex){
                fprintf(stderr, "exception caught in ThreadPl %s\n", name_.c_str());
                fprintf(stderr, "reason: %s\n", ex.what());
                abort();
            }
            catch (...){
                fprintf(stderr, "unknown exception caught in ThreadPl %s\n", name_.c_str());
                throw; // rethrow
            }
        }

         Task take() {
            MutexLockGuard lock(mutex_);
            while(queue_.empty() && isRunning_)
                notEmpty_.wait();
            Task task;
            if(!queue_.empty()){
                task = queue_.front();
                queue_.pop_front();
                if(maxQueueSize_>0)
                    notFull_.notify();
            }
            return task;
        }

        bool  isFull() const {
            mutex_.assertLocked();
            return maxQueueSize_>0&&queue_.size()>=maxQueueSize_;//queue_.size() can't be replaced by function queueSize(),which must caught deadLock.
        }
    };
}
#endif //THREADPOOL_H
