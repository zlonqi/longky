//
// Created by root on 2019/11/5.
//

#ifndef MUDUO_COUNTDOWNLATCH_H
#define MUDUO_COUNTDOWNLATCH_H

#include "Condition.h"

namespace tank{
    class CountDownLatch:noncopyable{
    public:
        explicit CountDownLatch(int count);
        void wait();
        void countDown();
        int getCount() const ;
    private:
        mutable MutexLock mutex_;//keyword mutable help a vary can be changed even in a const object or function
        Condition condition_ GUARDED_BY((mutex_));
        int count_ GUARDED_BY(mutex_);
    };
}//end namespace tank
#endif //MUDUO_COUNTDOWNLATCH_H
