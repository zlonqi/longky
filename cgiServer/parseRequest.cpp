#include "parseRequest.h"
#include "base/log/Logging.h"
#include "type.h"
#include "fastcgi.h"
#include "configLoad.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>

Parser:: Parser(const TcpConnectionPtr& conn,
                SSMap& params,
                string& postData):
                conn_(conn),
                params_(params),
                postData_(postData){
        LOG_INFO<<"Parsing cgi::requestion";
}

void* Parser::accept_request() {
    string method=params_["REQUEST_METHOD"];
    string path = params_["DOCUMENT_URI"];
    path = g_resource_base_path + path;
    //path = "/root/web/CLionProject/tank/fastcgi" + path;
    LOG_INFO << "path:"<<path;
    if (method != "GET" && method != "POST")
    {
        //tinyhttp仅仅实现了GET和POST
        string res = unimplemented();
        FastCgiCodec::respond(res, &response_);
        conn_->send(&response_);
        return nullptr;
    }
    struct stat st;

    if (stat(path.c_str(), &st) == -1) {
        //最后声明网页不存在
        string res = not_found();
        FastCgiCodec::respond(res, &response_);
        conn_->send(&response_);
        return nullptr;
    }

    if (!(st.st_mode & S_IXUSR) &&
        !(st.st_mode & S_IXGRP) &&
        !(st.st_mode & S_IXOTH)    ){
        //have no authorization to access re.
        string res = not_autho();
        FastCgiCodec::respond(res, &response_);
        conn_->send(&response_);
        return nullptr;
    }
    execute_cgi(path.c_str());
    //conn_->shutdown();//keep long connection.
    return nullptr;
}

void Parser::execute_cgi(const char* path) {
    char buf[1024];
    int cgi_output[2];//声明的读写管道，切莫被名称给忽悠，会给出图进行说明
    int cgi_input[2];//
    pid_t pid;
    int content_length = -1;
    string query_string;
    char c;
    int status;

    string method = params_["REQUEST_METHOD"];
    if(method == "POST"){
        content_length = atoi(params_["CONTENT_LENGTH"].c_str());
        if(content_length == -1){
            string res = bad_request();
            FastCgiCodec::respond(res, &response_);
            conn_->send(&response_);
            return ;
        }
    }
    if(method == "GET"){
       query_string = params_["QUERY_STRING"] ;
    }
    if (pipe(cgi_output) < 0) {
        cannot_execute();
        return;
    }
    if (pipe(cgi_input) < 0) {
        cannot_execute();
        return;
    }

    if ( (pid = fork()) < 0 ) {
        cannot_execute();
        return;
    }
    //fork出一个子进程运行cgi脚本
    if (pid == 0)  /* 子进程: 运行CGI 脚本 */
    {
        char meth_env[255];
        char query_env[255];
        char length_env[255];

        dup2(cgi_output[1], 1);//1代表着stdout，0代表着stdin，将系统标准输出重定向为cgi_output[1]
        dup2(cgi_input[0], 0);//将系统标准输入重定向为cgi_input[0]，这一点非常关键，
        //cgi程序中用的是标准输入输出进行交互
        close(cgi_output[0]);//关闭了cgi_output中的读通道
        close(cgi_input[1]);//关闭了cgi_input中的写通道
        //CGI标准需要将请求的方法存储环境变量中，然后和cgi脚本进行交互
        //存储REQUEST_METHOD
        sprintf(meth_env, "REQUEST_METHOD=%s", method.c_str());
        putenv(meth_env);
        if (strcasecmp(method.c_str(), "GET") == 0) {
            //存储QUERY_STRING
            sprintf(query_env, "QUERY_STRING=%s", query_string.c_str());
            putenv(query_env);
        } else {   /* POST */
            //存储CONTENT_LENGTH
            sprintf(length_env, "CONTENT_LENGTH=%d", content_length);
            putenv(length_env);
        }

        execl(path, path, nullptr);
        exit(0);
    }else{
        close(cgi_output[1]);//关闭了cgi_output中的写通道，注意这是父进程中cgi_output变量和子进程要区分开
        close(cgi_input[0]);//关闭了cgi_input中的读通道
        if (strcasecmp(method.c_str(), "POST") == 0)
            write(cgi_input[1],postData_.c_str(),postData_.length());
        string res ="";
        while(read(cgi_output[0],&c,1))
            res += c;
        //
        //printf("%s",res.c_str());
        FastCgiCodec::respond(res, &response_);
        conn_->send(&response_);
        //printf("send ok\n");
        close(cgi_output[0]);
        close(cgi_input[1]);
        waitpid(pid, &status, 0);
    }
}

string Parser::bad_request() {
    string res = "HTTP/1.0 400 BAD REQUEST\r\n";
    res += "Content-type: text/html\r\n\r\n";
    res += "<P>Your browser sent a bad request, ";
    res += "such as a POST without a Content-Length.\r\n";

    return res;
}

string Parser::not_found() {
    string res = "HTTP/1.0 404 NOT FOUND\r\n";
    res += "Content-Type: text/html\r\n\r\n";
    res += "<HTML><TITLE>404 Not Found</TITLE>\r\n";
    res += "<BODY><P>The server could not fulfill\r\n";
    res += "your request because the resource specified\r\n";
    res += "is unavailable or nonexistent.\r\n";
    res += "</BODY></HTML>\r\n";

    return res;
}

string Parser::not_autho() {
    string res = "HTTP/1.0 403 Forbidden\r\n";
    res += "Content-Type: text/html\r\n\r\n";
    res += "<HTML><TITLE> 403 Forbidden </TITLE>\r\n";
    res += "<BODY><P> 403 Forbidden \r\n";
    res += "the resource you accessed you have no permission for it \r\n";
    res += "</BODY></HTML>\r\n";

   return res;
}

void Parser::cannot_execute() {
    string res = "HTTP/1.0 500 Internal Server Error\r\n";
    res += "Content-type: text/html\r\n\r\n";
    res += "\r\n";
    res += "<P>Error prohibited CGI execution.\r\n";

    FastCgiCodec::respond(res, &response_);
    conn_->send(&response_);
}

string Parser::unimplemented() {
    string res = "HTTP/1.0 501 Method Not Implemented\r\n";
    res += "Content-Type: text/html\r\n\r\n";
    res += "<HTML><HEAD><TITLE>Method Not Implemented\r\n";
    res += "</TITLE></HEAD>\r\n";
    res += "<BODY><P>HTTP request method not supported.\r\n";
    res += "</BODY></HTML>\r\n";

   return res;
}
