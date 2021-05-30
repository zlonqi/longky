//
// Created by root on 2019/11/5.
//

#ifndef ATOMIC_H
#define ATOMIC_H

#include "noncopyable.h"
#include <stdint.h>

namespace tank{
    namespace detail{
        template <typename T>
        class AtomicIntegerT:noncopyable{
        public:
            AtomicIntegerT():value_(0){
            }

            T get(){
                return __atomic_load_n(&value_,__ATOMIC_SEQ_CST);//builtin atomic
            }
            T getAndAdd(T x){
                return __atomic_fetch_add(&value_,x,__ATOMIC_SEQ_CST);
            }
            T addAndGet(T x){
                return getAndAdd(x)+x;
            }
            T incrementAndGet(){
                return addAndGet(1);
            }
            T decrementAndGet(){
                return addAndGet(-1);
            }
            void add(T x){
                getAndAdd(x);
            }
            void increment(){
                incrementAndGet();
            }
            void decrement(){
                decrementAndGet();
            }
            T getAndSet(T newValue){
                return __atomic_exchange_n(&value_,newValue,__ATOMIC_SEQ_CST);
            }
        private:
            volatile T value_;//volatile forceRequire for update value_ in memory after operater value_ every time,so that Every Pthread can get the latest value_ ,rather than the mid-value in register.
        };//end class AtomicIntegerT
    }//end namespace detail
    typedef detail::AtomicIntegerT<int32_t > AtomicInt32;
    typedef detail::AtomicIntegerT<int64_t > AtomicInt64;
}//end namespace tank


#endif //ATOMIC_H
