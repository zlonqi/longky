#include "cgiserver.h"
#include <cstdio>
#include <boost/algorithm/string.hpp>

using namespace tank;
using namespace tank::net;
using namespace std;

string g_conf_path;
tank::AsyncLogging *g_asyncLog = nullptr;
TcpConnectionPtr g_conn = nullptr;
string g_heartBeat_identification;
TcpClient *g_client = nullptr;

void asyncOutput(const char *msg, int len) {
    g_asyncLog->append(msg, len);
}

void sendHeartBeat() {
    if (g_conn) {
        LOG_INFO << "-send HeartBeats";
        g_conn->send(g_heartBeat_identification);
    }
}

void loopSendHeartBeat(vector<EventLoop *> &loop, int N) {
    static int count = 0;
    loop[count]->runInLoop(sendHeartBeat);
    count = (count++) % N;
}

void threadSendHeartBeat(EventLoop *loop, ThreadPool *threadPool) {
    auto fun = std::bind(&ThreadPool::put, threadPool, _1);
    loop->runEvery(g_heartBeat_frequency * 2, std::bind(fun, sendHeartBeat));
}

void onConnection(const TcpConnectionPtr &conn) {
    if (conn->connected()) {
        g_conn = conn;
        g_heartBeat_identification = conn->localAddress().toIpPort() + "'"
                                     + std::to_string(time(nullptr)) + +"'" + std::to_string(getpid());
        LOG_INFO << "Connect to HeartBeatMonitor Successfully - " << g_heartBeat_identification;
    } else {
        g_conn = nullptr;
        LOG_WARN << "disConnect by peer" << g_heartBeat_identification;
        g_client;
        g_client->connect();
        LOG_INFO << "re-connect to HeartBeatMonitor";
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usuage: -configuration_path\n");
        printf("like: /root/web/CLionProject/tank/fastcgi/config.yaml\n");
        return 0;
    }
    g_conf_path = argv[1];

    if (!init(g_conf_path)) {
        printf("configuration error, syntax or path");
        return 0;
    }

    tank::timeZone_ = g_log_rollZoneTime;
    boost::algorithm::to_lower(g_log_level);
    if (g_log_level == "info")
        tank::Logger::setLogLevel(tank::Logger::INFO);
    else if (g_log_level == "warn")
        tank::Logger::setLogLevel(tank::Logger::WARN);
    else if (g_log_level == "debug")
        tank::Logger::setLogLevel(tank::Logger::DEBUG);
    else if (g_log_level == "error")
        tank::Logger::setLogLevel(tank::Logger::ERROR);
    else if (g_log_level == "fatal")
        tank::Logger::setLogLevel(tank::Logger::FATAL);
    else if (g_log_level == "trace")
        tank::Logger::setLogLevel(tank::Logger::TRACE);
    else {
        printf("log_level error\n");
        return 0;
    }

    std::string exe_path = argv[0];
    std::string logBaseName = "";
    int i = exe_path.length() - 1;
    while (exe_path[i] != '/') {
        logBaseName += exe_path[i];
        --i;
    }
    std::reverse(logBaseName.begin(), logBaseName.end());
    tank::AsyncLogging log(g_log_path + logBaseName,
                           g_log_maxSize,
                           g_log_rollDaySeconds,
                           g_log_rollZoneTime,
                           g_log_IOFrequency);

    log.start();
    g_asyncLog = &log;
    tank::Logger::setOutput(asyncOutput);

    LOG_INFO << "Loading configuration file Successfully.";
    InetAddress addr(static_cast<uint16_t>(g_cgi_port));
    LOG_INFO << "FastCGI listens on " << addr.toIpPort() << " poolthreads " << g_cgi_poolThreads << " loopthreads"
             << g_cgi_loopThreads;

    tank::net::EventLoop loop;
    CgiServer server(&loop, addr, g_cgi_poolThreads, g_cgi_loopThreads);
    server.start();

    InetAddress Monitor_addr(g_heartBeat_ip, static_cast<uint16_t>(g_heartBeat_port));
    std::shared_ptr<TcpClient> client(new TcpClient(&loop, Monitor_addr, "HeartBeatBeatCHecker"));
    client->setConnectionCallback(std::bind(onConnection, _1));
    client->connect();
    g_client = client.get();
    vector<EventLoop *> loopVector;
    loopVector.emplace_back(&loop);
    shared_ptr<EventLoopThreadPool> loops = server.loopThreads();
    for (int i = 0; i < g_cgi_loopThreads; ++i)
        loopVector.emplace_back(loops.get()->getNextLoop());

    loop.runAfter(g_heartBeat_frequency, std::bind(threadSendHeartBeat, &loop, server.getThreadPool()));
    loop.runEvery(g_heartBeat_frequency * 2, std::bind(loopSendHeartBeat, loopVector, g_cgi_loopThreads + 1));
    loop.loop();
}
