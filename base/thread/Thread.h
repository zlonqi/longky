//
// Created by root on 2019/11/5.
//

#ifndef MUDUO_THREAD_H
#define MUDUO_THREAD_H

#include "../Atomic.h"
#include "CountDownLatch.h"
#include "../Types.h"
#include "../time/Timestamp.h"

#include <functional>
#include <memory>
#include <pthread.h>

namespace muduo{
    class Thread:noncopyable{
    public:
        typedef std::function<void ()> ThreadFunc;
        explicit Thread(ThreadFunc,const string& name = string());
        ~Thread();

        void start();

        int join();//return pthread_join();

        bool isStarted() const {return started_;}

        pid_t tid() const {return pthreadId_;}

        const string& name() const{return name_;}

        static int numCreated(){return numCreated_.get();}
    private:
        void setDefaultName();
        bool started_;
        bool joined_;
        pthread_t  pthreadId_;
        pid_t tid_;
        ThreadFunc func_;
        string name_;
        CountDownLatch latch_;
        static  AtomicInt32 numCreated_;
    };
}
#endif //MUDUO_THREAD_H
