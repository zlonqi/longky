//#include "redisConnector.h"
#include "configLoad.h"
#include "Hiredis.h"
#include "ZLibString.h"
#include "../base/log/Logging.h"
#include "../tcpSocket/EventLoop.h"

#include <string>
#include <sys/stat.h>
#include <cerrno>
#include <cstdio>

using namespace muduo;
using namespace muduo::net;
std::unordered_map<string,int> g_zlibMap;

redisConnector::redisConnector(EventLoop *loop, const TcpConnectionPtr& conn,string ip, int port, int rate):
        conn_(conn),
        Hiredis_(new hiredis::Hiredis(loop,ip,port)),
        g_zlib(CZlibMgr::getInstance()),
        g_rate(rate){
}

redisConnector::~redisConnector(){
    //delete g_unComPressContent;
    delete Hiredis_;
}
void redisConnector::start(){
    Hiredis_->setConnectCallback(connectCallback);
    Hiredis_->setDisconnectCallback(disconnectCallback);
    Hiredis_->connect();
}

char* redisConnector::returnBody() {
    return g_unComPressContent;
}

void redisConnector::setMD5(string md5) {
    g_md5 = md5;
}

void redisConnector::setConn(const TcpConnectionPtr& conn){
    conn_ = conn;
}

void redisConnector::setFile(string file) {
    g_file = file;
}
void redisConnector::exeSET() {
    Hiredis_->command2(std::bind(&redisConnector::setCallback,this,_1,_2),comPressString,compressLen,g_md5);
}
string redisConnector::toString(long long value) {
    char buf[32];
    snprintf(buf, sizeof buf, "%lld", value);
    return buf;
}

string redisConnector::redisReplyToString(const redisReply *reply) {
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

void redisConnector::connectCallback(hiredis::Hiredis *c, int status) {
    if (status != REDIS_OK)
    {
        LOG_WARN << "connectCallback Error:" << c->errstr();
    }
    else
    {
        LOG_INFO << "Connected...";
    }
}

void redisConnector::disconnectCallback(hiredis::Hiredis *c, int status) {
    if (status != REDIS_OK)
    {
        LOG_ERROR << "disconnectCallback Error:" << c->errstr();
    }
    else
    {
        LOG_INFO << "Disconnected...";
    }
}

void redisConnector::getCallback(hiredis::Hiredis *c, redisReply *reply) {
    redisResult += redisReplyToString(reply);
    if(redisResult == "")
        flag = false;
    run(Hiredis_,g_file.c_str(),g_md5.c_str());
}

void redisConnector::setCallback(hiredis::Hiredis *c, redisReply *reply) {
    if(redisReplyToString(reply) != "OK")
        flag = false;
}

void redisConnector::run(hiredis::Hiredis *hiredis, const char *file, const char *md5) {
    int size = 0;
    if(flag){
        if(g_unComPressContent != nullptr){
            free(g_unComPressContent);
            g_unComPressContent = nullptr;
        }
        g_unComPressContent = (char*)malloc(g_zlibMap[string(md5)]);
        g_zlib->UnCompress(redisResult.c_str(),g_unComPressContent,redisResult.length());
        LOG_INFO << "successfully got redis data [md5]" << string(md5);
    }else{
        FILE* fp = nullptr;
        fp  = fopen(file,"rb");
        if(fp){
            struct stat st;
            fstat(fp->_fileno,&st);
            size = st.st_size;
            char* plainString = (char*)malloc(size);
            if(g_unComPressContent != nullptr){
                free(g_unComPressContent);
                g_unComPressContent = nullptr;
            }
            g_unComPressContent = plainString;
            ::fread(plainString,1,size,fp);
            //printf("%d\n",retn);
            compressLen = size;
            comPressString = (char*)malloc((int)size*g_rate);
            g_zlib->Compress(plainString,comPressString,compressLen);
            g_zlibMap[string(md5)] = size;

            fclose(fp);

            //vector<char*> v;
            //size_t* argvlen = (size_t*)malloc(3*sizeof(size_t));
            //size_t argvlen[3];
            //const char **argv = (char**)malloc(3 * sizeof(char*));
            //const char* argv[3];
            /*char* cmd = (char*)malloc(4*sizeof(char));
            cmd[0]='s';
            cmd[1]='e';
            cmd[2]='t';
            cmd[3]='\0';*/
            //argv[0] = cmd;
            //argvlen[0] = 3;
            //char* md5s = (char*)malloc(33);
            //memcpy(md5s,md5,33);
            //md5s[32] = '\0';
            //argv[1] = md5s;
            //argvlen[1] = 32;
            //argv[2] = comPressString;
            //argvlen[2] = compressLen;
            flag = true;
            conn_.lock()->getLoop()->runInLoop(std::bind(&redisConnector::exeSET,this));
            if(!flag)
                LOG_WARN << "set redis item fail";
            else{
                g_zlibMap[string(md5)] = size;
                LOG_INFO << "set redis item Successfully [md5]" << string(md5);
                LOG_INFO << "[compressedLength/orginalLength]: " << ((int)compressLen)/size;
            }
        }else{
            printf("Error [%s]\n",(char*)strerror(errno));
        }
    }
    string res = "HTTP/1.1 200 OK\r\n";
    res += "Connection:";
    res += params_[2];
    res += "\r\n";
    res += "Content-type:";
    res += mimeType[params_[1]];
    res += ";charset=utf-8";
    res += "\r\n";
    res += "Content-Length:";
    res += std::to_string(size);
    res += "\r\n\r\n";
    response_.append(res);
    printf("%s\n",g_unComPressContent);
    printf("%d\n",strlen(g_unComPressContent));
    string body(g_unComPressContent,g_unComPressContent+compressLen);
    response_.append(body);
    conn_.lock()->send(&response_);
    LOG_INFO << "requestId=" << boost::any_cast<string>(conn_.lock()->getContext())<< ";return file: "<< params_[0];
    if(params_[2] == "close"){
        conn_.lock()->shutdown();
    }
    //hiredis->disconnect();
}
