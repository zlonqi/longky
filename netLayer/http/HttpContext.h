//
// Created by root on 2020/1/11.
//

#ifndef HTTPCONTEXT_H
#define HTTPCONTEXT_H

#include "base/copyable.h"

#include "HttpRequest.h"

namespace tank
{
    namespace net
    {

        class Buffer;

        class HttpContext : public tank::copyable
        {
        public:
            enum HttpRequestParseState
            {
                kExpectRequestLine,
                kExpectHeaders,
                kExpectBody,
                kGotAll,
            };

            HttpContext()
                    : state_(kExpectRequestLine)
            {
            }

            // default copy-ctor, dtor and assignment are fine

            // return false if any error
            bool parseRequest(Buffer* buf, Timestamp receiveTime);

            bool gotAll() const
            { return state_ == kGotAll; }

            void reset()
            {
                state_ = kExpectRequestLine;
                HttpRequest dummy;
                request_.swap(dummy);
            }

            const HttpRequest& request() const
            { return request_; }

            HttpRequest& request()
            { return request_; }

        private:
            bool processRequestLine(const char* begin, const char* end);

            HttpRequestParseState state_;
            HttpRequest request_;
        };

    }  // namespace net
}  // namespace tank


#endif //HTTPCONTEXT_H
