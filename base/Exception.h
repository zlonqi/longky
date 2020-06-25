//
// Created by root on 2019/11/5.
//

#ifndef MUDUO_EXCEPTION_H
#define MUDUO_EXCEPTION_H

#include "Types.h"
#include <exception>

namespace  muduo{
class Exception:public std::exception{
public:
    Exception(string what);
    ~Exception() noexcept override = default;
    const char* what() const noexcept override {//override mean overwrite the baseClass's virtualFunction
        return message_.c_str();
    }
    const char* stackTrace() const noexcept {
        return stack_.c_str();
    }

private:
    string message_;
    string stack_;
};
}
#endif //MUDUO_EXCEPTION_H
