//
// Created by root on 2019/11/6.
//

#ifndef MUDUO_LOGFILE_H
#define MUDUO_LOGFILE_H


#include "../Mutex.h"
#include "../Types.h"
#include "../../net/EventLoop.h"

#include <memory>

using namespace muduo::net;
namespace muduo
{
    namespace FileUtil
    {
        class AppendFile;
    }

    class LogFile : noncopyable
    {
    public:
        LogFile(const string& basename,
                off_t rollSize,
                int rollTime,
                bool threadSafe = true,
                int flushInterval = 3,
                int checkEveryN = 1024);
        ~LogFile();

        void quit();
        void setTimer();
        void setRollTime(int rollTime);
        void setTimeZone(int timeZone);
        void append(const char* logline, int len);
        void flush();
        bool rollFile();

    private:
        void append_unlocked(const char* logline, int len);

        static string getLogFileName(const string& basename, time_t* now);

        EventLoop loop_;
        const string basename_;
        const off_t rollSize_;
        const int flushInterval_;
        const int checkEveryN_;

        int count_;

        int rollTime_;
        int timeZone_;
        std::unique_ptr<MutexLock> mutex_;
        time_t startOfPeriod_;
        time_t lastRoll_;
        time_t lastFlush_;
        std::unique_ptr<FileUtil::AppendFile> file_;

        const static int kRollPerSeconds_ = 60*60*24;
    };

}  // namespace muduo
#endif //MUDUO_LOGFILE_H
