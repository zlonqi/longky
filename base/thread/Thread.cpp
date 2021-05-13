//
// Created by root on 2019/11/5.
//
#include "../thread/Thread.h"
#include "../thread/CurrentThread.h"
#include "../Exception.h"

#include <type_traits>

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include<sys/prctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>

namespace tank{
    namespace detail{
        pid_t gettid(){
            return static_cast<pid_t >(::syscall(SYS_gettid));
        }
        void afterFork(){
            tank::CurrentThread::t_cacheTid=0;
            tank::CurrentThread::t_threadName="main";
            CurrentThread::tid();
        }

        class ThreadNameInitializer{
        public:
            ThreadNameInitializer(){
                tank::CurrentThread::t_threadName="main";
                CurrentThread::tid();
                pthread_atfork(nullptr,nullptr,&afterFork);
            }
        };
        ThreadNameInitializer init;
        struct  ThreadData{
            typedef tank::Thread::ThreadFunc ThreadFunc;
            ThreadFunc func_;
            string name_;
            pid_t * tid_;
            CountDownLatch* latch_;

            ThreadData(ThreadFunc func,const string& name,pid_t* tid,CountDownLatch *latch)
                :func_(std::move(func)),name_(name),tid_(tid),latch_(latch){
            }
            void runInThread(){
                *tid_=tank::CurrentThread::tid();
                tid_= nullptr;
                latch_->countDown();
                latch_=nullptr;

                tank::CurrentThread::t_threadName= name_.empty() ? "muduoThread" : name_.c_str();
                ::prctl(PR_SET_NAME, tank::CurrentThread::t_threadName);
                try{
                    func_();
                    tank::CurrentThread::t_threadName="finished";
                }catch (const Exception& ex){
                    tank::CurrentThread::t_threadName="crashed";
                    fprintf(stderr,"exception caught in Thread %s\n",name_.c_str());
                    fprintf(stderr,"reason:%s\n",ex.what());
                    fprintf(stderr,"stack trace:%s\n",ex.stackTrace());
                    abort();
                }catch (const std::exception& ex){
                    tank::CurrentThread::t_threadName="crashed";
                    fprintf(stderr,"exception caught in Thread %s\n",name_.c_str());
                    fprintf(stderr,"reason:%s\n",ex.what());
                    abort();
                }catch (...){
                    tank::CurrentThread::t_threadName="crashed";
                    fprintf(stderr,"unknown exception caught in Thread %s\n",name_.c_str());
                    throw ;
                }
            }//end function runInThread
        };//end struct ThreadData
        void* startThread(void* obj){
            ThreadData* data = static_cast<ThreadData*>(obj);
            data->runInThread();
            delete data;
            return nullptr;
        }
    }//end namespace detail

    void CurrentThread::cacheTid() {
        if(t_cacheTid==0){
            t_cacheTid=detail::gettid();
            t_tidStringLength=snprintf(t_tidString, sizeof(t_tidString),"%5d",t_cacheTid);
        }
    }
    bool CurrentThread::isMainThread() {
        return tid()==::getpid();
    }
    void CurrentThread::sleepUsec(int64_t usec) {
        struct timespec ts={0,0};
        ts.tv_sec= static_cast<time_t >(usec/Timestamp::kMicroSecondsPerSecond);//miao
        ts.tv_nsec= static_cast<long >(usec%Timestamp::kMicroSecondsPerSecond*1000);//Na miao
        ::nanosleep(&ts, nullptr);
    }
   // AtomicInt32 Thread
   Thread::Thread(ThreadFunc func, const std::string &name)
        :started_(false),joined_(false),pthreadId_(0),tid_(0)
         ,func_(std::move(func)),name_(name),latch_(1){
        setDefaultName();
    }
    Thread::~Thread() {
        if(started_&&!joined_)
            ::pthread_detach(pthreadId_);
    }
    AtomicInt32 Thread::numCreated_;
    void Thread::setDefaultName(){
        int num=numCreated_.incrementAndGet();
        if(name_.empty()){
            char buf[32];
            snprintf(buf, sizeof(buf),"Thread%d",num);
            name_=buf;
        }
    }
    void Thread::start() {
        assert(!started_);
        started_=true;
        detail::ThreadData* data=new detail::ThreadData(func_,name_,&tid_,&latch_);
        if(pthread_create(&pthreadId_, nullptr,&detail::startThread,data)){
            started_=false;
            delete data;
        }else{
            latch_.wait();
            assert(tid_>0);
        }
    }
    int Thread::join() {
        assert(started_);
        assert(!joined_);
        joined_=true;
        return pthread_join(pthreadId_, nullptr);//pthreadId must not be replaced by pthread_self()
    }
}//end namespace tank
