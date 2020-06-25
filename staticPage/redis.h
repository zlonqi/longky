#ifndef __REDIS_H__
#define __REDIS_H__

#include <string>

#include <hiredis/hiredis.h>

using std::string;


namespace ccx{

    class Redis
    {
    public:
        Redis(string ip, int port);
    public:
        bool Connect();
        void disConnect();
    public:
        string toString(long long value);
        string redisReplyToString(const redisReply* reply);
        void exeSET(char* key, char* value,int valenth);
        string exeGET(char* key);
        bool isRedisConnectWell_;
    private:
        void freeReply();
        bool isError();
    private:
        string _ip;
        int _port;
        redisContext * _context;
        redisReply * _reply;
    };
}

#endif