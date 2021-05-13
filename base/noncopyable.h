//
// Created by root on 2019/11/5.
//

#ifndef MUDUO_NONCOPYABLE_H
#define MUDUO_NONCOPYABLE_H

namespace tank{
    class noncopyable{
    public:
        noncopyable(const noncopyable&)= delete;
        void operator=(const noncopyable&)= delete;

    protected:
        noncopyable()= default;
        ~noncopyable()= default;
    };
}
#endif //MUDUO_NONCOPYABLE_H
