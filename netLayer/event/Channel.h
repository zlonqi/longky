//
// Created by root on 2019/11/8.
//

#ifndef MUDUO_CHANNEL_H
#define MUDUO_CHANNEL_H

#include "base/noncopyable.h"
#include "base/time/Timestamp.h"

#include <functional>
#include <memory>

namespace tank{
    namespace net{
        class EventLoop;//front declare
        class Channel:noncopyable{
        public:
            typedef std::function<void ()> EventCallback;
            typedef std::function<void (Timestamp)> ReadEventCallback;

            Channel(EventLoop* loop, int fd);
            ~Channel();

            void handleEvent(Timestamp receiveTiem);
            void setReadCallback(ReadEventCallback cb){readCallback_ = std::move(cb); }
            void setWriteCallback(EventCallback cb){ writeCallback_ = std::move(cb); }
            void setCloseCallback(EventCallback cb){ closeCallback_ = std::move(cb);}
            void setErrorCallback(EventCallback cb){ errorCallback_ = std::move(cb); }

            void tie(const std::shared_ptr<void>&);
            int fd() const { return  fd_; }
            int events() const { return events_; }
            void set_revents(int revt){ revents_=revt; }
            bool isNoneEvent() const { return events_ == kNoneEvent;}

            void enableReading(){ events_ |= kReadEvent; update();}
            void disableReading(){ events_ &= ~kReadEvent;update();}
            void enableWriting(){ events_ |= kWriteEvent; update();}
            void disableWriting(){ events_ &= ~kWriteEvent;update();}
            void disableAll(){ events_ = kNoneEvent;update();}
            bool isWriting() const { return events_ & kWriteEvent;}
            bool isReading() const { return events_ & kReadEvent;}

            //for Poller
            int index() { return index_;}
            void set_index(int idx){ index_ = idx;}

            //for debuging
            string reventsToString() const ;
            string eventsToString() const;

            void doNotLogHup(){ logHup_ = false; }
            EventLoop* ownerLoop(){ return loop_; }
            void remove();
        private:
            static int kNoneEvent;
            static int kReadEvent;
            static int kWriteEvent;

            EventLoop* loop_;
            const int  fd_;
            int        events_;
            int        revents_;//it's the received event types of epoll or poll
            int        index_;//used by Poller.
            bool       logHup_;

            std::weak_ptr<void > tie_;//tie_ can bind any types obj with the Channel obj.
            bool tied_;
            bool eventHandling_;
            bool addedToLoop_;
            ReadEventCallback readCallback_;
            EventCallback     writeCallback_;
            EventCallback     closeCallback_;
            EventCallback     errorCallback_;

            static string eventsToString(int fd, int event);

            void update();
            void handleEventWithGuard(Timestamp receiveTime);
        };//end class Channel
    }//end namespace tank
}
#endif //MUDUO_CHANNEL_H
