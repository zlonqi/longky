//
// Created by root on 2019/11/5.
//

#ifndef MUDUO_CURRENTTHREAD_H
#define MUDUO_CURRENTTHREAD_H

#include "../Types.h"
namespace tank{
    namespace CurrentThread{
        extern __thread int t_cacheTid;
        extern __thread char t_tidString[32];
        extern __thread int t_tidStringLength;
        extern __thread const char* t_threadName;
        void cacheTid();

        inline int tid(){
            if(__builtin_expect(t_cacheTid==0,0))//if (t_cacheTid==0) ==> exec cacheTid();
                cacheTid();
            return t_cacheTid;
        }
        inline const char* tidString(){//for logging
            return t_tidString;
        }
        inline int tidStringLength(){//for logging
            return t_tidStringLength;
        }
        inline const char* name(){
            return t_threadName;
        }
        bool isMainThread();
        void sleepUsec(int64_t usec);//for testing
        string stackTrace(bool demangle);
    }//end of CurrentThread
}//end of tank
#endif //MUDUO_CURRENTTHREAD_H
