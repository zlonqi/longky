//
// Created by root on 2019/11/6.
//

#include "ThreadPool.h"
#include "base/Exception.h"
#include "base/log/Logging.h"
#include <assert.h>
#include <stdio.h>

using namespace tank;

ThreadPool::ThreadPool(const std::string &nameArg)
    :mutex_(),
    notFull_(mutex_),
    notEmpty_(mutex_),
    name_(nameArg),
    maxQueueSize_(0),
    isRunning_(false){
}
ThreadPool::~ThreadPool() {
    if(isRunning_)
        stop();
}
void ThreadPool::start(int numThreads) {
    assert(threads_.empty());
    isRunning_=true;
    threads_.reserve(numThreads);
    for(int i=0;i<numThreads;++i){
        char id[32];
        snprintf(id, sizeof(id),"%d",i+1);
        threads_.emplace_back(new tank::Thread(
                std::bind(&ThreadPool::runInThread,this),name_+id
                ));
        threads_[i]->start();
    }
    if(numThreads==0 && threadInitCallback_)
        threadInitCallback_();
}

void ThreadPool::stop() {
    {
        MutexLockGuard lock(mutex_);
        isRunning_= false;
        notEmpty_.notifyAll();
    }
    for(auto& t:threads_)
        t->join();
}

void ThreadPool::runInThread() {
    try{
        if(threadInitCallback_){
            threadInitCallback_();
        }
        while (isRunning_){
            Task task(take());
            if(task){
                task();
                //LOG_INFO<<"take a task";
            }

        }
    }catch (const Exception& ex){
        fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
        fprintf(stderr, "reason: %s\n", ex.what());
        fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
        abort();
    }catch (const std::exception& ex){
        fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
        fprintf(stderr, "reason: %s\n", ex.what());
        abort();
    }
    catch (...){
        fprintf(stderr, "unknown exception caught in ThreadPool %s\n", name_.c_str());
        throw; // rethrow
    }
}

ThreadPool::Task ThreadPool::take() {
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

void ThreadPool::put(Task f) {
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

size_t ThreadPool::queueSize() const {
    MutexLockGuard lock(mutex_);
    return queue_.size();
}

bool ThreadPool::isFull() const {
    mutex_.assertLocked();
    return maxQueueSize_>0&&queue_.size()>=maxQueueSize_;//queue_.size() can't be replaced by function queueSize(),which must caught deadLock.
}
