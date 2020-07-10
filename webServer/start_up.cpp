#include "staticServer.h"
#include "configLoad.h"
#include "../base/log/AyncLogging.h"
#include "../base/log/Logging.h"
#include "../tcpSocket/EventLoopThreadPool.h"
#include "../tcpSocket/TcpClient.h"

#include <boost/algorithm/string.hpp>
#include <string>
#include <sys/stat.h>
#include <cstdio>
#include <csignal>
using namespace muduo;
using namespace muduo::net;

string g_conf_path;
muduo::AsyncLogging* g_asyncLog = nullptr;
std::weak_ptr<TcpConnection> g_conn;
string g_heartBeat_identification;
TcpClient* g_client = nullptr;
StaticServer* g_server=nullptr;
EventLoop* g_loop=nullptr;
bool closeHeartBeat = false;

void asyncOutput(const char* msg, int len)
{
    g_asyncLog->append(msg, len);
}

void SendHeartBeatInLoop(){
    if(!g_conn.expired()){
        LOG_INFO << time(nullptr) << "- loop send HeartBeats ";
        g_conn.lock()->send(g_heartBeat_identification);
    }
}

void SendHeartBeatInThread(){
    if(!g_conn.expired()){
        LOG_INFO << time(nullptr) << "- thread send HeartBeats ";
        g_conn.lock()->send(g_heartBeat_identification);
    }
}
void loopSendHeartBeat(vector<EventLoop*>& loop,int N){
    static int count = 0;
    loop[count]->runInLoop(SendHeartBeatInLoop);
    count++;
    count = count%N;
}

void threadSendHeartBeat(EventLoop* loop, ThreadPl<Parser>* threadPool){
    auto fun = std::bind(&ThreadPl<Parser>::put,threadPool,_1);
    loop->runEvery(g_heartBeat_frequency * 2,std::bind(fun,SendHeartBeatInThread));
}

void onConnection(const TcpConnectionPtr& conn){
    if(conn->connected()){
        g_conn = conn;
        g_heartBeat_identification = conn->localAddress().toIpPort() + "'"
                                     +std::to_string(time(nullptr))+ + "'" + std::to_string(getpid());
        LOG_INFO << "Connect to HeartBeatMonitor Successfully - " << g_heartBeat_identification;
    }else{
        //g_conn = nullptr;
        if(!closeHeartBeat){
            LOG_INFO << "Reset by peer ,re-connect to ...";
            g_client->connect();
        }else{
            LOG_INFO << "close HeartBeat Sender";
        }
    }
}

void stop(StaticServer* server,muduo::net::EventLoop* loop){
    closeHeartBeat = true;
    if(g_client)
        delete g_client;//loop_->runAfter(1, std::bind(&detail::removeConnector, connector_));
    g_client = nullptr;
    g_loop->runAfter(1.1,std::bind(&StaticServer::stop,server));
    g_loop->runAfter(1.2,std::bind(&EventLoop::quit,loop));
}

void process_exit(int signo){
    std::cout << "program recv signal [" << signo << "] to exit." << std::endl;
    stop(g_server,g_loop);
}

int main(int argc, char* argv[])
{
    if(argc < 2 ){
        printf("usuage: -configuration_path\n");
        printf("like: /root/web/CLionProject/muduo/staticPage/config.yaml\n");
        return 0;
    }
    g_conf_path = argv[1];

    if(!init(g_conf_path)){
        printf("configuration error, syntax or path");
        return 0;
    }
    muduo::timeZone_ = g_log_rollZoneTime;
    boost::algorithm::to_lower(g_log_level);
    if(g_log_level == "trace")
        muduo::Logger::setLogLevel(muduo::Logger::TRACE);
    else if(g_log_level == "debug")
        muduo::Logger::setLogLevel(muduo::Logger::DEBUG);
    else if(g_log_level == "info")
        muduo::Logger::setLogLevel(muduo::Logger::INFO);
    else if(g_log_level == "warn")
        muduo::Logger::setLogLevel(muduo::Logger::WARN);
    else if(g_log_level == "error")
        muduo::Logger::setLogLevel(muduo::Logger::ERROR);
    else if(g_log_level == "fatal")
        muduo::Logger::setLogLevel(muduo::Logger::FATAL);
    else{
        printf("log_level error\n");
        return 0;
    }

    std::string exe_path = argv[0];
    std::string logBaseName;
    int i = (int)exe_path.length()-1;
    while(exe_path[i] != '/'){
        logBaseName += exe_path[i];
        --i;
    }
    std::reverse(logBaseName.begin(),logBaseName.end());
    muduo::AsyncLogging log(g_log_path+logBaseName,
                            g_log_maxSize,
                            g_log_rollDaySeconds,
                            g_log_rollZoneTime,
                            g_log_IOFrequency
                            );

    log.start();
    g_asyncLog = &log;
    muduo::Logger::setOutput(asyncOutput);

    LOG_INFO << "Loading configuration file Successfully.";
    InetAddress addr(static_cast<uint16_t>(g_webServer_port));

    LOG_INFO << "StaticServer listens on " << addr.toIpPort() << " poolthreads " << g_webServer_poolThreads<<" loopthreads"<<g_webServer_loopThreads;

    muduo::net::EventLoop loop;
    StaticServer server(&loop, addr, g_webServer_poolThreads, g_webServer_loopThreads,g_maxConnections,g_conn_expire);
    server.start();
    g_server=&server;
    g_loop=&loop;

    boost::algorithm::to_lower(g_heartBeat_switch);
    vector<EventLoop*> loopVector;
    if(g_heartBeat_switch == "on"){
        InetAddress Monitor_addr(g_heartBeat_ip, static_cast<uint16_t>(g_heartBeat_port));
        g_client =new TcpClient(&loop,Monitor_addr,"HeartBeatBeatCHecker");
        g_client->setConnectionCallback(std::bind(onConnection, _1));
        g_client->connect();
        loopVector.emplace_back(&loop);

        shared_ptr<EventLoopThreadPool> loops =server.loopThreads();
        //cout << loops.use_count()<<endl;
        for(int j = 0;j < g_webServer_loopThreads; ++j){
            loopVector.emplace_back(loops->getNextLoop());
        }

        loop.runAfter(g_heartBeat_frequency,std::bind(threadSendHeartBeat,&loop,server.getThreadPool()));
        loop.runEvery(g_heartBeat_frequency * 2,std::bind(loopSendHeartBeat,loopVector,g_webServer_loopThreads+1));

    }
    if(g_stop_after < 1000000.0)//IF satify the equipment, give server a aliveTime, Or run it forever.
        loop.runAfter(g_stop_after,std::bind(stop,&server,&loop));

    //signal(SIGPIPE,SIG_IGN);
    signal(SIGINT,process_exit);
    signal(SIGTERM,process_exit);
    LOG_INFO << "Ignore SIGPIPE";
    loop.loop();
    return 0;
}
