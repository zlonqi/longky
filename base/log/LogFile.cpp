//
// Created by root on 2019/11/6.
//
#include "LogFIle.h"
#include "FileUtil.h"
#include "../time/ProcessInfo.h"
#include "../../net/EventLoop.h"

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>

using namespace std;
using namespace muduo;
LogFile::LogFile(const string& basename,
                 off_t rollSize,
                 int rollTime,
                 bool threadSafe,
                 int flushInterval,
                 int checkEveryN)
        : basename_(basename),
          rollSize_(rollSize),
          rollTime_(rollTime),
          flushInterval_(flushInterval),
          checkEveryN_(checkEveryN),
          count_(0),
          mutex_(threadSafe ? new MutexLock : NULL),
          startOfPeriod_(0),
          lastRoll_(0),
          lastFlush_(0)
{
   // assert(basename.find('/') == string::npos);
    rollFile();
    time_t now = ::time(nullptr) %kRollPerSeconds_;
    time_t dif = rollTime - now;
    if(dif < 0)
        dif = dif + kRollPerSeconds_;
    loop_.runAfter(dif,std::bind(&LogFile::setTimer,this));

}

LogFile::~LogFile() = default;

void LogFile::setTimer() {
    rollFile();
    loop_.runEvery(kRollPerSeconds_,std::bind(&LogFile::rollFile,this));
}
void LogFile::setRollTime(int rollTime) {
    rollTime_ = rollTime;
}

void LogFile::setTimeZone(int timeZone) {
    timeZone_ = timeZone;
}
void LogFile::append(const char* logline, int len)
{
    if (mutex_)
    {
        MutexLockGuard lock(*mutex_);
        append_unlocked(logline, len);
    }
    else
    {
        append_unlocked(logline, len);
    }
}

void LogFile::flush()
{
    if (mutex_)
    {
        MutexLockGuard lock(*mutex_);
        file_->flush();
    }
    else
    {
        file_->flush();
    }
}

void LogFile::quit(){
    loop_.quit();
}
void LogFile::append_unlocked(const char* logline, int len)
{
    file_->append(logline, len);

    if (file_->writtenBytes() > rollSize_)
    {
        rollFile();
    }
    loop_.runAfter(1,std::bind(&LogFile::quit,this));
    loop_.loop();
   /* else
    {
        ++count_;
        if (count_ >= checkEveryN_)
        {
            count_ = 0;
            time_t now = ::time(NULL);
            time_t thisPeriod_ = now / kRollPerSeconds_ * kRollPerSeconds_;
            int daySeconds = (now  + 3600 * timeZone_) % kRollPerSeconds_;
            if (thisPeriod_ != startOfPeriod_ && abs(daySeconds - rollTime_) < 300)
            {
                rollFile();
            }
            else if (now - lastFlush_ > flushInterval_)
            {
                lastFlush_ = now;
                file_->flush();
            }
        }
    }*/
}

bool LogFile::rollFile()
{
    time_t now = 0;
    string filename = getLogFileName(basename_, &now);
    time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;

    if (now > lastRoll_)
    {
        lastRoll_ = now;
        lastFlush_ = now;
        startOfPeriod_ = start;
        file_.reset(new FileUtil::AppendFile(filename));
        return true;
    }
    return false;
}

string LogFile::getLogFileName(const string& basename, time_t* now)
{
    string filename;
    filename.reserve(basename.size() + 64);
    filename = basename;

    char timebuf[32];
    struct tm tm;
    *now = time(NULL);
    //gmtime_r(now, &tm); // FIXME: localtime_r ?
    localtime_r(now,&tm);
    strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", &tm);
    filename += timebuf;

    filename += ProcessInfo::hostname();

    char pidbuf[32];
    snprintf(pidbuf, sizeof pidbuf, ".%d", ProcessInfo::pid());
    filename += pidbuf;

    filename += ".log";

    return filename;
}
