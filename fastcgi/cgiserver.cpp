#include "fastcgi.h"
#include "parseRequest.h"
#include "../base/log/Logging.h"
#include "../base/thread/ThreadPool.h"
#include "../net/EventLoop.h"
#include "../net/TcpServer.h"
#include "type.h"
#include "configLoad.h"
#include "../base/log/AyncLogging.h"
#include "../net/TcpClient.h"
#include "../net/EventLoopThreadPool.h"

#include <yaml-cpp/yaml.h>
#include <cstdio>
#include <boost/algorithm/string.hpp>

using namespace muduo;
using namespace muduo::net;
using namespace std;

string g_nginx_ip;
int g_cgi_port;
int g_cgi_poolThreads;
int g_cgi_loopThreads;

string g_webServer_nginxIP;

string g_resource_base_path;
string g_resource_index_path;

string g_log_level;
string g_log_path;
long long g_log_maxSize;
int g_log_rollDaySeconds;
int g_log_rollZoneTime;
int g_log_IOFrequency;

string g_heartBeat_ip;
int g_heartBeat_port;
int g_heartBeat_frequency;


string g_db_server;
string g_db_user;
string g_db_pswd;
string g_db_dbName;


class CgiServer
{

public:

	CgiServer(EventLoop * loop, const InetAddress & listenAddr, int poolThreads, int loopThreads = 0)
		: server_(loop, listenAddr, "CGIServer", muduo::net::TcpServer::kReusePort),
		  poolThreads_(poolThreads),
		  loopThreads_(loopThreads),
		  startTime_(Timestamp::now())
		{
			server_.setConnectionCallback(std::bind(&CgiServer::onConnection,this,_1));
			//server_.setMessageCallback(std::bind(&CgiServer::onRequest,this,_1,_2,_3));
		}


	void start()
		{
//			LOG_INFO << "starting " << numThreads_ << " threads.";
			threadPool_.start(poolThreads_);
			server_.setThreadNum(loopThreads_);
			server_.start();
		}

    std::shared_ptr<EventLoopThreadPool> loopThreads(){
        return server_.threadPool();
    }

    ThreadPool* getThreadPool(){
        return &threadPool_;
    }
private:

	void onConnection(const TcpConnectionPtr& conn)
	{
		if (conn->connected())
		{
			typedef shared_ptr<FastCgiCodec> CodecPtr;
			CodecPtr codec(new FastCgiCodec(std::bind(&CgiServer::onRequest,this,_1,_2,_3)));
			conn->setContext(codec);
			conn->setMessageCallback(
			        std::bind(&FastCgiCodec::onMessage, codec, _1, _2, _3));
			conn->setTcpNoDelay(true);
		}
	}

	void onRequest(const TcpConnectionPtr& conn, ParamMap& params, Buffer* in)
	{

	    std::string postData;
	    if (in->readableBytes() > 0)
	    {
	        postData = in->retrieveAllAsString();
	    }

	    // Call Backend Process
	    threadPool_.put(std::bind(processCgi,conn,params,postData));
	}
	static void processCgi(const TcpConnectionPtr& conn, ParamMap& params, string& postData){
        Parser parser(conn,params,postData);
        parser.accept_request();
	}

    TcpServer       server_;
	int 			poolThreads_;
	int 			loopThreads_;
	Timestamp		startTime_;
	ThreadPool		threadPool_;
};

bool init(string& conf_path){
    bool no_error = true;
    try {
        YAML::Node config = YAML::LoadFile(conf_path);
        g_nginx_ip = config["cgiServer"]["ip"].as<string>();
        g_cgi_port = config["cgiServer"]["port"].as<int >();
        g_cgi_poolThreads = config["cgiServer"]["poolThreads"].as<int >();
        g_cgi_loopThreads = config["cgiServer"]["loopThreads"].as<int >();
        g_webServer_nginxIP = config["webServer"]["nginxIP"].as<string>();

        g_log_level = config["log"]["level"].as<string>();
        g_log_path = config["log"]["path"].as<string>();
        g_log_maxSize = config["log"]["maxSize"]["G"].as<long long>()*1024*1024*1024
                        + config["log"]["maxSize"]["M"].as<long long>()*1024*1024
                        + config["log"]["maxSize"]["K"].as<int>()*1024
                        + config["log"]["maxSize"]["B"].as<int>();
        g_log_rollZoneTime = config["log"]["rollTime"]["zoneTime"].as<int>();
        g_log_rollDaySeconds = config["log"]["rollTime"]["hour"].as<int>()*3600
                               + config["log"]["rollTime"]["minute"].as<int>()*60
                               + config["log"]["rollTime"]["second"].as<int>();
        g_log_IOFrequency = config["log"]["IOFrequency"].as<int>();

        g_heartBeat_ip = config["heartBeat"]["server"]["ip"].as<string>();
        g_heartBeat_port = config["heartBeat"]["server"]["port"].as<int>();
        g_heartBeat_frequency = config["heartBeat"]["frequency"].as<int>();

        g_resource_base_path = config["resource"]["basePath"].as<string>();
        g_db_server = config["dataBase"]["server"].as<string>();
        g_db_user = config["dataBase"]["user"].as<string>();
        g_db_pswd = config["dataBase"]["pswd"].as<string>();
        g_db_dbName = config["dataBase"]["dbName"].as<string>();
    }catch (exception &e) {
        cout << e.what() << endl;
        no_error = false;
    }

    return no_error;
}
string g_conf_path;
muduo::AsyncLogging* g_asyncLog = nullptr;
TcpConnectionPtr g_conn = nullptr;
string g_heartBeat_identification;
TcpClient* g_client = nullptr;

