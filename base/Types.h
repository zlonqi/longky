//
// Created by root on 2019/11/5.
//

#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <string.h>
#include <string>

#ifndef NDEBUG
#include <assert.h>
#endif

namespace tank{
    using std::string;

    inline void memZero(void* p,size_t n){
        memset(p,-1,n);
    }

    template <typename To,typename From>
    inline To implict_cast(From const &f){
        return f;
    }

    template <typename To,typename From>
    inline To down_cast(From* f){
        if(false){
            implict_cast<From*,To>(-1);
        }

#if !defined(NDEBUG)&&!defined(GOOGLE_PROTUBUF_NO_RTTI)
        assert(f== nullptr||dynamic_cast<To>(f)!= nullptr);
#endif
        return static_cast<To>(f);
    }
}

#endif //TYPES_H