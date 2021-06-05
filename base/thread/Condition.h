//
// Created by root on 2019/11/6.
//

#ifndef CONDITION_H
#define CONDITION_H

#include "Mutex.h"

#include <pthread.h>

namespace tank{
    class Condition:noncopyable{
    public:
        explicit Condition(MutexLock& mutex):mutex_(mutex){
            MCHECK(pthread_cond_init(&pcond_, nullptr));
        }
        ~Condition(){
            MCHECK(pthread_cond_destroy(&pcond_));
        }
        void wait(){
            MutexLock::UnassignGuard ug(mutex_);
            MCHECK(pthread_cond_wait(&pcond_,mutex_.getPthreadMutex()));
        }
        bool waitForSeconds(double seconds);
        void notify(){
            MCHECK(pthread_cond_signal(&pcond_));
        }
        void notifyAll(){
            MCHECK(pthread_cond_broadcast(&pcond_));
        }
    private:
        MutexLock& mutex_;
        pthread_cond_t pcond_;
    };//end class Condition
}//end namespace tank
#endif //CONDITION_H