void asyncOutput(const char* msg, int len)
{
    g_asyncLog->append(msg, len);
}

void sendHeartBeat(){
    if(g_conn){
        LOG_INFO << "-send HeartBeats";
        g_conn->send(g_heartBeat_identification);
    }
}

void loopSendHeartBeat(vector<EventLoop*>& loop,int N){
    static int count = 0;
    loop[count]->runInLoop(sendHeartBeat);
    count = (count++)%N;
}

void threadSendHeartBeat(EventLoop* loop, ThreadPool* threadPool){
    auto fun = std::bind(&ThreadPool::put,threadPool,_1);
    loop->runEvery(g_heartBeat_frequency * 2,std::bind(fun,sendHeartBeat));
}

void onConnection(const TcpConnectionPtr& conn){
   if(conn->connected()){
       g_conn = conn;
       g_heartBeat_identification = conn->localAddress().toIpPort() + "'"
                                    +std::to_string(time(nullptr))+ + "'" + std::to_string(getpid());
       LOG_INFO << "Connect to HeartBeatMonitor Successfully - " << g_heartBeat_identification;
   }else{
       g_conn = nullptr;
       LOG_WARN << "disConnect by peer" << g_heartBeat_identification;
       g_client;
       g_client->connect();
       LOG_INFO << "re-connect to HeartBeatMonitor";
   }
}
int main(int argc, char* argv[])
{
    if(argc < 2 ){
        printf("usuage: -configuration_path\n");
        printf("like: /root/web/CLionProject/muduo/fastcgi/config.yaml\n");
        return 0;
    }
    g_conf_path = argv[1];

    if(!init(g_conf_path)){
        printf("configuration error, syntax or path");
        return 0;
    }

    muduo::timeZone_ = g_log_rollZoneTime;
    boost::algorithm::to_lower(g_log_level);
    if(g_log_level == "info")
        muduo::Logger::setLogLevel(muduo::Logger::INFO);
    else if(g_log_level == "warn")
        muduo::Logger::setLogLevel(muduo::Logger::WARN);
    else if(g_log_level == "debug")
        muduo::Logger::setLogLevel(muduo::Logger::DEBUG);
    else if(g_log_level == "error")
        muduo::Logger::setLogLevel(muduo::Logger::ERROR);
    else if(g_log_level == "fatal")
        muduo::Logger::setLogLevel(muduo::Logger::FATAL);
    else if(g_log_level == "trace")
        muduo::Logger::setLogLevel(muduo::Logger::TRACE);
    else{
        printf("log_level error\n");
        return 0;
    }

    std::string exe_path = argv[0];
    std::string logBaseName = "";
    int i = exe_path.length()-1;
    while(exe_path[i] != '/'){
        logBaseName += exe_path[i];
        --i;
    }
    std::reverse(logBaseName.begin(),logBaseName.end());
    muduo::AsyncLogging log(g_log_path+logBaseName,
                            g_log_maxSize,
                            g_log_rollDaySeconds,
                            g_log_rollZoneTime,
                            g_log_IOFrequency);

    log.start();
    g_asyncLog = &log;
    muduo::Logger::setOutput(asyncOutput);

    LOG_INFO << "Loading configuration file Successfully.";
    InetAddress addr(static_cast<uint16_t>(g_cgi_port));
    LOG_INFO << "FastCGI listens on " << addr.toIpPort() << " poolthreads " << g_cgi_poolThreads<<" loopthreads"<<g_cgi_loopThreads;

    muduo::net::EventLoop loop;
    CgiServer server(&loop, addr, g_cgi_poolThreads, g_cgi_loopThreads);
    server.start();

    InetAddress Monitor_addr(g_heartBeat_ip, static_cast<uint16_t>(g_heartBeat_port));
    std::shared_ptr<TcpClient> client(new TcpClient(&loop,Monitor_addr,"HeartBeatBeatCHecker"));
    client->setConnectionCallback(std::bind(onConnection, _1));
    client->connect();
    g_client = client.get();
    vector<EventLoop*> loopVector;
    loopVector.emplace_back(&loop);
    shared_ptr<EventLoopThreadPool> loops =server.loopThreads();
    for(int i = 0;i < g_cgi_loopThreads; ++i)
        loopVector.emplace_back(loops.get()->getNextLoop());

    loop.runAfter(g_heartBeat_frequency,std::bind(threadSendHeartBeat,&loop,server.getThreadPool()));
    loop.runEvery(g_heartBeat_frequency * 2,std::bind(loopSendHeartBeat,loopVector,g_cgi_loopThreads+1));
    loop.loop();
}
