#include "redis.h"

#include <cstdlib>
#include <iostream>
#include <exception>

namespace ccx{

    using std::cout;
    using std::endl;
    using std::stringstream;

    Redis::Redis(string ip,int port):
    _ip(ip),
    _port(port)
    {
    }

    bool Redis::Connect()
    {
        _context = ::redisConnect(_ip.c_str(), _port);
        if(_context && _context->err)
        {
            //cout << "connect redis error" << endl;
            return false;
            //exit(EXIT_FAILURE);
        }
        return true;
        //cout << "redis Connect success" << endl;
    }

    void Redis::disConnect()
    {
        ::redisFree(_context);
        //cout << "redis disConnect success" << endl;
    }

    string Redis::toString(long long value)
    {
        char buf[32];
        snprintf(buf, sizeof buf, "%lld", value);
        return buf;
    }
    string Redis::redisReplyToString(const redisReply* reply)
    {
        static const char* const types[] = { "",
                                             "REDIS_REPLY_STRING", "REDIS_REPLY_ARRAY",
                                             "REDIS_REPLY_INTEGER", "REDIS_REPLY_NIL",
                                             "REDIS_REPLY_STATUS", "REDIS_REPLY_ERROR" };
        string str = "";
        if (!reply) return str;

        if (reply->type == REDIS_REPLY_STRING ||
            reply->type == REDIS_REPLY_STATUS ||
            reply->type == REDIS_REPLY_ERROR)
        {
            str += string(reply->str, reply->len);
        }
        else if (reply->type == REDIS_REPLY_INTEGER)
        {
            str += toString(reply->integer);
        }
        else if (reply->type == REDIS_REPLY_ARRAY)
        {
            str += toString(reply->elements) + " ";
            for (size_t i = 0; i < reply->elements; i++)
            {
                str += " " + redisReplyToString(reply->element[i]);
            }
        }
        return str;
    }
    void Redis::exeSET(char* key, char* value,int valenth){
        int argc = 3;
        const char *argv[] = {"set",key,value};
        size_t argvlen[] = {3,32,(size_t)valenth};
        try{
            redisCommandArgv(_context,argc,argv,argvlen);
        }catch(std::exception& e){
            throw 1;
        }
    }

    string Redis::exeGET(char* key){
        int argc = 2;
        const char *argv[] = {"get",key};
        size_t argvlen[] = {3,32};
        redisReply* reply =(redisReply*) redisCommandArgv(_context, 2, argv, argvlen);
        string value;
        if(reply == nullptr) throw 0;
        if(reply->type != REDIS_REPLY_NIL){
            value =redisReplyToString(reply);
        }
        freeReplyObject(reply);
        return value;
    }
     void Redis::freeReply()
    {
        if(_reply)
        {
            ::freeReplyObject(_reply);
            _reply = NULL;
        }
    }

    bool Redis::isError()
    {
        if(NULL == _reply)
        {
            freeReply();
            disConnect();
            Connect();
            return true;
        }
        return false;
    }
}