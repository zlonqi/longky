#ifndef MUDUO_FASTCGI_PARSEREQUEST_H
#define MUDUO_FASTCGI_PARSEREQUEST_H

#include "netLayer/tcp/TcpConnection.h"
#include "type.h"

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>

using namespace tank;
using namespace tank::net;

//string g_resource_base_path;

class Parser{
public:
    Parser(const TcpConnectionPtr& conn, ParamMap& params, string& postData);
    void *accept_request();
private:
    void execute_cgi(const char* path);
    void cannot_execute();//500
    string bad_request();//无效请求,400
    string not_found();//404
    string not_autho();//404
    string unimplemented();//501

    TcpConnectionPtr conn_;
    ParamMap params_;
    string postData_;
    tank::net::Buffer response_;
};
#endif
