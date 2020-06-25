//
// Created by root on 2019/11/5.
//
#include "Exception.h"
#include "CurrentThread.h"

namespace muduo{
    Exception::Exception(std::string what)
        :message_(std::move(what))
        ,stack_(CurrentThread::stackTrace(false)) {
    }
}
