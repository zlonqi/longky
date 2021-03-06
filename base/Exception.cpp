//
// Created by root on 2019/11/6.
//
#include "Exception.h"
#include "CurrentThread.h"

namespace tank{
    Exception::Exception(std::string what)
        :message_(std::move(what))
        ,stack_(CurrentThread::stackTrace(false)) {
    }
}
